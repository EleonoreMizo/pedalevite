/*****************************************************************************

        MidSide.cpp
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
#include "mfx/pi/ms/MidSide.h"
#include "mfx/pi/ms/Param.h"
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
namespace ms
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



MidSide::MidSide (piapi::HostInterface &host)
:	_host (host)
,	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_param_proc (_state_set)
,	_param_change_flag ()
,	_conf_cur ()
,	_conf_old ()
{
	dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_OP      , Op_NONE);
	_state_set.set_val_nat (desc_set, Param_CHN_SWAP, 0);
	_state_set.set_val_nat (desc_set, Param_POL_L   , 0);
	_state_set.set_val_nat (desc_set, Param_POL_R   , 0);

	_state_set.add_observer (Param_OP      , _param_change_flag);
	_state_set.add_observer (Param_CHN_SWAP, _param_change_flag);
	_state_set.add_observer (Param_POL_L   , _param_change_flag);
	_state_set.add_observer (Param_POL_R   , _param_change_flag);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	MidSide::do_get_state () const
{
	return _state;
}



double	MidSide::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	fstb::unused (categ, note_id);
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	MidSide::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	fstb::unused (max_buf_len);
	latency = 0;

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	_param_change_flag.set ();

	update_param (true);
	_param_proc.req_steady ();

	clear_buffers ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	MidSide::do_process_block (piapi::ProcInfo &proc)
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

	// Signal processing
	const int      nbr_spl   = proc._nbr_spl;
	const bool     mono_flag = (nbr_chn_src != 2);

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
		compute_matrix (lvl_beg, _conf_old, mono_flag);

		dsp::StereoLevel  lvl_end;
		compute_matrix (lvl_end, _conf_cur, mono_flag);

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

	_conf_old = _conf_cur;
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	MidSide::clear_buffers ()
{
	_conf_old = _conf_cur;
}



void	MidSide::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		_conf_cur._op        = _state_set.get_val_enum <Op> (Param_OP);
		_conf_cur._swap_flag = bool (_state_set.get_val_int (Param_CHN_SWAP));
		_conf_cur._invl_flag = bool (_state_set.get_val_int (Param_POL_L));
		_conf_cur._invr_flag = bool (_state_set.get_val_int (Param_POL_R));
	}
}



/*
Single channel input assumes R = L in stereo mode, and S = 0 in mid/side mode.
Channel swapping happens after polarity, on the stereo side

M = L + R
S = L - R

L = (M + S) / 2
R = (M - S) / 2
*/

void	MidSide::compute_matrix (dsp::StereoLevel &mat, const Conf &conf, bool mono_flag)
{
	const float       pol_l = (conf._invl_flag) ? -1.f : 1.f;
	const float       pol_r = (conf._invr_flag) ? -1.f : 1.f;

	switch (conf._op)
	{
	case Op_NONE:
		if (mono_flag)
		{
			mat.set (pol_l, pol_r, 0, 0);
		}
		else
		{
			mat.set (
				(conf._swap_flag) ?     0 : pol_l,
				(conf._swap_flag) ? pol_l :     0,
				(conf._swap_flag) ? pol_r :     0,
				(conf._swap_flag) ?     0 : pol_r
			);
		}
		break;

	case Op_ENCODE:
		if (mono_flag)
		{
			mat.set (
				pol_l + pol_r,
				(conf._swap_flag) ? pol_r - pol_l : pol_l - pol_r,
				0, 0
			);
		}
		else
		{
			mat.set (
				pol_l,
				(conf._swap_flag) ? -pol_l :  pol_l,
				pol_r,
				(conf._swap_flag) ?  pol_r : -pol_r
			);
		}
		break;

	case Op_DECODE:
		if (mono_flag)
		{
			mat.set (pol_l * 0.5f, pol_r * 0.5f, 0, 0);
		}
		else
		{
			mat.set (
				pol_l * 0.5f,
				pol_r * 0.5f,
				pol_l * ((conf._swap_flag) ? -0.5f :  0.5f),
				pol_r * ((conf._swap_flag) ?  0.5f : -0.5f)
			);
		}
		break;

	default:
		assert (false);
		break;
	}
}



}  // namespace ms
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
