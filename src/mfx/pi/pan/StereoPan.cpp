/*****************************************************************************

        StereoPan.cpp
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
#include "mfx/dsp/StereoLevel.h"
#include "mfx/pi/pan/Param.h"
#include "mfx/pi/pan/StereoPan.h"
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
namespace pan
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



StereoPan::StereoPan ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_param_proc (_state_set)
,	_sample_freq (0)
,	_inv_fs (0)
,	_param_change_flag ()
,	_pos (0)
,	_pos_l (-1)
,	_pos_r (+1)
,	_law (0.5)
{
	dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_POS  ,  0);
	_state_set.set_val_nat (desc_set, Param_POS_L, -1);
	_state_set.set_val_nat (desc_set, Param_POS_R, +1);
	_state_set.set_val_nat (desc_set, Param_LAW  ,  0.5);

	_state_set.add_observer (Param_POS  , _param_change_flag);
	_state_set.add_observer (Param_POS_L, _param_change_flag);
	_state_set.add_observer (Param_POS_R, _param_change_flag);
	_state_set.add_observer (Param_LAW  , _param_change_flag);

	_state_set.set_ramp_time (Param_POS  , 0.010);
	_state_set.set_ramp_time (Param_POS_L, 0.010);
	_state_set.set_ramp_time (Param_POS_R, 0.010);
	_state_set.set_ramp_time (Param_LAW  , 0.010);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	StereoPan::do_get_state () const
{
	return _state;
}



double	StereoPan::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	fstb::unused (categ, note_id);
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	StereoPan::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	fstb::unused (max_buf_len);
	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	_param_change_flag.set ();

	update_param (true);
	_param_proc.req_steady ();

	clear_buffers ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	StereoPan::do_process_block (piapi::ProcInfo &proc)
{
	const int      nbr_chn_src = proc._dir_arr [piapi::Dir_IN ]._nbr_chn;
	const int      nbr_chn_dst = proc._dir_arr [piapi::Dir_OUT]._nbr_chn;
	assert (nbr_chn_src <= nbr_chn_dst);

	// Events
	_param_proc.handle_msg (proc);

	// Parameters
	_state_set.process_block (proc._nbr_spl);
	update_param ();
	if (_param_proc.is_full_reset ())
	{
		clear_buffers ();
	}

	const bool     mono_flag = (nbr_chn_src != 2);
	const int      nbr_spl   = proc._nbr_spl;
	_pos.tick (nbr_spl);
	_pos_l.tick (nbr_spl);
	_pos_r.tick (nbr_spl);
	_law.tick (nbr_spl);

	// Signal processing
	if (nbr_chn_dst != 2)
	{
		// Special case: mono to mono (bypass)
		dsp::mix::Align::copy_1_1 (
			proc._dst_arr [0], proc._src_arr [0], nbr_spl
		);
	}

	else
	{
		dsp::StereoLevel  lvl_beg;
		compute_matrix (
			lvl_beg,
			_pos.get_beg (),
			_pos_l.get_beg (),
			_pos_r.get_beg (),
			_law.get_beg (),
			mono_flag
		);

		dsp::StereoLevel  lvl_end;
		compute_matrix (
			lvl_end,
			_pos.get_end (),
			_pos_l.get_end (),
			_pos_r.get_end (),
			_law.get_end (),
			mono_flag
		);

		if (mono_flag)
		{
			dsp::mix::Align::copy_1_1_vlrauto (
				proc._dst_arr [0], proc._src_arr [0], nbr_spl,
				lvl_beg.get_l2l (), lvl_end.get_l2l ()
			);
			dsp::mix::Align::copy_1_1_vlrauto (
				proc._dst_arr [1], proc._src_arr [0], nbr_spl,
				lvl_beg.get_l2r (), lvl_end.get_l2r ()
			);
		}

		else
		{
			dsp::mix::Align::copy_mat_2_2_vlrauto (
				proc._dst_arr [0], proc._dst_arr [1],
				proc._src_arr [0], proc._src_arr [1],
				nbr_spl,
				lvl_beg, lvl_end
			);
		}
	}

	// Duplicates the remaining output channels
	for (int chn_index = 2; chn_index < nbr_chn_dst; ++chn_index)
	{
		dsp::mix::Align::copy_1_1 (
			proc._dst_arr [chn_index],
			proc._dst_arr [0],
			proc._nbr_spl
		);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	StereoPan::clear_buffers ()
{
	_pos.clear_buffers ();
	_pos_l.clear_buffers ();
	_pos_r.clear_buffers ();
	_law.clear_buffers ();
}



void	StereoPan::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		const float    pos   = float (_state_set.get_val_end_nat (Param_POS  ));
		const float    pos_l = float (_state_set.get_val_end_nat (Param_POS_L));
		const float    pos_r = float (_state_set.get_val_end_nat (Param_POS_R));
		const float    law   = float (_state_set.get_val_end_nat (Param_LAW  ));

		_pos.set_val (pos);
		_pos_l.set_val (pos_l);
		_pos_r.set_val (pos_r);
		_law.set_val (law);
	}
}



/*
First, each input channel is panned on the stereo field with pos_l and pos_r,
with [law] dB at the center (and 0 dB at full L or R).
Then the result is balanced with pos, compensated in order to get 0 dB at the
center this time.
In mono mode, only pos is taken into account.
These combinations should result in a neutral setting for pos = Center,
pos_l = L100% and pos_r = R100%
*/

void	StereoPan::compute_matrix (dsp::StereoLevel &mat, float pos, float pos_l, float pos_r, float law, bool mono_flag)
{
	const float    comp  = 1.f / law;
	const float    bal_l = compute_pan_l ( pos, law) * comp;
	const float    bal_r = compute_pan_l (-pos, law) * comp;

	if (mono_flag)
	{
		mat.set (bal_l, bal_r, 0, 0);
	}
	else
	{
		const float    dual_l2l = compute_pan_l ( pos_l, law);
		const float    dual_l2r = compute_pan_l (-pos_l, law);
		const float    dual_r2l = compute_pan_l ( pos_r, law);
		const float    dual_r2r = compute_pan_l (-pos_r, law);
		mat.set (
			dual_l2l * bal_l,
			dual_l2r * bal_r,
			dual_r2l * bal_l,
			dual_r2r * bal_r
		);
	}
}



// Result in [0 ; 1]
float	StereoPan::compute_pan_l (float pos, float law)
{
	assert (pos >= -1);
	assert (pos <= +1);
	assert (law >= 0.5f);
	assert (law <= float (fstb::SQRT2) * 0.5f);

	const float    c2  =  0.5f - law;
	const float    c1  = -0.5f;
	const float    c0  =         law;
	const float    lvl = (c2 * pos + c1) * pos + c0;

	return lvl;

}



}  // namespace pan
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
