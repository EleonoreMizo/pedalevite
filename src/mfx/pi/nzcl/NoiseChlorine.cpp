/*****************************************************************************

        NoiseChlorine.cpp
        Author: Laurent de Soras, 2017

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
#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/nzcl/NoiseChlorine.h"
#include "mfx/pi/nzcl/Param.h"
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
namespace nzcl
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



NoiseChlorine::NoiseChlorine ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_param_proc (_state_set)
,	_sample_freq (0)
,	_inv_fs (0)
,	_param_change_flag ()
,	_param_change_flag_misc ()
,	_param_change_flag_notch_arr ()
,	_chn_arr ()
,	_lvl_base (1e-4f)
,	_lvl_notch_arr ()
,	_buf_tmp_arr ()
{
	mfx::dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_LVL     , 1e-5);

	_state_set.add_observer (Param_LVL     , _param_change_flag_misc);
	
	_param_change_flag_misc.add_observer (_param_change_flag);

	// Notches
	for (int index = 0; index < Cst::_nbr_notches; ++index)
	{
		const int      base  = NoiseChlorineDesc::get_base_notch (index);
		auto &         cflag = _param_change_flag_notch_arr [index];

		_lvl_notch_arr [index] = 0;

		const double   freq = 80 << index;
		_state_set.set_val_nat (desc_set, base + ParamNotch_FREQ, freq);
		_state_set.set_val_nat (desc_set, base + ParamNotch_Q	  , 1);
		_state_set.set_val_nat (desc_set, base + ParamNotch_LVL , 0);

		_state_set.add_observer (base + ParamNotch_FREQ, cflag);
		_state_set.add_observer (base + ParamNotch_Q	  , cflag);
		_state_set.add_observer (base + ParamNotch_LVL , cflag);

		cflag.add_observer (_param_change_flag);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	NoiseChlorine::do_get_state () const
{
	return _state;
}



double	NoiseChlorine::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	fstb::unused (categ, note_id);
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	NoiseChlorine::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	const int      mbs_alig = (max_buf_len + 3) & -4;
	for (auto &buf : _buf_tmp_arr)
	{
		buf.resize (mbs_alig);
	}

	for (auto &chn : _chn_arr)
	{
		for (auto &notch : chn._notch_arr)
		{
			notch.reset (
				sample_freq,
				max_buf_len,
				&_buf_tmp_arr [0] [0],
				&_buf_tmp_arr [1] [0]
			);
		}
	}
	

	_param_change_flag_misc.set ();
	for (auto &cflag : _param_change_flag_notch_arr)
	{
		 cflag.set ();
	}

	update_param (true);
	clear_buffers ();
	_param_proc.req_steady ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	NoiseChlorine::do_process_block (piapi::ProcInfo &proc)
{
	// Channels
	const int      nbr_chn_src = proc._dir_arr [piapi::Dir_IN ]._nbr_chn;
	const int      nbr_chn_dst = proc._dir_arr [piapi::Dir_OUT]._nbr_chn;
	assert (nbr_chn_src <= nbr_chn_dst);
	const int      nbr_chn_proc = std::min (nbr_chn_src, nbr_chn_dst);

	// Events
	_param_proc.handle_msg (proc);

	const int      nbr_spl  = proc._nbr_spl;

	// Parameters
	_state_set.process_block (nbr_spl);
	update_param (false);
	if (_param_proc.is_full_reset ())
	{
		clear_buffers ();
	}

	// Signal
	for (int chn_index = 0; chn_index < nbr_chn_proc; ++chn_index)
	{
		Channel &      chn     = _chn_arr [chn_index];

		const float *  src_ptr = proc._src_arr [chn_index];
		float *        dst_ptr = proc._dst_arr [chn_index];
		for (int n_idx = 0; n_idx < Cst::_nbr_notches; ++n_idx)
		{
			chn._notch_arr [n_idx].process_block (dst_ptr, src_ptr, nbr_spl);
			src_ptr = dst_ptr;
		}
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



void	NoiseChlorine::clear_buffers ()
{
	for (auto &chn : _chn_arr)
	{
		for (auto &notch : chn._notch_arr)
		{
			notch.clear_buffers ();
		}
	}
}



void	NoiseChlorine::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		if (_param_change_flag_misc (true) || force_flag)
		{
			_lvl_base = float (_state_set.get_val_end_nat (Param_LVL));
			update_all_levels ();
		}

		// Notches
		for (int index = 0; index < Cst::_nbr_notches; ++index)
		{
			if (_param_change_flag_notch_arr [index] (true) || force_flag)
			{
				const int      base  = NoiseChlorineDesc::get_base_notch (index);

				const float    freq = float (
					_state_set.get_val_end_nat (base + ParamNotch_FREQ)
				);
				const float    q    = float (
					_state_set.get_val_end_nat (base + ParamNotch_Q)
				);
				_lvl_notch_arr [index] = float (
					_state_set.get_val_end_nat (base + ParamNotch_LVL)
				);

				for (auto &chn : _chn_arr)
				{
					Notch &        notch = chn._notch_arr [index];
					notch.set_freq (freq);
					notch.set_q (q);
					notch.set_lvl (_lvl_notch_arr [index] * _lvl_base);
				}
			}
		}
	}
}



void	NoiseChlorine::update_all_levels ()
{
	for (int index = 0; index < Cst::_nbr_notches; ++index)
	{
		const float    lvl_final = _lvl_notch_arr [index] * _lvl_base;
		for (auto &chn : _chn_arr)
		{
			chn._notch_arr [index].set_lvl (lvl_final);
		}
	}
}



}  // namespace nzcl
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
