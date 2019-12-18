/*****************************************************************************

        DistoPwm.cpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/fnc.h"
#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/distpwm/Param.h"
#include "mfx/pi/distpwm/DistoPwm.h"
#include "mfx/piapi/EventParam.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/EventType.h"

#include <algorithm>

#include <cassert>
#include <cmath>



namespace mfx
{
namespace pi
{
namespace distpwm
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



DistoPwm::DistoPwm ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_sample_freq (0)
,	_inv_fs (0)
,	_param_change_flag ()
,	_chn_arr ()
,	_pw (0)
,	_threshold (1e-4f)
{
	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_PW, 2.5e-4f);

	_state_set.add_observer (Param_PW, _param_change_flag);

	dsp::mix::Align::setup ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	DistoPwm::do_get_state () const
{
	return _state;
}



double	DistoPwm::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	fstb::unused (categ, note_id);
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	DistoPwm::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	float          b2_s [2] = { 0, 1 };
	float          a2_s [2] = { 1, 1 };
	float          a2_z [2];
	float          b2_z [2];
	dsp::iir::TransSZBilin::map_s_to_z_one_pole (
		b2_z, a2_z, b2_s, a2_s, 30, _sample_freq
	);
	float          b3_s [3] = { 1,                   0, 0 };
	float          a3_s [3] = { 1, float (fstb::SQRT2), 1 };
	float          a3_z [3];
	float          b3_z [3];
	dsp::iir::TransSZBilin::map_s_to_z (
		b3_z, a3_z, b3_s, a3_s, 1500, _sample_freq
	);
	for (auto &chn : _chn_arr)
	{
		chn._lpf_in.set_z_eq (b3_z, a3_z);
		chn._hpf_out.set_z_eq (b2_z, a2_z);
	}

	const int      mbl_align = max_buf_len & -4;
	_buf_tmp.resize (mbl_align);

	update_param (true);

	clear_buffers ();

	_state = State_ACTIVE;

	return Err_OK;
}



void	DistoPwm::do_clean_quick ()
{
	clear_buffers ();
}



void	DistoPwm::do_process_block (ProcInfo &proc)
{
	const int      nbr_chn_src =
		proc._nbr_chn_arr [piapi::PluginInterface::Dir_IN ];
	const int      nbr_chn_dst =
		proc._nbr_chn_arr [piapi::PluginInterface::Dir_OUT];
	assert (nbr_chn_src <= nbr_chn_dst);
	const int      nbr_chn_proc = std::min (nbr_chn_src, nbr_chn_dst);

	// Events
	for (int evt_cnt = 0; evt_cnt < proc._nbr_evt; ++evt_cnt)
	{
		const piapi::EventTs &  evt = *(proc._evt_arr [evt_cnt]);
		if (evt._type == piapi::EventType_PARAM)
		{
			const piapi::EventParam &  evtp = evt._evt._param;
			assert (evtp._categ == piapi::ParamCateg_GLOBAL);
			_state_set.set_val (evtp._index, evtp._val);
		}
	}

	// Parameters
	_state_set.process_block (proc._nbr_spl);
	update_param ();

	// Signal processing
	for (int chn_index = 0; chn_index < nbr_chn_proc; ++chn_index)
	{
		Channel &      chn     = _chn_arr [chn_index];
		const float *  src_ptr = proc._src_arr [chn_index];
		float *        dst_ptr = proc._dst_arr [chn_index];

		chn._lpf_in.process_block (&_buf_tmp [0], src_ptr, proc._nbr_spl);

		for (int pos = 0; pos < proc._nbr_spl; ++pos)
		{
			const float    lvl = 0.125f;
			float          x   = _buf_tmp [pos];
			float          val = lvl;
			if (chn._schmitt_pos_flag && x < -_threshold)
			{
				chn._schmitt_pos_flag = false;
			}
			else if (! chn._schmitt_pos_flag && x > _threshold)
			{
				chn._schmitt_pos_flag = true;
				chn._dur_cycle        = (x - _threshold) / (x - chn._spl_prev);
				val = -lvl + 2 * lvl * chn._dur_cycle;
			}

			const float    dif = chn._dur_cycle - _pw;
			if (dif >= 0)
			{
				if (dif <= 1)
				{
					val += (-lvl - val) * dif;
					chn._dur_cycle = 1e10f;
				}
				else
				{
					val = -lvl;
				}
			}

			_buf_tmp [pos] = val;

			chn._spl_prev = x;
			++ chn._dur_cycle;
		}

		chn._hpf_out.process_block (dst_ptr, &_buf_tmp [0], proc._nbr_spl);

	}

	// Duplicates the remaining output channels
	for (int chn_index = nbr_chn_proc; chn_index < nbr_chn_dst; ++chn_index)
	{
		dsp::mix::Align::copy_1_1 (
			proc._dst_arr [chn_index],
			proc._dst_arr [0],
			proc._nbr_spl
		);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DistoPwm::clear_buffers ()
{
	for (auto &chn : _chn_arr)
	{
		chn._lpf_in.clear_buffers ();
		chn._hpf_out.clear_buffers ();
		chn._schmitt_pos_flag = false;
		chn._dur_cycle        = 0;
		chn._spl_prev         = 0;
	}
}



void	DistoPwm::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		const float   pw_s = float (_state_set.get_val_tgt_nat (Param_PW));
		_pw = pw_s * _sample_freq;
	}
}



}  // namespace distpwm
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
