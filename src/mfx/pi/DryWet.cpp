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

#include "fstb/fnc.h"
#include "mfx/pi/param/MapS.h"
#include "mfx/pi/param/TplEnum.h"
#include "mfx/pi/param/TplLog.h"
#include "mfx/pi/param/TplMapped.h"
#include "mfx/pi/DryWet.h"
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



DryWet::DryWet ()
:	_state (State_CONSTRUCTED)
,	_desc_set (Param_NBR_ELT, 0)
,	_state_set ()
,	_sample_freq (0)
,	_param_change_flag ()
,	_level_wet (1)
,	_level_dry (0)
{
	// Bypass
	param::TplEnum *  enum_ptr = new param::TplEnum (
		"Off\nOn",
		"Bypass",
		""
	);
	_desc_set.add_glob (Param_BYPASS, enum_ptr);

	// Dry/Wet
	param::TplMapped <param::MapS <false> > * maps_ptr =
		new param::TplMapped <param::MapS <false> > (
			0, 1,
			"Effect Mix\nFX Mix",
			"%",
			param::HelperDispNum::Preset_FLOAT_PERCENT,
			0,
			"%5.1f"
		);
	maps_ptr->use_mapper ().config (
		maps_ptr->get_nat_min (),
		maps_ptr->get_nat_max ()
	);
	_desc_set.add_glob (Param_WET, maps_ptr);

	// Volume
	param::TplLog *   log_ptr = new param::TplLog (
		1e-3, 10,
		"Volume\nVol",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	_desc_set.add_glob (Param_GAIN, log_ptr);

	_state_set.init (piapi::ParamCateg_GLOBAL, _desc_set);

	_state_set.set_val (Param_BYPASS, 0            ); // Off
	_state_set.set_val (Param_WET   , 1            ); // 100 %
	_state_set.set_val (Param_GAIN  , _gain_neutral); // 0 dB

	for (int index = 0; index < Param_NBR_ELT; ++index)
	{
		_state_set.add_observer (index, _param_change_flag);
	}

	_state_set.set_ramp_time (Param_BYPASS, 0.010);
	_state_set.set_ramp_time (Param_WET   , 0.010);
	_state_set.set_ramp_time (Param_GAIN  , 0.010);
}



const float	DryWet::_gain_neutral = 0.75f;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



DryWet::State	DryWet::do_get_state () const
{
	return _state;
}



int	DryWet::do_init ()
{
	_state = State_INITIALISED;

	return Err_OK;
}



int	DryWet::do_restore ()
{
	_state = State_CONSTRUCTED;

	return Err_OK;
}



void	DryWet::do_get_nbr_io (int &nbr_i, int &nbr_o) const
{
	nbr_i = 2;
	nbr_o = 1;
}



bool	DryWet::do_prefer_stereo () const
{
	return false;
}



int	DryWet::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	DryWet::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



double	DryWet::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	DryWet::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;
	_sample_freq = float (sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	_state = State_ACTIVE;

	return Err_OK;
}



void	DryWet::do_process_block (ProcInfo &proc)
{
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

	_state_set.process_block (proc._nbr_spl);

	float          lvl_wet_beg = _level_wet;
	float          lvl_wet_end = _level_wet;
	float          lvl_dry_beg = _level_dry;
	float          lvl_dry_end = _level_dry;

	bool           ramp_flag   = false;
	if (_param_change_flag (true))
	{
		const float    byp_end =
			float (_state_set.get_val_end_nat (Param_BYPASS));
		const float    vol_end  =
			float (_state_set.get_val_end_nat (Param_GAIN));
		const float    mix_end  =
			float (_state_set.get_val_end_nat (Param_WET));

		lvl_wet_end =      mix_end  * (1 - byp_end) * vol_end;
#if defined (mfx_pi_DryWet_GAIN_WET_ONLY)
		lvl_dry_end = (1 - mix_end) *      byp_end;
#else
		lvl_dry_end = (1 - mix_end) *      byp_end  * vol_end;
#endif

		_level_wet = lvl_wet_end;
		_level_dry = lvl_dry_end;

		ramp_flag = (lvl_wet_end != lvl_wet_beg || lvl_dry_end != lvl_dry_beg);
	}

	// Special cases
	if (! ramp_flag && lvl_dry_end == 0) // Pure wet
	{
		copy (proc, 0, lvl_wet_end);
	}
	else if (! ramp_flag && lvl_wet_end == 0) // Bypass or pure dry
	{
		copy (proc, 1, lvl_dry_end);
	}
	else // Generic case
	{
		mix (proc, lvl_wet_beg, lvl_wet_end, lvl_dry_beg, lvl_dry_end);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DryWet::copy (const ProcInfo &proc, int chn_ofs, float lvl)
{
	static const int  o_in  = 3;
	static const int  o_out = 1;

	const int            nbr_in  = proc._nbr_chn_arr [Dir_IN ];
	const int            nbr_out = proc._nbr_chn_arr [Dir_OUT];
	const int            vol     = fstb::is_eq (lvl, 1.0f, 1e-3f) ? 0 : 1;
	const float * const* src_arr = proc._src_arr + chn_ofs * nbr_in;
	float       * const* dst_arr = proc._dst_arr;
	const int            nbr_spl = proc._nbr_spl;

	switch ((nbr_in << o_in) + (nbr_out << o_out) + vol)
	{
	// Mono to mono
	case (1 << o_in) + (1 << o_out) + 0:
		dsp::mix::Align::copy_1_1 (
			dst_arr [0],
			src_arr [0],
			nbr_spl
		);
		break;

	case (1 << o_in) + (1 << o_out) + 1:
		dsp::mix::Align::copy_1_1_v (
			dst_arr [0],
			src_arr [0],
			nbr_spl,
			lvl
		);
		break;

	// Mono to stereo
	case (1 << o_in) + (2 << o_out) + 0:
		dsp::mix::Align::copy_1_2 (
			dst_arr [0], dst_arr [1],
			src_arr [0],
			nbr_spl
		);
		break;

	case (1 << o_in) + (2 << o_out) + 1:
		dsp::mix::Align::copy_1_2_v (
			dst_arr [0], dst_arr [1],
			src_arr [0],
			nbr_spl,
			lvl
		);
		break;

	// Stereo to stereo
	case (2 << o_in) + (2 << o_out) + 0:
		dsp::mix::Align::copy_2_2 (
			dst_arr [0], dst_arr [1],
			src_arr [0], src_arr [1],
			nbr_spl
		);
		break;

	case (2 << o_in) + (2 << o_out) + 1:
		dsp::mix::Align::copy_2_2_v (
			dst_arr [0], dst_arr [1],
			src_arr [0], src_arr [1],
			nbr_spl,
			lvl
		);
		break;

	default:
		assert (false);
		break;
	}
}



void	DryWet::mix (const ProcInfo &proc, float lvl_wet_beg, float lvl_wet_end, float lvl_dry_beg, float lvl_dry_end)
{
	static const int  o_in  = 2;
	static const int  o_out = 0;

	const int            nbr_in  = proc._nbr_chn_arr [Dir_IN ];
	const int            nbr_out = proc._nbr_chn_arr [Dir_OUT];
	const float * const* src_arr = proc._src_arr;
	float       * const* dst_arr = proc._dst_arr;
	const int            nbr_spl = proc._nbr_spl;

	
	switch ((nbr_in << o_in) + (nbr_out << o_out))
	{
	// Mono to mono
	case (1 << o_in) + (1 << o_out):
		dsp::mix::Align::copy_1_1_vlrauto (
			dst_arr [0],
			src_arr [0],
			nbr_spl,
			lvl_wet_beg, lvl_wet_end
		);
		dsp::mix::Align::mix_1_1_vlrauto (
			dst_arr [0],
			src_arr [1],
			nbr_spl,
			lvl_dry_beg, lvl_dry_end
		);
		break;

	// Mono to stereo
	case (1 << o_in) + (2 << o_out):
		dsp::mix::Align::copy_1_1_vlrauto (
			dst_arr [0],
			src_arr [0],
			nbr_spl,
			lvl_wet_beg, lvl_wet_end
		);
		dsp::mix::Align::mix_1_1_vlrauto (
			dst_arr [0],
			src_arr [1],
			nbr_spl,
			lvl_dry_beg, lvl_dry_end
		);
		dsp::mix::Align::copy_1_1 (dst_arr [1], dst_arr [0], nbr_spl);
		break;

	// Stereo to stereo
	case (2 << o_in) + (2 << o_out):
		dsp::mix::Align::copy_2_2_vlrauto (
			dst_arr [0], dst_arr [1],
			src_arr [0+0], src_arr [0+1],
			nbr_spl,
			lvl_wet_beg, lvl_wet_end
		);
		dsp::mix::Align::mix_2_2_vlrauto (
			dst_arr [0], dst_arr [1],
			src_arr [2+0], src_arr [2+1],
			nbr_spl,
			lvl_dry_beg, lvl_dry_end
		);
		break;

	default:
		assert (false);
		break;
	}
}



}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
