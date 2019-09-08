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

#if defined (_MSC_VER)
	#include <intrin.h>
#endif

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
float	Approx::sin_rbj_halfpi (float x)
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

	union
	{
		int32_t        _i;
		float          _f;
	}              combo;
	combo._f = val;
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

	uint32_t       result;

#if defined (_MSC_VER)

	const uint32_t c0 = 0x80000000U;
	const uint32_t c1 = 2863311531U;

	result   = val;
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
	result = uint32_t (step_64 >> (62 - 32 - 1));
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

	uint32_t       result;

#if defined (_MSC_VER)

	const uint32_t x_32 = uint32_t (val) << 16;
	result  = uint32_t (__emulu (c4,     x_32) >> 32);
	result += c3;
	result  = uint32_t (__emulu (result, x_32) >> 32);
	result += c2;
	result  = uint32_t (__emulu (result, x_32) >> 32);
	result += c1;
	result  = uint32_t (__emulu (result, x_32) >> 32);
	result += c0;

#else

	const uint32_t x_32 = uint32_t (val) << 16;
	result  = uint32_t ((c4 * uint64_t (x_32)) >> 32);
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
float	Approx::tan_taylor5 (float x)
{
	const float    x_2 = x * x;
	const float    c_1 = 1;
	const float    c_3 = 1.0f / 3;
	const float    c_5 = 2.0f / 15;

	return ((c_5 * x_2 + c_3) * x_2 + c_1) * x;
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
