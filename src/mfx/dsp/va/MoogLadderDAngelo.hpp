/*****************************************************************************

        MoogLadderDAngelo.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_va_MoogLadderDAngelo_CODEHEADER_INCLUDED)
#define mfx_dsp_va_MoogLadderDAngelo_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/Approx.h"
#include "fstb/fnc.h"

#include <algorithm>

#include <cassert>
#include <cmath>
#include <cstdint>



namespace mfx
{
namespace dsp
{
namespace va
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int N, class SL, class SF>
void	MoogLadderDAngelo <N, SL, SF>::set_sample_freq (double sample_freq)
{
	assert (sample_freq > 0);

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	// Initialises the binomial coefficients
	std::array <uint64_t, N + 1> fact_arr { 1 };
	for (int k = 1; k <= N; ++k)
	{
		fact_arr [k] = fact_arr [k - 1] * k;
	}
	for (int k = 1; k <= N; ++k)
	{
		_bin_arr [k - 1] = int (
			fact_arr [N] / (fact_arr [k] * fact_arr [N - k])
		);
	}

	clear_buffers ();
	_dirty_flag = true;
}



template <int N, class SL, class SF>
void	MoogLadderDAngelo <N, SL, SF>::set_scale (float s)
{
	assert (s > 0);

	_vt         = s * 0.5f;
	_vt2        = 2 * _vt;
	_vt2i       = 1 / _vt2;
	update_gaincomp ();
	_dirty_flag = true;
}



template <int N, class SL, class SF>
void	MoogLadderDAngelo <N, SL, SF>::set_freq_natural (float f)
{
	assert (_sample_freq > 0);
	assert (f > 0);

	// Maximum is Fs / 8, otherwise it's not stable
	assert (f <= _sample_freq * 0.125f);

	set_freq_compensated (f * _alpha);
}



template <int N, class SL, class SF>
void	MoogLadderDAngelo <N, SL, SF>::set_freq_compensated (float f)
{
	assert (_sample_freq > 0);
	assert (f > 0);

	// Maximum is Fs / 8, otherwise it's not stable
	assert (f <= _sample_freq * 0.125f);

	_fc         = f;
	_dirty_flag = true;
}



template <int N, class SL, class SF>
void	MoogLadderDAngelo <N, SL, SF>::set_max_mod_freq (float f)
{
	assert (_sample_freq > 0);
	assert (f > 0);
	assert (f < _sample_freq * 0.5f);

	_gmax   = compute_g_max (f * _inv_fs);
	_k0smax = compute_k0_max (_gmax);
}



template <int N, class SL, class SF>
void	MoogLadderDAngelo <N, SL, SF>::set_reso_raw (float k)
{
	assert (_sample_freq > 0);
	assert (k >= 0);

	_k          = k;
	_alpha      = compute_alpha (k);
	_alpha_inv  = 1.f / _alpha;
	_dirty_flag = true;
	update_gaincomp ();
}



template <int N, class SL, class SF>
void	MoogLadderDAngelo <N, SL, SF>::set_reso_norm (float kn)
{
	assert (_sample_freq > 0);
	assert (kn >= 0);

	set_reso_raw (kn * _knorm_factor);
}



template <int N, class SL, class SF>
void	MoogLadderDAngelo <N, SL, SF>::set_gain_comp (float gc)
{
	assert (gc >= 0);
	assert (gc <= 1);

	_gaincomp = gc;
	update_gaincomp ();
}



template <int N, class SL, class SF>
float	MoogLadderDAngelo <N, SL, SF>::process_sample (float x)
{
	check_coef ();

	return process_sample_internal (x, _g, _k0s);
}



template <int N, class SL, class SF>
float	MoogLadderDAngelo <N, SL, SF>::process_sample (float x, float stage_in_ptr [N])
{
	check_coef ();

	float          yo = process_sample_input (x);
	float          y;
	for (int n = 0; n < N; ++n)
	{
		stage_in_ptr [n] = yo * _gc_mul_s;
		process_sample_stage (y, yo, n, _g, _k0s);
	}
	y = process_sample_fdbk (x, y);

	return y;
}



// m = 1 V/oct pitch modulation (0 = neutral)
template <int N, class SL, class SF>
float	MoogLadderDAngelo <N, SL, SF>::process_sample_pitch_mod (float x, float m)
{
	check_coef ();
	const float    k0s = fstb::limit (_k0s + _k0si * m, 0.f, _k0smax);
	const float    g   = fstb::limit (_g   + _gi   * m, 0.f, _gmax  );

	return process_sample_internal (x, g, k0s);
}



template <int N, class SL, class SF>
float	MoogLadderDAngelo <N, SL, SF>::process_sample_pitch_mod (float x, float m, float stage_in_ptr [N])
{
	check_coef ();
	const float    k0s = fstb::limit (_k0s + _k0si * m, 0.f, _k0smax);
	const float    g   = fstb::limit (_g   + _gi   * m, 0.f, _gmax  );

	float          yo = process_sample_input (x);
	float          y;
	for (int n = 0; n < N; ++n)
	{
		stage_in_ptr [n] = yo * _gc_mul_s;
		process_sample_stage (y, yo, n, g, k0s);
	}
	y = process_sample_fdbk (x, y);

	return y;
}



template <int N, class SL, class SF>
void	MoogLadderDAngelo <N, SL, SF>::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	check_coef ();
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		dst_ptr [pos] = process_sample_internal (src_ptr [pos], _g, _k0s);
	}
}



template <int N, class SL, class SF>
void	MoogLadderDAngelo <N, SL, SF>::process_block_pitch_mod (float dst_ptr [], const float src_ptr [], const float mod_ptr [], int nbr_spl)
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (mod_ptr != nullptr);
	assert (nbr_spl > 0);

	check_coef ();
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		const float    m   = mod_ptr [pos];
		const float    k0s = fstb::limit (_k0s + _k0si * m, 0.f, _k0smax);
		const float    g   = fstb::limit (_g   + _gi   * m, 0.f, _gmax  );
		dst_ptr [pos] = process_sample_internal (src_ptr [pos], g, k0s);
	}
}



template <int N, class SL, class SF>
void	MoogLadderDAngelo <N, SL, SF>::clear_buffers ()
{
	_si_arr.fill (0);
	_sf_arr.fill (0);
	_sg_arr.fill (0);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int N, class SL, class SF>
void	MoogLadderDAngelo <N, SL, SF>::check_coef ()
{
	if (_dirty_flag)
	{
		update_coef ();
	}
}



template <int N, class SL, class SF>
void	MoogLadderDAngelo <N, SL, SF>::update_coef ()
{
	const float    a = float (fstb::PI) * _fc * _inv_fs;
	_g =   fstb::Approx::tan_mystran (a)
	     * _alpha_inv;

	// Computes the g derivative, for fast 1 V/oct local modulations
	// z(x) = g(fc * 2^x) = tan (2^x * pi * fc / fs) = tan (a * 2^x)
	// with a = pi * fc / fs
	// z'(x) = a * ln (2) * 2^x / (cos (a * 2^x)) ^ 2
	// Let's assume z' is constant for small variations of x:
	// z'(0) = a * ln (2) * (g ^ 2 + 1)
	_gi = a * float (fstb::LN2) * (_g * _g + 1);
	const float    p0s = 1 / (1 + _g);
	const float    k0m = 2 * _vt * p0s;
	_k0s  = _g  * k0m;
	_k0si = _gi * k0m;
	const float    gn  = fstb::ipowp (1 - p0s, N);
	const float    kgn = _k * gn;
	const float    p0g = 1 / (1 + kgn);
	
	const float    gm1p0s   = (_g - 1) * p0s;
	float          gm1p0s_n = gm1p0s;
	for (int k = 0; k < N; ++k)
	{
		_r_arr [k] =            - _bin_arr [k] * kgn;
		_q_arr [k] = _r_arr [k] - _bin_arr [k] * gm1p0s_n;
		gm1p0s_n *= gm1p0s;
	}
	_k0g = -_vt2i * p0g;

	_dirty_flag = false;
}



template <int N, class SL, class SF>
void	MoogLadderDAngelo <N, SL, SF>::update_gaincomp ()
{
	_gc_mul   = 1 + std::min (_k, _knorm_factor) * _gaincomp;
	_gc_mul_s = _gc_mul * _vt2;
}



template <int N, class SL, class SF>
float	MoogLadderDAngelo <N, SL, SF>::process_sample_internal (float x, float g, float k0s)
{
	float          yo = process_sample_input (x);
	float          y;
	for (int n = 0; n < N; ++n)
	{
		process_sample_stage (y, yo, n, g, k0s);
	}
	y = process_sample_fdbk (x, y);

	return y;
}



template <int N, class SL, class SF>
void	MoogLadderDAngelo <N, SL, SF>::process_sample_stage (float &y, float &yo, int n, float g, float k0s)
{
	const float    yi = yo;
	const float    yd = k0s * (yi + _sf_arr [n]);
	y  = yd + _si_arr [n];
	yo = _shaper_lpf_arr [n] (_vt2i * y);

	_si_arr [n] = yd + y;
	_sf_arr [n] = g * (yo - yi) - yo;
}



template <int N, class SL, class SF>
float	MoogLadderDAngelo <N, SL, SF>::process_sample_input (float x)
{
	const float    fdbk = _shaper_fdbk (_sg_arr [0]);
	const float    yo   = _shaper_input (_k0g * (x + fdbk));

	return yo;
}



template <int N, class SL, class SF>
float	MoogLadderDAngelo <N, SL, SF>::process_sample_fdbk (float x, float y)
{
	// Feedback
	const float    yf = y * _k;
	for (int n = 0; n < N - 1; ++n)
	{
		_sg_arr [n] = _r_arr [n] * x + _q_arr [n] * yf + _sg_arr [n + 1];
	}
	_sg_arr [N - 1] = _r_arr [N - 1] * x + _q_arr [N - 1] * yf;

	// Gain compensation
	y *= _gc_mul;

	return y;
}



template <int N, class SL, class SF>
float	MoogLadderDAngelo <N, SL, SF>::compute_g_max (float fmax_over_fs)
{
	assert (fmax_over_fs > 0);
	assert (fmax_over_fs < 0.5f);

	return float (tan (fstb::PI * fmax_over_fs));
}



template <int N, class SL, class SF>
float	MoogLadderDAngelo <N, SL, SF>::compute_k0_max (float gmax)
{
	assert (gmax > 0);

	return 2 * _vt * (1 - 1 / (1 + gmax));
}



template <int N, class SL, class SF>
float	MoogLadderDAngelo <N, SL, SF>::compute_alpha (float k)
{
	float          alpha = 1 + k;

	if (N > 1)
	{
		const float    k_nrt = pow (k, 1.0f / N);
		alpha = float (sqrt (1 + k_nrt * (k_nrt - 2 * cos (fstb::PI / N))));
	}

	return alpha;
}



template <int N, class SL, class SF>
float	MoogLadderDAngelo <N, SL, SF>::compute_knorm_factor ()
{
	return 1.f / fstb::ipowp (float (cos (fstb::PI / N)), N);
}



}  // namespace va
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_va_MoogLadderDAngelo_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
