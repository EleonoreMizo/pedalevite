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



template <int N>
void	MoogLadderDAngelo <N>::set_sample_freq (double sample_freq)
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



template <int N>
void	MoogLadderDAngelo <N>::set_freq_natural (float f)
{
	assert (_sample_freq > 0);
	assert (f > 0);

	// Maximum is Fs / 8, otherwise it's not stable
	assert (f <= _sample_freq * 0.125f);

	set_freq_compensated (f * _alpha);
}



template <int N>
void	MoogLadderDAngelo <N>::set_freq_compensated (float f)
{
	assert (_sample_freq > 0);
	assert (f > 0);

	// Maximum is Fs / 8, otherwise it's not stable
	assert (f <= _sample_freq * 0.125f);

	_fc         = f;
	_dirty_flag = true;
}



template <int N>
void	MoogLadderDAngelo <N>::set_reso_raw (float k)
{
	assert (_sample_freq > 0);
	assert (k >= 0);

	_k          = k;
	_alpha      = compute_alpha (k);
	_alpha_inv  = 1.f / _alpha;
	_dirty_flag = true;
	update_gaincomp ();
}



template <int N>
void	MoogLadderDAngelo <N>::set_reso_norm (float kn)
{
	assert (_sample_freq > 0);
	assert (kn >= 0);

	set_reso_raw (kn * _knorm_factor);
}



template <int N>
void	MoogLadderDAngelo <N>::set_gain_comp (float gc)
{
	assert (gc >= 0);
	assert (gc <= 1);

	_gaincomp = gc;
	update_gaincomp ();
}



template <int N>
float	MoogLadderDAngelo <N>::process_sample (float x)
{
	if (_dirty_flag)
	{
		update_coef ();
	}

	// Input processing
	float          yo = clip_sigmoid (_k0g * (x + _sg_arr [0]));

	// Ladder stages
	float          y;
	for (int n = 0; n < N; ++n)
	{
		const float    yi = yo;
		const float    yd = _k0s * (yi + _sf_arr [n]);
		y  = yd + _si_arr [n];
		yo = clip_sigmoid (_vt2i * y);

		_si_arr [n] = yd + y;
		_sf_arr [n] = _g * (yo - yi) - yo;
	}

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



template <int N>
void	MoogLadderDAngelo <N>::clear_buffers ()
{
	_si_arr.fill (0);
	_sf_arr.fill (0);
	_sg_arr.fill (0);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int N>
void	MoogLadderDAngelo <N>::update_coef ()
{
	_g =   fstb::Approx::tan_mystran (float (fstb::PI) * _fc * _inv_fs)
	     * _alpha_inv;
	const float    p0s = 1 / (1 + _g);
	_k0s = 2 * _vt * _g * p0s;
	const float    gn = fstb::ipowp (1 - p0s, N);
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



template <int N>
void	MoogLadderDAngelo <N>::update_gaincomp ()
{
	_gc_mul = 1 + _k * _gaincomp;
}



template <int N>
float	MoogLadderDAngelo <N>::compute_alpha (float k)
{
	float          alpha = 1 + k;

	if (N > 1)
	{
		const float    k_nrt = pow (k, 1.0f / N);
		alpha = float (sqrt (1 + k_nrt * (k_nrt - 2 * cos (fstb::PI / N))));
	}

	return alpha;
}



template <int N>
float	MoogLadderDAngelo <N>::clip_sigmoid (float x)
{
#if 1
	// Good compromise between harmonics and tuning
	x = fstb::limit (x, -1.5f, 1.5f);
	const float    u = fstb::limit (x, -0.5f, 0.5f);
	x -= u;
	return u + x * (1 - 0.5f * fabs (x));

#elif 0
	// Bad tuning, worse than tanh(), but lots of harmonics
	x = fstb::limit (x, -2.f, 2.f);
	return x * (1 - 0.25f * fabs (x));

#else
	// Good tuning but low harmonics and higher aliasing
	return fstb::limit (x, -1.f, 1.f);

#endif
}



}  // namespace va
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_va_MoogLadderDAngelo_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
