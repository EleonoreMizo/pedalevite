/*****************************************************************************

        Tremolo.cpp
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

#include "fstb/Approx.h"
#include "fstb/def.h"
#include "fstb/ToolsSimd.h"
#include "mfx/pi/trem1/Param.h"
#include "mfx/pi/trem1/Tremolo.h"
#include "mfx/piapi/Dir.h"
#include "mfx/piapi/Err.h"
#include "mfx/piapi/EventParam.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/EventType.h"
#include "mfx/piapi/ProcInfo.h"
#include "mfx/dsp/mix/Align.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace trem1
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Tremolo::Tremolo (piapi::HostInterface &host)
:	_host (host)
,	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_param_proc (_state_set)
,	_sample_freq (0)
,	_param_change_shape_flag ()
,	_param_change_amp_flag ()
,	_lfo_pos (0)
,	_lfo_step (0)
,	_lfo_wf (Waveform_SIN)
,	_amt (0)
,	_bias (0)
,	_sat (0.5f)
{
	_state_set.init (piapi::ParamCateg_GLOBAL, _desc.use_desc_set ());

	_state_set.set_val (Param_FREQ, 0.45); // Previously: 0.21 -> ~130 ms
	_state_set.set_val (Param_AMT , 0.31); // Previously: 0.42 -> 0.31 %
	_state_set.set_val (Param_WF  , 0);
	_state_set.set_val (Param_GSAT, 0.75);
	_state_set.set_val (Param_BIAS, 0.5);

	_state_set.add_observer (Param_FREQ, _param_change_shape_flag);
	_state_set.add_observer (Param_WF  , _param_change_shape_flag);
	_state_set.add_observer (Param_AMT , _param_change_amp_flag);
	_state_set.add_observer (Param_GSAT, _param_change_amp_flag);
	_state_set.add_observer (Param_BIAS, _param_change_amp_flag);

	_state_set.set_ramp_time (Param_AMT , 0.010);
	_state_set.set_ramp_time (Param_GSAT, 0.010);
	_state_set.set_ramp_time (Param_BIAS, 0.010);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	Tremolo::do_get_state () const
{
	return _state;
}



double	Tremolo::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	fstb::unused (categ, note_id);
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	Tremolo::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	fstb::unused (max_buf_len);

	latency = 0;
	_sample_freq = float (sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();
	_param_change_shape_flag.set ();

	clear_buffers ();
	_param_proc.req_steady ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	Tremolo::do_process_block (piapi::ProcInfo &proc)
{
	// Events
	_param_proc.handle_msg (proc);

	// Parameters
	_state_set.process_block (proc._nbr_spl);

	float          amt_beg  = _amt;
	float          amt_end  = _amt;
	float          bias_beg = _bias;
	float          bias_end = _bias;
	float          sat_beg  = _sat;
	float          sat_end  = _sat;

	// Depth-related parameters
	if (_param_change_amp_flag (true))
	{
		amt_end  = float (_state_set.get_val_end_nat (Param_AMT));
		bias_end = float (_state_set.get_val_end_nat (Param_BIAS));
		sat_end  = float (_state_set.get_val_end_nat (Param_GSAT));

		_amt  = amt_end;
		_bias = bias_end;
		_sat  = sat_end;
	}

	// LFO
	if (_param_change_shape_flag (true))
	{
		_lfo_wf = Waveform (fstb::round_int (
			_state_set.get_val_tgt_nat (Param_WF)
		));

		const double      lfo_freq = _state_set.get_val_tgt_nat (Param_FREQ);
		_lfo_step = lfo_freq / _sample_freq;
	}

	if (_param_proc.is_full_reset ())
	{
		clear_buffers ();
	}

	const float    lfo_beg = get_lfo_val (float (_lfo_pos));
	_lfo_pos += _lfo_step * proc._nbr_spl;
	if (_lfo_pos >= 0.5f)
	{
		_lfo_pos -= std::round (_lfo_pos);
	}

	const float    lfo_end = get_lfo_val (float (_lfo_pos));

	// Computes the resulting volume
	const auto     v_amt  = fstb::Vf32::set_pair (amt_beg , amt_end );
	const auto     v_bias = fstb::Vf32::set_pair (bias_beg, bias_end);
	const auto     v_sat  = fstb::Vf32::set_pair (sat_beg , sat_end );
	const auto     v_lfo  = fstb::Vf32::set_pair (lfo_beg , lfo_end );
	const auto     one    = fstb::Vf32 (1);
	const auto     half   = fstb::Vf32 (0.5f);
	const auto     zero   = fstb::Vf32::zero ();
	const auto     v_satm = fstb::Vf32 (1e-4f);

	auto           vol    = one + v_amt * (v_lfo + v_bias);
	vol = fstb::max (vol, zero);

	// Saturation:
	// x = (1 - (1 - min (0.5 * s * x, 1)) ^ 2) / s
	const auto     v_satl = fstb::max (v_sat, v_satm);
	vol = one - fstb::min (vol * v_satl * half, one);
	vol = (one - vol * vol) / v_satl;

	const float    vol_beg = vol.template extract <0> ();
	const float    vol_end = vol.template extract <1> ();

	// Signal processing
	if (proc._dir_arr [piapi::Dir_OUT]._nbr_chn > 1)
	{	
		const int      nbr_chn_in = proc._dir_arr [piapi::Dir_IN ]._nbr_chn;
		dsp::mix::Align::copy_2_2_vlrauto (
			proc._dst_arr [0],
			proc._dst_arr [1],
			proc._src_arr [0],
			proc._src_arr [nbr_chn_in - 1],
			proc._nbr_spl,
			vol_beg, vol_end
		);
	}
	else
	{
		dsp::mix::Align::copy_1_1_vlrauto (
			proc._dst_arr [0],
			proc._src_arr [0],
			proc._nbr_spl,
			vol_beg, vol_end
		);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Tremolo::clear_buffers ()
{
	_lfo_pos = 0;
}



float	Tremolo::get_lfo_val (float pos) const
{
	float          val = 0;

	switch (_lfo_wf)
	{
	case Waveform_SIN:
		val = fstb::Approx::sin_nick_2pi (pos);
		break;
	case Waveform_SQUARE:
		val = (pos >= 0) ? 1.0f : -1.0f;
		break;
	case Waveform_TRI:
		val = 1 - 4 * fabsf (0.5f - fabsf (pos + 0.25f));
		break;
	case Waveform_RAMP_U:
		val = pos * 2;
		break;
	case Waveform_RAMP_D:
		val = pos * -2;
		break;
	default:
		assert (false);
		break;
	}

	return val;
}



}  // namespace trem1
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
