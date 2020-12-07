/*****************************************************************************

        ReverbDattorro.cpp
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

#include "fstb/Approx.h"
#include "fstb/fnc.h"
#include "fstb/DataAlign.h"
#include "fstb/ToolsSimd.h"
#include "mfx/dsp/iir/Svf2p.h"
#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/dsp/mix/Simd.h"
#include "mfx/dsp/spat/ReverbDattorro.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace spat
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	ReverbDattorro::set_sample_freq (double sample_freq)
{
	_sample_freq = float (      sample_freq);
	_inv_fs      = float (1.0 / sample_freq);
	_lfo_max_depth_spl = _lfo_max_depth_s * _sample_freq;
	_min_mod_dly_time  = 0.001f * _sample_freq;

	_state.setup (_max_blk_size, _max_blk_size);
	_state.set_delay (_max_blk_size);

	_freeze.set_sample_freq (sample_freq);
	_freeze.set_time (0.010f);

	static constexpr float  fs_ref = 29761.f;

	// Input all-pass delay times, seconds
	static constexpr std::array <float, _nbr_input_apd> dly_pre_arr
	{{
		142 / fs_ref,
		107 / fs_ref,
		379 / fs_ref,
		277 / fs_ref
	}};

	// Tank delay times, seconds
	static constexpr std::array <
		std::array <
			std::array <float, 2>, // AP delay, Standard delay
			_nbr_apd_per_chn
		>,
		_nbr_chn
	> dly_tnk_arr
	{{
		{{
			{  672 / fs_ref, 4453 / fs_ref },
			{ 1800 / fs_ref, 3720 / fs_ref }
		}},
		{{
			{  908 / fs_ref, 4217 / fs_ref },
			{ 2656 / fs_ref, 3163 / fs_ref }
		}}
	}};

	// Output delay tap times, seconds
	static constexpr std::array <
		std::array <float, _nbr_taps_out>,
		_nbr_chn
	> tap_dly_arr
	{{
		{
			 266 / fs_ref,
			2974 / fs_ref,
			1913 / fs_ref,
			1996 / fs_ref,
			1990 / fs_ref,
			 187 / fs_ref,
			1066 / fs_ref
		},
		{
			 353 / fs_ref,
			3627 / fs_ref,
			1228 / fs_ref,
			2673 / fs_ref,
			2111 / fs_ref,
			 335 / fs_ref,
			 121 / fs_ref
		}
	}};

	constexpr float   margin_s   = _lfo_max_depth_s + _rnd_max_depth_s;
	const float       margin_spl = margin_s * _sample_freq;

	for (int chn_cnt = 0; chn_cnt < _nbr_chn; ++chn_cnt)
	{
		auto &         chn = _chn_arr [chn_cnt];

		for (int apd_cnt = 0; apd_cnt < _nbr_input_apd; ++apd_cnt)
		{
			const float    dly_spl = dly_pre_arr [apd_cnt] * _sample_freq;
			chn._input.set_apd_max_delay (apd_cnt, fstb::ceil_int (dly_spl));
			chn._input.set_apd_delay_flt (apd_cnt, dly_spl);
		}

		chn._dly_1_nosz = dly_tnk_arr [chn_cnt] [0] [1] * _sample_freq;
		chn._dly_2_nosz = dly_tnk_arr [chn_cnt] [1] [1] * _sample_freq;

		for (int lfo_cnt = 0; lfo_cnt < _nbr_apd_per_chn; ++lfo_cnt)
		{
			auto &         mds = chn._lfo_arr [lfo_cnt];
			mds._dly_nosz = dly_tnk_arr [chn_cnt] [lfo_cnt] [0] * _sample_freq;
			mds._lfo_val  = 0;
			mds._dly_max  =
				fstb::ceil_int (mds._dly_nosz * _max_room_size + margin_spl);
			mds._rnd_per  = fstb::round_int (_lfo_per_base * _sample_freq);
			mds._rnd_pos  = 0;
			mds._rnd_step = 0;
			mds._rnd_val  = 0;
		}

		chn._tank_1.set_delay_max (
			fstb::ceil_int (chn._dly_1_nosz * _max_room_size)
		);
		chn._tank_2.set_delay_max (
			fstb::ceil_int (chn._dly_2_nosz * _max_room_size)
		);
		chn._tank_1.set_apd_max_delay (0, chn._lfo_arr [0]._dly_max);
		chn._tank_2.set_apd_max_delay (0, chn._lfo_arr [1]._dly_max);

		for (int k = 0; k < _nbr_taps_out; ++k)
		{
			chn._tap_dly_arr [k] = fstb::round_int (
				tap_dly_arr [chn_cnt] [k] * _sample_freq
			);
		}
	}

	_rnd_depth = _rnd_max_depth_s * _sample_freq;

	update_diffusion_input ();
	update_diffusion_tank ();
	update_delay_times ();
	compute_update_filter (
		_filt_spec_input, &ReverbDattorro::update_filter_input_coefs
	);
	compute_update_filter (
		_filt_spec_tank, &ReverbDattorro::update_filter_tank_coefs
	);
	clear_buffers ();

	// Makes sure the delays are initialized
	_chn_arr [0]._tank_1.set_apd_delay_flt (0, _chn_arr [0]._lfo_arr [0]._dly_nomod);
	_chn_arr [0]._tank_2.set_apd_delay_flt (0, _chn_arr [0]._lfo_arr [1]._dly_nomod);
	_chn_arr [1]._tank_1.set_apd_delay_flt (0, _chn_arr [1]._lfo_arr [0]._dly_nomod);
	_chn_arr [1]._tank_2.set_apd_delay_flt (0, _chn_arr [1]._lfo_arr [1]._dly_nomod);
}



void	ReverbDattorro::set_room_size (float sz)
{
	assert (sz >= _min_room_size);
	assert (sz <= _max_room_size);

	_room_size = sz;
	update_delay_times ();
}



void	ReverbDattorro::set_decay (float decay)
{
	assert (decay >= 0);
	assert (decay <= 1);

	_decay = decay;
}



// all_flag: indicates we want to spread the shift on all the all-pass delays
// and not just one of them.
void	ReverbDattorro::set_shimmer_pitch (float cents, bool all_flag)
{
	constexpr int  cents_per_octave = 12 * 100;
	assert (cents >= -1 * cents_per_octave);
	assert (cents <= +1 * cents_per_octave);

	if (all_flag)
	{
		// Divides the shift by the number of modulated delays. This helps
		// keeping the same apparent pitch shift independent of the method.
		constexpr auto div =
			  std::tuple_size <ChannelArray>::value
			* std::tuple_size <Channel::MdsArray>::value;
		cents *= 1.f / float (div);
	}

	// Converts the pitch in cents to a playback rate
	const float    p_oct  = cents * (1.f / float (cents_per_octave));
	const float    pb_spd = fstb::Approx::exp2 (p_oct);

	// Equivalent number of additional samples in an LFO period
	const float    spl_pp = (pb_spd - 1) * _sample_freq * _lfo_per_base;

	// Depth (multiplier of a bipolar value, hence the 0.5) in samples. The
	// value is added to the delay (positive is running backward), so we have
	// to flip the sign.
	_lfo_depth = spl_pp * -0.5f;

	const float    depth_abs = fabsf (_lfo_depth);
	if (depth_abs <= _lfo_max_depth_spl)
	{
		// Uses the default speed
		_lfo_speed = 1.f / _lfo_per_base;
	}
	else
	{
		// If the excursion is too large, increases the rate to compensate
		_lfo_speed = depth_abs / (_lfo_max_depth_spl * _lfo_per_base);
		_lfo_depth = std::copysign (_lfo_max_depth_spl, _lfo_depth);
	}

	// Computes the corresponding LFO step.
	// 2 because raw values go from -1 to +1.
	const float    lfo_step = 2.f * _lfo_speed * _inv_fs;

	// Sets the value according to all_flag
	const float    lfo_step_fill = (all_flag) ? lfo_step : 0.f;
	for (auto &chn : _chn_arr)
	{
		for (auto &mds : chn._lfo_arr)
		{
			mds._lfo_step = lfo_step_fill;
		}
	}

	// In single shifter mode, we use the 2nd all-pass delay of channel 1
	// because it has the longest base delay, and this will help preserving
	// the functionality with small room sizes.
	_chn_arr [1]._lfo_arr [1]._lfo_step = lfo_step;
}



void	ReverbDattorro::set_diffusion_input (float amount)
{
	assert (amount >= 0);
	assert (amount <= 1);

	_diffuse_in = amount;
	update_diffusion_input ();
}



void	ReverbDattorro::set_diffusion_tank (float amount)
{
	assert (amount >= 0);
	assert (amount <= 1);

	_diffuse_tnk = amount;
	update_diffusion_tank ();
}



// If lo <= hi, the filter is a band-pass
// if lo >  hi, the filter is a notch
void	ReverbDattorro::set_filter_input_bp (float lo, float hi)
{
	assert (lo >= 1);
	assert (lo < _sample_freq * 0.5f);
	assert (hi >= 1);
	assert (hi < _sample_freq * 0.5f);

	_filt_spec_input._f_lo = lo;
	_filt_spec_input._f_hi = hi;
	compute_update_filter (
		_filt_spec_input, &ReverbDattorro::update_filter_input_coefs
	);
}



// Trapezoidal-integrated SVF coefficients and mixers
// See iir::Svf2p to compute them from basic parameters
void	ReverbDattorro::set_filter_input_coefs (float g0, float g1, float g2, float v0m, float v1m, float v2m)
{
	_filt_spec_input._g0  = g0;
	_filt_spec_input._g1  = g1;
	_filt_spec_input._g2  = g2;
	_filt_spec_input._v0m = v0m;
	_filt_spec_input._v1m = v1m;
	_filt_spec_input._v2m = v2m;
	update_filter_input_coefs (g0, g1, g2, v0m, v1m, v2m);
}



void	ReverbDattorro::set_filter_tank_bp (float lo, float hi)
{
	assert (lo >= 1);
	assert (lo < _sample_freq * 0.5f);
	assert (hi >= 1);
	assert (hi < _sample_freq * 0.5f);

	_filt_spec_tank._f_lo = lo;
	_filt_spec_tank._f_hi = hi;
	compute_update_filter (
		_filt_spec_tank, &ReverbDattorro::update_filter_tank_coefs
	);
}



// Trapezoidal-integrated SVF coefficients and mixers
// See iir::Svf2p to compute them from basic parameters
// Avoid gain > 1 for any frequency band
// override_freeze_flag sets the filter even if the freeze mode is activated.
void	ReverbDattorro::set_filter_tank_coefs (float g0, float g1, float g2, float v0m, float v1m, float v2m, bool override_freeze_flag)
{
	_filt_spec_tank._g0  = g0;
	_filt_spec_tank._g1  = g1;
	_filt_spec_tank._g2  = g2;
	_filt_spec_tank._v0m = v0m;
	_filt_spec_tank._v1m = v1m;
	_filt_spec_tank._v2m = v2m;
	if (_freeze.get_val_tgt () < 0.5f || override_freeze_flag)
	{
		update_filter_tank_coefs (g0, g1, g2, v0m, v1m, v2m);
	}
}



// Tank filter is set to "neutral" during freeze. However it is possible
// to set the filter afterwards.
void	ReverbDattorro::freeze_tank (bool freeze_flag)
{
	if (freeze_flag)
	{
		_freeze.set_val (1.f);
		update_filter_tank_coefs (0, 0, 0, 1, 0, 0);
	}
	else
	{
		_freeze.set_val (0.f);
		update_filter_tank_coefs (
			_filt_spec_tank._g0, _filt_spec_tank._g1, _filt_spec_tank._g2,
			_filt_spec_tank._v0m, _filt_spec_tank._v1m, _filt_spec_tank._v2m
		);
	}
}



// For an unknown reason, all-pass delay filtering in the predelay part is
// extremly slow when put first (total time x6~x7). The slowness doesn't occur
// if this processing is done after the tank, and the result is the same,
// given the parameters stay constant (LTI system).
// The problem only appears on x86 architecture.
#define mfx_dsp_spat_ReverbDattorro_INVERSE_ORDER

std::pair <float, float>	ReverbDattorro::process_sample (float xl, float xr)
{
	assert (_sample_freq > 0);

	// LFO and modulation update
	const float    dly_apd_01 = process_modulation (_chn_arr [0]._lfo_arr [0]);
	const float    dly_apd_02 = process_modulation (_chn_arr [0]._lfo_arr [1]);
	const float    dly_apd_11 = process_modulation (_chn_arr [1]._lfo_arr [0]);
	const float    dly_apd_12 = process_modulation (_chn_arr [1]._lfo_arr [1]);
	_chn_arr [0]._tank_1.set_apd_delay_flt (0, dly_apd_01);
	_chn_arr [0]._tank_2.set_apd_delay_flt (0, dly_apd_02);
	_chn_arr [1]._tank_1.set_apd_delay_flt (0, dly_apd_11);
	_chn_arr [1]._tank_2.set_apd_delay_flt (0, dly_apd_12);

	// Audio processing
#if ! defined (mfx_dsp_spat_ReverbDattorro_INVERSE_ORDER)
	process_predelay (xl, xr);
#endif

	const float    decay   = (_freeze.get_val_tgt () < 0.5f) ? _decay : 1.f;
	const float    frz_val = _freeze.process_sample ();
	const float    vol_in  = 1 - frz_val;
	xl *= vol_in;
	xr *= vol_in;

	float          x = _state.read_at (_max_blk_size);

	x  = _chn_arr [0]._tank_1.process_sample (x + xr);
	x *= decay;
	x  = _chn_arr [0]._tank_2.process_sample (x);
	x *= decay;

	x  = _chn_arr [1]._tank_1.process_sample (x + xl);
	x *= decay;
	x  = _chn_arr [1]._tank_2.process_sample (x);
	x *= decay;

	_state.process_sample (x);

	// Output
	xl  = _chn_arr [1]._tank_1.read_delay ( _chn_arr [0]._tap_dly_arr [0]);
	xl += _chn_arr [1]._tank_1.read_delay ( _chn_arr [0]._tap_dly_arr [1]);
	xl -= _chn_arr [1]._tank_2.read_apd (0, _chn_arr [0]._tap_dly_arr [2]);
	xl += _chn_arr [1]._tank_2.read_delay ( _chn_arr [0]._tap_dly_arr [3]);
	xl -= _chn_arr [0]._tank_2.read_delay ( _chn_arr [0]._tap_dly_arr [4]);
	xl -= _chn_arr [0]._tank_2.read_apd (0, _chn_arr [0]._tap_dly_arr [5]);
	xl -= _chn_arr [0]._tank_1.read_delay ( _chn_arr [0]._tap_dly_arr [6]);

	xr  = _chn_arr [0]._tank_1.read_delay ( _chn_arr [1]._tap_dly_arr [0]);
	xr += _chn_arr [0]._tank_1.read_delay ( _chn_arr [1]._tap_dly_arr [1]);
	xr -= _chn_arr [0]._tank_2.read_apd (0, _chn_arr [1]._tap_dly_arr [2]);
	xr += _chn_arr [0]._tank_2.read_delay ( _chn_arr [1]._tap_dly_arr [3]);
	xr -= _chn_arr [1]._tank_2.read_delay ( _chn_arr [1]._tap_dly_arr [4]);
	xr -= _chn_arr [1]._tank_2.read_apd (0, _chn_arr [1]._tap_dly_arr [5]);
	xr -= _chn_arr [1]._tank_1.read_delay ( _chn_arr [1]._tap_dly_arr [6]);

#if defined (mfx_dsp_spat_ReverbDattorro_INVERSE_ORDER)
	process_predelay (xl, xr);
#endif

	return std::make_pair (xl, xr);
}



void	ReverbDattorro::process_block (float dst_l_ptr [], float dst_r_ptr [], const float src_l_ptr [], const float src_r_ptr [], int nbr_spl)
{
	assert (_sample_freq > 0);
	assert (dst_l_ptr != nullptr);
	assert (dst_r_ptr != nullptr);
	assert (src_l_ptr != nullptr);
	assert (src_r_ptr != nullptr);
	assert (nbr_spl > 0);

	alignas (16) std::array <int32_t, _max_blk_size>   dly_apd_01;
	alignas (16) std::array <int32_t, _max_blk_size>   dly_apd_02;
	alignas (16) std::array <int32_t, _max_blk_size>   dly_apd_11;
	alignas (16) std::array <int32_t, _max_blk_size>   dly_apd_12;
	alignas (16) std::array <float  , _max_blk_size>   buf_l;
	alignas (16) std::array <float  , _max_blk_size>   buf_r;
	alignas (16) std::array <float  , _max_blk_size>   buf_a;
	alignas (16) std::array <float  , _max_blk_size>   buf_b;

	const int32_t * const   dly_apd_01_ptr = dly_apd_01.data ();
	const int32_t * const   dly_apd_02_ptr = dly_apd_02.data ();
	const int32_t * const   dly_apd_11_ptr = dly_apd_11.data ();
	const int32_t * const   dly_apd_12_ptr = dly_apd_12.data ();

	using MixUs = mix::Simd <fstb::DataAlign <true >, fstb::DataAlign <false> >;
	using MixUd = mix::Simd <fstb::DataAlign <false>, fstb::DataAlign <true > >;
	using MixA  = mix::Simd <fstb::DataAlign <true >, fstb::DataAlign <true > >;

	const float    decay  = (_freeze.get_val_tgt () < 0.5f) ? _decay : 1.f;
	float          vol_in = 1 - _freeze.get_val_cur ();

	int            pos_blk = 0;
	do
	{
		int            work_len   = std::min (nbr_spl - pos_blk, _max_blk_size);
		work_len = std::min (work_len, _chn_arr [0]._tank_1.compute_max_block_len ());
		work_len = std::min (work_len, _chn_arr [0]._tank_2.compute_max_block_len ());
		work_len = std::min (work_len, _chn_arr [1]._tank_1.compute_max_block_len ());
		work_len = std::min (work_len, _chn_arr [1]._tank_2.compute_max_block_len ());

		const float    vol_in_end = 1 - _freeze.skip_block (work_len);

		process_modulation_block (dly_apd_01.data (), _chn_arr [0]._lfo_arr [0], work_len);
		process_modulation_block (dly_apd_02.data (), _chn_arr [0]._lfo_arr [1], work_len);
		process_modulation_block (dly_apd_11.data (), _chn_arr [1]._lfo_arr [0], work_len);
		process_modulation_block (dly_apd_12.data (), _chn_arr [1]._lfo_arr [1], work_len);

		const float *  src2_l_ptr = src_l_ptr + pos_blk;
		const float *  src2_r_ptr = src_r_ptr + pos_blk;
		float *        dst2_l_ptr = dst_l_ptr + pos_blk;
		float *        dst2_r_ptr = dst_r_ptr + pos_blk;

#if ! defined (mfx_dsp_spat_ReverbDattorro_INVERSE_ORDER)
		process_predelay_block (
			buf_l.data (), buf_r.data (), src2_l_ptr, src2_r_ptr, work_len
		);
		MixA::scale_2_vlrauto (
			buf_l.data (), buf_r.data (),
			work_len,
			vol_in, vol_in_end
		);
#else // mfx_dsp_spat_ReverbDattorro_INVERSE_ORDER
		MixUs::copy_2_2_vlrauto (
			buf_l.data (), buf_r.data (),
			src2_l_ptr, src2_r_ptr,
			work_len,
			vol_in, vol_in_end
		);
#endif // mfx_dsp_spat_ReverbDattorro_INVERSE_ORDER

		_state.read_block_at (buf_a.data (), _max_blk_size, work_len);

		MixA::mix_1_1 (buf_a.data (), buf_r.data (), work_len);
		_chn_arr [0]._tank_1.process_block_var_dly (
			buf_a.data (), buf_a.data (), &dly_apd_01_ptr, work_len
		);
		MixA::scale_1_v (buf_a.data (), work_len, decay);
		_chn_arr [0]._tank_2.process_block_var_dly (
			buf_a.data (), buf_a.data (), &dly_apd_02_ptr, work_len
		);
		MixA::scale_1_v (buf_a.data (), work_len, decay);

		MixA::mix_1_1 (buf_a.data (), buf_l.data (), work_len);
		_chn_arr [1]._tank_1.process_block_var_dly (
			buf_a.data (), buf_a.data (), &dly_apd_11_ptr, work_len
		);
		MixA::scale_1_v (buf_a.data (), work_len, decay);
		_chn_arr [1]._tank_2.process_block_var_dly (
			buf_a.data (), buf_a.data (), &dly_apd_12_ptr, work_len
		);
		MixA::scale_1_v (buf_a.data (), work_len, decay);

		_state.push_block (buf_a.data (), work_len);

		// Output, negative taps
		_chn_arr [1]._tank_2.read_apd_block (buf_l.data (), 0, _chn_arr [0]._tap_dly_arr [2], work_len);
		_chn_arr [0]._tank_2.read_apd_block (buf_r.data (), 0, _chn_arr [1]._tap_dly_arr [2], work_len);
		_chn_arr [0]._tank_2.read_delay_block (buf_a.data (), _chn_arr [0]._tap_dly_arr [4], work_len);
		_chn_arr [1]._tank_2.read_delay_block (buf_b.data (), _chn_arr [1]._tap_dly_arr [4], work_len);
		MixA::mix_2_2 (buf_l.data (), buf_r.data (), buf_a.data (), buf_b.data (), work_len);
		_chn_arr [0]._tank_2.read_apd_block (buf_a.data (), 0, _chn_arr [0]._tap_dly_arr [5], work_len);
		_chn_arr [1]._tank_2.read_apd_block (buf_b.data (), 0, _chn_arr [1]._tap_dly_arr [5], work_len);
		MixA::mix_2_2 (buf_l.data (), buf_r.data (), buf_a.data (), buf_b.data (), work_len);
		_chn_arr [0]._tank_1.read_delay_block (buf_a.data (), _chn_arr [0]._tap_dly_arr [6], work_len);
		_chn_arr [1]._tank_1.read_delay_block (buf_b.data (), _chn_arr [1]._tap_dly_arr [6], work_len);
		MixA::mix_2_2 (buf_l.data (), buf_r.data (), buf_a.data (), buf_b.data (), work_len);

		// Tap inversion
		MixUd::copy_2_2_v (dst2_l_ptr, dst2_r_ptr, buf_l.data (), buf_r.data (), work_len, -1);

		// Output, positive taps
		_chn_arr [1]._tank_1.read_delay_block (buf_l.data (), _chn_arr [0]._tap_dly_arr [0], work_len);
		_chn_arr [0]._tank_1.read_delay_block (buf_r.data (), _chn_arr [1]._tap_dly_arr [0], work_len);
		_chn_arr [1]._tank_1.read_delay_block (buf_a.data (), _chn_arr [0]._tap_dly_arr [1], work_len);
		_chn_arr [0]._tank_1.read_delay_block (buf_b.data (), _chn_arr [1]._tap_dly_arr [1], work_len);
		MixA::mix_2_2 (buf_l.data (), buf_r.data (), buf_a.data (), buf_b.data (), work_len);
		_chn_arr [1]._tank_2.read_delay_block (buf_a.data (), _chn_arr [0]._tap_dly_arr [3], work_len);
		_chn_arr [0]._tank_2.read_delay_block (buf_b.data (), _chn_arr [1]._tap_dly_arr [3], work_len);
		MixUd::mix_2_1 (dst2_l_ptr, buf_l.data (), buf_a.data (), work_len);
		MixUd::mix_2_1 (dst2_r_ptr, buf_r.data (), buf_b.data (), work_len);

#if defined (mfx_dsp_spat_ReverbDattorro_INVERSE_ORDER)
		process_predelay_block (
			dst2_l_ptr, dst2_r_ptr, dst2_l_ptr, dst2_r_ptr, work_len
		);
#endif

		pos_blk += work_len;
		vol_in   = vol_in_end;
	}
	while (pos_blk < nbr_spl);
}



#undef mfx_dsp_spat_ReverbDattorro_INVERSE_ORDER



void	ReverbDattorro::flush_tank ()
{
	for (auto &chn : _chn_arr)
	{
		chn._input.clear_buffers ();
		chn._tank_1.clear_buffers ();
		chn._tank_2.clear_buffers ();
	}
	_state.clear_buffers ();
}



void	ReverbDattorro::clear_buffers ()
{
	flush_tank ();
	reset_lfo ();
	_freeze.clear_buffers ();
	_rnd_gen.set_seed ();
}



constexpr int	ReverbDattorro::_nbr_chn;
constexpr float	ReverbDattorro::_min_room_size;
constexpr float	ReverbDattorro::_max_room_size;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	ReverbDattorro::update_diffusion_input ()
{
	const float    coef_1 = 0.7f * _diffuse_in;
	const float    coef_2 = 0.5f * _diffuse_in;
	for (auto &chn : _chn_arr)
	{
		chn._input.set_apd_coef (0, coef_1);
		chn._input.set_apd_coef (1, coef_1);
		chn._input.set_apd_coef (2, coef_2);
		chn._input.set_apd_coef (3, coef_2);
	}
}



void	ReverbDattorro::update_diffusion_tank ()
{
	const float    coef_1 = -0.750f * _diffuse_tnk;
	const float    coef_2 =  0.625f * _diffuse_tnk;
	for (auto &chn : _chn_arr)
	{
		chn._tank_1.set_apd_coef (0, coef_1);
		chn._tank_2.set_apd_coef (0, coef_2);
	}
}



void	ReverbDattorro::update_delay_times ()
{
	assert (_sample_freq > 0);

	for (int chn_cnt = 0; chn_cnt < _nbr_chn; ++chn_cnt)
	{
		auto &         chn = _chn_arr [chn_cnt];

		const int      d1  = fstb::round_int (chn._dly_1_nosz * _room_size);
		int            d2  = fstb::round_int (chn._dly_2_nosz * _room_size);
		if (chn_cnt == 1)
		{
			// Last delay of channel 1 is shortened to make room for _state.
			// The smallest room sizes make the delay negative, so we have to
			// fix it. However the induced error is not compensated anywhere.
			// Ideally, it would be easier to deal with this if all the delay
			// lines involved in the reverberator were sharing a single buffer
			// with multiple R/W cursors.
			d2 -= _max_blk_size;
			d2  = std::max (d2, 0);
		}
		chn._tank_1.set_delay (d1);
		chn._tank_2.set_delay (d2);
		chn._lfo_arr [0]._dly_nomod = chn._lfo_arr [0]._dly_nosz * _room_size;
		chn._lfo_arr [1]._dly_nomod = chn._lfo_arr [1]._dly_nosz * _room_size;
	}
}



void	ReverbDattorro::compute_update_filter (FilterSpec &spec, void (ReverbDattorro::*set_coefs) (float g0, float g1, float g2, float v0m, float v1m, float v2m))
{
	if (spec._f_hi > 0)
	{
		compute_filter_coef (spec);
		(this->*set_coefs) (
			spec._g0, spec._g1, spec._g2,
			spec._v0m, spec._v1m, spec._v2m
		);
	}
}



void	ReverbDattorro::compute_filter_coef (FilterSpec &spec) const
{
	assert (spec._f_lo > 0);
	assert (spec._f_lo < _sample_freq * 0.5f);
	assert (spec._f_hi > 0);
	assert (spec._f_hi < _sample_freq * 0.5f);

	// Frequency prewarping
	const double   lo_w =
		iir::TransSZBilin::prewarp_freq (spec._f_lo, _sample_freq);
	const double   hi_w =
		iir::TransSZBilin::prewarp_freq (spec._f_hi, _sample_freq);

	// Builds an analogue biquad using warped frequencies
	/*** To do: use _f_lo as reference freq, this should save operations ***/
	const double   f0_w   = sqrt (lo_w * hi_w);
	const double   t_lo   = f0_w / lo_w;
	const double   t_hi   = f0_w / hi_w;

	// High-pass part (low-cut)
	const double   bl [2] = { 0, t_lo };
	const double   al [2] = { 1, t_lo };

	// Low-pass part (high-cut)
	const double   bh [2] = { 1, 0    };
	const double   ah [2] = { 1, t_hi };

	// Convolve both into a biquad
	double         b [3];
	if (spec._f_lo <= spec._f_hi)
	{
		// Multiplication
		b [0] = bl [0] * bh [0];
		b [1] = bl [0] * bh [1] + bl [1] * bh [0];
		b [2] = bl [1] * bh [1];
	}
	else
	{
		// Sum
		b [0] = bl [0] * ah [0]                   + bh [0] * al [0];
		b [1] = bl [0] * ah [1] + bl [1] * ah [0] + bh [0] * al [1] + bh [1] * al [0];
		b [2] = bl [1] * ah [1]                   + bh [1] * al [1];
	};
	const double   a [3] =
	{
		al [0] * ah [0],
		al [0] * ah [1] + al [1] * ah [0],
		al [1] * ah [1]
	};

	// Inverse frequency warping
	const double   f0 = iir::TransSZBilin::unwarp_freq (f0_w, _sample_freq);

	// Conversion from analogue biquad to filter coefficients
	iir::Svf2p::conv_s_eq_to_svf (
		spec._g0, spec._g1, spec._g2,
		spec._v0m, spec._v1m, spec._v2m,
		b, a, f0, _sample_freq
	);
}



void	ReverbDattorro::update_filter_input_coefs (float g0, float g1, float g2, float v0m, float v1m, float v2m)
{
	for (auto &chn : _chn_arr)
	{
		chn._input.set_filter_coefs (g0, g1, g2);
		chn._input.set_filter_mix (v0m, v1m, v2m);
	}
}



void	ReverbDattorro::update_filter_tank_coefs (float g0, float g1, float g2, float v0m, float v1m, float v2m)
{
	for (auto &chn : _chn_arr)
	{
		chn._tank_1.set_filter_coefs (g0, g1, g2);
		chn._tank_1.set_filter_mix (v0m, v1m, v2m);
	}
}



void	ReverbDattorro::reset_lfo ()
{
	// Starts the LFOs with different phases
	for (int chn_cnt = 0; chn_cnt < _nbr_chn; ++chn_cnt)
	{
		auto &         chn = _chn_arr [chn_cnt];
		for (int lfo_cnt = 0; lfo_cnt < _nbr_apd_per_chn; ++lfo_cnt)
		{
			auto &         mds = chn._lfo_arr [lfo_cnt];
			const int      idx = chn_cnt * 2 + lfo_cnt;
			mds._lfo_val  = idx * (2.f / _nbr_lfo_tot) - 1.f;
			mds._rnd_pos  = idx * mds._rnd_per / _nbr_lfo_tot;
		}
	}
}



void	ReverbDattorro::process_predelay (float &xl, float &xr)
{
	xl = _chn_arr [0]._input.process_sample (xl);
	xr = _chn_arr [1]._input.process_sample (xr);
}



void	ReverbDattorro::process_predelay_block (float dst_l_ptr [], float dst_r_ptr [], const float src_l_ptr [], const float src_r_ptr [], int nbr_spl)
{
	_chn_arr [0]._input.process_block (dst_l_ptr, src_l_ptr, nbr_spl);
	_chn_arr [1]._input.process_block (dst_r_ptr, src_r_ptr, nbr_spl);
}



// Returns the modulated delay time, in samples
float	ReverbDattorro::process_modulation (ModDlyState &mds)
{
	mds._lfo_val += mds._lfo_step;
	mds._rnd_val += mds._rnd_step;
	++ mds._rnd_pos;
	check_mod_counters (mds);

	// Final delay value
	float          dly_mod =
		  mds._dly_nomod
		+ mds._lfo_val * _lfo_depth
		+ mds._rnd_val * _rnd_depth;
	dly_mod = fstb::limit (dly_mod, _min_mod_dly_time, float (mds._dly_max));

	return dly_mod;
}



// dly_ptr must be aligned on a 16-byte boundary, and the allocated zone must
// be a multiple of 16
void	ReverbDattorro::process_modulation_block (int32_t dly_ptr [], ModDlyState &mds, int nbr_spl)
{
	assert (fstb::is_ptr_align_nz (dly_ptr, 16));
	assert (nbr_spl > 0);
	assert (nbr_spl <= _max_blk_size);

	const float    dly_mod_fix_min = _min_mod_dly_time * ApfLine_nbr_phases;
	const float    dly_mod_fix_max = float (mds._dly_max * ApfLine_nbr_phases);

	static_assert (
		((_max_blk_size & 15) == 0),
		"_max_blk_size must be a multiple of 16"
	);
	alignas (16) std::array <float, _max_blk_size> dly_fix_flt;

	// First, computes all the delays as floating point data, premultiplied
	int            pos_blk = 0;
	do
	{
		check_mod_counters (mds);

		int            work_len    = nbr_spl - pos_blk;
		const int      rem_len_rnd = mds._rnd_per - mds._rnd_pos;
		work_len = std::min (work_len, rem_len_rnd);
		if (mds._lfo_step > 0)
		{
			const int      rem_len_lfo =
				fstb::ceil_int ((1 - mds._lfo_val) / mds._lfo_step);
			work_len = std::min (work_len, rem_len_lfo);
		}

		const float    dly_mod =
			  mds._dly_nomod
			+ mds._lfo_val * _lfo_depth
			+ mds._rnd_val * _rnd_depth;
		const float    dly_inc =
			  mds._lfo_step * _lfo_depth
			+ mds._rnd_step * _rnd_depth;
		float          dly_mod_fix = dly_mod * ApfLine_nbr_phases;
		const float    dly_inc_fix = dly_inc * ApfLine_nbr_phases;
		const int      blk_end = pos_blk + work_len;
		for (int pos = pos_blk; pos < blk_end; ++pos)
		{
			dly_fix_flt [pos] = dly_mod_fix;
			dly_mod_fix += dly_inc_fix;
		}

		mds._lfo_val += work_len * mds._lfo_step;
		mds._rnd_val += work_len * mds._rnd_step;
		mds._rnd_pos += work_len;
		pos_blk      += work_len;
	}
	while (pos_blk < nbr_spl);

	check_mod_counters (mds);

	// Now converts everything to integer in one pass with bound checking
	const auto     dly_min_v = fstb::ToolsSimd::set1_f32 (dly_mod_fix_min);
	const auto     dly_max_v = fstb::ToolsSimd::set1_f32 (dly_mod_fix_max);
	for (int pos = 0; pos < nbr_spl; pos += 16)
	{
		auto           x0f = fstb::ToolsSimd::load_f32 (&dly_fix_flt [pos     ]);
		auto           x4f = fstb::ToolsSimd::load_f32 (&dly_fix_flt [pos +  4]);
		auto           x8f = fstb::ToolsSimd::load_f32 (&dly_fix_flt [pos +  8]);
		auto           xcf = fstb::ToolsSimd::load_f32 (&dly_fix_flt [pos + 12]);
		x0f = fstb::ToolsSimd::min_f32 (x0f, dly_max_v);
		x4f = fstb::ToolsSimd::min_f32 (x4f, dly_max_v);
		x8f = fstb::ToolsSimd::min_f32 (x8f, dly_max_v);
		xcf = fstb::ToolsSimd::min_f32 (xcf, dly_max_v);
		x0f = fstb::ToolsSimd::max_f32 (x0f, dly_min_v);
		x4f = fstb::ToolsSimd::max_f32 (x4f, dly_min_v);
		x8f = fstb::ToolsSimd::max_f32 (x8f, dly_min_v);
		xcf = fstb::ToolsSimd::max_f32 (xcf, dly_min_v);
		const auto     x0i = fstb::ToolsSimd::conv_f32_to_s32 (x0f);
		const auto     x4i = fstb::ToolsSimd::conv_f32_to_s32 (x4f);
		const auto     x8i = fstb::ToolsSimd::conv_f32_to_s32 (x8f);
		const auto     xci = fstb::ToolsSimd::conv_f32_to_s32 (xcf);
		fstb::ToolsSimd::store_s32 (dly_ptr + pos     , x0i);
		fstb::ToolsSimd::store_s32 (dly_ptr + pos +  4, x4i);
		fstb::ToolsSimd::store_s32 (dly_ptr + pos +  8, x8i);
		fstb::ToolsSimd::store_s32 (dly_ptr + pos + 12, xci);
	}
}



void	ReverbDattorro::check_mod_counters (ModDlyState &mds)
{
	// LFO
	while (mds._lfo_val >= 1)
	{
		mds._lfo_val -= 2;
	}

	// Random modulation
	if (mds._rnd_pos >= mds._rnd_per)
	{
		mds._rnd_pos  = 0;
		const float    val_tgt = _rnd_gen.gen_flt () * 2 - 1;
		mds._rnd_step = (val_tgt - mds._rnd_val) / mds._rnd_per;
	}
}



constexpr int	ReverbDattorro::_max_blk_size;
constexpr int	ReverbDattorro::_nbr_apd_per_chn;
constexpr int	ReverbDattorro::_nbr_lfo_tot;
constexpr int	ReverbDattorro::_nbr_input_apd;
constexpr int	ReverbDattorro::_nbr_taps_out;



}  // namespace spat
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
