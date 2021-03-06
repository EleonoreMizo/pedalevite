/*****************************************************************************

        Approx.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_Approx_CODEHEADER_INCLUDED)
#define fstb_Approx_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/fnc.h"

#if defined (_MSC_VER)
	#include <intrin.h>
#endif

#include <cassert>
#include <cmath>



namespace fstb
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// f(x) ~ sin (x)
// x in [-pi/2 ; pi/2]
// Max error: 2.411e-8
// Scaled formula
ToolsSimd::VectF32	Approx::sin_rbj (ToolsSimd::VectF32 x) noexcept
{
	const auto    a  = ToolsSimd::set1_f32 ( 2.628441118718695e-06f);
	const auto    b  = ToolsSimd::set1_f32 (-1.982061326014539e-04f);
	const auto    c  = ToolsSimd::set1_f32 ( 0.008333224441393f);
	const auto    d  = ToolsSimd::set1_f32 (-0.166666657479818f);
	const auto    e  = ToolsSimd::set1_f32 ( 1);
	const auto    x2 = x * x;

	return (((((a * x2 + b) * x2 + c) * x2 + d) * x2) + e) * x;
}



// f(x) ~ cos (x)
// x in [-pi ; pi]
// Max error: 2.411e-8
// Scaled formula
ToolsSimd::VectF32	Approx::cos_rbj (ToolsSimd::VectF32 x) noexcept
{
	x = ToolsSimd::abs (x);

	const auto     hp  = ToolsSimd::set1_f32 (float ( 0.5 * fstb::PI));
	x = hp - x;

	return sin_rbj (x);
}



// f1 (x) ~ sin (x)
// f2 (x) ~ cos (x)
// x in [-3*pi ; 3*pi]
// Scaled formula
void	Approx::cos_sin_rbj (ToolsSimd::VectF32 &c, ToolsSimd::VectF32 &s, ToolsSimd::VectF32 x) noexcept
{
	const auto     hp  = ToolsSimd::set1_f32 (float ( 0.5 * fstb::PI));
	const auto     hpm = ToolsSimd::set1_f32 (float (-0.5 * fstb::PI));
	const auto     tp  = ToolsSimd::set1_f32 (float ( 2   * fstb::PI));
	const auto     p   = ToolsSimd::set1_f32 (float (       fstb::PI));
	const auto     pm  = ToolsSimd::set1_f32 (float (      -fstb::PI));

	// x -> [-pi ; pi]
	x = restrict_angle_to_mpi_pi (x, pm, p, tp);

	auto           xs = x;

	// xs -> [-pi/2 ; pi/2]
	xs = restrict_sin_angle_to_mhpi_hpi (xs, hpm, hp, pm, p);

	auto           xc = x + hp;

	// xc -> [-pi ; pi]
	xc = fstb::ToolsSimd::select (
		fstb::ToolsSimd::cmp_gt_f32 (xc, p ), xc - tp, xc
	);

	// xc -> [-pi/2 ; pi/2]
	xc = restrict_sin_angle_to_mhpi_hpi (xc, hpm, hp, pm, p);

	s = sin_rbj (xs);
	c = sin_rbj (xc);
}



// f(x) ~ sin (x * pi/2)
// x in [-1 ; 1]
// Max error: 2.411e-8
// Original formula
template <typename T>
constexpr T	Approx::sin_rbj_halfpi (T x) noexcept
{
	constexpr T    a  = T ( 0.0001530302);
	constexpr T    b  = T (-0.0046768800);
	constexpr T    c  = T ( 0.0796915849);
	constexpr T    d  = T (-0.6459640619);
	constexpr T    e  = T ( 1.5707963268);
	const T        x2 = x * x;

	return (((((a * x2 + b) * x2 + c) * x2 + d) * x2) + e) * x;

	// Other coefficients found by Olli Niemitalo
	// Constaints: f(0) = 0, f(1) = 1, f'(1) = 0, odd-symmetry
	// https://dsp.stackexchange.com/questions/46629/finding-polynomial-approximations-of-a-sine-wave/46761#46761
	// 5th order, continuous derivative, peak harmonic distortion -78.99 dB
	//	   1.569778813, -0.6395576276, 0.06977881382
	// 7th order, continuous derivative, peak harmonic distortion -123.8368 dB
	//    1.570781972, -0.6458482979, 0.07935067784, -0.004284352588
	// 5th order, peak harmonic distortion -92.6 dB
	//    1.570034357, -0.6425216143, 0.07248725712
	// 7th order, peak harmonic distortion -133.627 dB
	//    1.5707953785726114835, -0.64590724797262922190, 0.079473610232926783079, -0.0043617408329090447344
}



ToolsSimd::VectF32	Approx::sin_rbj_halfpi (ToolsSimd::VectF32 x) noexcept
{
	const auto    a  = ToolsSimd::set1_f32 ( 0.0001530302f);
	const auto    b  = ToolsSimd::set1_f32 (-0.0046768800f);
	const auto    c  = ToolsSimd::set1_f32 ( 0.0796915849f);
	const auto    d  = ToolsSimd::set1_f32 (-0.6459640619f);
	const auto    e  = ToolsSimd::set1_f32 ( 1.5707963268f);
	const auto    x2 = x * x;

	return (((((a * x2 + b) * x2 + c) * x2 + d) * x2) + e) * x;
}



// f(x) ~ sin (x * pi)
// x in [-0.5 ; 1]
// Max error: 2.411e-8
ToolsSimd::VectF32	Approx::sin_rbj_pi (ToolsSimd::VectF32 x) noexcept
{
	const auto     one   = ToolsSimd::set1_f32 (1);
	const auto     two_x = one - ToolsSimd::abs (one - x - x);

	return sin_rbj_halfpi (two_x);
}



// f1(x) ~ sin (x * pi)
// f2(x) ~ sin (x * pi/2)
// x in [-0.5 ; 1]
// Max error: 2.411e-8
void	Approx::sin_rbj_halfpi_pi (float &sx, float &s2x, float x) noexcept
{
	const auto     xv = ToolsSimd::set_2f32 (x, 1 - fabsf (1 - 2*x));
	const auto     yv = sin_rbj_halfpi (xv);
	sx  = ToolsSimd::Shift <0>::extract (yv);
	s2x = ToolsSimd::Shift <1>::extract (yv);
}



// f1(x) ~ sin (x)
// x in [-pi ; pi]
// Max error: 0.919e-3
template <typename T>
T	Approx::sin_nick (T x) noexcept
{
	assert (x >= T (-fstb::PI));
	assert (x <= T (+fstb::PI));

	constexpr T    b = T ( 4 /  fstb::PI);
	constexpr T    c = T (-4 / (fstb::PI * fstb::PI));
	constexpr T    d = 0.224008f;

	const T        y = b * x + c * x * std::abs (x);
	const T        z = d * (y * std::abs (y) - y) + y;

	return z;
}

ToolsSimd::VectF32	Approx::sin_nick (ToolsSimd::VectF32 x) noexcept
{
	const auto     b = ToolsSimd::set1_f32 (float ( 4 /  fstb::PI));
	const auto     c = ToolsSimd::set1_f32 (float (-4 / (fstb::PI * fstb::PI)));
	const auto     d = ToolsSimd::set1_f32 (0.224008f);

	const auto     y = b * x + c * x * ToolsSimd::abs (x);
	const auto     z = d * (y * ToolsSimd::abs (y) - y) + y;

	return z;
}



// f1(x) ~ sin (x * 2 * pi)
// x in [-0.5 ; 0.5]
// Max error: 0.919e-3
template <typename T>
T	Approx::sin_nick_2pi (T x) noexcept
{
	assert (x >= T (-0.5));
	assert (x <= T (+0.5));

	constexpr T    b = T (  8);
	constexpr T    c = T (-16);
	constexpr T    d = T (  0.224008);

	const float    y = b * x + c * x * std::abs (x);
	const float    z = d * (y * std::abs (y) - y) + y;

	return z;
}

ToolsSimd::VectF32	Approx::sin_nick_2pi (ToolsSimd::VectF32 x) noexcept
{
	const auto     b = ToolsSimd::set1_f32 (  8);
	const auto     c = ToolsSimd::set1_f32 (-16);
	const auto     d = ToolsSimd::set1_f32 (  0.224008f);

	const auto     y = b * x + c * x * ToolsSimd::abs (x);
	const auto     z = d * (y * ToolsSimd::abs (y) - y) + y;

	return z;
}



// Retuns { cos, sin }
template <typename T>
std::array <T, 2>	Approx::cos_sin_nick_2pi (T x) noexcept
{
	assert (x >= T (-0.5));
	assert (x <= T (+0.5));

	// cos (x) = sin (x + pi/2)
	const auto     xc = (x >= T (0.25)) ? x - T (0.75) : x + T (0.25);

	constexpr T    b = T (  8);
	constexpr T    c = T (-16);
	constexpr T    d = T (  0.224008);

	const T        yc = b * xc + c * xc * std::abs (xc);
	const T        ys = b * x  + c * x  * std::abs (x );
	const T        zc = d * (yc * std::abs (yc) - yc) + yc;
	const T        zs = d * (ys * std::abs (ys) - ys) + ys;

	return std::array <T, 2> { zc, zs };
}

std::array <ToolsSimd::VectF32, 2>	Approx::cos_sin_nick_2pi (ToolsSimd::VectF32 x) noexcept
{
	// cos (x) = sin (x + pi/2)
	const auto     c_025  = ToolsSimd::set1_f32 (0.25f);
	const auto     c_075  = ToolsSimd::set1_f32 (0.75f);
	const auto     ge_025 = ToolsSimd::cmp_lt_f32 (c_025, x);
	const auto     xc     = ToolsSimd::select (ge_025, x - c_075, x + c_025);

	const auto     b  = ToolsSimd::set1_f32 (  8);
	const auto     c  = ToolsSimd::set1_f32 (-16);
	const auto     d  = ToolsSimd::set1_f32 (  0.224008f);

	const auto     yc = b * xc + c * xc * ToolsSimd::abs (xc);
	const auto     ys = b * x  + c * x  * ToolsSimd::abs (x );
	const auto     zc = d * (yc * ToolsSimd::abs (yc) - yc) + yc;
	const auto     zs = d * (ys * ToolsSimd::abs (ys) - ys) + ys;

	return std::array <ToolsSimd::VectF32, 2> { zc, zs };
}



float	Approx::log2 (float val) noexcept
{
	assert (val > 0);

	Combo32        combo { val };
	int            x     = combo._i;
	const int      log_2 = ((x >> 23) & 255) - 128;
	x        &= ~(255 << 23);
	x        +=   127 << 23;
	combo._i  = x;
	val       = combo._f;

	const float		k = 0.5f;
	const float		a = (k - 1  ) / (k + 1);
	const float		b = (4 - 2*k) / (k + 1);	// 1 - 3*a
	const float		c = 2*a;
	val = (a * val + b) * val + c;

	return (val + float (log_2));
}



/*
Piece-wise linear approximation
Warning, log2_crude (1) != 0
Errors:
RMS:  0.0236228
mean: -0.0145537
min:  -0.0430415 @ 0.180279
max:  0.0430603 @ 0.125
Source: Evan Balster
https://github.com/romeric/fastapprox/pull/5
*/
float	Approx::log2_crude (float val) noexcept
{
	assert (val > 0);

	Combo32        xv { val };
	Combo32        lv;
	lv._i = 0x43800000 | (xv._i >> 8);

	return lv._f - 382.95695f;
}



float	Approx::exp2 (float val) noexcept
{
	// Truncated val for integer power of 2
	const int      tx = floor_int (val);

	// Float remainder of power of 2
	val -= static_cast <float> (tx);

	// Quadratic approximation of 2^x in [0 ; 1]
	const float    a = 1.0f / 3.0f;
	const float    b = 2.0f / 3.0f;
	const float    c = 1.0f;
	val = (a * val + b) * val + c;

	Combo32        combo { val };

	// Add integer power of 2 to exponent
	combo._i += tx << 23;
	val       = combo._f;

	assert (val >= 0);

	return (val);
}



/*
Piece-wise linear approximation
Warning, exp2_crude (0) != 1
Errors:
RMS:  0.0204659
mean: 0.0103297
min:  -0.0294163 @ 1.04308
max:  0.0302728 @ 1.48549
Source: Evan Balster
https://github.com/romeric/fastapprox/pull/5
*/
float	Approx::exp2_crude (float val) noexcept
{
	Combo32        xv { val + 382.95695f };
	xv._i = ((((xv._i < 0x43808000) ? 0 : xv._i) << 8) & 0x7FFFFF00);

	return xv._f;
}



/*
Possible coefficients (found by Andrew Simper):
5th order, max error 2.44e-7:
	1.000000000000000
	0.69315168779795
	0.2401596780318
	0.055817593635
	0.008992164746
	0.001878875789
7th order, max error 1.64e-7:
	1.000000000000000
	0.693147180559945
	0.2402265069591007
	0.0555044941070
	0.009615262656
	0.001341316600
	0.000143623130
	0.000021615988
https://www.kvraudio.com/forum/viewtopic.php?p=7677357#p7677357
*/
float	Approx::exp2_5th (float val) noexcept
{
	// Truncated val for integer power of 2
	const int      tx = floor_int (val);

	// Float remainder of power of 2
	val -= static_cast <float> (tx);

	// Quadratic approximation of 2^x in [0 ; 1]
	const float    c0 = 1;
	const float    c1 = 0.69315168779795f;
	const float    c2 = 0.2401596780318f;
	const float    c3 = 0.055817593635f;
	const float    c4 = 0.008992164746f;
	const float    c5 = 0.001878875789f;
	val = ((((c5 * val + c4) * val + c3) * val + c2) * val + c1) * val + c0;

	union
	{
		int32_t        _i;
		float          _f;
	}              combo;
	combo._f = val;

	// Add integer power of 2 to exponent
	combo._i += tx << 23;
	val       = combo._f;

	assert (val >= 0);

	return (val);
}



// Approximation is good close to 0, but diverges for larger absolute values.
// A is the approximation accuracy (the bigger, the larger the valid range)
// A = 10 is a good start
template <int A, typename T>
T	Approx::exp_m (T val) noexcept
{
	static_assert (A > 0, "A must be strictly positive");
	static_assert (A <= 16, "A is too large, precision will suffer.");

	const int      p = 1 << A;
	const T        x = T (1) + val * (T (1) / p);

	return ipowpc <p> (x);
}



// Crude approximation, a > 0
// Errors vary mostly with the absolute value of b
// Source: Aleksey Voxengo
double	Approx::pow (double a, double b) noexcept
{
	assert (a > 0);

#if (fstb_ENDIAN == fstb_ENDIAN_BIG)
	const int      lsw = 1;
	const int      msw = 0;
#else
	const int      lsw = 0;
	const int      msw = 1;
#endif

	union
	{
		double         _d;
		int32_t        _x [2];
	} u;

	u._d       = a;
	u._x [msw] = int32_t (b * (u._x [msw] - 0x3FEEC825) + 0x3FEEDD44);
	u._x [lsw] = 0;

	return u._d;
}



/*
==============================================================================
Name: fast_partial_exp2_int_16_to_int_32
Description:
	Compute a fixed-point approximation of the following function :
	[0 ; 1[ -> [0.5 ; 1[
	  f : x -> 2^(x-1)
	The approximation uses the following polynomial :
	f(x) = (((x+1)^2) + 2) / 6
Input parameters :
	- val: Unsigned linear input, Fixed-point 0:16 data. Integer part
		is automatically discarded.
Returns: Unsigned exponential output, fixed-point 0:32 data, range is
	[0x80000000 ; 0xFFFFFFFF]
Throws: Nothing
==============================================================================
*/

uint32_t	Approx::fast_partial_exp2_int_16_to_int_32 (int val) noexcept
{
	const uint32_t c2 = 1431655766U;

#if defined (_MSC_VER)

	const uint32_t c0 = 0x80000000U;
	const uint32_t c1 = 2863311531U;

	uint32_t       result = val;
	result <<= 15;
	result  |= c0;
	result   = uint32_t (__emulu (result, result) >> 32);
	result   = uint32_t (__emulu (result,     c1) >> 32);
	result  += c2;

#else

	const int      resol = 16;
	const uint32_t mask  = (1 << resol) - 1;

	val &= mask;
	int64_t        step_64 (val + (1 << resol));
	step_64 *= step_64;
	step_64 *= (1 << (62 - (resol + 1) * 2)) / 3 + 1820;
	uint32_t       result = uint32_t (step_64 >> (62 - 32 - 1));
	result += c2;

#endif

	assert ((result & 0x80000000U) != 0);

	return result;
}



// Way more accurate (error < 8e-5 -> ~0.08 cent)
// Polynomial approximation by Olli Niemitalo
// y = (((1/150 * x + 2/75) * x + 3/25) * x + 26/75) * x + 0.5
uint32_t	Approx::fast_partial_exp2_int_16_to_int_32_4th (int val) noexcept
{
	const uint32_t c0 = 0x80000000U;
	const uint32_t c1 = 0x58BF258C;
	const uint32_t c2 = 0x1EB851EB;
	const uint32_t c3 = 0x06D3A06D;
	const uint32_t c4 = 0x01B4E81B;

#if defined (_MSC_VER)

	const uint32_t x_32 = uint32_t (val) << 16;
	uint32_t       result = uint32_t (__emulu (c4,     x_32) >> 32);
	result += c3;
	result  = uint32_t (__emulu (result, x_32) >> 32);
	result += c2;
	result  = uint32_t (__emulu (result, x_32) >> 32);
	result += c1;
	result  = uint32_t (__emulu (result, x_32) >> 32);
	result += c0;

#else

	const uint32_t x_32 = uint32_t (val) << 16;
	uint32_t       result  = uint32_t ((c4 * uint64_t (x_32)) >> 32);
	result += c3;
	result  = uint32_t ((result * uint64_t (x_32)) >> 32);
	result += c2;
	result  = uint32_t ((result * uint64_t (x_32)) >> 32);
	result += c1;
	result  = uint32_t ((result * uint64_t (x_32)) >> 32);
	result += c0;

#endif

	assert ((result & 0x80000000U) != 0);

	return (result);
}



// Errors:
// below 0.01% up to pi/8
// below 1.33% up to pi/4
// https://www.desmos.com/calculator/6ghwlhxumj
template <typename T>
constexpr T	Approx::tan_taylor5 (T x) noexcept
{
	const T        x_2 = x * x;
	constexpr T    c_1 = T (1);
	constexpr T    c_3 = T (1) / T (3);
	constexpr T    c_5 = T (2) / T (15);

	return ((c_5 * x_2 + c_3) * x_2 + c_1) * x;
}

ToolsSimd::VectF32	Approx::tan_taylor5 (ToolsSimd::VectF32 x) noexcept
{
	const auto     x_2 = x * x;
	const auto     c_1 = fstb::ToolsSimd::set1_f32 (1        );
	const auto     c_3 = fstb::ToolsSimd::set1_f32 (1.0f / 3 );
	const auto     c_5 = fstb::ToolsSimd::set1_f32 (2.0f / 15);

	return ((c_5 * x_2 + c_3) * x_2 + c_1) * x;
}



// Formula by mystran
// https://www.kvraudio.com/forum/viewtopic.php?p=7491289#p7491289
// tan x = sin (x) / cos (x)
//       = sin (x) / sqrt (1 - sin (x) ^ 2)
// https://www.desmos.com/calculator/6ghwlhxumj
template <typename T>
T	Approx::tan_mystran (T x) noexcept
{
	constexpr T    c1 = T ( 1);
	constexpr T    c3 = T (-1) / T (6);
	constexpr T    c5 = T ( 1) / T (120);
	constexpr T    c7 = T (-1) / T (5040);

	const T        x2 = x * x;
	const T        s  = (((c7 * x2 + c5) * x2 + c3) * x2 + c1) * x;
	const T        c  = T (sqrt (1 - s * s));

	return s / c;
}

ToolsSimd::VectF32	Approx::tan_mystran (ToolsSimd::VectF32 x) noexcept
{
	const auto     c1 = fstb::ToolsSimd::set1_f32 ( 1         );
	const auto     c3 = fstb::ToolsSimd::set1_f32 (-1.f / 6   );
	const auto     c5 = fstb::ToolsSimd::set1_f32 ( 1.f / 120 );
	const auto     c7 = fstb::ToolsSimd::set1_f32 (-1.f / 5040);

	const auto     x2 = x * x;
	auto           s  = c7;
	s  = fstb::ToolsSimd::fmadd (s, x2, c5);
	s  = fstb::ToolsSimd::fmadd (s, x2, c3);
	s  = fstb::ToolsSimd::fmadd (s, x2, c1);
	s *= x;

	return s * fstb::ToolsSimd::rsqrt_approx2 (c1 - s * s);
}



/*
Very high precision. Relative error is 1 % at 0.9993 * pi/2
PadeApproximant [Tan[x],{x,0,{5,5}}]
With a = 15, maximum relative error: -1.35e-8 on range [-pi/4 ; pi/4]

If we use the identity tan (x) = sign (x) / tan (pi/2 - |x|), we can
make the function continuous (C1 at least) by using
a = (-967680 + 241920 * pi + 26880 * pi^2 - 1680 * pi^3 + pi^5) / (4 * pi^4)
  = 14.999975509385927280627711005255
Maximum relative error becomes 4.081e-9 on the whole ]-pi/2 ; pi/2[ range.
Actually the function behaves globally better on the whole range even when the
identity is not used. The relative error is larger below +/-0.7, but this is
not really a problem as the precision was unnecessary high near 0.
*/

template <typename T>
constexpr T	Approx::tan_pade55 (T x) noexcept
{
//	constexpr T    a   = 15;
	constexpr T    a   = T (14.999975509385927280627711005255);
	const T        x2  = x * x;
	const T        num = (    x2 - 105) * x2 + 945;
	const T        den = (a * x2 - 420) * x2 + 945;

	return x * num / den;
}

ToolsSimd::VectF32	Approx::tan_pade55 (ToolsSimd::VectF32 x) noexcept
{
	const auto     c0  = fstb::ToolsSimd::set1_f32 ( 945.f);
	const auto     n2  = fstb::ToolsSimd::set1_f32 (-105.f);
	const auto     d2  = fstb::ToolsSimd::set1_f32 (-420.f);
//	const auto     d4  = fstb::ToolsSimd::set1_f32 (  15.f);
	const auto     d4  = fstb::ToolsSimd::set1_f32 (14.999975509385927280627711005255f);

	const auto     x2  = x * x;
	const auto     num = (     x2 + n2) * x2 + c0;
	const auto     den = (d4 * x2 + d2) * x2 + c0;

	return x * num * fstb::ToolsSimd::rcp_approx2 (den);
}



/*
Formula by Jim Shima
http://dspguru.com/dsp/tricks/fixed-point-atan2-with-self-normalization/
Max error: +/-6.2e-3 rad
The original formula minimizes the average absolute error, so a coefficient
was tweaked to reduce the maximum error (1.015e-2 previously).

order-5 version:
	c5  = -0.0775572
	c3  =  0.287314
	c1  = -sum(c*)
	err = 8.4e-4
order-7 version:
	c7  =  0.0386379
	c5  = -0.145917
	c3  =  0.321088
	c1  = -sum(c*)
	err = 1.2e-4
FindFit[
	Table[Pi/4 - ArcTan[(x-1)/999], {x, 1000}],
	Pi/4 + (-Pi/4 - c3 - c5 - c7) ((x-1)/999)
		   + c3 ((x-1)/999)^3
		   + c5 ((x-1)/999)^5
		   + c7 ((x-1)/999)^7,
	{c7, c5, c3},
	x
]
*/

template <typename T>
constexpr T	Approx::atan2_3th (T y, T x) noexcept
{
	constexpr T    c3 = T (0.18208); // Original formula: 0.1963
	constexpr T    c1 = - T (fstb::PI * 0.25) - c3;
	const auto     b  = atan2_beg (y, x);
	const auto     r  = b [0];
	const auto     c0 = b [1];
	const auto     r2 = r * r;
	T              a  = ((c3 * r2) + c1) * r + c0;
	if (y < T (0))
	{
		a = -a;
	}

	return a;
}

ToolsSimd::VectF32	Approx::atan2_3th (ToolsSimd::VectF32 y, ToolsSimd::VectF32 x) noexcept
{
	using TS = fstb::ToolsSimd;

	const auto     c3 = TS::set1_f32 (0.18208f);
	const auto     c1 = TS::set1_f32 (float (fstb::PI * -0.25 - 0.18208));

	const auto     ys = TS::signbit (y);
	const auto     b  = atan2_beg (y, x);
	const auto     r  = b [0];
	const auto     c0 = b [1];
	const auto     r2 = r * r;
	auto           a  = c3;
	a = TS::fmadd (a, r2, c1);
	a = TS::fmadd (a, r , c0);
	a = TS::xor_f32 (a, ys);

	return a;
}



template <typename T>
constexpr T	Approx::atan2_7th (T y, T x) noexcept
{
	const auto     c7 = T ( 0.0386379);
	const auto     c5 = T (-0.145917);
	const auto     c3 = T ( 0.0386379);
	const auto     c1 = T (fstb::PI * -0.25 - 0.0386379 - -0.145917 - 0.0386379);

	const auto     b  = atan2_beg (y, x);
	const auto     r  = b [0];
	const auto     c0 = b [1];
	const auto     r2 = r * r;
	T              a  = ((((c7 * r2 + c5) * r2 + c3) * r2) + c1) * r + c0;
	if (y < T (0))
	{
		a = -a;
	}

	return a;
}

ToolsSimd::VectF32	Approx::atan2_7th (ToolsSimd::VectF32 y, ToolsSimd::VectF32 x) noexcept
{
	using TS = fstb::ToolsSimd;

	const auto     c7 = TS::set1_f32 ( 0.0386379f);
	const auto     c5 = TS::set1_f32 (-0.145917f);
	const auto     c3 = TS::set1_f32 ( 0.0386379f);
	const auto     c1 = TS::set1_f32 (float (
		fstb::PI * -0.25 - 0.0386379 - -0.145917 - 0.0386379
	));

	const auto     ys = TS::signbit (y);
	const auto     b  = atan2_beg (y, x);
	const auto     r  = b [0];
	const auto     c0 = b [1];
	const auto     r2 = r * r;
	auto           a  = c7;
	a = TS::fmadd (a, r2, c5);
	a = TS::fmadd (a, r2, c3);
	a = TS::fmadd (a, r2, c1);
	a = TS::fmadd (a, r , c0);
	a = TS::xor_f32 (a, ys);

	return a;
}



// Formula by mystran
// https://www.kvraudio.com/forum/viewtopic.php?f=33&t=521377
// s (x) = x / sqrt (1 + x^2)
// h3 (x) = s (x + 0.183 * x^3)
// Max error: 3.64e-3
// https://www.desmos.com/calculator/sjxol8khaz
template <typename T>
T	Approx::tanh_mystran (T x) noexcept
{
	constexpr T    p   = T (0.183);

	// x <- x + 0.183 * x^3
	T              x2 = x * x;
	x += x * x2 * p;

	// x <- x / sqrt (1 + x^2)
	x2 = x * x;
	x /= T (sqrt (T (1) + x2));

	return x;
}

ToolsSimd::VectF32	Approx::tanh_mystran (ToolsSimd::VectF32 x) noexcept
{
	const auto     p   = fstb::ToolsSimd::set1_f32 (0.183f);
	const auto     one = fstb::ToolsSimd::set1_f32 (1.0f);

	// x <- x + 0.183 * x^3
	auto           x2 = x * x;
	x += x * x2 * p;

	// x <- x / sqrt (1 + x^2)
	x2 = x * x;
	x *= fstb::ToolsSimd::rsqrt_approx2 (one + x2);

	return x;
}



// Formula by 2DaT
// https://www.kvraudio.com/forum/viewtopic.php?p=7503081#p7503081
// Max error: 3.14e-6
// https://www.desmos.com/calculator/sjxol8khaz
template <typename T>
T	Approx::tanh_2dat (T x) noexcept
{
	constexpr T    n0      = T (4.351839500e+06);
	constexpr T    n1      = T (5.605646250e+05);
	constexpr T    n2      = T (1.263485352e+04);
	constexpr T    n3      = T (4.751771164e+01);
	constexpr T    d0      = n0;
	constexpr T    d1      = T (2.011170000e+06);
	constexpr T    d2      = T (1.027901563e+05);
	constexpr T    d3      = T (1.009453430e+03);
	constexpr T    max_val = T (7.7539052963256836);

	const T        s   = x;
	x = std::min (std::abs (x), max_val);
	const T        x2  = x * x;
	const T        xs  = (s < 0) ? -x : x;
	const T        num = (((     n3  * x2 + n2) * x2 + n1) * x2 + n0) * xs;
	const T        den = (((x2 + d3) * x2 + d2) * x2 + d1) * x2 + d0;

	return num / den;
}

ToolsSimd::VectF32	Approx::tanh_2dat (ToolsSimd::VectF32 x) noexcept
{
	const auto     n0      = fstb::ToolsSimd::set1_f32 (4.351839500e+06f);
	const auto     n1      = fstb::ToolsSimd::set1_f32 (5.605646250e+05f);
	const auto     n2      = fstb::ToolsSimd::set1_f32 (1.263485352e+04f);
	const auto     n3      = fstb::ToolsSimd::set1_f32 (4.751771164e+01f);
	const auto     d0      = n0;
	const auto     d1      = fstb::ToolsSimd::set1_f32 (2.011170000e+06f);
	const auto     d2      = fstb::ToolsSimd::set1_f32 (1.027901563e+05f);
	const auto     d3      = fstb::ToolsSimd::set1_f32 (1.009453430e+03f);
	const auto     max_val = fstb::ToolsSimd::set1_f32 (7.7539052963256836f);

	const auto     s   = fstb::ToolsSimd::signbit (x);
	x = fstb::ToolsSimd::abs (x);
	x = fstb::ToolsSimd::min_f32 (x, max_val);
	const auto     x2  = x * x;
	const auto     xs  = fstb::ToolsSimd::xor_f32 (x, s);
	auto           num = n3;
	num  = fstb::ToolsSimd::fmadd (num, x2, n2);
	num  = fstb::ToolsSimd::fmadd (num, x2, n1);
	num  = fstb::ToolsSimd::fmadd (num, x2, n0);
	num *= xs;
	auto           den = x2 + d3;
	den  = fstb::ToolsSimd::fmadd (den, x2, d2);
	den  = fstb::ToolsSimd::fmadd (den, x2, d1);
	den  = fstb::ToolsSimd::fmadd (den, x2, d0);

	return fstb::ToolsSimd::div_approx2 (num, den);
}



// This one fits in the range [0, 4] and matches the derivative at 4 to zero
// so you have a smooth C1 at the point of clipping
// ends at just under 1, but has a normalised error of +-6.5e-4
// tanh_approx(4) = 0.9998
// tanh(4) = 0.999329
// Source: Andrew Simper
// https://discord.com/channels/507604115854065674/507630527847596046/702375822941618207
template <typename T>
T	Approx::tanh_andy (T x) noexcept
{
	x = fstb::limit (x, T (-4), T (+4));

	constexpr T    n3  = T (0.0812081221471);
	constexpr T    n1  = T (1);
	constexpr T    d4  = T (0.00624523306500);
	constexpr T    d2  = T (0.412523749044);
	constexpr T    d0  = T (1);

	const T        x2  = x * x;
	const T        num = (n3 * x2 + n1) * x;
	const T        den = (d4 * x2 + d2) * x2 + d0;

	return num / den;
}



// Approximates 1 / sqrt (x)
// P  relative error
// 0: 3.5e-2
// 1: 8.92e-4
// 2: 4.7e-6
// 3: ?
// Ref:
// Chris Lomont, Fast Inverse Square Root, 2003-02
// Robin Green, Even Faster Math Functions, 2020-03, GDC
template <int P>
float	Approx::rsqrt (float x) noexcept
{
	static_assert (
		(P >= 0 && P <= 3),
		"The number of Newton iterations must be in [0 ; 3]"
	);
	assert (x >= 0);

	constexpr int     cs  =
		  (P == 0) ? 0x5F37624F
		: (P == 2) ? 0x5F37599E
		:            0x5F375A86;

	const float    xh = x * 0.5f;
	union
	{
		int32_t        _i;
		float          _f;
	}              c;
	c._f = x;
	c._i = cs - (c._i >> 1);
	x    = c._f;

	constexpr float   nr1 = (P == 1) ? 1.5008909f : 1.5f;
	if (P > 0) { x *= nr1  - xh * x * x; }
	if (P > 1) { x *= 1.5f - xh * x * x; }
	if (P > 2) { x *= 1.5f - xh * x * x; }

	return x;
}

template <int P>
double	Approx::rsqrt (double x) noexcept
{
	static_assert (
		(P >= 0 && P <= 4),
		"The number of Newton iterations must be in [0 ; 4]"
	);
	assert (x >= 0);

	const double   xh = x * 0.5;
	union
	{
		int64_t        _i;
		double         _f;
	}              c;
	c._f = x;
	c._i = 0x5FE6EC85E7DE30DALL - (c._i >> 1);
	x    = c._f;

	if (P > 0) { x *= 1.5 - xh * x * x; }
	if (P > 1) { x *= 1.5 - xh * x * x; }
	if (P > 2) { x *= 1.5 - xh * x * x; }
	if (P > 3) { x *= 1.5 - xh * x * x; }

	return x;
}



// Approximation of the Wright Omega function:
// omega (x) = W0 (exp (x))
// where W0 is one of the Lambert W functions.
// Formula from:
// Stefano D'Angelo, Leonardo Gabrielli, Luca Turchet,
// Fast Approximation of the Lambert W Function for Virtual Analog Modelling,
// DAFx-19
template <typename T>
T	Approx::wright_omega_3 (T x) noexcept
{
	const T        a  = T (-1.314293149877800e-3);
	const T        b  = T ( 4.775931364975583e-2);
	const T        c  = T ( 3.631952663804445e-1);
	const T        d  = T ( 6.313183464296682e-1);
	const T        x1 = T (-3.341459552768620);
	const T        x2 = T ( 8.0);

	T              y  = T ( 0);
	if (x >= x2)
	{
		y = x - T (Approx::log2 (float (x))) * T (LN2);
	}
	else if (x > x1)
	{
		y = (((a * x + b) * x) + c) * x + d;
	}

	return y;
}

ToolsSimd::VectF32	Approx::wright_omega_3 (ToolsSimd::VectF32 x) noexcept
{
	const auto     a   = ToolsSimd::set1_f32 (-1.314293149877800e-3f);
	const auto     b   = ToolsSimd::set1_f32 ( 4.775931364975583e-2f);
	const auto     c   = ToolsSimd::set1_f32 ( 3.631952663804445e-1f);
	const auto     d   = ToolsSimd::set1_f32 ( 6.313183464296682e-1f);
	const auto     x1  = ToolsSimd::set1_f32 (-3.341459552768620f);
	const auto     x2  = ToolsSimd::set1_f32 ( 8.0f);
	const auto     ln2 = ToolsSimd::set1_f32 (float (LN2));

	const auto     y0  = ToolsSimd::set_f32_zero ();
	const auto     y1  = (((a * x + b) * x) + c) * x + d;
	const auto     y2  = x - (ToolsSimd::log2_approx (x)) * ln2;

	const auto     tx1 = fstb::ToolsSimd::cmp_lt_f32 (x, x1);
	const auto     tx2 = fstb::ToolsSimd::cmp_lt_f32 (x, x2);
	auto           y   = y0;
	y = fstb::ToolsSimd::select (tx1, y, y1);
	y = fstb::ToolsSimd::select (tx2, y, y2);

	return y;
}



// One Newton-Raphson iteration added
template <typename T>
T	Approx::wright_omega_4 (T x) noexcept
{
	T              y = wright_omega_3 (x);
	y -=
		  (y - Approx::exp2 (float (x - y) * float (LOG2_E)))
		/ (y + T (1));

	return y;
}

ToolsSimd::VectF32	Approx::wright_omega_4 (ToolsSimd::VectF32 x) noexcept
{
	const auto     one     = ToolsSimd::set1_f32 (1);
	const auto     l2e     = ToolsSimd::set1_f32 (float (LOG2_E));

	auto           y       = wright_omega_3 (x);
	const auto     num     = y - ToolsSimd::exp2_approx ((x - y) * l2e);
	const auto     den_inv = ToolsSimd::rcp_approx2 (y + one);
	y -= num * den_inv;

	return y;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// [-3*pi ; 3*pi] -> [-pi ; pi]
// p = pi, pm = -pi, tp = 2*pi
ToolsSimd::VectF32	Approx::restrict_angle_to_mpi_pi (ToolsSimd::VectF32 x, const ToolsSimd::VectF32 &pm, const ToolsSimd::VectF32 &p, const ToolsSimd::VectF32 &tp) noexcept
{
	x = fstb::ToolsSimd::select (
		fstb::ToolsSimd::cmp_lt_f32 (x, pm), x + tp, x
	);
	x = fstb::ToolsSimd::select (
		fstb::ToolsSimd::cmp_gt_f32 (x, p ), x - tp, x
	);

	return x;
}



// [-pi ; pi] -> [-pi/2 ; pi/2]
// hpm = -pi/2, hp = pi/2, pm = -pi, p = pi
ToolsSimd::VectF32	Approx::restrict_sin_angle_to_mhpi_hpi (ToolsSimd::VectF32 x, const ToolsSimd::VectF32 &hpm, const ToolsSimd::VectF32 &hp, const ToolsSimd::VectF32 &pm, const ToolsSimd::VectF32 &p) noexcept
{
	x = fstb::ToolsSimd::select (
		fstb::ToolsSimd::cmp_lt_f32 (x, hpm), pm - x, x
	);
	x = fstb::ToolsSimd::select (
		fstb::ToolsSimd::cmp_gt_f32 (x, hp ), p  - x, x
	);

	return x;
}



template <typename T>
constexpr std::array <T, 2>	Approx::atan2_beg (T y, T x) noexcept
{
	constexpr T    c0p = T (fstb::PI * 0.25);
	constexpr T    c0n = T (fstb::PI * 0.75);
	constexpr T    eps = T (1e-10);

	const T        ya  = T (std::abs (y)) + eps;
	T              c0  = T (0);
	T              r   = T (0);
	if (x < T (0))
	{
		r  = (x + ya) / (ya - x);
		c0 = c0n;
	}
	else
	{
		r  = (x - ya) / (x + ya);
		c0 = c0p;
	}

	return std::array <T, 2> { r, c0 };
}

std::array <ToolsSimd::VectF32, 2>	Approx::atan2_beg (ToolsSimd::VectF32 y, ToolsSimd::VectF32 x) noexcept
{
	using TS = fstb::ToolsSimd;

	const auto     c0p  = TS::set1_f32 (float (fstb::PI * 0.25));
	const auto     c0n  = TS::set1_f32 (float (fstb::PI * 0.75));
	const auto     eps  = TS::set1_f32 (1e-10f);

	const auto     ya   = TS::abs (y) + eps;
	const auto     xlt0 = TS::cmp_lt0_f32 (x);
	const auto     xpya = x + ya;
	const auto     xmya = x - ya;
	const auto     yamx = ya - x;
	const auto     c0   = TS::select (xlt0, c0n, c0p);
	const auto     rnum = TS::select (xlt0, xpya, xmya);
	const auto     rden = TS::select (xlt0, yamx, xpya);
	const auto     r    = TS::div_approx2 (rnum, rden);

	return std::array <ToolsSimd::VectF32, 2> { r, c0 };
}



}  // namespace fstb



#endif   // fstb_Approx_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
