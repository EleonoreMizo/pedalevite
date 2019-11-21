/*****************************************************************************

        DistTone.cpp
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

#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/pi/dtone1/DistTone.h"
#include "mfx/pi/dtone1/Param.h"
#include "mfx/piapi/Err.h"
#include "mfx/piapi/EventParam.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/EventType.h"
#include "mfx/piapi/ProcInfo.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace dtone1
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



DistTone::DistTone ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_sample_freq (0)
,	_param_change_shape_flag ()
,	_param_change_freq_flag ()
,	_inv_fs (0)
,	_chn_arr ()
,	_tone (0.5f)
,	_mid (0.5f)
,	_freq (530)
,	_b_s ()
,	_a_s ()
{
	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_TONE  ,   0.5);
	_state_set.set_val_nat (desc_set, Param_MID   ,   0);
	_state_set.set_val_nat (desc_set, Param_CENTER, 530);

	_state_set.add_observer (Param_TONE  , _param_change_shape_flag);
	_state_set.add_observer (Param_MID   , _param_change_shape_flag);
	_state_set.add_observer (Param_CENTER, _param_change_freq_flag);

	for (int p = 0; p < Param_NBR_ELT; ++p)
	{
		_state_set.set_ramp_time (p, 0.010);
	}

	update_filter_shape ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	DistTone::do_get_state () const
{
	return _state;
}



double	DistTone::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	DistTone::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;
	_sample_freq = float (sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	_inv_fs = float (1.0 / _sample_freq);
	update_filter_coef ();

	clear_buffers ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	DistTone::do_clean_quick ()
{
	clear_buffers ();
}



void	DistTone::do_process_block (piapi::ProcInfo &proc)
{
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

	bool           coef_update_flag = false;
	if (_param_change_shape_flag (true))
	{
		_tone = float (_state_set.get_val_end_nat (Param_TONE));
		_mid  = float (_state_set.get_val_end_nat (Param_MID ));
		update_filter_shape ();
		coef_update_flag = true;
	}

	if (_param_change_freq_flag (true))
	{
		_freq = float (_state_set.get_val_end_nat (Param_CENTER));
		coef_update_flag = true;
	}

	if (coef_update_flag)
	{
		update_filter_coef ();
	}

	// Signal processing
	const int      nbr_chn = proc._nbr_chn_arr [piapi::Dir_IN];
	assert (nbr_chn == proc._nbr_chn_arr [piapi::Dir_OUT]);
	for (int chn = 0; chn < nbr_chn; ++chn)
	{
		_chn_arr [chn].process_block (
			proc._dst_arr [chn],
			proc._src_arr [chn],
			proc._nbr_spl
		);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DistTone::clear_buffers ()
{
	for (auto &chn : _chn_arr)
	{
		chn.clear_buffers ();
	}
}



void	DistTone::update_filter_shape ()
{
	const float    c11 =   22e-9f;
	const float    c12 =  100e-9f;
	const float    r3  =   20e3f;
	const float    r15 = 2200.f;
	const float    r16 = 6800.f;
	const float    r17 = 6800.f;
	const float    ff1 = 530.0f; // Reference frequency
	const float    ff2 = ff1 * ff1;
	const float    g   = 2;

	const float    a = 0.52f;
	const float    p = ((a - 1) * _tone + (2 - a)) * _tone;
	const float    m = _mid * 0.6f + 0.4f;
	_b_s [0] =        g * r17                                        + (1 - p) * (      g * r3);
	_b_s [1] =  ff1 * g * c11 * r17 * (r15 + r16 + r3) * (1 + m * 2) + (1 - p) * (ff1 * g * r3 * c11 * r15);
	_b_s [2] = (ff2 * g * c11 * c12 * r16 * r17 * r3) * p;
	_a_s [0] =            r16 + r17 + r3;
	_a_s [1] =  ff1     * ((c12 * r16 * (r17 + r3) + c11 * (r17 * (r16 + r3) + r15 * (r16 + r17 + r3)))) * (1 - m * 0.5f);
	_a_s [2] =  ff2     * (c11 * c12 * r16 * (r17 * r3 + r15 * (r17 + r3)));
}



void	DistTone::update_filter_coef ()
{
	float          bz [3];
	float          az [3];
	const float    k       =
		dsp::iir::TransSZBilin::compute_k_approx (_freq * _inv_fs);
	dsp::iir::TransSZBilin::map_s_to_z_approx (bz, az, &_b_s [0], &_a_s [0], k);
	for (auto &hpf : _chn_arr)
	{
		hpf.set_z_eq (bz, az);
	}
}



}  // namespace dtone1
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
