/*****************************************************************************

        Delay2.cpp
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
#include "fstb/fnc.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/dsp/StereoLevel.h"
#include "mfx/pi/dly2/Param.h"
#include "mfx/pi/dly2/Delay2.h"
#include "mfx/pi/dly2/FilterType.h"
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
namespace dly2
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Delay2::Delay2 ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_param_proc (_state_set)
,	_sample_freq (0)
,	_inv_fs (0)
,	_param_change_flag_misc ()
,	_param_change_flag_duck ()
,	_param_change_flag ()
,	_tap_arr ()
,	_line_arr ()
,	_taps ()
,	_reverb ()
,	_env_duck ()
,	_buf_tmp_zone ()
,	_buf_tap_arr ()
,	_buf_line_arr ()
,	_buf_fdbk_arr ()
,	_buf_duck ()
,	_nbr_lines (1)
,	_xfdbk_cur (0)
,	_xfdbk_old (0)
,	_duck_time (0.100f)
,	_duck_sens (1)
,	_freeze_flag (false)
,	_duck_flag (false)
{
	dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_TAPS_GAIN_IN, 1);
	_state_set.set_val_nat (desc_set, Param_TAPS_VOL    , 1);
	_state_set.set_val_nat (desc_set, Param_DRY_VOL     , 1);
	_state_set.set_val_nat (desc_set, Param_DRY_SPREAD  , 0);
	_state_set.set_val_nat (desc_set, Param_FREEZE      , 0);
	_state_set.set_val_nat (desc_set, Param_NBR_LINES   , 1);
	_state_set.set_val_nat (desc_set, Param_X_FDBK      , 0);
	_state_set.set_val_nat (desc_set, Param_DUCK_SENS   , 1);
	_state_set.set_val_nat (desc_set, Param_DUCK_TIME   , 0.1);

	_state_set.add_observer (Param_TAPS_GAIN_IN, _param_change_flag_misc);
	_state_set.add_observer (Param_TAPS_VOL    , _param_change_flag_misc);
	_state_set.add_observer (Param_DRY_VOL     , _param_change_flag_misc);
	_state_set.add_observer (Param_DRY_SPREAD  , _param_change_flag_misc);
	_state_set.add_observer (Param_FREEZE      , _param_change_flag_misc);
	_state_set.add_observer (Param_NBR_LINES   , _param_change_flag_misc);
	_state_set.add_observer (Param_X_FDBK      , _param_change_flag_misc);
	_state_set.add_observer (Param_DUCK_SENS   , _param_change_flag_duck);
	_state_set.add_observer (Param_DUCK_TIME   , _param_change_flag_duck);

	_param_change_flag_misc.add_observer (_param_change_flag);
	_param_change_flag_duck.add_observer (_param_change_flag);

	// Taps
	for (int index = 0; index < Cst::_nbr_taps; ++index)
	{
		const int      base = Delay2Desc::get_tap_base (index);

		_state_set.set_val_nat (desc_set, base + ParamTap_GAIN_IN , 0);
		_state_set.set_val_nat (desc_set, base + ParamTap_SPREAD  , 0);
		_state_set.set_val_nat (desc_set, base + ParamTap_DLY_BASE, 0.125 * (index + 1));
		_state_set.set_val_nat (desc_set, base + ParamTap_DLY_REL , 1);
		_state_set.set_val_nat (desc_set, base + ParamTap_PITCH   , 0);
		_state_set.set_val_nat (desc_set, base + ParamTap_CUT_LO  , 16);
		_state_set.set_val_nat (desc_set, base + ParamTap_CUT_HI  , 20480);
		_state_set.set_val_nat (desc_set, base + ParamTap_PAN     , 0);

		InfoTap &      info = _tap_arr [index];

		_state_set.add_observer (base + ParamTap_GAIN_IN , info._param_change_flag_input);
		_state_set.add_observer (base + ParamTap_SPREAD  , info._param_change_flag_input);
		_state_set.add_observer (base + ParamTap_DLY_BASE, info._param_change_flag_delay);
		_state_set.add_observer (base + ParamTap_DLY_REL , info._param_change_flag_delay);
		_state_set.add_observer (base + ParamTap_PITCH   , info._param_change_flag_delay);
		_state_set.add_observer (base + ParamTap_CUT_LO  , info._param_change_flag_eq);
		_state_set.add_observer (base + ParamTap_CUT_HI  , info._param_change_flag_eq);
		_state_set.add_observer (base + ParamTap_PAN     , info._param_change_flag_mix);

		info._param_change_flag_input.add_observer (info._param_change_flag);
		info._param_change_flag_delay.add_observer (info._param_change_flag);
		info._param_change_flag_eq   .add_observer (info._param_change_flag);
		info._param_change_flag_mix  .add_observer (info._param_change_flag);

		info._param_change_flag.add_observer (_param_change_flag);

		_state_set.set_ramp_time (base + ParamTap_GAIN_IN , 0.010f);
		_state_set.set_ramp_time (base + ParamTap_SPREAD  , 0.010f);
		_state_set.set_ramp_time (base + ParamTap_PAN     , 0.010f);
	}

	// Lines
	for (int index = 0; index < Cst::_nbr_lines; ++index)
	{
		const int      base = Delay2Desc::get_line_base (index);

		InfoLine &     info = _line_arr [index];

		info._delay.init (index, desc_set, _state_set);

		_state_set.set_val_nat (desc_set, base + ParamLine_GAIN_IN    , 1);
		_state_set.set_val_nat (desc_set, base + ParamLine_DLY_BASE   , 0.375);
		_state_set.set_val_nat (desc_set, base + ParamLine_DLY_REL    , 1);
		_state_set.set_val_nat (desc_set, base + ParamLine_DLY_BBD_SPD, 1);
		_state_set.set_val_nat (desc_set, base + ParamLine_PITCH      , 0);
		_state_set.set_val_nat (desc_set, base + ParamLine_FDBK       , 0.5);
		_state_set.set_val_nat (desc_set, base + ParamLine_CUT_LO     , 16);
		_state_set.set_val_nat (desc_set, base + ParamLine_CUT_HI     , 20480);
		_state_set.set_val_nat (desc_set, base + ParamLine_PAN        , 0);
		_state_set.set_val_nat (desc_set, base + ParamLine_VOL        , 1);
		_state_set.set_val_nat (desc_set, base + ParamLine_DUCK_AMT   , 0);
		_state_set.set_val_nat (desc_set, base + ParamLine_FX_REV_MX  , 0);
		_state_set.set_val_nat (desc_set, base + ParamLine_FX_REV_DC  , 0.5);
		_state_set.set_val_nat (desc_set, base + ParamLine_FX_REV_DA  , 0.5);

		_state_set.add_observer (base + ParamLine_GAIN_IN    , info._param_change_flag_input);
		_state_set.add_observer (base + ParamLine_DLY_BASE   , info._param_change_flag_delay);
		_state_set.add_observer (base + ParamLine_DLY_REL    , info._param_change_flag_delay);
		_state_set.add_observer (base + ParamLine_DLY_BBD_SPD, info._param_change_flag_delay);
		_state_set.add_observer (base + ParamLine_PITCH      , info._param_change_flag_delay);
		_state_set.add_observer (base + ParamLine_FDBK       , info._param_change_flag_fdbk);
		_state_set.add_observer (base + ParamLine_CUT_LO     , info._param_change_flag_eq);
		_state_set.add_observer (base + ParamLine_CUT_HI     , info._param_change_flag_eq);
		_state_set.add_observer (base + ParamLine_PAN        , info._param_change_flag_mix);
		_state_set.add_observer (base + ParamLine_VOL        , info._param_change_flag_mix);
		_state_set.add_observer (base + ParamLine_DUCK_AMT   , info._param_change_flag_duck);
		_state_set.add_observer (base + ParamLine_FX_REV_MX  , info._param_change_flag_rev);
		_state_set.add_observer (base + ParamLine_FX_REV_DC  , info._param_change_flag_rev);
		_state_set.add_observer (base + ParamLine_FX_REV_DA  , info._param_change_flag_rev);

		info._param_change_flag_input .add_observer (info._param_change_flag);
		info._param_change_flag_delay .add_observer (info._param_change_flag);
		info._param_change_flag_fdbk  .add_observer (info._param_change_flag);
		info._param_change_flag_eq    .add_observer (info._param_change_flag);
		info._param_change_flag_mix   .add_observer (info._param_change_flag);
		info._param_change_flag_duck  .add_observer (info._param_change_flag);
		info._param_change_flag_rev   .add_observer (info._param_change_flag);

		info._param_change_flag.add_observer (_param_change_flag);

		_state_set.set_ramp_time (base + ParamLine_GAIN_IN    , 0.010f);
		_state_set.set_ramp_time (base + ParamLine_FDBK       , 0.010f);
		_state_set.set_ramp_time (base + ParamLine_PAN        , 0.010f);
		_state_set.set_ramp_time (base + ParamLine_VOL        , 0.010f);
		_state_set.set_ramp_time (base + ParamLine_DUCK_AMT   , 0.010f);

		info._rev_mix.set_val (0);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	Delay2::do_get_state () const
{
	return _state;
}



double	Delay2::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	fstb::unused (categ, note_id);
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	Delay2::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	const int      buf_len_align = (max_buf_len + 3) & ~3;
	const int      nbr_buf       = std::max (
		int (StageTaps::Buf_NBR_ELT),
		int (DelayLineBbdPitch::Buf_NBR_ELT)
	);
	_buf_tmp_zone.resize (buf_len_align * nbr_buf);
	for (auto &buf : _buf_line_arr)
	{
		buf.resize (buf_len_align);
	}
	for (auto &buf : _buf_tap_arr)
	{
		buf.resize (buf_len_align);
	}
	for (auto &buf : _buf_fdbk_arr)
	{
		buf.resize (buf_len_align);
	}
	_buf_duck.resize (buf_len_align);

	_param_change_flag_misc.set ();
	for (int index = 0; index < Cst::_nbr_taps; ++index)
	{
		InfoTap &      info = _tap_arr [index];
		info._param_change_flag_input.set ();
		info._param_change_flag_delay.set ();
		info._param_change_flag_eq   .set ();
		info._param_change_flag_mix  .set ();
	}
	_taps.reset (sample_freq, max_buf_len, &_buf_tmp_zone [0], buf_len_align);
	for (int index = 0; index < Cst::_nbr_lines; ++index)
	{
		InfoLine &     info = _line_arr [index];
		info._delay.reset (
			sample_freq, max_buf_len, &_buf_tmp_zone [0], buf_len_align
		);
		const int      ramp_len  = fstb::round_int (sample_freq * 0.010);
		const float    ramp_step = 1.0f / float (ramp_len);
		info._rev_mix.set_time (ramp_len, ramp_step);
		info._param_change_flag_input.set ();
		info._param_change_flag_delay.set ();
		info._param_change_flag_fdbk .set ();
		info._param_change_flag_eq   .set ();
		info._param_change_flag_mix  .set ();
		info._param_change_flag_rev  .set ();
	}
	_reverb.reset (sample_freq, max_buf_len);
	_env_duck.set_sample_freq (sample_freq);

	update_param (true);

	clear_buffers ();
	_param_proc.req_steady ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	Delay2::do_process_block (piapi::ProcInfo &proc)
{
	const int      nbr_chn_src = proc._dir_arr [piapi::Dir_IN ]._nbr_chn;
	const int      nbr_chn_dst = proc._dir_arr [piapi::Dir_OUT]._nbr_chn;
	assert (nbr_chn_src <= nbr_chn_dst);

	// Events
	_param_proc.handle_msg (proc);

	// Parameters
	_state_set.process_block (proc._nbr_spl);
	for (auto &info : _line_arr)
	{
		DelayLineBbdPitch &  line = info._delay;
		line.set_ramp_time (proc._nbr_spl);
	}
	update_param ();

	if (_param_proc.is_full_reset ())
	{

		/*** To do: something quicker ***/
		clear_buffers ();


	}
	if (_param_proc.is_req_steady_state ())
	{
		_xfdbk_old = _xfdbk_cur;
	}

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Signal processing

	// Ducking
	if (_duck_flag)
	{
		square_block (&_buf_duck [0], proc._src_arr, proc._nbr_spl, nbr_chn_src);
		min_block (&_buf_duck [0], proc._nbr_spl, _duck_sens * _duck_sens);
		_env_duck.process_block_raw (&_buf_duck [0], &_buf_duck [0], proc._nbr_spl);
		// The content is left squared. We'll take the sqrt() on the position we
		// really want and which are constrained by the delay lines.
	}

	// Taps
	std::array <float * const, Cst::_nbr_lines>  buf_tap_arr =
	{{
		&_buf_tap_arr [0] [0], &_buf_tap_arr [1] [0]
	}};
	_taps.process_block (
		&buf_tap_arr [0],
		proc._dst_arr,
		proc._src_arr,
		proc._nbr_spl,
		nbr_chn_src,
		nbr_chn_dst
	);

	// Reverb
	for (int line_index = 0; line_index < _nbr_lines; ++line_index)
	{
		InfoLine &     info = _line_arr [line_index];
		info._rev_mix.tick (proc._nbr_spl);

		const float    rmix_beg = info._rev_mix.get_beg ();
		const float    rmix_end = info._rev_mix.get_end ();
		if (rmix_beg + rmix_end >= 1e-6f)
		{
			_reverb.process_block (
				&_buf_tmp_zone [0],
				&_buf_tap_arr [line_index] [0],
				proc._nbr_spl,
				line_index
			);

			const float    dry_beg = 1 - rmix_beg;
			const float    dry_end = 1 - rmix_end;
			const float    wet_beg =     rmix_beg;
			const float    wet_end =     rmix_end;

			dsp::mix::Align::scale_1_vlrauto (
				&_buf_tap_arr [line_index] [0],
				proc._nbr_spl,
				dry_beg,
				dry_end
			);

			dsp::mix::Align::mix_1_1_vlrauto (
				&_buf_tap_arr [line_index] [0],
				&_buf_tmp_zone [0],
				proc._nbr_spl,
				wet_beg * cdsp::fv::FreeverbCore::_scalein,
				wet_end * cdsp::fv::FreeverbCore::_scalein
			);
		}
	}

	// Feedback delay lines
	float          xfdbk_beg = _xfdbk_old;
	float          xfdbk_stp =
		(_xfdbk_cur - _xfdbk_old) * fstb::rcp_uint <float> (proc._nbr_spl);
	int            block_pos = 0;
	do
	{
		// Computes the block length depending on the feedback requirements
		int            work_len = proc._nbr_spl - block_pos;
		for (int line_index = 0; line_index < _nbr_lines; ++line_index)
		{
			DelayLineBbdPitch &  line    = _line_arr [line_index]._delay;
			const int            max_len = line.start_and_compute_max_proc_len ();
			work_len = std::min (work_len, max_len);
		}

		// Read the content of the lines
		std::array <float, 2>   fdbk_beg_arr = {{ 0, 0 }};
		std::array <float, 2>   fdbk_end_arr = {{ 0, 0 }};
		for (int line_index = 0; line_index < _nbr_lines; ++line_index)
		{
			DelayLineBbdPitch &  line = _line_arr [line_index]._delay;
			line.read_line (
				&_buf_line_arr [line_index] [0],
				work_len,
				fdbk_beg_arr [line_index],
				fdbk_end_arr [line_index]
			);
		}

		// Feedback
		float          xfdbk_end = xfdbk_beg + xfdbk_stp * work_len;
		if (_nbr_lines == 1)
		{
			dsp::mix::Align::copy_1_1_vlrauto (
				&_buf_fdbk_arr [0] [0],
				&_buf_line_arr [0] [0],
				work_len,
				fdbk_beg_arr [0],
				fdbk_end_arr [0]
			);
		}
		else if (_nbr_lines >= 2)
		{
			const dsp::StereoLevel  lvl_beg (
				fdbk_beg_arr [0] * (1 - xfdbk_beg),
				fdbk_beg_arr [1] *      xfdbk_beg ,
				fdbk_beg_arr [0] *      xfdbk_beg ,
				fdbk_beg_arr [1] * (1 - xfdbk_beg)
			);
			const dsp::StereoLevel  lvl_end (
				fdbk_end_arr [0] * (1 - xfdbk_end),
				fdbk_end_arr [1] *      xfdbk_end ,
				fdbk_end_arr [0] *      xfdbk_end ,
				fdbk_end_arr [1] * (1 - xfdbk_end)
			);
			dsp::mix::Align::copy_mat_2_2_vlrauto (
				&_buf_fdbk_arr [0] [0],
				&_buf_fdbk_arr [1] [0],
				&_buf_line_arr [0] [0],
				&_buf_line_arr [1] [0],
				work_len,
				lvl_beg,
				lvl_end
			);
		}

		// Mix
		for (int line_index = 0; line_index < _nbr_lines; ++line_index)
		{
			InfoLine &     info = _line_arr [line_index];
			info._delay.finish_processing (
				proc._dst_arr,
				&_buf_line_arr [line_index] [0],
				&_buf_tap_arr [line_index] [block_pos],
				&_buf_fdbk_arr [line_index] [0],
				&_buf_duck [block_pos],
				block_pos,
				(nbr_chn_dst >= 2),
				true
			);
		}

		// Next sub-block
		xfdbk_beg  = xfdbk_end;
		block_pos += work_len;
	}
	while (block_pos < proc._nbr_spl);

	set_next_block ();
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Delay2::clear_buffers ()
{
	_taps.clear_buffers ();

	for (auto &info : _line_arr)
	{
		info._delay.clear_buffers ();
		info._rev_mix.clear_buffers ();
	}
	_reverb.clear_buffers ();
	_env_duck.clear_buffers ();

	update_duck_state ();
}



void	Delay2::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		if (_param_change_flag_misc (true) || force_flag)
		{
			_nbr_lines = fstb::round_int (
				_state_set.get_val_tgt_nat (Param_NBR_LINES)
			);
			_xfdbk_cur = float (_state_set.get_val_end_nat (Param_X_FDBK));

			const float    taps_gain_in =
				float (_state_set.get_val_end_nat (Param_TAPS_GAIN_IN));
			const float    taps_vol     =
				float (_state_set.get_val_end_nat (Param_TAPS_VOL));
			const float    dry_vol      =
				float (_state_set.get_val_end_nat (Param_DRY_VOL));
			const float    dry_spread   =
				float (_state_set.get_val_end_nat (Param_DRY_SPREAD));
			const bool     freeze_flag  =
				(float (_state_set.get_val_end_nat (Param_FREEZE)) >= 0.5f);

			if (freeze_flag != _freeze_flag)
			{
				_freeze_flag = freeze_flag;
				for (auto &info : _line_arr)
				{
					info._param_change_flag_fdbk.set ();
				}
			}
			_taps.set_level_tap_input ((_freeze_flag) ? 0 : taps_gain_in);
			_taps.set_tap_gain (Cst::_nbr_taps, (_freeze_flag) ? 0.0f : 1.0f);
			_taps.set_level_predelay (taps_vol);
			_taps.set_level_dry (dry_vol);
			_taps.set_tap_spread (Cst::_nbr_taps, dry_spread);
		}

		if (_param_change_flag_duck (true) || force_flag)
		{
			const float    at =
				float (_state_set.get_val_end_nat (Param_DUCK_TIME));
			const float    rt = at * 4;
			_env_duck.set_times (at, rt);

			const float    sensitivity =
				float (_state_set.get_val_end_nat (Param_DUCK_SENS));
			_duck_sens = sensitivity;
			for (auto &info : _line_arr)
			{
				info._delay.set_duck_sensitivity (sensitivity);
			}
		}

		// Taps
		for (int index = 0; index < Cst::_nbr_taps; ++index)
		{
			const int      base = Delay2Desc::get_tap_base (index);
			InfoTap &      info = _tap_arr [index];

			if (info._param_change_flag (true) || force_flag)
			{
				if (info._param_change_flag_input (true) || force_flag)
				{
					const float   gain =
						float (_state_set.get_val_end_nat (base + ParamTap_GAIN_IN));
					_taps.set_tap_gain (index, gain);
					_taps.set_tap_vol (index, gain);

					const float   spread =
						float (_state_set.get_val_end_nat (base + ParamTap_SPREAD));
					_taps.set_tap_spread (index, spread);
				}

				if (info._param_change_flag_delay (true) || force_flag)
				{
					const float    time_base =
						float (_state_set.get_val_tgt_nat (base + ParamTap_DLY_BASE));
					const float    time_mul  =
						float (_state_set.get_val_tgt_nat (base + ParamTap_DLY_REL));
					const float    delay_s   = std::min (
						time_base * time_mul,
						Cst::_max_delay * (1.0f / 1000)
					);
					_taps.set_tap_delay_time (index, delay_s);
					const float    pitch     =
						float (_state_set.get_val_tgt_nat (base + ParamTap_PITCH));
					const float    rate = fstb::Approx::exp2 (pitch);
					_taps.set_tap_pitch_rate (index, rate);
				}

				if (info._param_change_flag_eq (true) || force_flag)
				{
					_taps.set_tap_freq_lo (index, float (
						_state_set.get_val_end_nat (base + ParamTap_CUT_LO)
					));
					_taps.set_tap_freq_hi (index, float (
						_state_set.get_val_end_nat (base + ParamTap_CUT_HI)
					));
				}

				if (info._param_change_flag_mix (true) || force_flag)
				{
					const float   pan =
						float (_state_set.get_val_end_nat (base + ParamTap_PAN));
					_taps.set_tap_pan (index, pan);
				}
			}
		}

		// Lines
		for (int index = 0; index < Cst::_nbr_lines; ++index)
		{
			const int      base = Delay2Desc::get_line_base (index);
			InfoLine &     info = _line_arr [index];

			if (info._param_change_flag (true) || force_flag)
			{
				if (info._param_change_flag_input (true) || force_flag)
				{
					info._delay.set_input_gain (
						float (_state_set.get_val_end_nat (base + ParamLine_GAIN_IN))
					);
				}

				if (info._param_change_flag_delay (true) || force_flag)
				{
					const float    time_base =
						  float (_state_set.get_val_tgt_nat (base + ParamLine_DLY_BASE));
					const float    time_mul  =
						float (_state_set.get_val_tgt_nat (base + ParamLine_DLY_REL));
					const float    delay_s   = std::min (
						time_base * time_mul,
						Cst::_max_delay * (1.0f / 1000)
					);
					info._delay.set_delay_time (delay_s);

					info._delay.set_bbd_speed (
						float (_state_set.get_val_end_nat (base + ParamLine_DLY_BBD_SPD))
					);

					const float    pitch =
						float (_state_set.get_val_tgt_nat (base + ParamLine_PITCH));
					const float    ratio = fstb::Approx::exp2 (pitch);
					info._delay.set_grain_pitch (ratio);
				}

				if (info._param_change_flag_fdbk (true) || force_flag)
				{
					const float    fdbk =
						float (_state_set.get_val_end_nat (base + ParamLine_FDBK));
					info._delay.set_feedback (_freeze_flag ? 1.0f : fdbk);
				}

				if (info._param_change_flag_eq (true) || force_flag)
				{
					info._delay.set_freq_lo (float (
						_state_set.get_val_end_nat (base + ParamLine_CUT_LO)
					));
					info._delay.set_freq_hi (float (
						_state_set.get_val_end_nat (base + ParamLine_CUT_HI)
					));
				}

				if (info._param_change_flag_mix (true) || force_flag)
				{
					info._delay.set_vol (float (
						_state_set.get_val_end_nat (base + ParamLine_VOL)
					));
					info._delay.set_pan (float (
						_state_set.get_val_end_nat (base + ParamLine_PAN)
					));
				}

				if (info._param_change_flag_duck (true) || force_flag)
				{
					const float    amt =
						float (_state_set.get_val_end_nat (base + ParamLine_DUCK_AMT));
					info._delay.set_duck_amount (amt);
					info._duck_flag = (amt >= 1e-3f);

					update_duck_state ();
				}

				if (info._param_change_flag_rev (true) || force_flag)
				{
					const float    decay = float (
						_state_set.get_val_end_nat (base + ParamLine_FX_REV_DC)
					);
					const float    damp  = float (
						_state_set.get_val_end_nat (base + ParamLine_FX_REV_DA)
					);
					_reverb.set_reflectivity (decay, index);
					_reverb.set_damp (damp, index);
					info._rev_mix.set_val (float (
						_state_set.get_val_end_nat (base + ParamLine_FX_REV_MX)
					));
				}
			}
		}
	}
}



void	Delay2::set_next_block ()
{
	_xfdbk_old = _xfdbk_cur;
}



void	Delay2::update_duck_state ()
{
	_duck_flag = false;
	for (auto &info : _line_arr)
	{
		if (info._duck_flag)
		{
			_duck_flag = true;
			break;
		}
	}
}



void	Delay2::square_block (float dst_ptr [], const float * const src_ptr_arr [], int nbr_spl, int nbr_chn)
{
	static const float   not_zero = 1e-30f;	// -600 dB
	if (nbr_chn == 1)
	{
		dsp::mix::Align::sum_square_n_1 (
			dst_ptr, src_ptr_arr, nbr_spl, nbr_chn, not_zero
		);
	}
	else if (nbr_chn == 2)
	{
		dsp::mix::Align::sum_square_n_1_v (
			dst_ptr, src_ptr_arr, nbr_spl, nbr_chn, not_zero, 0.5f
		);
	}
	else
	{
		dsp::mix::Align::sum_square_n_1_v (
			dst_ptr, src_ptr_arr, nbr_spl, nbr_chn, not_zero, 1.0f / nbr_chn
		);
	}
}



// Buffer must be aligned in address and size
void	Delay2::min_block (float dst_ptr [], int nbr_spl, float val_max)
{
	const auto     vm = fstb::ToolsSimd::set1_f32 (val_max);
	for (int pos = 0; pos < nbr_spl; pos += 4)
	{
		auto        x = fstb::ToolsSimd::load_f32 (dst_ptr + pos);
		x = fstb::ToolsSimd::min_f32 (x, vm);
		fstb::ToolsSimd::store_f32 (dst_ptr + pos, x);
	}
}



}  // namespace dly2
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
