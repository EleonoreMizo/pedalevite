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
#include "mfx/dsp/iir/Svf2p.h"
#include "mfx/dsp/iir/TransSZBilin.h"
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

	constexpr float   margin = _lfo_max_depth_s + _rnd_max_depth_s;

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
		chn._tank_1.set_apd_max_delay (
			0,
			fstb::ceil_int (chn._lfo_arr [0]._dly_nosz * _max_room_size + margin)
		);
		chn._tank_2.set_apd_max_delay (
			0,
			fstb::ceil_int (chn._lfo_arr [1]._dly_nosz * _max_room_size + margin)
		);

		for (int k = 0; k < _nbr_taps_out; ++k)
		{
			chn._tap_dly_arr [k] = fstb::round_int (
				tap_dly_arr [chn_cnt] [k] * _sample_freq
			);
		}
	}

	update_diffusion_input ();
	update_diffusion_tank ();
	update_delay_times ();
	update_filter_bp (_filt_spec_input, &ReverbDattorro::set_filter_input_coefs);
	update_filter_bp (_filt_spec_tank, &ReverbDattorro::set_filter_tank_coefs);
	clear_buffers ();
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



void	ReverbDattorro::set_shimmer_pitch (float cents)
{
	constexpr int  cents_per_octave = 12 * 100;
	assert (cents >= -1 * cents_per_octave);
	assert (cents <= +1 * cents_per_octave);

	// Converts the pitch in cents to a playback rate
	const float    p_oct  = cents * (1.f / float (cents_per_octave));
	const float    pb_spd = fstb::Approx::exp2 (p_oct);

	// Equivalent number of additional samples in an LFO period
	const float    spl_pp = (pb_spd - 1) * _sample_freq * _lfo_per_base;

	// Bipolar depth in samples. The value is added to the delay (positive is
	// running backward), so we have to flip the sign.
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

	// Computes the LFO step
	_lfo_step = 2.f * _lfo_speed * _inv_fs; 
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



void	ReverbDattorro::set_filter_input_bp (float lo, float hi)
{
	assert (lo >= 1);
	assert (hi < _sample_freq * 0.5f);
	assert (lo < hi);

	_filt_spec_input._f_lo = lo;
	_filt_spec_input._f_hi = hi;
	update_filter_bp (_filt_spec_input, &ReverbDattorro::set_filter_input_coefs);
}



// Trapezoidal-integrated SVF coefficients and mixers
// See iir::Svf2p to compute them from basic parameters
void	ReverbDattorro::set_filter_input_coefs (float g0, float g1, float g2, float v0m, float v1m, float v2m)
{
	for (auto &chn : _chn_arr)
	{
		chn._input.set_filter_coefs (g0, g1, g2);
		chn._input.set_filter_mix (v0m, v1m, v2m);
	}
}



void	ReverbDattorro::set_filter_tank_bp (float lo, float hi)
{
	assert (lo >= 1);
	assert (hi < _sample_freq * 0.5f);
	assert (lo < hi);

	_filt_spec_tank._f_lo = lo;
	_filt_spec_tank._f_hi = hi;
	update_filter_bp (_filt_spec_tank, &ReverbDattorro::set_filter_tank_coefs);
}



// Trapezoidal-integrated SVF coefficients and mixers
// See iir::Svf2p to compute them from basic parameters
// Avoid gain > 1 for any frequency band
void	ReverbDattorro::set_filter_tank_coefs (float g0, float g1, float g2, float v0m, float v1m, float v2m)
{
	for (auto &chn : _chn_arr)
	{
		chn._tank_1.set_filter_coefs (g0, g1, g2);
		chn._tank_1.set_filter_mix (v0m, v1m, v2m);
	}
}



void	ReverbDattorro::freeze_tank (bool freeze_flag)
{
	_freeze.set_val ((freeze_flag) ? 1.f : 0.f);
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
	const float    coef_1 = 0.750f * _diffuse_tnk;
	const float    coef_2 = 0.625f * _diffuse_tnk;
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
			d2 -= _max_blk_size;
		}
		chn._tank_1.set_delay (d1);
		chn._tank_2.set_delay (d2);
		chn._lfo_arr [0]._dly_nomod = chn._lfo_arr [0]._dly_nosz * _room_size;
		chn._lfo_arr [1]._dly_nomod = chn._lfo_arr [1]._dly_nosz * _room_size;
	}
}



void	ReverbDattorro::update_filter_bp (const BPFilterSpec &spec, void (ReverbDattorro::*set_coefs) (float g0, float g1, float g2, float v0m, float v1m, float v2m))
{
	if (spec._f_hi > 0)
	{
		float          g0;
		float          g1;
		float          g2;
		float          v0m;
		float          v1m;
		float          v2m;
		compute_filter_coef (g0, g1, g2, v0m, v1m, v2m, spec);
		(this->*set_coefs) (g0, g1, g2, v0m, v1m, v2m);
	}
}



void	ReverbDattorro::compute_filter_coef (float &g0, float &g1, float &g2, float &v0m, float &v1m, float &v2m, const BPFilterSpec &spec) const
{
	assert (spec._f_lo > 0);
	assert (spec._f_lo < spec._f_hi);
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
	const double   b [3]  =
	{
		bl [0] * bh [0],
		bl [0] * bh [1] + bl [1] * bh [0],
		bl [1] * bh [1]
	};
	const double   a [3]  =
	{
		al [0] * ah [0],
		al [0] * ah [1] + al [1] * ah [0],
		al [1] * ah [1]
	};

	// Inverse frequency warping
	const double   f0 = iir::TransSZBilin::unwarp_freq (f0_w, _sample_freq);

	// Conversion from analogue biquad to filter coefficients
	iir::Svf2p::conv_s_eq_to_svf (
		g0, g1, g2, v0m, v1m, v2m,
		b, a, f0, _sample_freq
	);
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



// Returns the modulated delay time, in samples
float	ReverbDattorro::process_modulation (ModDlyState &mds)
{
	// LFO
	mds._lfo_val += _lfo_step;
	while (mds._lfo_val >= 1)
	{
		mds._lfo_val -= 2;
	}

	// Random modulation
	mds._rnd_val += mds._rnd_step;
	++ mds._rnd_pos;
	if (mds._rnd_pos >= mds._rnd_per)
	{
		mds._rnd_pos  = 0;
		const float    val_tgt = _rnd_gen.gen_flt () * 2 - 1;
		mds._rnd_step = (val_tgt - mds._rnd_val) / mds._rnd_per;
	}

	// Final delay value
	float          dly_mod =
		  mds._dly_nomod
		+ mds._lfo_val * _lfo_depth
		+ mds._rnd_pos * _rnd_depth;
	dly_mod = std::max (dly_mod, _min_mod_dly_time);

	return dly_mod;
}



}  // namespace spat
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/