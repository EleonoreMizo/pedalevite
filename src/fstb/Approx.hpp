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
ToolsSimd::VectF32	Approx::sin_rbj (ToolsSimd::VectF32 x)
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
ToolsSimd::VectF32	Approx::cos_rbj (ToolsSimd::VectF32 x)
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
void	Approx::cos_sin_rbj (ToolsSimd::VectF32 &c, ToolsSimd::VectF32 &s, ToolsSimd::VectF32 x)
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
fstb_CONSTEXPR14 float	Approx::sin_rbj_halfpi (float x)
{
	const float   a  =  0.0001530302f;
	const float   b  = -0.0046768800f;
	const float   c  =  0.0796915849f;
	const float   d  = -0.6459640619f;
	const float   e  =  1.5707963268f;
	const float   x2 = x * x;

	return (((((a * x2 + b) * x2 + c) * x2 + d) * x2) + e) * x;

	// Other coefficients found by Ollie Niemitalo
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



ToolsSimd::VectF32	Approx::sin_rbj_halfpi (ToolsSimd::VectF32 x)
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
ToolsSimd::VectF32	Approx::sin_rbj_pi (ToolsSimd::VectF32 x)
{
	const auto     one   = ToolsSimd::set1_f32 (1);
	const auto     two_x = one - ToolsSimd::abs (one - x - x);

	return sin_rbj_halfpi (two_x);
}



// f1(x) ~ sin (x * pi)
// f2(x) ~ sin (x * pi/2)
// x in [-0.5 ; 1]
// Max error: 2.411e-8
void	Approx::sin_rbj_halfpi_pi (float &sx, float &s2x, float x)
{
	const auto     xv = ToolsSimd::set_2f32 (x, 1 - fabs (1 - 2*x));
	const auto     yv = sin_rbj_halfpi (xv);
	sx  = ToolsSimd::Shift <0>::extract (yv);
	s2x = ToolsSimd::Shift <1>::extract (yv);
}



// f1(x) ~ sin (x)
// x in [-pi ; pi]
// Max error: 0.919e-3
float	Approx::sin_nick (float x)
{
	const float    b = float ( 4 /  fstb::PI);
	const float    c = float (-4 / (fstb::PI * fstb::PI));
	const float    d = 0.224008f;

	const float    y = b * x + c * x * fabs (x);
	const float    z = d * (y * fabs (y) - y) + y;

	return z;
}

ToolsSimd::VectF32	Approx::sin_nick (ToolsSimd::VectF32 x)
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
float	Approx::sin_nick_2pi (float x)
{
	const float    b =   8;
	const float    c = -16;
	const float    d =   0.224008f;

	const float    y = b * x + c * x * fabs (x);
	const float    z = d * (y * fabs (y) - y) + y;

	return z;
}

ToolsSimd::VectF32	Approx::sin_nick_2pi (ToolsSimd::VectF32 x)
{
	const auto     b = ToolsSimd::set1_f32 (  8);
	const auto     c = ToolsSimd::set1_f32 (-16);
	const auto     d = ToolsSimd::set1_f32 (  0.224008f);

	const auto     y = b * x + c * x * ToolsSimd::abs (x);
	const auto     z = d * (y * ToolsSimd::abs (y) - y) + y;

	return z;
}



float	Approx::log2 (float val)
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

	return (val + log_2);
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
float	Approx::log2_crude (float val)
{
	assert (val > 0);

	Combo32        xv { val };
	Combo32        lv;
	lv._i = 0x43800000 | (xv._i >> 8);

	return lv._f - 382.95695f;
}



float	Approx::exp2 (float val)
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
float	Approx::exp2_crude (float val)
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
float	Approx::exp2_5th (float val)
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
T	Approx::exp_m (T val)
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
double	Approx::pow (double a, double b)
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

uint32_t	Approx::fast_partial_exp2_int_16_to_int_32 (int val)
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
uint32_t	Approx::fast_partial_exp2_int_16_to_int_32_4th (int val)
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
fstb_CONSTEXPR14 float	Approx::tan_taylor5 (float x)
{
	const float    x_2 = x * x;
	const float    c_1 = 1;
	const float    c_3 = 1.0f / 3;
	const float    c_5 = 2.0f / 15;

	return ((c_5 * x_2 + c_3) * x_2 + c_1) * x;
}

ToolsSimd::VectF32	Approx::tan_taylor5 (ToolsSimd::VectF32 x)
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
float	Approx::tan_mystran (float x)
{
	const float    c1 =  1;
	const float    c3 = -1.f / 6;
	const float    c5 =  1.f / 120;
	const float    c7 = -1.f / 5040;

	const float    x2 = x * x;
	const float    s  = (((c7 * x2 + c5) * x2 + c3) * x2 + c1) * x;
	const float    c  = sqrt (1 - s * s);

	return s / c;
}

ToolsSimd::VectF32	Approx::tan_mystran (ToolsSimd::VectF32 x)
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
fstb_CONSTEXPR14 T	Approx::tan_pade55 (T x)
{
//	constexpr T    a   = 15;
	constexpr T    a   = T (14.999975509385927280627711005255);
	const T        x2  = x * x;
	const T        num = (    x2 - 105) * x2 + 945;
	const T        den = (a * x2 - 420) * x2 + 945;

	return x * num / den;
}

ToolsSimd::VectF32	Approx::tan_pade55 (ToolsSimd::VectF32 x)
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



// Formula by mystran
// https://www.kvraudio.com/forum/viewtopic.php?f=33&t=521377
// s (x) = x / sqrt (1 + x^2)
// h3 (x) = s (x + 0.183 * x^3)
// Max error: 3.64e-3
// https://www.desmos.com/calculator/sjxol8khaz
float	Approx::tanh_mystran (float x)
{
	const float    p   = 0.183f;

	// x <- x + 0.183 * x^3
	float          x2 = x * x;
	x += x * x2 * p;

	// x <- x / sqrt (1 + x^2)
	x2 = x * x;
	x /= sqrt (1 + x2);

	return x;
}

ToolsSimd::VectF32	Approx::tanh_mystran (ToolsSimd::VectF32 x)
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
float	Approx::tanh_2dat (float x)
{
	const float    n0      = 4.351839500e+06f;
	const float    n1      = 5.605646250e+05f;
	const float    n2      = 1.263485352e+04f;
	const float    n3      = 4.751771164e+01f;
	const float    d0      = n0;
	const float    d1      = 2.011170000e+06f;
	const float    d2      = 1.027901563e+05f;
	const float    d3      = 1.009453430e+03f;
	const float    max_val = 7.7539052963256836f;

	const float    s   = x;
	x = std::min (float (fabs (x)), max_val);
	const float    x2  = x * x;
	const float    xs  = (s < 0) ? -x : x;
	float          num = (((     n3  * x2 + n2) * x2 + n1) * x2 + n0) * xs;
	const float    den = (((x2 + d3) * x2 + d2) * x2 + d1) * x2 + d0;

	return num / den;
}

ToolsSimd::VectF32	Approx::tanh_2dat (ToolsSimd::VectF32 x)
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
T	Approx::tanh_andy (T x)
{
	x = fstb::limit (x, T (-4), T (+4));

	const T        n3 = T (0.0812081221471);
	const T        n1 = T (1);
	const T        d4 = T (0.00624523306500);
	const T        d2 = T (0.412523749044);
	const T        d0 = T (1);

	const T        x2  = x * x;
	const T        num = (n3 * x2 + n1) * x;
	const T        den = (d4 * x2 + d2) * x2 + d0;

	return num / den;
}



// Approximation of the Wright Omega function:
// omega (x) = W0 (exp (x))
// where W0 is one of the Lambert W functions.
// Formula from:
// Stefano D'Angelo, Leonardo Gabrielli, Luca Turchet,
// Fast Approximation of the Lambert W Function for Virtual Analog Modelling,
// DAFx-19
template <typename T>
T	Approx::wright_omega_3 (T x)
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

ToolsSimd::VectF32	Approx::wright_omega_3 (ToolsSimd::VectF32 x)
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
T	Approx::wright_omega_4 (T x)
{
	T              y = wright_omega_3 (x);
	y -=
		  (y - Approx::exp2 (float (x - y) * float (LOG2_E)))
		/ (y + T (1));

	return y;
}

ToolsSimd::VectF32	Approx::wright_omega_4 (ToolsSimd::VectF32 x)
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
ToolsSimd::VectF32	Approx::restrict_angle_to_mpi_pi (ToolsSimd::VectF32 x, const ToolsSimd::VectF32 &pm, const ToolsSimd::VectF32 &p, const ToolsSimd::VectF32 &tp)
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
ToolsSimd::VectF32	Approx::restrict_sin_angle_to_mhpi_hpi (ToolsSimd::VectF32 x, const ToolsSimd::VectF32 &hpm, const ToolsSimd::VectF32 &hp, const ToolsSimd::VectF32 &pm, const ToolsSimd::VectF32 &p)
{
	x = fstb::ToolsSimd::select (
		fstb::ToolsSimd::cmp_lt_f32 (x, hpm), pm - x, x
	);
	x = fstb::ToolsSimd::select (
		fstb::ToolsSimd::cmp_gt_f32 (x, hp ), p  - x, x
	);

	return x;
}



}  // namespace fstb



#endif   // fstb_Approx_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
