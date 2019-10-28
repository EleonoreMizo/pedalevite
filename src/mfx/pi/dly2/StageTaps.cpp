/*****************************************************************************

        StageTaps.cpp
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

#include "fstb/DataAlign.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/dsp/StereoLevel.h"
#include "mfx/pi/dly2/StageTaps.h"

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace pi
{
namespace dly2
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



StageTaps::StageTaps ()
:	_sample_freq (0)
,	_max_block_size (0)
,	_ramp_time (0)
,	_tmp_buf_ptr (0)
,	_buf_size (0)
,	_level_tap_input (1)
,	_level_predelay (1)
,	_level_dry (1)
,	_tap_arr ()
,	_delay_arr ()
,	_interp ()
,	_xfade_shape_normal ()
,	_xfade_shape_pitchshift ()
,	_min_dly_time (0)
{
	dsp::mix::Align::setup ();
}



// tmp_ptr must be aligned on 16-byte boundaries.
// It should contain 4 buffers
void	StageTaps::reset (double sample_freq, int max_block_size, float tmp_ptr [], int buf_size)
{
	assert (sample_freq > 0);
	assert (max_block_size > 0);
	assert (fstb::DataAlign <true>::check_ptr (tmp_ptr));
	assert (buf_size > 0);

	_sample_freq    = float (sample_freq);
	_max_block_size = max_block_size;
	_tmp_buf_ptr    = tmp_ptr;
	_buf_size       = buf_size;
	set_ramp_time (max_block_size);

	_xfade_shape_normal.set_duration (0.025f);
	_xfade_shape_normal.set_sample_freq (sample_freq);
	_xfade_shape_pitchshift.set_duration (0.125f);
	_xfade_shape_pitchshift.set_sample_freq (sample_freq);

	for (int tap_index = 0; tap_index <= Cst::_nbr_taps; ++tap_index)
	{
		Tap &          tap = _tap_arr [tap_index];

		for (int chn_index = 0; chn_index < _nbr_chn; ++chn_index)
		{
			Tap::Channel & chn = tap._chn_arr [chn_index];
			chn._reader.set_tmp_buf (use_buf (Buf_READER), buf_size);
			chn._reader.set_delay_line (_delay_arr [chn_index]);
			chn._reader.set_resampling_range (-4.0f, 4.0f);
			chn._reader.set_crossfade_normal (
				_xfade_shape_normal.get_len (),
				_xfade_shape_normal.use_shape ()
			);
			chn._reader.set_crossfade_pitchshift (
				_xfade_shape_pitchshift.get_len (),
				_xfade_shape_pitchshift.use_shape ()
			);
			assert (chn._reader.is_ready ());
			chn._eq.set_sample_freq (sample_freq);
		}
	}

	for (auto &td : _delay_arr)
	{
		td.set_interpolator (_interp);
		td.set_sample_freq (sample_freq, 0);

		// When the delay is set to the maximum, we need room to push first
		// the new data, then read the delayed data.
		const double   add_dly = max_block_size / sample_freq;
		td.set_max_delay_time (Cst::_max_delay / 1000.0 + add_dly);
	}

	// We add a microsecond because of possible rounding errors
	_min_dly_time = float (_delay_arr [0].get_min_delay_time () + 1e-6);

	clear_buffers ();
}



void	StageTaps::clear_buffers ()
{
	_level_tap_input.clear_buffers ();
	_level_predelay .clear_buffers ();
	_level_dry      .clear_buffers ();

	for (auto &tap : _tap_arr)
	{
		tap._gain.clear_buffers ();
		tap._pan.clear_buffers ();
		tap._vol.clear_buffers ();
		tap._spread.clear_buffers ();
		tap._delay_time.clear_buffers ();

		for (auto &chn : tap._chn_arr)
		{
			chn._reader.clear_buffers ();
			chn._eq.clear_buffers ();
		}
	}

	for (auto &dly : _delay_arr)
	{
		dly.clear_buffers ();
	}
}



void	StageTaps::set_ramp_time (int nbr_spl)
{
	assert (nbr_spl > 0);

	if (nbr_spl != _ramp_time)
	{
		_ramp_time = nbr_spl;
		const float    time_step = fstb::rcp_uint <float> (nbr_spl);

		_level_tap_input.set_time (nbr_spl, time_step);
		_level_predelay .set_time (nbr_spl, time_step);
		_level_dry      .set_time (nbr_spl, time_step);

		for (auto &tap : _tap_arr)
		{
			tap._gain.set_time (nbr_spl, time_step);
			tap._pan.set_time (nbr_spl, time_step);
			tap._vol.set_time (nbr_spl, time_step);
			tap._spread.set_time (nbr_spl, time_step);
			tap._delay_time.set_time (nbr_spl, time_step);
		}
	}
}



void	StageTaps::set_level_tap_input (float lvl)
{
	assert (_ramp_time > 0);
	assert (lvl >= 0);

	_level_tap_input.set_val (lvl);
}



void	StageTaps::set_level_predelay (float lvl)
{
	assert (_ramp_time > 0);
	assert (lvl >= 0);

	_level_predelay.set_val (lvl);
}



void	StageTaps::set_level_dry (float lvl)
{
	assert (_ramp_time > 0);
	assert (lvl >= 0);

	_level_dry.set_val (lvl);
}



void	StageTaps::set_tap_pan (int index, float pan)
{
	assert (_ramp_time > 0);
	assert (index >= 0);
	assert (index <= Cst::_nbr_taps);
	assert (pan >= -1);
	assert (pan <=  1);

	Tap &          tap = _tap_arr [index];
	tap._pan.set_val (pan);
}



void	StageTaps::set_tap_vol (int index, float vol)
{
	assert (_ramp_time > 0);
	assert (index >= 0);
	assert (index <= Cst::_nbr_taps);
	assert (vol >= 0);

	Tap &          tap = _tap_arr [index];
	tap._vol.set_val (vol);
}



void	StageTaps::set_tap_gain (int index, float gain)
{
	assert (_ramp_time > 0);
	assert (index >= 0);
	assert (index <= Cst::_nbr_taps);
	assert (gain >= 0);

	Tap &          tap = _tap_arr [index];
	tap._gain.set_val (gain);
}



void	StageTaps::set_tap_spread (int index, float spread)
{
	assert (_ramp_time > 0);
	assert (index >= 0);
	assert (index <= Cst::_nbr_taps);
	assert (spread >= -1);
	assert (spread <=  1);

	Tap &          tap = _tap_arr [index];
	tap._spread.set_val (spread);
}



void	StageTaps::set_tap_delay_time (int index, float dly)
{
	assert (_ramp_time > 0);
	assert (index >= 0);
	assert (index < Cst::_nbr_taps);
	assert (dly >= 0);

	Tap &          tap = _tap_arr [index];
	tap._delay_time.set_val (dly);

	const float    dly_actual = std::max (dly, _min_dly_time);
	for (int chn_idx = 0; chn_idx < _nbr_chn; ++chn_idx)
	{
		dsp::dly::DelayLineReaderPitch <float> &  reader =
			tap._chn_arr [chn_idx]._reader;
		reader.set_delay_time (dly_actual, _ramp_time);
	}
}



void	StageTaps::set_tap_pitch_rate (int index, float rate)
{
	assert (_ramp_time > 0);
	assert (index >= 0);
	assert (index < Cst::_nbr_taps);

	Tap &          tap = _tap_arr [index];
	for (int chn_idx = 0; chn_idx < _nbr_chn; ++chn_idx)
	{
		dsp::dly::DelayLineReaderPitch <float> &  reader =
			tap._chn_arr [chn_idx]._reader;
		reader.set_grain_pitch (rate);
	}
}



void	StageTaps::set_tap_freq_lo (int index, float f)
{
	assert (_ramp_time > 0);
	assert (index >= 0);
	assert (index < Cst::_nbr_taps);

	Tap &          tap = _tap_arr [index];
	for (auto &chn : tap._chn_arr)
	{
		chn._eq.set_freq_lo (f);
	}
}



void	StageTaps::set_tap_freq_hi (int index, float f)
{
	assert (_ramp_time > 0);
	assert (index >= 0);
	assert (index < Cst::_nbr_taps);

	Tap &          tap = _tap_arr [index];
	for (auto &chn : tap._chn_arr)
	{
		chn._eq.set_freq_hi (f);
	}
}



void	StageTaps::process_block (float * const line_ptr_arr [Cst::_nbr_lines], float * const dst_ptr_arr [_nbr_chn], const float * const src_ptr_arr [_nbr_chn], int nbr_spl, int nbr_chn_src, int nbr_chn_dst)
{
	assert (_sample_freq > 0);
	assert (line_ptr_arr != 0);
	assert (dst_ptr_arr != 0);
	assert (src_ptr_arr != 0);
	assert (nbr_spl > 0);
	assert (nbr_spl <= _max_block_size);
	assert (nbr_chn_src > 0);
	assert (nbr_chn_src <= _nbr_chn);
	assert (nbr_chn_dst > 0);
	assert (nbr_chn_dst <= _nbr_chn);

	// Tick parameters
	_level_tap_input.tick (nbr_spl);
	_level_predelay .tick (nbr_spl);
	_level_dry      .tick (nbr_spl);

	for (auto &tap : _tap_arr)
	{
		tap._gain.tick (nbr_spl);
		tap._pan.tick (nbr_spl);
		tap._vol.tick (nbr_spl);
		tap._spread.tick (nbr_spl);
		tap._delay_time.tick (nbr_spl);
	}

	// Inserts incoming data into the delay lines
	const float *  src_ptr = src_ptr_arr [0];
	for (int chn_idx = 0; chn_idx < _nbr_chn; ++chn_idx)
	{
		if (chn_idx < nbr_chn_src)
		{
			src_ptr = src_ptr_arr [chn_idx];
			const float    lvl_beg = _level_tap_input.get_beg ();
			const float    lvl_end = _level_tap_input.get_end ();
			if (! (   fstb::is_eq (lvl_beg, 1.0f, 1e-3f)
			       && fstb::is_eq (lvl_end, 1.0f, 1e-3f)))
			{
				float *         tmp_ptr = use_buf (Buf_RAW_L);
				dsp::mix::Align::copy_1_1_vlrauto (
					tmp_ptr, src_ptr, nbr_spl, lvl_beg, lvl_end
				);
				src_ptr = tmp_ptr;
			}
		}

		dsp::dly::DelayLine &	delay = _delay_arr [chn_idx];
		delay.push_block (src_ptr, nbr_spl);
	}

	// Reads the dry tap
	Tap &          tap_dry = _tap_arr [Cst::_nbr_taps];
	mix_tap (            // To the output (dry part)
		dst_ptr_arr [0],
		dst_ptr_arr [nbr_chn_dst - 1],
		src_ptr_arr [0],
		src_ptr_arr [nbr_chn_src - 1],
		nbr_spl,
		_level_dry,
		tap_dry._vol,
		tap_dry._pan,
		false,
		(nbr_chn_dst == 1)
	);
	mix_tap (            // To the feedback delay lines
		line_ptr_arr [0],
		line_ptr_arr [1],
		src_ptr_arr [0],
		src_ptr_arr [nbr_chn_src - 1],
		nbr_spl,
		_ramp_one,
		tap_dry._gain,
		tap_dry._spread,
		false,
		false
	);

	// Reads delayed taps
	for (int tap_idx = 0; tap_idx < Cst::_nbr_taps; ++tap_idx)
	{
		Tap &          tap = _tap_arr [tap_idx];

		// Stores raw data into the temporary buffers
		for (int chn_idx = 0; chn_idx < _nbr_chn; ++chn_idx)
		{
			dsp::dly::DelayLineReaderPitch <float> &  reader =
				tap._chn_arr [chn_idx]._reader;
			float *        tmp_ptr = use_buf (
				static_cast <Buf> (int (Buf_RAW_L) + chn_idx)
			);
			reader.read_data (tmp_ptr, nbr_spl, -nbr_spl);
		}

		// To the output
		mix_tap (
			use_buf (Buf_OUT_L),
			use_buf (Buf_OUT_R),
			use_buf (Buf_RAW_L),
			use_buf (Buf_RAW_R),
			nbr_spl,
			_level_predelay,
			tap._vol,
			tap._pan,
			false,
			(nbr_chn_dst == 1)
		);
		for (int chn_idx = 0; chn_idx < nbr_chn_dst; ++chn_idx)
		{
			float *        buf_ptr = use_buf (
				static_cast <Buf> (int (Buf_OUT_L) + chn_idx)
			);
			Eq &           eq = tap._chn_arr [chn_idx]._eq;
			if (eq.is_active ())
			{
				eq.process_block (buf_ptr, buf_ptr, nbr_spl);
			}
			dsp::mix::Align::mix_1_1 (dst_ptr_arr [chn_idx], buf_ptr, nbr_spl);
		}

		// To the feedback delay lines
		mix_tap (
			line_ptr_arr [0],
			line_ptr_arr [1],
			use_buf (Buf_RAW_L),
			use_buf (Buf_RAW_R),
			nbr_spl,
			_ramp_one,
			tap._gain,
			tap._spread,
			true,
			false
		);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



float *	StageTaps::use_buf (Buf index)
{
	assert (index >= 0);
	assert (index < Buf_NBR_ELT);

	return _tmp_buf_ptr + index * _buf_size;
}



void	StageTaps::mix_tap (float dst_0_ptr [], float dst_1_ptr [], const float src_0_ptr [], const float src_1_ptr [], int nbr_spl, const dsp::ctrl::Ramp &vol_1, const dsp::ctrl::Ramp &vol_2, const dsp::ctrl::Ramp &pan, bool mix_flag, bool mono_out_flag)
{
	const float    vol_beg = vol_1.get_beg () * vol_2.get_beg ();
	const float    vol_end = vol_1.get_end () * vol_2.get_end ();

	// Mono
	if (mono_out_flag)
	{
		if (mix_flag)
		{
			dsp::mix::Align::mix_2_1_vlrauto (
				dst_0_ptr,
				src_0_ptr,
				src_1_ptr,
				nbr_spl,
				vol_beg * 0.5f,
				vol_end * 0.5f
			);
		}
		else
		{
			dsp::mix::Align::copy_2_1_vlrauto (
				dst_0_ptr,
				src_0_ptr,
				src_1_ptr,
				nbr_spl,
				vol_beg * 0.5f,
				vol_end * 0.5f
			);
		}
	}

	// Stereo
	else
	{
		const float    pan_beg = pan.get_beg ();
		const float    pan_end = pan.get_end ();
		dsp::StereoLevel  sl_beg;
		dsp::StereoLevel  sl_end;
		vol_pan_to_lvl (sl_beg, pan_beg, vol_beg);
		vol_pan_to_lvl (sl_end, pan_end, vol_end);

		if (mix_flag)
		{
			dsp::mix::Align::mix_mat_2_2_vlrauto (
				dst_0_ptr, dst_1_ptr,
				src_0_ptr, src_1_ptr,
				nbr_spl,
				sl_beg, sl_end
			);
		}
		else
		{
			dsp::mix::Align::copy_mat_2_2_vlrauto (
				dst_0_ptr, dst_1_ptr,
				src_0_ptr, src_1_ptr,
				nbr_spl,
				sl_beg, sl_end
			);
		}
	}
}



void	StageTaps::vol_pan_to_lvl (dsp::StereoLevel &sl, float pan, float vol)
{
	sl.set (
		std::min (1.f - pan, 1.f) * vol,
		std::max (      pan, 0.f) * vol,
		std::max (    - pan, 0.f) * vol,
		std::min (1.f + pan, 1.f) * vol
	);
}



const dsp::ctrl::Ramp StageTaps::_ramp_one = dsp::ctrl::Ramp (1);



}  // namespace dly2
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
