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
#include "fstb/ToolsSimd.h"
#include "mfx/pi/param/HelperDispNum.h"
#include "mfx/pi/param/MapPiecewiseLinLog.h"
#include "mfx/pi/param/TplEnum.h"
#include "mfx/pi/param/TplLin.h"
#include "mfx/pi/param/TplLog.h"
#include "mfx/pi/param/TplMapped.h"
#include "mfx/pi/Tremolo.h"
#include "mfx/piapi/EventParam.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/EventType.h"
#include "mfx/dsp/mix/Align.h"

#include <cassert>



namespace mfx
{
namespace pi
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Tremolo::Tremolo ()
:	_state (State_CONSTRUCTED)
,	_desc_set (Param_NBR_ELT, 0)
,	_state_set ()
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
	typedef param::TplMapped <param::MapPiecewiseLinLog> TplPll;

	// Period
	TplPll *   pll_ptr = new TplPll (
		0.02, 2,
		"Period\nPer",
		"ms",
		param::HelperDispNum::Preset_FLOAT_MILLI,
		0,
		"%6.1f"
	);
	pll_ptr->use_mapper ().set_first_value (  0.02);
	pll_ptr->use_mapper ().add_segment (0.75, 0.25, true);
	pll_ptr->use_mapper ().add_segment (1   , 2   , true);
	pll_ptr->set_categ (piapi::ParamDescInterface::Categ_TIME_S);
	_desc_set.add_glob (Param_PER, pll_ptr);

	// Amount
	pll_ptr = new TplPll (
		0, 10,
		"Amount\nAmt",
		"%",
		param::HelperDispNum::Preset_FLOAT_PERCENT,
		0,
		"%6.1f"
	);
	pll_ptr->use_mapper ().set_first_value (   0);
	pll_ptr->use_mapper ().add_segment (0.25,  0.25, false);
	pll_ptr->use_mapper ().add_segment (0.75,  1.5 , true);
	pll_ptr->use_mapper ().add_segment (1   , 10   , true);
	_desc_set.add_glob (Param_AMT, pll_ptr);

	// Waveform
	param::TplEnum *  enum_ptr = new param::TplEnum (
		"Sine\nSquare\nTriangle\nSaw Up\nSaw Down",
		"Waveform\nWF",
		""
	);
	assert (enum_ptr->get_nat_max () == Waveform_NBR_ELT - 1);
	_desc_set.add_glob (Param_WF, enum_ptr);

	// Gain Saturation
	param::TplLin *   lin_ptr = new param::TplLin (
		0, 1,
		"Gain Staturation\nGain Sat\nGSat",
		"%",
		0,
		"%5.1f"
	);
	_desc_set.add_glob (Param_GSAT, lin_ptr);

	// Bias
	lin_ptr = new param::TplLin (
		-1, 1,
		"Bias",
		"%",
		0,
		"%+6.1f"
	);
	lin_ptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_PERCENT);
	_desc_set.add_glob (Param_BIAS, lin_ptr);

	_state_set.init (piapi::ParamCateg_GLOBAL, _desc_set);

	_state_set.set_val (Param_PER , 0.55); // Previously: 0.21 -> ~130 ms
	_state_set.set_val (Param_AMT , 0.31); // Previously: 0.42 -> 0.31 %
	_state_set.set_val (Param_WF  , 0);
	_state_set.set_val (Param_GSAT, 0.75);
	_state_set.set_val (Param_BIAS, 0.5);

	_state_set.add_observer (Param_PER , _param_change_shape_flag);
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



int	Tremolo::do_init ()
{
	_state = State_INITIALISED;

	return Err_OK;
}



int	Tremolo::do_restore ()
{
	_state = State_CONSTRUCTED;

	return Err_OK;
}



void	Tremolo::do_get_nbr_io (int &nbr_i, int &nbr_o) const
{
	nbr_i = 1;
	nbr_o = 1;
}



bool	Tremolo::do_prefer_stereo () const
{
	return false;
}



int	Tremolo::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	Tremolo::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



double	Tremolo::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	Tremolo::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;
	_sample_freq = float (sample_freq);
	_lfo_pos = 0;

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();
	_param_change_shape_flag.set ();

	_state = State_ACTIVE;

	return Err_OK;
}



void	Tremolo::do_process_block (ProcInfo &proc)
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
	const float    lfo_beg = get_lfo_val (float (_lfo_pos));

	if (_param_change_shape_flag (true))
	{
		_lfo_wf = Waveform (fstb::round_int (
			_state_set.get_val_tgt_nat (Param_WF)
		));

		const double      lfo_per = _state_set.get_val_tgt_nat (Param_PER);
		_lfo_step = 1.0 / (lfo_per * _sample_freq);
	}

	_lfo_pos += _lfo_step * proc._nbr_spl;
	if (_lfo_pos >= 0.5f)
	{
		_lfo_pos -= std::round (_lfo_pos);
	}

	const float    lfo_end = get_lfo_val (float (_lfo_pos));

	// Computes the resulting volume
	const auto     v_amt  = fstb::ToolsSimd::set_2f32 (amt_beg , amt_end );
	const auto     v_bias = fstb::ToolsSimd::set_2f32 (bias_beg, bias_end);
	const auto     v_sat  = fstb::ToolsSimd::set_2f32 (sat_beg , sat_end );
	const auto     v_lfo  = fstb::ToolsSimd::set_2f32 (lfo_beg , lfo_end );
	const auto     one    = fstb::ToolsSimd::set1_f32 (1);
	const auto     half   = fstb::ToolsSimd::set1_f32 (0.5f);
	const auto     zero   = fstb::ToolsSimd::set_f32_zero ();

	auto           vol    = one + v_amt * (v_lfo + v_bias);
	vol = fstb::ToolsSimd::max_f32 (vol, zero);

	// Saturation:
	// x = (1 - (1 - min (0.5 * s * x, 1)) ^ 2) / s
	const auto     inv_s  = fstb::ToolsSimd::rcp_approx2 (v_sat);
	vol = one - fstb::ToolsSimd::min_f32 (vol * v_sat * half, one);
	vol = (one - vol * vol) * inv_s;

	const float    vol_beg = fstb::ToolsSimd::Shift <0>::extract (vol);
	const float    vol_end = fstb::ToolsSimd::Shift <1>::extract (vol);

	// Signal processing
	if (proc._nbr_chn_arr [piapi::PluginInterface::Dir_OUT] > 1)
	{	
		const int      nbr_chn_in =
			proc._nbr_chn_arr [piapi::PluginInterface::Dir_IN ];
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
		val = 1 - 4 * fabs (0.5f - fabs (pos + 0.25f));
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



}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
