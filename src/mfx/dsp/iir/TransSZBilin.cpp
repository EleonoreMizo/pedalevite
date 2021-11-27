/*****************************************************************************

        TransSZBilin.cpp
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
#include "mfx/dsp/iir/TransSZBilin.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: prewarp_freq_rel_1
Description:
	Warp a single frequency, given a reference frequency.
	The output is a frequency (multiplier) relative to fref, hence suitable to
	be directly integrated as coefficient of an s equation centered on 1.
Input parameters:
	- f0: Frequency to be warped, in ]0 ; fs/2[
	- fref: Reference frequency, in ]0 ; fs/2[
	- fs: Sampling frequency, > 0.
Returns: the warped frequency, relative to 1, in ]0 ; +oo[
Throws: Nothing
==============================================================================
*/

double	TransSZBilin::prewarp_freq_rel_1 (double f0, double fref, double fs) noexcept
{
	assert (f0 > 0);
	assert (f0 < fs * 0.5);
	assert (fref > 0);
	assert (fref < fs * 0.5);
	assert (fs > 0);

	const double   mult   = fstb::PI / fs;
	const double   fref_w = tan (fref * mult);
	const double   f0_w   = tan (f0   * mult);

	const double   f_w_rel = f0_w / fref_w;
	assert (f_w_rel > 0);

	return f_w_rel;
}



/*
==============================================================================
Name: prewarp_freq_rel
Description:
	Warp a single frequency, given a reference frequency.
Input parameters:
	- f0: Frequency to be warped, in ]0 ; fs/2[
	- fref: Reference frequency, in ]0 ; fs/2[
	- fs: Sampling frequency, > 0.
Returns: the warped frequency, in ]0 ; +oo[
Throws: Nothing
==============================================================================
*/

double	TransSZBilin::prewarp_freq_rel (double f0, double fref, double fs) noexcept
{
	return prewarp_freq_rel_1 (f0, fref, fs) * fref;
}



/*
==============================================================================
Name: prewarp_freq_rel_mul
Description:
	Returns the multiplier to apply on the frequency to be warped.
Input parameters:
	- f0: Frequency to be warped, in ]0 ; fs/2[
	- fref: Reference frequency, in ]0 ; fs/2[
	- fs: Sampling frequency, > 0.
Returns: the multiplier, in ]0 ; +oo[
Throws: Nothing
==============================================================================
*/

double	TransSZBilin::prewarp_freq_rel_mul (double f0, double fref, double fs) noexcept
{
	assert (fs > 0);
	assert (f0 > 0);
	assert (f0 < fs * 0.5);
	assert (fref > 0);
	assert (fref < fs * 0.5);

	const double   mult   = fstb::PI / fs;
	const double   fref_w = tan (fref * mult);
	const double   f0_w   = tan (f0   * mult);

	const double   f_w_rel_mul = (f0_w * fref) / (f0 * fref_w);
	assert (f_w_rel_mul > 0);

	return f_w_rel_mul;
}



/*
==============================================================================
Name: prewarp_root_rel
Description:
	Prewarp a (complex conjugate) root (pole or zero) for a filter whose main
	frequency is normalized (centered on 1). The output is suitable to be
	directly converted to coefficients of an s equation centered on 1.
Input parameters:
	- root: The root to be warped, relative to fref.
	- fref: Reference filter frequency, in ]0 ; fs/2[
	- fs: Sampling frequency, > 0.
Returns: the warped root
Throws: Nothing
==============================================================================
*/

std::complex <double>	TransSZBilin::prewarp_root_rel (const std::complex <double> &root, double fref, double fs) noexcept
{
	assert (fs > 0);
	assert (root.real () <= 0);
	assert (fref > 0);
	assert (fref < fs * 0.5);

	const double   f0    = std::abs (root) * fref;
	assert (f0 > 0);
	const double   scale = prewarp_freq_rel_mul (f0, fref, fs);

	return root * scale;
}



/*
==============================================================================
Name: prewarp_biquad
Description:
	Changes the coefficient of a biquad in order to make a certain frequency
	correctly warped.
Input parameters:
	- fref: Reference frequency for the biquad, in ]0 ; fs/2[
	- fs: Sampling frequency, > 0.
Input/output parameters:
	- b: numerator of the transfer function in the S-plane.
	- a: denominator of the transfer function in the S-plane.
Throws: Nothing
==============================================================================
*/

void	TransSZBilin::prewarp_biquad (double b [3], double a [3], double fref, double fs) noexcept
{
	assert (b != nullptr);
	assert (a != nullptr);
	assert (a [0] != 0);
	assert (a [2] != 0);

	const double   freq = sqrt (a [2] / a [0]) * fref;
	const double   mul  = prewarp_freq_rel_mul (freq, fref, fs);
	const double   mul2 = mul * mul;
	a [1] *= mul;
	b [1] *= mul;
	a [2] *= mul2;
	b [2] *= mul2;
}



void	TransSZBilin::map_s_to_z_approx (float z_eq_b [3], float z_eq_a [3], const float s_eq_b [3], const float s_eq_a [3], float k) noexcept
{
	assert (z_eq_b != nullptr);
	assert (z_eq_a != nullptr);
	assert (s_eq_b != nullptr);
	assert (s_eq_a != nullptr);

#if defined (fstb_HAS_SIMD)

	const auto     kv   = fstb::Vf32 (k);
	const auto     kk   = fstb::Vf32 (k * k);

	const auto     x0s  = fstb::Vf32::set_pair (s_eq_a [0], s_eq_b [0]);
	const auto     x1s  = fstb::Vf32::set_pair (s_eq_a [1], s_eq_b [1]);
	const auto     x2s  = fstb::Vf32::set_pair (s_eq_a [2], s_eq_b [2]);

	const auto     x1k  = x1s * kv;
	const auto     x2kk = x2s * kk;
	const auto     x2kk_plus_x0   = x2kk + x0s;
	auto           x0z  = x2kk_plus_x0 + x1k;
	auto           x2z  = x2kk_plus_x0 - x1k;
	const auto     x0s_minus_x2kk = x0s - x2kk;
	auto           x1z  = x0s_minus_x2kk + x0s_minus_x2kk;

	// On a0z only. Requires accuracy
	const auto     mult =
		fstb::ToolsSimd::Shift <0>::spread (x0z.rcp_approx2 ());

	x0z *= mult;
	x1z *= mult;
	x2z *= mult;

	z_eq_b [0] = fstb::ToolsSimd::Shift <1>::extract (x0z);
	z_eq_b [1] = fstb::ToolsSimd::Shift <1>::extract (x1z);
	z_eq_b [2] = fstb::ToolsSimd::Shift <1>::extract (x2z);

	z_eq_a [0] = 1;
	z_eq_a [1] = fstb::ToolsSimd::Shift <0>::extract (x1z);
	z_eq_a [2] = fstb::ToolsSimd::Shift <0>::extract (x2z);

#else

	const float    kk = k*k;

	const float    b1k  = s_eq_b [1] * k;
	const float    a1k  = s_eq_a [1] * k;
	const float    b2kk = s_eq_b [2] * kk;
	const float    a2kk = s_eq_a [2] * kk;
	const float    b2kk_plus_b0 = b2kk + s_eq_b [0];
	const float    a2kk_plus_a0 = a2kk + s_eq_a [0];
	const float    b0z = b2kk_plus_b0 + b1k;
	const float    a0z = a2kk_plus_a0 + a1k;
	const float    b2z = b2kk_plus_b0 - b1k;
	const float    a2z = a2kk_plus_a0 - a1k;
	const float    b1z = 2 * (s_eq_b [0] - b2kk);
	const float    a1z = 2 * (s_eq_a [0] - a2kk);

	// IIR coefficients
	assert (! fstb::is_null (a0z));
	const float    mult = 1 / a0z;

	z_eq_b [0] = b0z * mult;
	z_eq_b [1] = b1z * mult;
	z_eq_b [2] = b2z * mult;

	z_eq_a [0] = 1;
	z_eq_a [1] = a1z * mult;
	z_eq_a [2] = a2z * mult;

#endif
}



void	TransSZBilin::map_s_to_z_approx (fstb::Vf32 z_eq_b [3], fstb::Vf32 z_eq_a [3], const fstb::Vf32 s_eq_b [3], const fstb::Vf32 s_eq_a [3], fstb::Vf32 k) noexcept
{
	assert (z_eq_b != nullptr);
	assert (z_eq_a != nullptr);
	assert (s_eq_b != nullptr);
	assert (s_eq_a != nullptr);

	const auto     kk   = k * k;

	const auto     b0s  = fstb::Vf32::load (&s_eq_b [0]);
	const auto     b1s  = fstb::Vf32::load (&s_eq_b [1]);
	const auto     b2s  = fstb::Vf32::load (&s_eq_b [2]);

	const auto     b1k  = b1s * k;
	const auto     b2kk = b2s * kk;
	const auto     b2kk_plus_b0   = b2kk + b0s;
	const auto     b0z  = b2kk_plus_b0 + b1k;
	const auto     b2z  = b2kk_plus_b0 - b1k;
	const auto     b0s_minus_b2kk = b0s - b2kk;
	const auto     b1z  = b0s_minus_b2kk + b0s_minus_b2kk;

	const auto     a0s  = fstb::Vf32::load (&s_eq_a [0]);
	const auto     a1s  = fstb::Vf32::load (&s_eq_a [1]);
	const auto     a2s  = fstb::Vf32::load (&s_eq_a [2]);

	const auto     a1k  = a1s * k;
	const auto     a2kk = a2s * kk;
	const auto     a2kk_plus_a0   = a2kk + a0s;
	const auto     a0z  = a2kk_plus_a0 + a1k;
	const auto     a2z  = a2kk_plus_a0 - a1k;
	const auto     a0s_minus_a2kk = a0s - a2kk;
	const auto     a1z  = a0s_minus_a2kk + a0s_minus_a2kk;

	const auto     mult = a0z.rcp_approx2 ();   // Requires accuracy

	(b0z * mult).store (&z_eq_b [0]);
	(b1z * mult).store (&z_eq_b [1]);
	(b2z * mult).store (&z_eq_b [2]);

	fstb::Vf32 (1).store (&z_eq_a [0]);
	(a1z * mult  ).store (&z_eq_a [1]);
	(a2z * mult  ).store (&z_eq_a [2]);
}



void	TransSZBilin::map_s_to_z_one_pole_approx (float z_eq_b [2], float z_eq_a [2], const float s_eq_b [2], const float s_eq_a [2], float k) noexcept
{
	assert (z_eq_b != nullptr);
	assert (z_eq_a != nullptr);
	assert (s_eq_b != nullptr);
	assert (s_eq_a != nullptr);

#if defined (fstb_HAS_SIMD)

	// s to z bilinear transform
	const auto     kv   = fstb::Vf32 (k);

	const auto     x0s  = fstb::Vf32::set_pair (s_eq_a [0], s_eq_b [0]);
	const auto     x1s  = fstb::Vf32::set_pair (s_eq_a [1], s_eq_b [1]);

	const auto     x1k = x1s * kv;
	auto           x1z = x0s - x1k;
	auto           x0z = x0s + x1k;

	// On a0z only. Requires accuracy
	const auto     mult =
		fstb::ToolsSimd::Shift <0>::spread (x0z.rcp_approx2 ());

	x0z *= mult;
	x1z *= mult;

	z_eq_b [0] = fstb::ToolsSimd::Shift <1>::extract (x0z);
	z_eq_b [1] = fstb::ToolsSimd::Shift <1>::extract (x1z);

	z_eq_a [0] = 1;
	z_eq_a [1] = fstb::ToolsSimd::Shift <0>::extract (x1z);

#else

	const float    b1k = s_eq_b [1] * k;
	const float    b1z = s_eq_b [0] - b1k;
	const float    b0z = s_eq_b [0] + b1k;

	const float    a1k = s_eq_a [1] * k;
	const float    a1z = s_eq_a [0] - a1k;
	const float    a0z = s_eq_a [0] + a1k;

	// IIR coefficients
	assert (! fstb::is_null (a0z));
	const float    mult = 1.f / a0z;

	z_eq_b [0] = b0z * mult;
	z_eq_b [1] = b1z * mult;

	z_eq_a [0] = 1;
	z_eq_a [1] = a1z * mult;

#endif
}



void	TransSZBilin::map_s_to_z_one_pole_approx (fstb::Vf32 z_eq_b [2], fstb::Vf32 z_eq_a [2], const fstb::Vf32 s_eq_b [2], const fstb::Vf32 s_eq_a [2], fstb::Vf32 k) noexcept
{
	assert (z_eq_b != nullptr);
	assert (z_eq_a != nullptr);
	assert (s_eq_b != nullptr);
	assert (s_eq_a != nullptr);

	const auto     b0s  = fstb::Vf32::load (&s_eq_b [0]);
	const auto     b1s  = fstb::Vf32::load (&s_eq_b [1]);

	const auto     b1k = b1s * k;
	const auto     b1z = b0s - b1k;
	const auto     b0z = b0s + b1k;

	const auto     a0s  = fstb::Vf32::load (&s_eq_a [0]);
	const auto     a1s  = fstb::Vf32::load (&s_eq_a [1]);

	const auto     a1k = a1s * k;
	const auto     a1z = a0s - a1k;
	const auto     a0z = a0s + a1k;

	const auto     mult = a0z.rcp_approx2 ();

	(b0z * mult).store (&z_eq_b [0]);
	(b1z * mult).store (&z_eq_b [1]);

	fstb::Vf32 (1).store (&z_eq_a [0]);
	(a1z * mult  ).store (&z_eq_a [1]);
}



void	TransSZBilin::map_s_to_z_ap1_approx (float z_eq_b [2], float k) noexcept
{
	assert (z_eq_b != nullptr);

	z_eq_b [0] = map_s_to_z_ap1_approx_b0 (k);
	z_eq_b [1] = 1;
}



void	TransSZBilin::map_s_to_z_ap1_approx (fstb::Vf32 z_eq_b [2], fstb::Vf32 k) noexcept
{
	assert (z_eq_b != nullptr);

	map_s_to_z_ap1_approx_b0 (k).store (&z_eq_b [0]);
	fstb::Vf32 (1).store (&z_eq_b [1]);
}



float	TransSZBilin::map_s_to_z_ap1_approx_b0 (float k) noexcept
{
	const float    a1z = 1 - k;
	const float    a0z = 1 + k;

	// IIR coefficients
	assert (! fstb::is_null (a0z));

#if defined (fstb_HAS_SIMD)
	const auto     mult = fstb::Vf32 (a0z).rcp_approx2 ();
	const float    m1   = fstb::ToolsSimd::Shift <0>::extract (mult);
	const float    b0   = a1z * m1;
#else
	const float    b0   = a1z / a0z;
#endif

	return b0;
}



fstb::Vf32	TransSZBilin::map_s_to_z_ap1_approx_b0 (fstb::Vf32 k) noexcept
{
	const auto     one = fstb::Vf32 (1);
	const auto     a1z = one - k;
	const auto     a0z = one + k;

	// IIR coefficients
	assert (a0z != fstb::Vf32 (0));
	const auto     b0  = a1z / a0z;
	
	return b0;
}



void	TransSZBilin::map_s_to_z_ap2_approx (float z_eq_b [3], float s_eq_b1, float k) noexcept
{
	assert (z_eq_b != nullptr);

	const float    kk  = k * k;

	const float    a1k = s_eq_b1 * k;
	const float    a2kk_plus_a0 = kk + 1;
	const float    a0z = a2kk_plus_a0 + a1k;
	const float    a2z = a2kk_plus_a0 - a1k;
	const float    a1z = 2 * (1 - kk);

	// IIR coefficients
	assert (! fstb::is_null (a0z));
#if defined (fstb_HAS_SIMD)
	const auto     mult = fstb::Vf32 (a0z).rcp_approx2 ();
	const auto     axz  = fstb::Vf32::set_pair (a2z, a1z);
	const auto     z_eq = axz * mult;
	z_eq_b [0] = fstb::ToolsSimd::Shift <0>::extract (z_eq);
	z_eq_b [1] = fstb::ToolsSimd::Shift <1>::extract (z_eq);
#else
	const float    mult = 1.f / a0z;
	z_eq_b [0] = a2z * mult;
	z_eq_b [1] = a1z * mult;
#endif
	z_eq_b [2] = 1;
}



void	TransSZBilin::map_s_to_z_ap2_approx (fstb::Vf32 z_eq_b [3], fstb::Vf32 s_eq_b1, fstb::Vf32 k) noexcept
{
	assert (z_eq_b != nullptr);

	const auto     one = fstb::Vf32 (1);
	const auto     kk  = k * k;

	const auto     a1k = s_eq_b1 * k;
	const auto     a2kk_plus_a0 = kk + one;
	const auto     a0z = a2kk_plus_a0 + a1k;
	const auto     a2z = a2kk_plus_a0 - a1k;
	const auto     a1zh = one - kk;
	const auto     a1z = a1zh + a1zh;

	// IIR coefficients
	assert (a0z != fstb::Vf32 (0));
	const auto     mult = a0z.rcp_approx2 ();
	const auto     b0   = a2z * mult;
	const auto     b1   = a1z * mult;
	b0 .store (&z_eq_b [0]);
	b1 .store (&z_eq_b [1]);
	one.store (&z_eq_b [2]);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
