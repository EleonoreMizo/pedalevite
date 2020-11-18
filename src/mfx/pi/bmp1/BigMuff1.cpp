/*****************************************************************************

        BigMuff1.cpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/fnc.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/bmp1/Param.h"
#include "mfx/pi/bmp1/BigMuff1.h"
#include "mfx/piapi/Dir.h"
#include "mfx/piapi/Err.h"
#include "mfx/piapi/EventParam.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/EventType.h"
#include "mfx/piapi/ProcInfo.h"

#include <algorithm>

#include <cassert>
#include <cmath>



namespace mfx
{
namespace pi
{
namespace bmp1
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



BigMuff1::BigMuff1 ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_param_proc (_state_set)
,	_sample_freq (0)
,	_inv_fs (0)
,	_param_change_flag ()
,	_param_change_flag_core ()
,	_param_change_flag_misc ()
,	_chn_arr (_max_nbr_chn)
,	_ovrspl (Ovrspl_X1)
,	_ovrspl_rate (1)
,	_buf_tmp ()
{
	dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_SUS    , 0.25);
	_state_set.set_val_nat (desc_set, Param_TONE   , 0.5);
	_state_set.set_val_nat (desc_set, Param_VOL    , 1.0);
	_state_set.set_val_nat (desc_set, Param_PREGAIN, 1);
	_state_set.set_val_nat (desc_set, Param_OVRSPL , Ovrspl_X1);

	_state_set.add_observer (Param_SUS    , _param_change_flag_core);
	_state_set.add_observer (Param_TONE   , _param_change_flag_core);
	_state_set.add_observer (Param_VOL    , _param_change_flag_core);
	_state_set.add_observer (Param_PREGAIN, _param_change_flag_misc);
	_state_set.add_observer (Param_OVRSPL , _param_change_flag_misc);

	_param_change_flag_core.add_observer (_param_change_flag);
	_param_change_flag_misc.add_observer (_param_change_flag);

	_state_set.set_ramp_time (Param_TONE   , 0.010);
	_state_set.set_ramp_time (Param_PREGAIN, 0.010);

	for (auto &chn : _chn_arr)
	{
		chn._uspl.set_coefs (&_coef_42 [0], &_coef_21 [0]);
		chn._dspl.set_coefs (&_coef_42 [0], &_coef_21 [0]);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	BigMuff1::do_get_state () const
{
	return _state;
}



double	BigMuff1::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	fstb::unused (categ, note_id);
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	BigMuff1::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	const int      mbs_align = (max_buf_len + 3) & ~3;
	_buf_tmp.resize (mbs_align);
	_buf_ovr.resize (mbs_align * _ovrspl_ratio_max);

	update_param (true);

	clear_buffers ();
	_param_proc.req_steady ();

	// Switching the device on makes a loud plop, the time for the capacitors
	// to charge. So we process a few empty samples to skip the plop.
	/*** To do:
		make a DC analysis and directly fill the energy-storage components with
		the result.
	***/
	const int      plop_len = fstb::round_int (_sample_freq * 0.2f);
	for (auto &chn : _chn_arr)
	{
		for (int k = 0; k < plop_len; ++k)
		{
			chn._bmp.process_sample (0);
		}
	}

	_state = State_ACTIVE;

	latency = 0;

	return piapi::Err_OK;
}



void	BigMuff1::do_process_block (piapi::ProcInfo &proc)
{
	const int      nbr_chn_src = proc._dir_arr [piapi::Dir_IN ]._nbr_chn;
	const int      nbr_chn_dst = proc._dir_arr [piapi::Dir_OUT]._nbr_chn;
	assert (nbr_chn_src <= nbr_chn_dst);
	const int      nbr_chn_proc = std::min (nbr_chn_src, nbr_chn_dst);

	// Events
	_param_proc.handle_msg (proc);

	// Parameters
	_state_set.process_block (proc._nbr_spl);
	update_param ();
	if (_param_proc.is_full_reset ())
	{
		clear_buffers ();
	}
	else if (_param_proc.is_req_steady_state ())
	{
		_gain_old = _gain_cur;
	}

	// Signal processing
	const int      nbr_spl     = proc._nbr_spl;
	const int      nbr_spl_ovr = nbr_spl * _ovrspl_rate;
	for (int chn_index = 0; chn_index < nbr_chn_proc; ++chn_index)
	{
		Channel &      chn = _chn_arr [chn_index];

		// Gain, pre
		const float *  src_ptr = proc._src_arr [chn_index];
		float *        dst_ptr = _buf_tmp.data ();
		dsp::mix::Align::copy_1_1_vlrauto (
			dst_ptr, src_ptr, proc._nbr_spl, _gain_old, _gain_cur
		);
		src_ptr = dst_ptr;

		// Upsampling
		dst_ptr = proc._dst_arr [chn_index];
		if (_ovrspl == Ovrspl_X2)
		{
			dst_ptr = _buf_ovr.data ();
			chn._uspl.process_block_2x (dst_ptr, src_ptr, nbr_spl);
			src_ptr = dst_ptr;
		}
		else if (_ovrspl == Ovrspl_X4)
		{
			dst_ptr = _buf_ovr.data ();
			chn._uspl.process_block (dst_ptr, src_ptr, nbr_spl);
			src_ptr = dst_ptr;
		}

		// Main processing
		chn._bmp.process_block (dst_ptr, src_ptr, nbr_spl_ovr);

		// Downsampling
		dst_ptr = proc._dst_arr [chn_index];
		if (_ovrspl == Ovrspl_X2)
		{
			chn._dspl.process_block_2x (dst_ptr, src_ptr, nbr_spl);
		}
		else if (_ovrspl == Ovrspl_X4)
		{
			chn._dspl.process_block (dst_ptr, src_ptr, nbr_spl);
		}

		// Gain, post
		if (_sig_scale != 1)
		{
			dsp::mix::Align::scale_1_v (dst_ptr, nbr_spl, 1.f / _sig_scale);
		}
	}

	_gain_old = _gain_cur;

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



void	BigMuff1::clear_buffers ()
{
	for (auto &chn : _chn_arr)
	{
		chn._bmp.clear_buffers ();
		chn._uspl.clear_buffers ();
		chn._dspl.clear_buffers ();
	}
	_gain_old = _gain_cur;
}



void	BigMuff1::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		if (_param_change_flag_misc (true) || force_flag)
		{
			_ovrspl = Ovrspl (fstb::round_int (
				_state_set.get_val_tgt_nat (Param_OVRSPL)
			));
			update_oversampling ();

			_gain_cur =
				_sig_scale * float (_state_set.get_val_end_nat (Param_PREGAIN));
		}

		if (_param_change_flag_core (true) || force_flag)
		{
			const float    sus  = float (_state_set.get_val_end_nat (Param_SUS ));
			const float    tone = float (_state_set.get_val_end_nat (Param_TONE));
			const float    vol  = float (_state_set.get_val_end_nat (Param_VOL ));

			// Converts the real gain to a pot position
			/*** To do: get rid of hard-coded constants ***/
			const float    sus_fix = sus * (151.f / 150.f) - 1.f / 150.f;

			for (auto &chn : _chn_arr)
			{
				chn._bmp.set_pot (dsp::va::BigMuffPi::Pot_SUS , sus_fix);
				chn._bmp.set_pot (dsp::va::BigMuffPi::Pot_TONE, tone);
				chn._bmp.set_pot (dsp::va::BigMuffPi::Pot_VOL , vol);
			}
		}
	}
}



void	BigMuff1::update_oversampling ()
{
	_ovrspl_rate = 1 << int (_ovrspl);
	for (auto &chn : _chn_arr)
	{
		chn._bmp.set_sample_freq (_sample_freq * _ovrspl_rate);
	}
}



}  // namespace bmp1
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
