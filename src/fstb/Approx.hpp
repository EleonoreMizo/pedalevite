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
#include "fstb/Poly.h"
#include "fstb/ToolsSimd.h"

#if defined (_MSC_VER)
	#include <intrin.h>
#endif

#include <cassert>
#include <cmath>



namespace fstb
{



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

// For some reasons, Clang complains if the following functions are not
// defined before they are called.



template <typename T>
constexpr T	Approx::log2_poly2 (T x) noexcept
{
	return Poly::horner (x, T (-5.f / 3), T (2.f), T (-1.f / 3));
}

/*
n=50
FindFit[Table[
  Log[x]/Log[2], {x, 1, 2, 1/n}], {c0 + c1 (1 + (x - 1)/n) + 
   c2 (1 + (x - 1)/n)^2 + c3 (1 + (x - 1)/n)^3 + 
   c4 (1 + (x - 1)/n)^4 + 
   c5 (1 + (x - 1)/n)^5, {c0 + c1 + c2 + c3 + c4 + c5 == 0, 
   c0 + 2 c1 + 4 c2 + 8 c3 + 16 c4 + 32 c5 == 1, 
   c1 + 2 c2 + 3 c3 + 4 c4 + 5 c5 == 
    2 (c1 + 4 c2 + 12 c3 + 24 c4 + 80 c5)}}, {c0, c1, c2, c3, c4, c5},
  x]
*/
template <typename T>
constexpr T	Approx::log2_poly5 (T x) noexcept
{
	return Poly::estrin (x,
		T (-2.4395118595618),
		T ( 3.80998968934317),
		T (-1.75998771172059),
		T ( 0.40029024875655),
		T (-0.000133317241258202),
		T (-0.0106470495760765)
	);
}

/*
n=20
FindFit[Table[
  Log[x]/Log[2], {x, 1, 2, 1/n}], {c0 + c1 (1 + (x - 1)/n) + 
   c2 (1 + (x - 1)/n)^2 + c3 (1 + (x - 1)/n)^3 + 
   c4 (1 + (x - 1)/n)^4 + c5 (1 + (x - 1)/n)^5 + 
   c6 (1 + (x - 1)/n)^6 + 
   c7 (1 + (x - 1)/n)^7, {c0 + c1 + c2 + c3 + c4 + c5 + c6 + c7 == 0, 
   c0 + 2 c1 + 4 c2 + 8 c3 + 16 c4 + 32 c5 + 64 c6 + 128 c7 == 1, 
   c1 + 2 c2 + 3 c3 + 4 c4 + 5 c5 + 6 c6 + 7 c7 == 
    2 (c1 + 4 c2 + 12 c3 + 24 c4 + 80 c5 + 192 c6 + 448 c7)}}, {c0, 
  c1, c2, c3, c4, c5, c6, c7}, x]
*/
template <typename T>
constexpr T	Approx::log2_poly7 (T x) noexcept
{
	return Poly::estrin (x,
		T (-2.88240401363533),
		T ( 5.33677339735672),
		T (-3.72166286493998),
		T ( 1.42721785195822),
		T (-8.62639500355707e-6),
		T (-0.237597672916119),
		T ( 0.0884727724765693),
		T (-0.0107908439050664)
	);
}



// Quadratic approximation of 2^x in [0 ; 1]
template <typename T>
constexpr T	Approx::exp2_poly2 (T x) noexcept
{
	return Poly::horner (x, T (1), T (2.0 / 3.0), T (1.0 / 3.0));
}

// Coefficients found by Andrew Simper
// https://www.kvraudio.com/forum/viewtopic.php?p=7677357#p7677357
template <typename T>
constexpr T	Approx::exp2_poly5 (T x) noexcept
{
	return Poly::estrin (x,
		T (1               ),
		T (0.69315168779795),
		T (0.2401596780318 ),
		T (0.055817593635  ),
		T (0.008992164746  ),
		T (0.001878875789  )
	);
}

// Coefficients found by Andrew Simper
template <typename T>
constexpr T	Approx::exp2_poly7 (T x) noexcept
{
	return Poly::estrin (x,
		T (1                 ),
		T (0.693147180559945 ),
		T (0.2402265069591007),
		T (0.0555044941070   ),
		T (0.009615262656    ),
		T (0.001341316600    ),
		T (0.000143623130    ),
		T (0.000021615988    )
	);
}



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// f(x) ~ sin (x)
// x in [-pi/2 ; pi/2]
// Max error: 2.411e-8
// Scaled formula
template <typename T>
constexpr T	Approx::sin_rbj (T x) noexcept
{
	const auto    a  = T ( 2.628441118718695e-06);
	const auto    b  = T (-1.982061326014539e-04);
	const auto    c  = T ( 0.008333224441393);
	const auto    d  = T (-0.166666657479818);
	const auto    x2 = x * x;

	return ((((a * x2 + b) * x2 + c) * x2 + d) * x2) * x + x;
}



// f(x) ~ cos (x)
// x in [-pi ; pi]
// Max error: 2.411e-8
// Scaled formula
template <typename T>
T	Approx::cos_rbj (T x) noexcept
{
	using std::abs;

	x = abs (x);

	const auto     hp = T (0.5 * PI);
	x = hp - x;

	return sin_rbj (x);
}



// f1 (x) ~ sin (x)
// f2 (x) ~ cos (x)
// x in [-3*pi ; 3*pi]
// Scaled formula
void	Approx::cos_sin_rbj (Vf32 &c, Vf32 &s, Vf32 x) noexcept
{
	const auto     hp  = Vf32 ( 0.5 * PI);
	const auto     hpm = Vf32 (-0.5 * PI);
	const auto     tp  = Vf32 ( 2   * PI);
	const auto     p   = Vf32 (       PI);
	const auto     pm  = Vf32 (      -PI);

	// x -> [-pi ; pi]
	x = restrict_angle_to_mpi_pi (x, pm, p, tp);

	auto           xs = x;

	// xs -> [-pi/2 ; pi/2]
	xs = restrict_sin_angle_to_mhpi_hpi (xs, hpm, hp, pm, p);

	auto           xc = x + hp;

	// xc -> [-pi ; pi]
	xc = select ((xc > p ), xc - tp, xc);

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
	const auto     a  = T ( 0.0001530302);
	const auto     b  = T (-0.0046768800);
	const auto     c  = T ( 0.0796915849);
	const auto     d  = T (-0.6459640619);
	const auto     e  = T ( 1.5707963268);
	const T        x2 = x * x;

	return Poly::horner (x2, e, d, c, b, a) * x;

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



// f(x) ~ sin (x * pi)
// x in [-0.5 ; 1]
// Max error: 2.411e-8
template <typename T>
constexpr T	Approx::sin_rbj_pi (T x) noexcept
{
	using std::abs;

	const auto     one   = T (1);
	const auto     two_x = one - abs (one - x - x);

	return sin_rbj_halfpi (two_x);
}



// f1(x) ~ sin (x * pi)
// f2(x) ~ sin (x * pi/2)
// x in [-0.5 ; 1]
// Max error: 2.411e-8
void	Approx::sin_rbj_halfpi_pi (float &sx, float &s2x, float x) noexcept
{
	const auto     xv = Vf32::set_pair (x, 1 - fabsf (1 - 2*x));
	const auto     yv = sin_rbj_halfpi (xv);
	sx  = yv.template extract <0> ();
	s2x = yv.template extract <1> ();
}



// f1(x) ~ sin (x)
// x in [-pi ; pi]
// Max error: 0.919e-3
// d = 1 - pi/4 gives exact derivative at 0 but a max error of 2.787e-3
template <typename T>
T	Approx::sin_nick (T x) noexcept
{
	assert (x >= T (-PI));
	assert (x <= T (+PI));

	const auto     b = T ( 4 /  PI);
	const auto     c = T (-4 / (PI * PI));
	const auto     d = T (0.224008);

	using std::abs;
	const auto     y = b * x + c * x * abs (x);
	const auto     z = d * (y * abs (y) - y) + y;

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

	const auto     b = T (  8);
	const auto     c = T (-16);
	const auto     d = T (  0.224008);

	using std::abs;
	const auto     y = b * x + c * x * abs (x);
	const auto     z = d * (y * abs (y) - y) + y;

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

std::array <Vf32, 2>	Approx::cos_sin_nick_2pi (Vf32 x) noexcept
{
	// cos (x) = sin (x + pi/2)
	const auto     c_025  = Vf32 (0.25f);
	const auto     c_075  = Vf32 (0.75f);
	const auto     ge_025 = (c_025 < x);
	const auto     xc     = select (ge_025, x - c_075, x + c_025);

	const auto     b  = Vf32 (  8);
	const auto     c  = Vf32 (-16);
	const auto     d  = Vf32 (  0.224008f);

	const auto     yc = b * xc + c * xc * abs (xc);
	const auto     ys = b * x  + c * x  * abs (x );
	const auto     zc = d * (yc * abs (yc) - yc) + yc;
	const auto     zs = d * (ys * abs (ys) - ys) + ys;

	return std::array <Vf32, 2> { zc, zs };
}



// C1 continuity
float	Approx::log2 (float val) noexcept
{
	return log2_base (val, log2_poly2 <float>);
}

Vf32	Approx::log2 (Vf32 val) noexcept
{
	return val.log2_base (Approx::log2_poly2 <Vf32>);
}



// C1 continuity
// Max error: 1.8e-4
float	Approx::log2_5th (float val) noexcept
{
	return log2_base (val, log2_poly5 <float>);
}

Vf32	Approx::log2_5th (Vf32 val) noexcept
{
	return val.log2_base (Approx::log2_poly5 <Vf32>);
}



// C1 continuity
// Max error: 5.6e-6
float	Approx::log2_7th (float val) noexcept
{
	return log2_base (val, log2_poly7 <float>);
}

Vf32	Approx::log2_7th (Vf32 val) noexcept
{
	return val.log2_base (Approx::log2_poly7 <Vf32>);
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

Vf32	Approx::log2_crude (Vf32 val) noexcept
{
	assert (val > Vf32 (0));

	const auto     c0 = Vs32 (0x43800000);
	auto           i  = ToolsSimd::cast_s32 (val);
	i >>= 8;
	i  |= c0;
	const auto     d  = Vf32 (382.95695f);
	val = ToolsSimd::cast_f32 (i) - d;

	return val;
}



// C1 continuity
float	Approx::exp2 (float val) noexcept
{
	return exp2_base (val, exp2_poly2 <float>);
}

Vf32	Approx::exp2 (Vf32 val) noexcept
{
	return val.exp2_base (Approx::exp2_poly2 <Vf32>);
}



// C1 continuity
// max error on [0; 1]: 2.44e-7.
// Measured on [-20; +20]: 8.15e-7
float	Approx::exp2_5th (float val) noexcept
{
	return exp2_base (val, exp2_poly5 <float>);
}

Vf32	Approx::exp2_5th (Vf32 val) noexcept
{
	return val.exp2_base (Approx::exp2_poly5 <Vf32>);
}



// C1 continuity
// max error on [0; 1]: 1.64e-7.
// Measured on [-20; +20]: 7.91e-7
float	Approx::exp2_7th (float val) noexcept
{
	return exp2_base (val, exp2_poly7 <float>);
}

Vf32	Approx::exp2_7th (Vf32 val) noexcept
{
	return val.exp2_base (Approx::exp2_poly7 <Vf32>);
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

Vf32	Approx::exp2_crude (Vf32 val) noexcept
{
	const auto     c8 = Vs32 (0x43808000);
	const auto     d  = Vf32 (382.95695f);
	const auto     m  = Vs32 (0x7FFFFF00);

	auto           i  = ToolsSimd::cast_s32 (val + d);

	i  &= (i >= c8);
	i <<= 8;
	i  &= m;
	val = ToolsSimd::cast_f32 (i);

	return val;
}



// Approximation is good close to 0, but diverges for larger absolute values.
// A is the approximation accuracy (the bigger, the larger the valid range)
// A = 10 is a good start
template <int A, typename T>
constexpr T	Approx::exp_m (T val) noexcept
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
double	Approx::pow_crude (double a, double b) noexcept
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
	const auto     x_2 = x * x;
	const auto     c_3 = T (1.0 / 3);
	const auto     c_5 = T (2.0 / 15);

	return (c_5 * x_2 + c_3) * x_2 * x + x;
}



// Formula by mystran
// https://www.kvraudio.com/forum/viewtopic.php?p=7491289#p7491289
// tan x = sin (x) / cos (x)
//       = sin (x) / sqrt (1 - sin (x) ^ 2)
// https://www.desmos.com/calculator/6ghwlhxumj
template <typename T>
T	Approx::tan_mystran (T x) noexcept
{
	constexpr auto c1 = T ( 1);
	constexpr auto c3 = T (-1) / T (6);
	constexpr auto c5 = T ( 1) / T (120);
	constexpr auto c7 = T (-1) / T (5040);

	const auto     x2 = x * x;
	const auto     s  = Poly::horner (x2, c1, c3, c5, c7) * x;
	using std::sqrt;
	const auto     c  = T (sqrt (1 - s * s));

	return s / c;
}

Vf32	Approx::tan_mystran (Vf32 x) noexcept
{
	const auto     c1 = Vf32 ( 1         );
	const auto     c3 = Vf32 (-1.f / 6   );
	const auto     c5 = Vf32 ( 1.f / 120 );
	const auto     c7 = Vf32 (-1.f / 5040);

	const auto     x2 = x * x;
	const auto     s  = Poly::horner (x2, c1, c3, c5, c7) * x;

	return s * (c1 - s * s).rsqrt ();
}



// Formula: Andrew Simper
// https://discord.com/channels/507604115854065674/548502835608944681/872677465003274282
// Max relative error: 0.111 % on the +/-0.965 * pi/2 range
template <typename T>
constexpr T	Approx::tan_pade33 (T x) noexcept
{
//	x = limit (x, T (-1.54), T (1.54));
	const auto     x2  = x * x;
	const auto     num = T (-0.075021) * x2 + T (1.00111);
	const auto     den = T (-0.405097) * x2 + T (1);

	return x * num / den;
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
//	const auto     a   = T (15);
	const auto     d4  = T (14.999975509385927280627711005255);
	const auto     c0  = T (945);
	const auto     n2  = T (-105);
	const auto     d2  = T (-420);
	const auto     x2  = x * x;
	const auto     num = (     x2 + n2) * x2 + c0;
	const auto     den = (d4 * x2 + d2) * x2 + c0;

	return x * num / den;
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
	c7  =  0.0386378786305289
	c5  = -0.145916508504993
	c3  =  0.321088057777014
	c1  = -sum(c*)
	err = 1.2e-4

n = 1000
FindFit[
	Table[-ArcTan[x], {x, 0, 1, 1/n}],
	  (-Pi/4 - c3 - c5 - c7) ((x - 1)/n)
	+ c3 ((x - 1)/n)^3
	+ c5 ((x - 1)/n)^5
	+ c7 ((x - 1)/n)^7,
	{c7, c5, c3}, x
]
*/

template <typename T>
constexpr T	Approx::atan2_3th (T y, T x) noexcept
{
	constexpr auto c3 = T (0.18208); // Original formula: 0.1963
	constexpr auto c1 = T (-PI * 0.25) - c3;
	const auto     b  = atan2_beg (y, x);
	const auto     r  = b [0];
	const auto     c0 = b [1];
	const auto     r2 = r * r;

	auto           a  = ((c3 * r2) + c1) * r + c0;
	if (y < T (0))
	{
		a = -a;
	}

	return a;
}

Vf32	Approx::atan2_3th (Vf32 y, Vf32 x) noexcept
{
	const auto     c3 = Vf32 (0.18208f);
	const auto     c1 = Vf32 (PI * -0.25 - 0.18208);

	const auto     ys = y.signbit ();
	const auto     b  = atan2_beg (y, x);
	const auto     r  = b [0];
	const auto     c0 = b [1];
	const auto     r2 = r * r;
	auto           a  = c3;
	a  = fma (a, r2, c1);
	a  = fma (a, r , c0);
	a ^= ys;

	return a;
}



// Max error: +/-1.2e-4 rad
template <typename T>
constexpr T	Approx::atan2_7th (T y, T x) noexcept
{
	constexpr auto c7 = T ( 0.0386378786305289);
	constexpr auto c5 = T (-0.145916508504993);
	constexpr auto c3 = T ( 0.321088057777014);
	constexpr auto c1 = T (PI * -0.25 - 0.0386378786305289 - -0.145916508504993 - 0.321088057777014);

	const auto     b  = atan2_beg (y, x);
	const auto     r  = b [0];
	const auto     c0 = b [1];
	const auto     r2 = r * r;
	auto           a  = Poly::horner (r2, c1, c3, c5, c7) * r + c0;
	if (y < T (0))
	{
		a = -a;
	}

	return a;
}

Vf32	Approx::atan2_7th (Vf32 y, Vf32 x) noexcept
{
	const auto     c7 = Vf32 ( 0.0386378786305289f);
	const auto     c5 = Vf32 (-0.145916508504993f);
	const auto     c3 = Vf32 ( 0.321088057777014f);
	const auto     c1 = Vf32 (PI * -0.25 - 0.0386378786305289 - -0.145916508504993 - 0.321088057777014);

	const auto     ys = y.signbit ();
	const auto     b  = atan2_beg (y, x);
	const auto     r  = b [0];
	const auto     c0 = b [1];
	const auto     r2 = r * r;
	auto           a  = fma (Poly::horner (r2, c1, c3, c5, c7), r, c0);
	a ^= ys;

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
	const auto     p  = T (0.183);

	// x <- x + 0.183 * x^3
	auto           x2 = x * x;
	const auto     s  = x + x * x2 * p;

	return tanh_from_sinh (s);
}



// Formula by Urs Heckmann, modified by Andrew Simper
// https://www.desmos.com/calculator/s86jxqzqgo
// Max error: 1.33e-5
// Coefficients for 9th-order alternative:
// (1), 0.166658916965, 8.34772973761e-3, 1.92238891072e-4, 3.54691185595e-6
template <typename T>
T	Approx::tanh_urs (T x) noexcept
{
	const auto     c3 = T (0.166769511323);
	const auto     c5 = T (8.18265554763e-3);
	const auto     c7 = T (2.43041131705e-4);

	const auto     x2 = x * x;
	const auto     s  = Poly::horner (x2, c3, c5, c7) * x2 * x + x;

	return tanh_from_sinh (s);
}



// Formula by 2DaT
// https://www.kvraudio.com/forum/viewtopic.php?p=7503081#p7503081
// Max error: 3.14e-7 (measured: 5.45e-7)
// https://www.desmos.com/calculator/s86jxqzqgo
template <typename T>
constexpr T	Approx::tanh_2dat (T x) noexcept
{
	constexpr auto n0      = T (4.351839500e+06);
	constexpr auto n1      = T (5.605646250e+05);
	constexpr auto n2      = T (1.263485352e+04);
	constexpr auto n3      = T (4.751771164e+01);
	constexpr auto d0      = n0;
	constexpr auto d1      = T (2.011170000e+06);
	constexpr auto d2      = T (1.027901563e+05);
	constexpr auto d3      = T (1.009453430e+03);
	constexpr auto max_val = T (7.7539052963256836);

	x = limit (x, -max_val, max_val);
	const auto     x2  = x * x;
	const auto     num = Poly::horner (x2, n0, n1, n2, n3     ) * x;
	const auto     den = Poly::horner (x2, d0, d1, d2, d3 + x2);

	return num / den;
}

Vf32	Approx::tanh_2dat (Vf32 x) noexcept
{
	const auto     n0      = Vf32 (4.351839500e+06f);
	const auto     n1      = Vf32 (5.605646250e+05f);
	const auto     n2      = Vf32 (1.263485352e+04f);
	const auto     n3      = Vf32 (4.751771164e+01f);
	const auto     d0      = n0;
	const auto     d1      = Vf32 (2.011170000e+06f);
	const auto     d2      = Vf32 (1.027901563e+05f);
	const auto     d3      = Vf32 (1.009453430e+03f);
	const auto     max_val = Vf32 (7.7539052963256836f);

	const auto     s   = x.signbit ();
	x = abs (x);
	x = min (x, max_val);
	const auto     x2  = x * x;
	const auto     xs  = x ^ s;
	const auto     num = Poly::horner (x2, n0, n1, n2, n3     ) * xs;
	const auto     den = Poly::horner (x2, d0, d1, d2, d3 + x2);

	return num / den;
}



// This one fits in the range [0, 4] and matches the derivative at 4 to zero
// so you have a smooth C1 at the point of clipping
// ends at just under 1, but has a normalised error of +-6.5e-4
// tanh_approx(4) = 0.9998
// tanh(4) = 0.999329
// Source: Andrew Simper
// https://discord.com/channels/507604115854065674/507630527847596046/702375822941618207
template <typename T>
constexpr T	Approx::tanh_andy (T x) noexcept
{
	x = limit (x, T (-4), T (+4));

	const auto     n3  = T (0.0812081221471);
	const auto     d0  = T (1);
	const auto     d2  = T (0.412523749044);
	const auto     d4  = T (0.00624523306500);

	const auto     x2  = x * x;
	const auto     num = n3 * x2 * x + x;
	const auto     den = Poly::horner (x2, d0, d2, d4);

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
// Jan Kadlec, http://rrrola.wz.cz/inv_sqrt.html, 2010
// Matthew Robertson, A Brief History of InvSqrt, 2012
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
		  (P == 0) ? 0x5F37642F
		: (P == 1) ? 0x5F1FFFF9
		: (P == 2) ? 0x5F37599E
		:            0x5F375A86;

	constexpr float   hmul = (P == 1) ? 0.703952253f       : 0.5f;
	constexpr float   nrc  = (P == 1) ? 2.38924456f * hmul : 1.5f;

	const float    xh = x * hmul;
	union
	{
		int32_t        _i;
		float          _f;
	}              c;
	c._f = x;
	c._i = cs - (c._i >> 1);
	x    = c._f;

	if (P > 0) { x *= nrc - xh * x * x; }
	if (P > 1) { x *= nrc - xh * x * x; }
	if (P > 2) { x *= nrc - xh * x * x; }

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
	c._i = 0x5FE6EB50C7B537A9LL - (c._i >> 1);
	x    = c._f;

	if (P > 0) { x *= 1.5 - xh * x * x; }
	if (P > 1) { x *= 1.5 - xh * x * x; }
	if (P > 2) { x *= 1.5 - xh * x * x; }
	if (P > 3) { x *= 1.5 - xh * x * x; }

	return x;
}

template <int P>
Vf32	Approx::rsqrt (Vf32 x) noexcept
{
	static_assert (
		(P >= 0 && P <= 3),
		"The number of Newton iterations must be in [0 ; 3]"
	);
	assert (x >= Vf32::zero ());

	// Result is more or less equivalent to rough approx + 1 N-R iteration.
	auto           r = x.rsqrt_approx ();

	if (P > 1)
	{
		const auto     xh  = Vf32 (0.5f) * x;
		const auto     nrc = Vf32 (1.5f);
		r *= nrc - xh * r * r;
		if (P > 2)
		{
			r *= nrc - xh * r * r;
		}
	}

	return r;
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
		y = Poly::horner (x, d, c, b, a);
	}

	return y;
}

Vf32	Approx::wright_omega_3 (Vf32 x) noexcept
{
	const auto     a   = Vf32 (-1.314293149877800e-3f);
	const auto     b   = Vf32 ( 4.775931364975583e-2f);
	const auto     c   = Vf32 ( 3.631952663804445e-1f);
	const auto     d   = Vf32 ( 6.313183464296682e-1f);
	const auto     x1  = Vf32 (-3.341459552768620f);
	const auto     x2  = Vf32 ( 8.0f);
	const auto     ln2 = Vf32 (float (LN2));

	const auto     y0  = Vf32::zero ();
	const auto     y1  = Poly::horner (x, d, c, b, a);
	const auto     y2  = x - (Approx::log2 (x)) * ln2;

	const auto     tx1 = (x < x1);
	const auto     tx2 = (x < x2);
	auto           y   = y0;
	y = select (tx1, y, y1);
	y = select (tx2, y, y2);

	return y;
}



// One Newton-Raphson iteration added
template <typename T>
T	Approx::wright_omega_4 (T x) noexcept
{
	T              y = wright_omega_3 (x);
	y -=
		  (y - Approx::exp2 ((x - y) * T (LOG2_E)))
		/ (y + T (1));

	return y;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// [-3*pi ; 3*pi] -> [-pi ; pi]
// p = pi, pm = -pi, tp = 2*pi
Vf32	Approx::restrict_angle_to_mpi_pi (Vf32 x, const Vf32 &pm, const Vf32 &p, const Vf32 &tp) noexcept
{
	x = select ((x < pm), x + tp, x);
	x = select ((x > p ), x - tp, x);

	return x;
}



// [-pi ; pi] -> [-pi/2 ; pi/2]
// hpm = -pi/2, hp = pi/2, pm = -pi, p = pi
Vf32	Approx::restrict_sin_angle_to_mhpi_hpi (Vf32 x, const Vf32 &hpm, const Vf32 &hp, const Vf32 &pm, const Vf32 &p) noexcept
{
	x = select ((x < hpm), pm - x, x);
	x = select ((x > hp ), p  - x, x);

	return x;
}



template <typename P>
float	Approx::log2_base (float val, P poly) noexcept
{
	assert (val > 0);

	Combo32        combo { val };
	int            x     = combo._i;
	const int      log_2 = ((x >> 23) & 255) - 127;
	x        &= ~(255 << 23);
	x        +=   127 << 23;
	combo._i  = x;
	val       = combo._f;

	// Approximation of 1 + log2 (x) in [1 ; 2] -> [0 ; 1]
	val       = poly (val);

	return val + float (log_2);
}



template <typename P>
float	Approx::exp2_base (float val, P poly) noexcept
{
	// Truncated val for integer power of 2
	const int      tx = floor_int (val);

	// Float remainder of power of 2
	val -= static_cast <float> (tx);

	// Approximation of 2^x in [0 ; 1]
	val = poly (val);

	Combo32        combo { val };

	// Add integer power of 2 to exponent
	combo._i += tx << 23;
	val       = combo._f;

	assert (val >= 0);

	return val;
}



// Computes x / sqrt (1 + x^2)
// because tanh (x) = sinh (x) / sqrt (1 + sinh (x) ^ 2)
template <typename T>
T	Approx::tanh_from_sinh (T x) noexcept
{
	using std::sqrt;
	return x / T (sqrt (T (1) + x * x));
}

Vf32	Approx::tanh_from_sinh (Vf32 x) noexcept
{
	return x * (x * x + Vf32 (1)).rsqrt ();
}



template <typename T>
constexpr std::array <T, 2>	Approx::atan2_beg (T y, T x) noexcept
{
	constexpr T    c0p = T (PI * 0.25);
	constexpr T    c0n = T (PI * 0.75);
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

std::array <Vf32, 2>	Approx::atan2_beg (Vf32 y, Vf32 x) noexcept
{
	const auto     c0p  = Vf32 (PI * 0.25);
	const auto     c0n  = Vf32 (PI * 0.75);
	const auto     eps  = Vf32 (1e-10f);

	const auto     ya   = abs (y) + eps;
	const auto     xlt0 = x.is_lt_0 ();
	const auto     xpya = x + ya;
	const auto     xmya = x - ya;
	const auto     yamx = ya - x;
	const auto     c0   = select (xlt0, c0n, c0p);
	const auto     rnum = select (xlt0, xpya, xmya);
	const auto     rden = select (xlt0, yamx, xpya);
	const auto     r    = rnum / rden;

	return std::array <Vf32, 2> { r, c0 };
}



}  // namespace fstb



#endif   // fstb_Approx_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
