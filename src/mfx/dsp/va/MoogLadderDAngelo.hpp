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
void	MoogLadderDAngelo <N, SL, SF>::set_sample_freq (double sample_freq) noexcept
{
	assert (sample_freq > 0);

	_d._sample_freq = float (    sample_freq);
	_d._inv_fs      = float (1 / sample_freq);

	// Initialises the binomial coefficients
	std::array <uint64_t, N + 1> fact_arr { 1 };
	for (int k = 1; k <= N; ++k)
	{
		fact_arr [k] = fact_arr [k - 1] * k;
	}
	for (int k = 1; k <= N; ++k)
	{
		_d._bin_arr [k - 1] = int (
			fact_arr [N] / (fact_arr [k] * fact_arr [N - k])
		);
	}

	clear_buffers ();
	_d._dirty_flag = true;
}



template <int N, class SL, class SF>
void	MoogLadderDAngelo <N, SL, SF>::set_scale (float s) noexcept
{
	assert (s > 0);

	_d._vt         = s * 0.5f;
	_d._vt2        = s;
	_d._vt2i       = 1 / s;
	update_gaincomp ();
	_d._dirty_flag = true;
}



template <int N, class SL, class SF>
void	MoogLadderDAngelo <N, SL, SF>::set_freq_natural (float f) noexcept
{
	assert (_d._sample_freq > 0);
	assert (f > 0);

	// Maximum is Fs / 8, otherwise it's not stable
	assert (f <= _d._sample_freq * 0.125f);

	set_freq_compensated (f * _d._alpha);
}



template <int N, class SL, class SF>
void	MoogLadderDAngelo <N, SL, SF>::set_freq_compensated (float f) noexcept
{
	assert (_d._sample_freq > 0);
	assert (f > 0);

	// Maximum is Fs / 8, otherwise it's not stable
	assert (f <= _d._sample_freq * 0.125f);

	_d._fc         = f;
	_d._dirty_flag = true;
}



template <int N, class SL, class SF>
void	MoogLadderDAngelo <N, SL, SF>::set_max_mod_freq (float f) noexcept
{
	assert (_d._sample_freq > 0);
	assert (f > 0);
	assert (f < _d._sample_freq * 0.5f);

	_d._gmax   = _d.compute_g_max (f * _d._inv_fs);
	_d._k0smax = _d.compute_k0_max (_d._gmax);
}



template <int N, class SL, class SF>
void	MoogLadderDAngelo <N, SL, SF>::set_reso_raw (float k) noexcept
{
	assert (_d._sample_freq > 0);
	assert (k >= 0);

	_d._k          = k;
	_d._alpha      = FilterData::compute_alpha (k);
	_d._alpha_inv  = 1.f / _d._alpha;
	_d._dirty_flag = true;
	update_gaincomp ();
}



template <int N, class SL, class SF>
void	MoogLadderDAngelo <N, SL, SF>::set_reso_norm (float kn) noexcept
{
	assert (_d._sample_freq > 0);
	assert (kn >= 0);

	set_reso_raw (kn * _d._knorm_factor);
}



template <int N, class SL, class SF>
void	MoogLadderDAngelo <N, SL, SF>::set_gain_comp (float gc) noexcept
{
	assert (gc >= 0);
	assert (gc <= 1);

	_d._gaincomp = gc;
	update_gaincomp ();
}



template <int N, class SL, class SF>
float	MoogLadderDAngelo <N, SL, SF>::process_sample (float x) noexcept
{
	check_coef ();

	return process_sample_internal (x, _d._g, _d._k0s);
}



template <int N, class SL, class SF>
float	MoogLadderDAngelo <N, SL, SF>::process_sample (float x, float stage_in_ptr [N]) noexcept
{
	check_coef ();

	float          yo = process_sample_input (x);
	float          y  = 0.f;
	for (int n = 0; n < N; ++n)
	{
		stage_in_ptr [n] = yo * _d._gc_mul_s;
		process_sample_stage (y, yo, n, _d._g, _d._k0s);
	}
	y = process_sample_fdbk (x, y);

	return y;
}



// m = 1 V/oct pitch modulation (0 = neutral)
template <int N, class SL, class SF>
float	MoogLadderDAngelo <N, SL, SF>::process_sample_pitch_mod (float x, float m) noexcept
{
	check_coef ();
	const float    k0s = fstb::limit (_d._k0s + _d._k0si * m, 0.f, _d._k0smax);
	const float    g   = fstb::limit (_d._g   + _d._gi   * m, 0.f, _d._gmax  );

	return process_sample_internal (x, g, k0s);
}



template <int N, class SL, class SF>
float	MoogLadderDAngelo <N, SL, SF>::process_sample_pitch_mod (float x, float m, float stage_in_ptr [N]) noexcept
{
	check_coef ();
	const float    k0s = fstb::limit (_d._k0s + _d._k0si * m, 0.f, _d._k0smax);
	const float    g   = fstb::limit (_d._g   + _d._gi   * m, 0.f, _d._gmax  );

	float          yo = process_sample_input (x);
	float          y;
	for (int n = 0; n < N; ++n)
	{
		stage_in_ptr [n] = yo * _d._gc_mul_s;
		process_sample_stage (y, yo, n, g, k0s);
	}
	y = process_sample_fdbk (x, y);

	return y;
}



template <int N, class SL, class SF>
void	MoogLadderDAngelo <N, SL, SF>::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	check_coef ();
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		dst_ptr [pos] = process_sample_internal (src_ptr [pos], _d._g, _d._k0s);
	}
}



template <int N, class SL, class SF>
void	MoogLadderDAngelo <N, SL, SF>::process_block_pitch_mod (float dst_ptr [], const float src_ptr [], const float mod_ptr [], int nbr_spl) noexcept
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (mod_ptr != nullptr);
	assert (nbr_spl > 0);

	check_coef ();
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		const float    m   = mod_ptr [pos];
		const float    k0s = fstb::limit (_d._k0s + _d._k0si * m, 0.f, _d._k0smax);
		const float    g   = fstb::limit (_d._g   + _d._gi   * m, 0.f, _d._gmax  );
		dst_ptr [pos] = process_sample_internal (src_ptr [pos], g, k0s);
	}
}



template <int N, class SL, class SF>
void	MoogLadderDAngelo <N, SL, SF>::clear_buffers () noexcept
{
	_d._si_arr.fill (0);
	_d._sf_arr.fill (0);
	_d._sg_arr.fill (0);
}



template <int N, class SL, class SF>
MoogLadderDAngeloData <N> &	MoogLadderDAngelo <N, SL, SF>::use_data () noexcept
{
	return _d;
}



template <int N, class SL, class SF>
const MoogLadderDAngeloData <N> &	MoogLadderDAngelo <N, SL, SF>::use_data () const noexcept
{
	return _d;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int N, class SL, class SF>
void	MoogLadderDAngelo <N, SL, SF>::check_coef () noexcept
{
	if (_d._dirty_flag)
	{
		update_coef ();
	}
}



template <int N, class SL, class SF>
void	MoogLadderDAngelo <N, SL, SF>::update_coef () noexcept
{
	const float    a = float (fstb::PI) * _d._fc * _d._inv_fs;
	_d._g = fstb::Approx::tan_mystran (a) * _d._alpha_inv;

	// Computes the g derivative, for fast 1 V/oct local modulations
	// z(x) = g(fc * 2^x) = tan (2^x * pi * fc / fs) = tan (a * 2^x)
	// with a = pi * fc / fs
	// z'(x) = a * ln (2) * 2^x / (cos (a * 2^x)) ^ 2
	// Let's assume z' is constant for small variations of x:
	// z'(0) = a * ln (2) * (g ^ 2 + 1)
	_d._gi = a * float (fstb::LN2) * (_d._g * _d._g + 1);
	const float    p0s = 1 / (1 + _d._g);
	const float    k0m = 2 * _d._vt * p0s;
	_d._k0s  = _d._g  * k0m;
	_d._k0si = _d._gi * k0m;
	const float    gn  = fstb::ipowp (1 - p0s, N);
	const float    kgn = _d._k * gn;
	const float    p0g = 1 / (1 + kgn);
	
	const float    gm1p0s   = (_d._g - 1) * p0s;
	float          gm1p0s_n = gm1p0s;
	for (int k = 0; k < N; ++k)
	{
		_d._r_arr [k] =               - _d._bin_arr [k] * kgn;
		_d._q_arr [k] = _d._r_arr [k] - _d._bin_arr [k] * gm1p0s_n;
		gm1p0s_n *= gm1p0s;
	}
	_d._k0g = -_d._vt2i * p0g;

	_d._dirty_flag = false;
}



template <int N, class SL, class SF>
void	MoogLadderDAngelo <N, SL, SF>::update_gaincomp () noexcept
{
	_d._gc_mul   = 1 + std::min (_d._k, _d._knorm_factor) * _d._gaincomp;
	_d._gc_mul_s = _d._gc_mul * _d._vt2;
}



template <int N, class SL, class SF>
float	MoogLadderDAngelo <N, SL, SF>::process_sample_internal (float x, float g, float k0s) noexcept
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
void	MoogLadderDAngelo <N, SL, SF>::process_sample_stage (float &y, float &yo, int n, float g, float k0s) noexcept
{
	const float    yi = yo;
	const float    yd = k0s * (yi + _d._sf_arr [n]);
	y  = yd + _d._si_arr [n];
	yo = _shaper_lpf_arr [n] (_d._vt2i * y);

	_d._si_arr [n] = yd + y;
	_d._sf_arr [n] = g * (yo - yi) - yo;
}



template <int N, class SL, class SF>
float	MoogLadderDAngelo <N, SL, SF>::process_sample_input (float x) noexcept
{
	const float    fdbk = _shaper_fdbk (_d._k0g * _d._sg_arr [0]);
	const float    yo   = _shaper_input (_d._k0g * x + fdbk);

	return yo;
}



template <int N, class SL, class SF>
float	MoogLadderDAngelo <N, SL, SF>::process_sample_fdbk (float x, float y) noexcept
{
	// Feedback
	const float    yf = y * _d._k;
	for (int n = 0; n < N - 1; ++n)
	{
		_d._sg_arr [n] =
			_d._r_arr [n] * x + _d._q_arr [n] * yf + _d._sg_arr [n + 1];
	}
	_d._sg_arr [N - 1] = _d._r_arr [N - 1] * x + _d._q_arr [N - 1] * yf;

	// Gain compensation
	y *= _d._gc_mul;

	return y;
}



}  // namespace va
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_va_MoogLadderDAngelo_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
