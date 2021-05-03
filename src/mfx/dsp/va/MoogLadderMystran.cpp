/*****************************************************************************

        MoogLadderMystran.cpp
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
#include "mfx/dsp/va/MoogLadderMystran.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace va
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	MoogLadderMystran::set_sample_freq (double sample_freq) noexcept
{
	assert (sample_freq > 0);

	_sample_freq = float (           sample_freq);
	_piofs       = float (fstb::PI / sample_freq);
}



void	MoogLadderMystran::set_freq (float freq) noexcept
{
	assert (_sample_freq > 0);
	assert (freq > 0);
	assert (freq < _sample_freq * 0.5f);

	_fc = freq;
	_dirty_flag = true;
}



// Normalised resonance. 1 = self osc
void	MoogLadderMystran::set_reso (float reso) noexcept
{
	assert (reso >= 0);
	assert (reso <= float (_max_reso));

	_reso = reso;
	_r    = reso * 4;
	update_gaincomp ();

	_alpha      = compute_alpha (_r);
	_alpha_inv  = 1.f / _alpha;
	_dirty_flag = _freq_comp_flag;
}



void	MoogLadderMystran::set_gain_comp (float gc) noexcept
{
	assert (gc >= 0);
	assert (gc <= 1);

	_gaincomp = gc;
	update_gaincomp ();
}



void	MoogLadderMystran::set_freq_comp (bool comp_flag) noexcept
{
	if (_freq_comp_flag != comp_flag)
	{
		_dirty_flag     = true;
		_freq_comp_flag = comp_flag;
	}
}



float	MoogLadderMystran::process_sample (float x) noexcept
{
	assert (_sample_freq > 0);

	if (_dirty_flag)
	{
		update_cutoff ();
	}

	float          xx;
	float          y0;
	float          y1;
	float          y2;
	const float    y3 = process_sample_internal (x, _f, xx, y0, y1, y2);

	return y3 * _gc_mul;
}



float	MoogLadderMystran::process_sample (float x, float stage_in_ptr [4]) noexcept
{
	assert (_sample_freq > 0);

	if (_dirty_flag)
	{
		update_cutoff ();
	}

	float          xx;
	float          y0;
	float          y1;
	float          y2;
	const float    y3 = process_sample_internal (x, _f, xx, y0, y1, y2);
	stage_in_ptr [0] = xx * _gc_mul;
	stage_in_ptr [1] = y0 * _gc_mul;
	stage_in_ptr [2] = y1 * _gc_mul;
	stage_in_ptr [3] = y2 * _gc_mul;

	return y3 * _gc_mul;
}



float	MoogLadderMystran::process_sample_pitch_mod (float x, float m) noexcept
{
	assert (_sample_freq > 0);

	if (_dirty_flag)
	{
		update_cutoff ();
	}

	float          xx;
	float          y0;
	float          y1;
	float          y2;
	const float    f  = _f + _fi * m;
	const float    y3 = process_sample_internal (x, f, xx, y0, y1, y2);

	return y3 * _gc_mul;
}



float	MoogLadderMystran::process_sample_pitch_mod (float x, float m, float stage_in_ptr [4]) noexcept
{
	assert (_sample_freq > 0);

	if (_dirty_flag)
	{
		update_cutoff ();
	}

	float          xx;
	float          y0;
	float          y1;
	float          y2;
	const float    f  = _f + _fi * m;
	const float    y3 = process_sample_internal (x, f, xx, y0, y1, y2);
	stage_in_ptr [0] = xx * _gc_mul;
	stage_in_ptr [1] = y0 * _gc_mul;
	stage_in_ptr [2] = y1 * _gc_mul;
	stage_in_ptr [3] = y2 * _gc_mul;

	return y3 * _gc_mul;
}



void	MoogLadderMystran::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept
{
	assert (_sample_freq > 0);
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	if (_dirty_flag)
	{
		update_cutoff ();
	}

	float          xx;
	float          y0;
	float          y1;
	float          y2;
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		dst_ptr [pos] =
			process_sample_internal (src_ptr [pos], _f, xx, y0, y1, y2);
	}
}



void	MoogLadderMystran::process_block_pitch_mod (float dst_ptr [], const float src_ptr [], const float mod_ptr [], int nbr_spl) noexcept
{
	assert (_sample_freq > 0);
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (mod_ptr != nullptr);
	assert (nbr_spl > 0);

	if (_dirty_flag)
	{
		update_cutoff ();
	}

	float          xx;
	float          y0;
	float          y1;
	float          y2;
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		const float    f = _f + _fi * mod_ptr [pos];
		dst_ptr [pos] =
			process_sample_internal (src_ptr [pos], f, xx, y0, y1, y2);
	}
}



void	MoogLadderMystran::clear_buffers () noexcept
{
	_zi = 0;
	_s_arr.fill (0);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



float	MoogLadderMystran::process_sample_internal (float x, float f, float &xx, float &y0, float &y1, float &y2) noexcept
{
	// Input with half delay, for non-linearities
	const float    ih = 0.5f * (x + _zi);
	_zi = x;

	// Evaluates the non-linear gains
	const float    t0 = tanh_xdx (ih - _r * _s_arr [3]);
	const float    t1 = tanh_xdx (          _s_arr [0]);
	const float    t2 = tanh_xdx (          _s_arr [1]);
	const float    t3 = tanh_xdx (          _s_arr [2]);
	const float    t4 = tanh_xdx (          _s_arr [3]);

	// g# the denominators for solutions of individual stages
	const float    g0 = 1 / (1 + f * t1);
	const float    g1 = 1 / (1 + f * t2);
	const float    g2 = 1 / (1 + f * t3);
	const float    g3 = 1 / (1 + f * t4);

	// f# are just factored out of the feedback solution
	const float    f3 = f * t3 * g3;
	const float    f2 = f * t2 * g2 * f3;
	const float    f1 = f * t1 * g1 * f2;
	const float    f0 = f * t0 * g0 * f1;

	// Solves feedback
	const float    y3n =
		       g3 * _s_arr [3]
		+ f3 * g2 * _s_arr [2]
		+ f2 * g1 * _s_arr [1]
		+ f1 * g0 * _s_arr [0]
		+ f0 * x;
	const float    y3d = 1 + _r * f0;
	const float    y3  = y3n / y3d;

	// Then solves the remaining outputs (with the non-linear gains here)
	xx = t0 *      (x          - _r * y3);
	y0 = t1 * g0 * (_s_arr [0] + f * xx);
	y1 = t2 * g1 * (_s_arr [1] + f * y0);
	y2 = t3 * g2 * (_s_arr [2] + f * y1);

	// Updates state
	const float    fd = 2 * f;
	_s_arr [0] += fd * (xx - y0);
	_s_arr [1] += fd * (y0 - y1);
	_s_arr [2] += fd * (y1 - y2);
	_s_arr [3] += fd * (y2 - t4 * y3);

	return y3;
}



void	MoogLadderMystran::update_gaincomp () noexcept
{
	_gc_mul = 1 + std::min (_r, 4.f) * _gaincomp;
}



void	MoogLadderMystran::update_cutoff () noexcept
{
	const float    a = _piofs * _fc;
	_f  = fstb::Approx::tan_taylor5 (a);
	if (_freq_comp_flag)
	{
		_f *= _alpha_inv;
	}

	// Computes the f derivative, for fast 1 V/oct local modulations
	// z(x) = f (fc * 2^x) = tan (2^x * pi * fc / fs) = tan (a * 2^x)
	// with a = pi * fc / fs
	// z'(x) = a * ln (2) * 2^x / (cos (a * 2^x)) ^ 2
	// Let's assume z' is constant for small variations of x:
	// z'(0) = a * ln (2) * (f ^ 2 + 1)
	_fi = a * float (fstb::LN2) * (_f * _f + 1);
}



float	MoogLadderMystran::compute_alpha (float k) const noexcept
{
	const float    k_rt4 = float (sqrt (sqrt (k)));
	const float    alpha = float (
		sqrt (1 + k_rt4 * (k_rt4 - float (fstb::SQRT2)))
	);

	return alpha;
}



// tanh(x)/x approximation, flatline at very high inputs
// so might not be safe for very large feedback gains.
constexpr float	MoogLadderMystran::tanh_xdx (float x) noexcept
{
	const float    x2 = x * x;

#if 0
	// Limit is 1/15 so very large means ~15 or +23 dB
	// IIRC I got this as Pade-approx for tanh(sqrt(x))/sqrt(x)
	return   ((     x2 + 105) * x2 + 945)
	       / ((15 * x2 + 420) * x2 + 945);

#elif 1
	// Pade 2-3:
	// (21x^2 + 1260x + 10395) / (x^3 + 210x^2 + 4725x + 10395)
	// This one is the most stable at very high resonance or input gain.
	return   ((       21  * x2 + 1260) * x2 + 10395)
	       / (((x2 + 210) * x2 + 4725) * x2 + 10395);

#elif 0
	// Pade 3-3:
	// (x^3 + 378x^2 + 17325x + 135135) / (28x^3 + 3150x^2 + 62370x + 135135)
	return   (((     x2 +  378) * x2 + 17325) * x2 + 135135)
	       / (((28 * x2 + 3150) * x2 + 62370) * x2 + 135135);

#else

	float          xx = fstb::limit (x, -1.5f, 1.5f);
	const float    u  = fstb::limit (x, -0.5f, 0.5f);
	xx -= u;
	return (x == 0) ? 1 : (u + xx * (1 - 0.5f * fabs (xx))) / x;

#endif
}



}  // namespace va
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
