/*****************************************************************************

        TransSZBilin.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_iir_TransSZBilin_CODEHEADER_INCLUDED)
#define mfx_dsp_iir_TransSZBilin_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#if ! defined (NDEBUG)
	#include "fstb/fnc.h"
#endif
#include "fstb/ToolsSimd.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



double	TransSZBilin::prewarp_freq (double f0, double fs)
{
	assert (f0 > 0);
	assert (f0 < fs * 0.5);
	assert (fs > 0);

	const double   f_w = tan (f0 * fstb::PI / fs);
	assert (f_w > 0);

	return f_w;
}



/*
==============================================================================
Name: map_s_to_z
Description:
	Transforms the continous s-plane equation in a discrete z-plane equation
	which can be used directly to filter signal, using the bilinear transfrom.

        ---        k                  ---        -k
        \   a[k].s                    \   a'[k].z
        /__                           /__
H(s) = --------------   ==>   H(z) = ---------------
        ---       k                   ---        -k
        \   b[k].s                    \   b'[k].z
        /__                           /__

	The S-plane equation is relative to a given frequency f0, therefore a pole
	or zero with radius of 1 in the equation is located on f0.

Output parameters:
	- z_eq_b: Num. of the equation in the Z plane, index is the power of 1/z.
	- z_eq_a: Denom. of the equation in the Z plane.
	- fs: Sampling frequency, > 0.
	- f0: Main biquad frequency, in ]0 ; fs/2[
Input parameters:
	- s_eq_b: Num. of the equation in the S plane, index is the power of s.
	- s_eq_a: Denom. of the equation in the S plane.
Throws: Nothing
==============================================================================
*/

template <class TS, class TZ>
void	TransSZBilin::map_s_to_z (TZ z_eq_b [3], TZ z_eq_a [3], const TS s_eq_b [3], const TS s_eq_a [3], double f0, double fs)
{
	assert (z_eq_b != 0);
	assert (z_eq_a != 0);
	assert (s_eq_b != 0);
	assert (s_eq_a != 0);

	// s to z bilinear transform
	const double   inv_k = prewarp_freq (f0, fs);
	assert (! fstb::is_null (inv_k));
	const double   k  = 1 / inv_k;
	const double   kk = k*k;

	const double   b1k  = s_eq_b [1] * k;
	const double   b2kk = s_eq_b [2] * kk;
	const double   b2kk_plus_b0 = b2kk + s_eq_b [0];
	const double   b0z = b2kk_plus_b0 + b1k;
	const double   b2z = b2kk_plus_b0 - b1k;
	const double   b1z = 2 * (s_eq_b [0] - b2kk);

	const double   a1k  = s_eq_a [1] * k;
	const double   a2kk = s_eq_a [2] * kk;
	const double   a2kk_plus_a0 = a2kk + s_eq_a [0];
	const double   a0z = a2kk_plus_a0 + a1k;
	const double   a2z = a2kk_plus_a0 - a1k;
	const double   a1z = 2 * (s_eq_a [0] - a2kk);

	// IIR coefficients
	assert (! fstb::is_null (a0z));
	const double	mult = 1 / a0z;

	z_eq_b [0] = TZ (b0z * mult);
	z_eq_b [1] = TZ (b1z * mult);
	z_eq_b [2] = TZ (b2z * mult);

	z_eq_a [0] = TZ (1);
	z_eq_a [1] = TZ (a1z * mult);
	z_eq_a [2] = TZ (a2z * mult);
}



template <class TS, class TZ>
void	TransSZBilin::map_s_to_z_one_pole (TZ z_eq_b [2], TZ z_eq_a [2], const TS s_eq_b [2], const TS s_eq_a [2], double f0, double fs)
{
	assert (z_eq_b != 0);
	assert (z_eq_a != 0);
	assert (s_eq_b != 0);
	assert (s_eq_a != 0);

	// s to z bilinear transform
	const double   inv_k = prewarp_freq (f0, fs);
	assert (! fstb::is_null (inv_k));
	const double   k = 1 / inv_k;

	const double   b1k = s_eq_b [1] * k;
	const double   b1z = s_eq_b [0] - b1k;
	const double   b0z = s_eq_b [0] + b1k;

	const double   a1k = s_eq_a [1] * k;
	const double   a1z = s_eq_a [0] - a1k;
	const double   a0z = s_eq_a [0] + a1k;

	// IIR coefficients
	assert (! fstb::is_null (a0z));
	const double   mult = 1 / a0z;

	z_eq_b [0] = TZ (b0z * mult);
	z_eq_b [1] = TZ (b1z * mult);

	z_eq_a [0] = TZ (1);
	z_eq_a [1] = TZ (a1z * mult);
}



// 1st-order all-pass filter
template <class TS, class TZ>
void	TransSZBilin::map_s_to_z_ap1 (TZ z_eq_b [2], double f0, double fs)
{
	assert (z_eq_b != 0);

	// s to z bilinear transform
	const double   inv_k = prewarp_freq (f0, fs);
	assert (! fstb::is_null (inv_k));
	const double   k = 1 / inv_k;

	const double   a1z = 1 - k;
	const double   a0z = 1 + k;

	// IIR coefficients
	assert (! fstb::is_null (a0z));
	z_eq_b [0] = float (a1z / a0z);
	z_eq_b [1] = 1;
}



// 2nd-order all-pass filter
template <class TS, class TZ>
void	TransSZBilin::map_s_to_z_ap2 (TZ z_eq_b [3], TS s_eq_b1, double f0, double fs)
{
	assert (z_eq_b != 0);

	// s to z bilinear transform
	const double   inv_k = prewarp_freq (f0, fs);
	assert (! fstb::is_null (inv_k));
	const double   k = 1 / inv_k;
	const double   kk = k*k;

	const double   a1k = s_eq_b1 * k;
	const double   a2kk_plus_a0 = kk + 1;
	const double   a0z = a2kk_plus_a0 + a1k;
	const double   a2z = a2kk_plus_a0 - a1k;
	const double   a1z = 2 * (1 - kk);

	// IIR coefficients
	assert (! fstb::is_null (a0z));
	const double   mult = 1 / a0z;
	z_eq_b [0] = TZ (a2z * mult);
	z_eq_b [1] = TZ (a1z * mult);
	z_eq_b [2] = TZ (1);
}



/*
From:
k   = 1 / prewarp_freq (f0, fs)
bz0 =     bs0 + k * bs1 +    k * k * bs2
bz1 = 2 * bs0           -2 * k * k * bs2
bz2 =     bs0 - k * bs1 +    k * k * bs2

bz0 =     bs0 + kbs1 +    kkbs2
bz1 = 2 * bs0        -2 * kkbs2
bz2 =     bs0 - kbs1 +    kkbs2

BZ = M * BS
M^-1 * BZ = M^-1 * M * BS
BS = M^-1 * BZ

M =
	[ 1  1  1 ]
	[ 2  0 -2 ]
	[ 1 -1  1 ]

M^-1 = 0.25 *
	[ 1  1  1 ]
	[ 2  0 -2 ]
	[ 1 -1  1 ]
     = 0.25 * M

as0 = (az0 + az1 + az2) * 0.25
as1 = (az0       - az2) * 0.5  /  k
as2 = (az0 - az1 + az2) * 0.25 / (k * k)
*/

template <class TS, class TZ>
void	TransSZBilin::map_z_to_s (TS s_eq_b [3], TS s_eq_a [3], const TZ z_eq_b [3], const TZ z_eq_a [3], double f0, double fs)
{
	assert (z_eq_b != 0);
	assert (z_eq_a != 0);
	assert (s_eq_b != 0);
	assert (s_eq_a != 0);

	const double   inv_k = prewarp_freq (f0, fs);
	assert (! fstb::is_null (inv_k));
	const double   inv_kk = inv_k * inv_k;

	s_eq_a [0] = TS ((z_eq_a [0] + z_eq_a [1] + z_eq_a [2]) * 0.25         );
	s_eq_a [1] = TS ((z_eq_a [0]              - z_eq_a [2]) * 0.5  * inv_k );
	s_eq_a [2] = TS ((z_eq_a [0] - z_eq_a [1] + z_eq_a [2]) * 0.25 * inv_kk);

	s_eq_b [0] = TS ((z_eq_b [0] + z_eq_b [1] + z_eq_b [2]) * 0.25         );
	s_eq_b [1] = TS ((z_eq_b [0]              - z_eq_b [2]) * 0.5  * inv_k );
	s_eq_b [2] = TS ((z_eq_b [0] - z_eq_b [1] + z_eq_b [2]) * 0.25 * inv_kk);
}



template <class TS, class TZ>
void	TransSZBilin::map_z_to_s_one_pole (TS s_eq_b [2], TS s_eq_a [2], const TZ z_eq_b [2], const TZ z_eq_a [2], double f0, double fs)
{
	assert (z_eq_b != 0);
	assert (z_eq_a != 0);
	assert (s_eq_b != 0);
	assert (s_eq_a != 0);

	const double   inv_k = prewarp_freq (f0, fs);
	assert (! fstb::is_null (inv_k));

	s_eq_a [0] = TS ((z_eq_a [0] + z_eq_a [1]) * 0.5        );
	s_eq_a [1] = TS ((z_eq_a [0] - z_eq_a [1]) * 0.5 * inv_k);

	s_eq_b [0] = TS ((z_eq_b [0] + z_eq_b [1]) * 0.5        );
	s_eq_b [1] = TS ((z_eq_b [0] - z_eq_a [1]) * 0.5 * inv_k);
}



// f = f0 / fs
// returns an approximation of 1 / tan (pi * f)
// ~ (1 - (pi^2*f^2)/3 - (pi^4*f^4)/36) / (pi*f)
float	TransSZBilin::compute_k_approx (float f)
{
	const float    pi2      = float (fstb::PI * fstb::PI);
	const float    c0       = float (1 / fstb::PI);
	const float    c2       = float (-fstb::PI       /  3);
	const float    c4       = float (-fstb::PI * pi2 / 36);

	const float    f2       = f * f;
	const float    num      = (f2 * c4 + c2) * f2 + c0;
	const auto     den_v    = fstb::ToolsSimd::set1_f32 (f);
	const auto     invden_v = fstb::ToolsSimd::rcp_approx (den_v);
	const float    invden   = fstb::ToolsSimd::Shift <0>::extract (invden_v);
	const float    k        = num * invden;

	return k;
}



fstb::ToolsSimd::VectF32	TransSZBilin::compute_k_approx (fstb::ToolsSimd::VectF32 f)
{
	const float    pi2    = float (fstb::PI * fstb::PI);
	const auto     c0     = fstb::ToolsSimd::set1_f32 (float (1 / fstb::PI));
	const auto     c2     = fstb::ToolsSimd::set1_f32 (float (-fstb::PI       /  3));
	const auto     c4     = fstb::ToolsSimd::set1_f32 (float (-fstb::PI * pi2 / 36));

	const auto     f2     = f * f;
	const auto     num    = (f2 * c4 + c2) * f2 + c0;
	const auto     invden = fstb::ToolsSimd::rcp_approx (f);
	const auto     k      = num * invden;

	return k;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_iir_TransSZBilin_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
