/*****************************************************************************

        DryWet.cpp
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
#include "mfx/pi/dwm/DryWet.h"
#include "mfx/pi/dwm/Param.h"
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
namespace dwm
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



DryWet::DryWet ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_param_proc (_state_set)
,	_sample_freq (0)
,	_param_change_flag ()
,	_pin_arr ()
,	_nbr_pins (1)
,	_dly_spl (0)
,	_level_wet (1)
,	_level_dry (0)
{
	_state_set.init (piapi::ParamCateg_GLOBAL, _desc.use_desc_set ());

	_state_set.set_val (Param_BYPASS, 0                        ); // Off
	_state_set.set_val (Param_WET   , 1                        ); // 100 %
	_state_set.set_val (Param_GAIN  , DryWetDesc::_gain_neutral); // 0 dB

	for (int index = 0; index < Param_NBR_ELT; ++index)
	{
		_state_set.add_observer (index, _param_change_flag);
	}

	_state_set.set_ramp_time (Param_BYPASS, 0.010);
	_state_set.set_ramp_time (Param_WET   , 0.010);
	_state_set.set_ramp_time (Param_GAIN  , 0.010);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



DryWet::State	DryWet::do_get_state () const
{
	return _state;
}



double	DryWet::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	fstb::unused (categ, note_id);
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	DryWet::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	fstb::unused (max_buf_len);

	latency = 0;
	_sample_freq = float (sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	for (auto &pin : _pin_arr)
	{
		for (auto &chn : pin)
		{
			chn._delay.setup (_max_dly_spl, max_buf_len);
		}
	}

	clear_buffers ();
	_param_proc.req_steady ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



// Input pins are interleaved (wet0/dry0/wet1/dry1...)
void	DryWet::do_process_block (piapi::ProcInfo &proc)
{
	_param_proc.handle_msg (proc);

	_state_set.process_block (proc._nbr_spl);

	if (_param_proc.is_full_reset () && _dly_spl > 0)
	{
		clear_dly_buf_quick ();
	}

	float          lvl_wet_beg = _level_wet;
	float          lvl_wet_end = _level_wet;
	float          lvl_dry_beg = _level_dry;
	float          lvl_dry_end = _level_dry;

	bool           ramp_flag   = false;
	if (_param_change_flag (true))
	{
		set_dw_param (lvl_dry_beg, lvl_wet_beg, false);
		set_dw_param (lvl_dry_end, lvl_wet_end, true);

		_level_wet = lvl_wet_end;
		_level_dry = lvl_dry_end;

		ramp_flag  = (lvl_wet_end != lvl_wet_beg || lvl_dry_end != lvl_dry_beg);
	}

	for (int pin_idx = 0; pin_idx < _nbr_pins; ++pin_idx)
	{
		// Special cases
		if (! ramp_flag && lvl_dry_end == 0) // Pure wet
		{
			copy (pin_idx, proc, 0, lvl_wet_end);
		}
		else if (! ramp_flag && lvl_wet_end == 0) // Bypass or pure dry
		{
			copy (pin_idx, proc, 1, lvl_dry_end);
		}
		else // Generic case
		{
			mix (pin_idx, proc, lvl_wet_beg, lvl_wet_end, lvl_dry_beg, lvl_dry_end);
		}
	}
}



void	DryWet::do_set_aux_param (int dly_spl, int pin_mult)
{
	const int      dly_old = _dly_spl;

	_dly_spl  = dly_spl;
	_nbr_pins = pin_mult;

	for (int pin_cnt = 0; pin_cnt < _nbr_pins; ++pin_cnt)
	{
		for (auto &chn : _pin_arr [pin_cnt])
		{
			chn._delay.set_delay (dly_spl);
		}
	}

	// Delay time has changed: we most likely switched to another effect, or
	// changed the settings in a way we cannot ensure signal continuity.
	// Therefore the best thing to do is to clean the delay buffers.
	if (dly_old != _dly_spl)
	{
		_param_proc.req_all ();
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DryWet::clear_buffers ()
{
	for (auto &pin : _pin_arr)
	{
		for (auto &chn : pin)
		{
			chn._delay.clear_buffers ();
		}
	}
}



void	DryWet::clear_dly_buf_quick ()
{
	for (auto &pin : _pin_arr)
	{
		for (auto &chn : pin)
		{
			chn._delay.clear_buffers_quick ();
		}
	}
}



void	DryWet::set_dw_param (float &dry, float &wet, bool end_flag) const
{
	float          byp;
	float          vol;
	float          mix;
	if (end_flag)
	{
		byp = float (_state_set.get_val_end_nat (Param_BYPASS));
		vol = float (_state_set.get_val_end_nat (Param_GAIN));
		mix = float (_state_set.get_val_end_nat (Param_WET));
	}
	else
	{
		byp = float (_state_set.get_val_beg_nat (Param_BYPASS));
		vol = float (_state_set.get_val_beg_nat (Param_GAIN));
		mix = float (_state_set.get_val_beg_nat (Param_WET));
	}

	const float    wet_mix  = mix * (1 - byp);

	wet =      wet_mix  * vol;
#if defined (mfx_pi_dwm_DryWet_GAIN_WET_ONLY)
	dry = (1 - wet_mix);
#else
	dry = (1 - wet_mix) * vol;
#endif
}



void	DryWet::copy (int pin_idx, const piapi::ProcInfo &proc, int chn_ofs, float lvl)
{
	static const int  o_out = 1;

	const int            nbr_in  = proc._dir_arr [piapi::Dir_IN ]._nbr_chn;
	const int            nbr_out = proc._dir_arr [piapi::Dir_OUT]._nbr_chn;
	assert (nbr_in == nbr_out); /*** To do: why? ***/
	const int            pin_ofs_src = pin_idx * nbr_in * 2;
	const int            pin_ofs_dst = pin_idx * nbr_out;
	const int            vol     = fstb::is_eq (lvl, 1.0f, 1e-3f) ? 0 : 1;
	const float * const* src_arr = proc._src_arr + pin_ofs_src + chn_ofs * nbr_in;
	float       * const* dst_arr = proc._dst_arr + pin_ofs_dst;
	const int            nbr_spl = proc._nbr_spl;

	if (_dly_spl > 0)
	{
		ChannelArray & chn_arr = _pin_arr [pin_idx];

		for (int chn_index = 0; chn_index < nbr_in; ++chn_index)
		{
			Channel &      chn = chn_arr [chn_index];

			chn._delay.process_block (
				dst_arr [chn_index],
				src_arr [chn_index],
				nbr_spl
			);
		}

		// Dry part, we use the delayed data as source
		if (chn_ofs == 1)
		{
			src_arr = dst_arr;
		}
	}

	switch ((nbr_out << o_out) + vol)
	{
	// Mono to mono
	case (1 << o_out) + 0:
		if (src_arr != dst_arr)
		{
			dsp::mix::Align::copy_1_1 (
				dst_arr [0],
				src_arr [0],
				nbr_spl
			);
		}
		break;

	case (1 << o_out) + 1:
		if (src_arr != dst_arr)
		{
			dsp::mix::Align::copy_1_1_v (
				dst_arr [0],
				src_arr [0],
				nbr_spl,
				lvl
			);
		}
		else
		{
			dsp::mix::Align::scale_1_v (
				dst_arr [0],
				nbr_spl,
				lvl
			);
		}
		break;

	// Stereo to stereo
	case (2 << o_out) + 0:
		if (src_arr != dst_arr)
		{
			dsp::mix::Align::copy_2_2 (
				dst_arr [0], dst_arr [1],
				src_arr [0], src_arr [1],
				nbr_spl
			);
		}
		break;

	case (2 << o_out) + 1:
		if (src_arr != dst_arr)
		{
			dsp::mix::Align::copy_2_2_v (
				dst_arr [0], dst_arr [1],
				src_arr [0], src_arr [1],
				nbr_spl,
				lvl
			);
		}
		else
		{
			dsp::mix::Align::scale_2_v (
				dst_arr [0], dst_arr [1],
				nbr_spl,
				lvl
			);
		}
		break;

	default:
		assert (false);
		break;
	}
}



void	DryWet::mix (int pin_idx, const piapi::ProcInfo &proc, float lvl_wet_beg, float lvl_wet_end, float lvl_dry_beg, float lvl_dry_end)
{
	static const int  o_in  = 2;
	static const int  o_out = 0;

	const int            nbr_in  = proc._dir_arr [piapi::Dir_IN ]._nbr_chn;
	const int            nbr_out = proc._dir_arr [piapi::Dir_OUT]._nbr_chn;
	const int            pin_ofs_src = pin_idx * nbr_in * 2;
	const int            pin_ofs_dst = pin_idx * nbr_out;
	const float * const* wet_arr = proc._src_arr + pin_ofs_src;
	const float * const* dry_arr = wet_arr + nbr_in;
	float       * const* dst_arr = proc._dst_arr + pin_ofs_dst;
	const int            nbr_spl = proc._nbr_spl;

	if (_dly_spl > 0)
	{
		ChannelArray & chn_arr = _pin_arr [pin_idx];

		for (int chn_index = 0; chn_index < nbr_in; ++chn_index)
		{
			Channel &      chn = chn_arr [chn_index];

			chn._delay.process_block (
				dst_arr [chn_index],
				dry_arr [chn_index],
				nbr_spl
			);
		}

		// Dry part, we use the delayed data as source
		dry_arr = dst_arr;
	}

	switch ((nbr_in << o_in) + (nbr_out << o_out))
	{
	// Mono to mono
	case (1 << o_in) + (1 << o_out):
		if (dst_arr != dry_arr)
		{
			dsp::mix::Align::copy_1_1_vlrauto (
				dst_arr [0],
				dry_arr [0],
				nbr_spl,
				lvl_dry_beg, lvl_dry_end
			);
		}
		else
		{
			dsp::mix::Align::scale_1_vlrauto (
				dst_arr [0],
				nbr_spl,
				lvl_dry_beg, lvl_dry_end
			);
		}
		dsp::mix::Align::mix_1_1_vlrauto (
			dst_arr [0],
			wet_arr [0],
			nbr_spl,
			lvl_wet_beg, lvl_wet_end
		);
		break;

	// Mono to stereo
	case (1 << o_in) + (2 << o_out):
		if (dst_arr != dry_arr)
		{
			dsp::mix::Align::copy_1_1_vlrauto (
				dst_arr [0],
				dry_arr [0],
				nbr_spl,
				lvl_dry_beg, lvl_dry_end
			);
		}
		else
		{
			dsp::mix::Align::scale_1_vlrauto (
				dst_arr [0],
				nbr_spl,
				lvl_dry_beg, lvl_dry_end
			);
		}
		dsp::mix::Align::mix_1_1_vlrauto (
			dst_arr [0],
			wet_arr [0],
			nbr_spl,
			lvl_wet_beg, lvl_wet_end
		);
		dsp::mix::Align::copy_1_1 (dst_arr [1], dst_arr [0], nbr_spl);
		break;

	// Stereo to stereo
	case (2 << o_in) + (2 << o_out):
		if (dst_arr != dry_arr)
		{
			dsp::mix::Align::copy_2_2_vlrauto (
				dst_arr [0], dst_arr [1],
				dry_arr [0], dry_arr [1],
				nbr_spl,
				lvl_dry_beg, lvl_dry_end
			);
		}
		else
		{
			dsp::mix::Align::scale_2_vlrauto (
				dst_arr [0], dst_arr [1],
				nbr_spl,
				lvl_dry_beg, lvl_dry_end
			);
		}
		dsp::mix::Align::mix_2_2_vlrauto (
			dst_arr [0], dst_arr [1],
			wet_arr [0], wet_arr [1],
			nbr_spl,
			lvl_wet_beg, lvl_wet_end
		);
		break;

	default:
		assert (false);
		break;
	}
}



}  // namespace dwm
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
