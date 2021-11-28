/*****************************************************************************

        ToolsSimd.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_ToolsSimd_CODEHEADER_INCLUDED)
#define fstb_ToolsSimd_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"
#include "fstb/Poly.h"

#include <algorithm>
#include <utility>

#include <cfloat>
#include <cmath>



namespace fstb
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Vf32	ToolsSimd::swap_2f32 (Vf32 v) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 { { v._x [2], v._x [3], v._x [0], v._x [1] } };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_shuffle_ps (v, v, (2<<0) + (3<<2) + (0<<4) + (1<<6));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const float32x2_t v01 = vget_low_f32 (v);
	const float32x2_t v23 = vget_high_f32 (v);
	return vcombine_f32 (v23, v01);
#endif // fstb_ARCHI
}



Vs32	ToolsSimd::reverse_s32 (Vs32 x) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	std::swap (x._x [0], x._x [3]);
	std::swap (x._x [1], x._x [2]);
#elif fstb_ARCHI == fstb_ARCHI_X86
	x = _mm_shuffle_epi32 (x, (3<<0) + (2<<2) + (1<<4) + (0<<6));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	x = vrev64q_s32 (vcombine_s32 (vget_high_s32 (x), vget_low_s32 (x)));
#endif // fstb_ARCHI
	return x;
}



// Returns x * a + b
Vf32	ToolsSimd::fmadd (Vf32 x, Vf32 a, Vf32 b) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 { {
		x._x [0] * a._x [0] + b._x [0],
		x._x [1] * a._x [1] + b._x [1],
		x._x [2] * a._x [2] + b._x [2],
		x._x [3] * a._x [3] + b._x [3]
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_add_ps (_mm_mul_ps (x, a), b);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	#if defined (__ARM_FEATURE_FMA)
	return vfmaq_f32 (b, x, a);
	#else
	return vmlaq_f32 (b, x, a);
	#endif
#endif // fstb_ARCHI
}



// Returns x * a - b
Vf32	ToolsSimd::fmsub (Vf32 x, Vf32 a, Vf32 b) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 { {
		x._x [0] * a._x [0] - b._x [0],
		x._x [1] * a._x [1] - b._x [1],
		x._x [2] * a._x [2] - b._x [2],
		x._x [3] * a._x [3] - b._x [3]
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_sub_ps (_mm_mul_ps (x, a), b);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	#if defined (__ARM_FEATURE_FMA)
	return vfmsq_f32 (b, x, a);
	#else
	return vmlsq_f32 (b, x, a);
	#endif
#endif // fstb_ARCHI
}



Vf32	ToolsSimd::div_approx (Vf32 n, Vf32 d) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 { {
		n._x [0] / d._x [0],
		n._x [1] / d._x [1],
		n._x [2] / d._x [2],
		n._x [3] / d._x [3]
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_div_ps (n, d);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return n * d.rcp_approx ();
#endif // fstb_ARCHI
}



Vf32	ToolsSimd::sqrt (Vf32 v) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 { {
		sqrtf (v._x [0]),
		sqrtf (v._x [1]),
		sqrtf (v._x [2]),
		sqrtf (v._x [3])
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_sqrt_ps (v);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const uint32x4_t  nz_flag = vtstq_u32 (
		vreinterpretq_u32_f32 (v),
		vreinterpretq_u32_f32 (v)
	);
	float32x4_t    rs      = vrsqrteq_f32 (v);
	rs *= vrsqrtsq_f32 (v, rs * rs);
	rs *= vrsqrtsq_f32 (v, rs * rs);
	rs *= vrsqrtsq_f32 (v, rs * rs);
	const auto     sqrt_a  = rs * float32x4_t (v);
	return vreinterpretq_f32_u32 (vandq_u32 (
		vreinterpretq_u32_f32 (sqrt_a),
		nz_flag
	));
#endif // fstb_ARCHI
}



Vf32	ToolsSimd::sqrt_approx (Vf32 v) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 { {
		sqrtf (v._x [0]),
		sqrtf (v._x [1]),
		sqrtf (v._x [2]),
		sqrtf (v._x [3])
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	// Zero and denormal values will produce INF with _mm_rsqrt_ps(), so
	// we need a mask.
	const __m128   z_flag  = _mm_cmplt_ps (v, _mm_set1_ps (FLT_MIN));
	const __m128   rsqrt_a = _mm_rsqrt_ps (v);
	const __m128   sqrt_a  = _mm_mul_ps (v, rsqrt_a);
	const __m128   sqrt_m  = _mm_andnot_ps (z_flag, sqrt_a);
	return sqrt_m;
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const uint32x4_t  nz_flag = vtstq_u32 (
		vreinterpretq_u32_f32 (v),
		vreinterpretq_u32_f32 (v)
	);
	auto           rs      = vrsqrteq_f32 (v);
	rs *= vrsqrtsq_f32 (rs * float32x4_t (v), rs);
	const auto     sqrt_a  = rs * float32x4_t (v);
	return vreinterpretq_f32_u32 (vandq_u32 (
		vreinterpretq_u32_f32 (sqrt_a),
		nz_flag
	));
#endif // fstb_ARCHI
}



Vf32	ToolsSimd::rsqrt_approx (Vf32 v) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	// Ref:
	// Robin Green, Even Faster Math Functions, 2020-03, GDC
	// Chris Lomont, Fast Inverse Square Root, 2003-02
	const float    xh0 = v._x [0] * 0.5f;
	const float    xh1 = v._x [1] * 0.5f;
	const float    xh2 = v._x [2] * 0.5f;
	const float    xh3 = v._x [3] * 0.5f;
	Combo          c;
	c._vf32    = v;
	c._s32 [0] = 0x5F375A82 - (c._s32 [0] >> 1);
	c._s32 [1] = 0x5F375A82 - (c._s32 [1] >> 1);
	c._s32 [2] = 0x5F375A82 - (c._s32 [2] >> 1);
	c._s32 [3] = 0x5F375A82 - (c._s32 [3] >> 1);
	float          x0 = c._f32 [0];
	float          x1 = c._f32 [1];
	float          x2 = c._f32 [2];
	float          x3 = c._f32 [3];
	x0 *= 1.5008909f - xh0 * x0 * x0;
	x1 *= 1.5008909f - xh1 * x1 * x1;
	x2 *= 1.5008909f - xh2 * x2 * x2;
	x3 *= 1.5008909f - xh3 * x3 * x3;
	return Vf32 { { x0, x1, x2, x3 } };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_rsqrt_ps (v);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	auto           rs = vrsqrteq_f32 (v);
	rs *= vrsqrtsq_f32 (rs * float32x4_t (v), rs);
	return rs;
#endif // fstb_ARCHI
}



Vf32	ToolsSimd::rsqrt_approx2 (Vf32 v) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 { {
		1.f / sqrtf (v._x [0]),
		1.f / sqrtf (v._x [1]),
		1.f / sqrtf (v._x [2]),
		1.f / sqrtf (v._x [3])
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	__m128         rs = _mm_rsqrt_ps (v);
	rs = _mm_set1_ps (0.5f) * rs * (_mm_set1_ps (3) - __m128 (v) * rs * rs);
	return rs;
#elif fstb_ARCHI == fstb_ARCHI_ARM
	auto           rs = vrsqrteq_f32 (v);
	rs *= vrsqrtsq_f32 (rs * float32x4_t (v), rs);
	rs *= vrsqrtsq_f32 (rs * float32x4_t (v), rs);
	return rs;
#endif // fstb_ARCHI
}



template <typename P>
Vf32	ToolsSimd::log2_base (Vf32 x, P poly) noexcept
{
	const int32_t  log2_sub = 127;

#if ! defined (fstb_HAS_SIMD)

	assert (
	      x._x [0] > 0
		&& x._x [1] > 0
		&& x._x [2] > 0
		&& x._x [3] > 0
	);
	Combo          c;
	c._vf32 = x;
	const int      x0 = c._s32 [0];
	const int      x1 = c._s32 [1];
	const int      x2 = c._s32 [2];
	const int      x3 = c._s32 [3];
	const Vf32     log2_int { {
		float (((x0 >> 23) & 255) - log2_sub),
		float (((x1 >> 23) & 255) - log2_sub),
		float (((x2 >> 23) & 255) - log2_sub),
		float (((x3 >> 23) & 255) - log2_sub)
	} };
	c._s32 [0] = (x0 & ~(255 << 23)) + (127 << 23);
	c._s32 [1] = (x1 & ~(255 << 23)) + (127 << 23);
	c._s32 [2] = (x2 & ~(255 << 23)) + (127 << 23);
	c._s32 [3] = (x3 & ~(255 << 23)) + (127 << 23);
	Vf32           part { {
		c._f32 [0],
		c._f32 [1],
		c._f32 [2],
		c._f32 [3]
	} };

#else // fstb_HAS_SIMD

#if fstb_ARCHI == fstb_ARCHI_X86

	// Extracts the exponent
	__m128i        xi = _mm_castps_si128 (x);
	xi = _mm_srli_epi32 (xi, 23);
	const __m128i  l2_sub = _mm_set1_epi32 (log2_sub);
	xi = _mm_sub_epi32 (xi, l2_sub);
	const auto     log2_int = Vf32 { _mm_cvtepi32_ps (xi) };

#elif fstb_ARCHI == fstb_ARCHI_ARM

	int32x4_t      xi = vreinterpretq_s32_f32 (x);
	xi = vshrq_n_s32 (xi, 23);
	const int32x4_t   l2_sub = vdupq_n_s32 (log2_sub);
	xi -= l2_sub;
	const auto     log2_int = Vf32 { vcvtq_f32_s32 (xi) };

#endif // fstb_ARCHI

	// Extracts the multiplicative part in [1 ; 2[
	const auto     mask_mantissa = Vf32 (1.17549421e-38f); // Binary: (1 << 23) - 1
	auto           part          = x & mask_mantissa;
	const auto     bias          = Vf32 (1.0f);            // Binary: 127 << 23
	part |= bias;

#endif // fstb_HAS_SIMD

	// Computes the log2(x) polynomial approximation [1 ; 2[ -> [0 ; 1[
	part = poly (part);

	// Sums the components
	const auto     total = log2_int + part;

	return total;
}



// Formula by 2DaT
// 12-13 ulp
// https://www.kvraudio.com/forum/viewtopic.php?f=33&t=532048
Vf32	ToolsSimd::log2_approx2 (Vf32 v) noexcept
{
#if ! defined (fstb_HAS_SIMD)

	assert (
	      v._x [0] > 0
		&& v._x [1] > 0
		&& v._x [2] > 0
		&& v._x [3] > 0
	);
	/*** To do: approximation ***/
	return Vf32 { {
		logf (v._x [0]) * float (LOG2_E),
		logf (v._x [1]) * float (LOG2_E),
		logf (v._x [2]) * float (LOG2_E),
		logf (v._x [3]) * float (LOG2_E),
	} };

#else // fstb_HAS_SIMD

	const auto     c0    = Vf32 (1.011593342e+01f);
	const auto     c1    = Vf32 (1.929443550e+01f);
	const auto     d0    = Vf32 (2.095932245e+00f);
	const auto     d1    = Vf32 (1.266638851e+01f);
	const auto     d2    = Vf32 (6.316540241e+00f);
	const auto     one   = Vf32 (1.0f);
	const auto     multi = Vf32 (1.41421356237f);
	const auto     mantissa_mask = Vs32 ((1 << 23) - 1);

#if fstb_ARCHI == fstb_ARCHI_X86

	__m128i        x_i           = _mm_castps_si128 (v);
	__m128i        spl_exp       = _mm_castps_si128 (v * multi);
	spl_exp = _mm_sub_epi32 (spl_exp, _mm_castps_si128 (one));
	spl_exp = _mm_andnot_si128 (mantissa_mask, spl_exp);
	const auto     spl_mantissa  =
		Vf32 { _mm_castsi128_ps (_mm_sub_epi32 (x_i, spl_exp)) };
	spl_exp = _mm_srai_epi32 (spl_exp, 23);
	const auto     log2_exponent = Vf32 { _mm_cvtepi32_ps (spl_exp) };

#elif fstb_ARCHI == fstb_ARCHI_ARM

	const int32x4_t   x_i        = vreinterpretq_s32_f32 (v);
	int32x4_t      spl_exp       = vreinterpretq_s32_f32 (v * multi);
	spl_exp = spl_exp - vreinterpretq_s32_f32 (one);
	spl_exp = vandq_s32 (vmvnq_s32 (mantissa_mask), spl_exp);
	const auto     spl_mantissa  = Vf32 { vreinterpretq_f32_s32 (x_i - spl_exp) };
	spl_exp = vshrq_n_s32 (spl_exp, 23);
	const auto     log2_exponent = Vf32 { vcvtq_f32_s32 (spl_exp) };

#endif // fstb_ARCHI

	auto           num = spl_mantissa + c1;
	num = fmadd (num, spl_mantissa, c0);
	num *= spl_mantissa - one;

	auto           den = d2;
	den = fmadd (den, spl_mantissa, d1);
	den = fmadd (den, spl_mantissa, d0);

	auto           res = num / den;
	res += log2_exponent;

	return res;

#endif // fstb_HAS_SIMD
}



template <typename P>
Vf32	ToolsSimd::exp2_base (Vf32 x, P poly) noexcept
{
#if ! defined (fstb_HAS_SIMD)

	const int32_t  tx0 = floor_int (x._x [0]);
	const int32_t  tx1 = floor_int (x._x [1]);
	const int32_t  tx2 = floor_int (x._x [2]);
	const int32_t  tx3 = floor_int (x._x [3]);
	const Vf32     frac { {
		x._x [0] - static_cast <float> (tx0),
		x._x [1] - static_cast <float> (tx1),
		x._x [2] - static_cast <float> (tx2),
		x._x [3] - static_cast <float> (tx3)
	} };

	Combo          combo { poly (frac) };

	combo._s32 [0] += tx0 << 23;
	combo._s32 [1] += tx1 << 23;
	combo._s32 [2] += tx2 << 23;
	combo._s32 [3] += tx3 << 23;
	assert (
	      combo._f32 [0] >= 0
		&& combo._f32 [1] >= 0
		&& combo._f32 [2] >= 0
		&& combo._f32 [3] >= 0
	);
	return combo._vf32;

#else // fstb_HAS_SIMD

	// Separates the integer and fractional parts
#if fstb_ARCHI == fstb_ARCHI_X86
	const auto     round_toward_m_i = Vf32 (-0.5f);
	auto           xi        = _mm_cvtps_epi32 (x + round_toward_m_i);
	const auto     val_floor = Vf32 { _mm_cvtepi32_ps (xi) };
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const int      round_ofs = 256;
	int32x4_t      xi = vcvtq_s32_f32 (x + Vf32 (round_ofs));
	xi -= vdupq_n_s32 (round_ofs);
	const auto     val_floor = Vf32 { vcvtq_f32_s32 (xi) };
#endif // fstb_ARCHI

	auto           frac = x - val_floor;

	// Computes the polynomial approximation of 2^x [0 ; 1] -> [1 ; 2]
	frac = poly (frac);

	// Integer part
#if fstb_ARCHI == fstb_ARCHI_X86
	xi = _mm_slli_epi32 (xi, 23);
	xi = _mm_add_epi32 (xi, _mm_castps_si128 (frac));
	return _mm_castsi128_ps (xi);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	xi = vshlq_n_s32 (xi, 23);
	xi = xi + vreinterpretq_s32_f32 (frac);
	return vreinterpretq_f32_s32 (xi);
#endif // fstb_ARCHI

#endif // fstb_HAS_SIMD
}



// Formula by 2DaT
// Coefficients fixed by Andrew Simper to achieve true C0 continuity
// 3-4 ulp
// https://www.kvraudio.com/forum/viewtopic.php?p=7161124#p7161124
// https://www.kvraudio.com/forum/viewtopic.php?p=7677266#p7677266
Vf32	ToolsSimd::exp2_approx2 (Vf32 v) noexcept
{
#if ! defined (fstb_HAS_SIMD)

	/*** To do: approximation ***/
	return Vf32 { {
		expf (v._x [0] * float (LN2)),
		expf (v._x [1] * float (LN2)),
		expf (v._x [2] * float (LN2)),
		expf (v._x [3] * float (LN2)),
	} };

#else // fstb_HAS_SIMD

	// [-0.5, 0.5] 2^x approx polynomial ~ 2.4 ulp
	const auto     c0 = Vf32 (1.000000088673463f);
	const auto     c1 = Vf32 (0.69314693211407f);
	const auto     c2 = Vf32 (0.24022037362574f);
	const auto     c3 = Vf32 (0.0555072548370f);
	const auto     c4 = Vf32 (0.0096798351988f);
	const auto     c5 = Vf32 (0.0013285658116f);

	// Note: the following set of coefficients has a larger error (0.00043
	// cents, maybe 7 ulp?) but ensures C2 continuity:
	// c0 = 1.000000237
	// c1 = 0.69314655
	// c2 = 0.24021519
	// c3 = 0.05550965
	// c4 = 0.00969821
	// c5 = 0.00132508

	// i = round (v)
	// v = v - i   
#if fstb_ARCHI == fstb_ARCHI_X86
	auto           i = _mm_cvtps_epi32 (v);          
	v -= _mm_cvtepi32_ps (i);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const int      round_ofs = 256;
	const auto     r = Vf32 (round_ofs + 0.5f);
	auto           i = vcvtq_s32_f32 (v + r);
	i -= Vs32 (round_ofs);
	v -= vcvtq_f32_s32 (i);
#endif // fstb_ARCHI

	// Estrin-Horner evaluation scheme
	const auto     v2  = v * v;
	const auto     p23 = fmadd (c3, v, c2);
	const auto     p01 = fmadd (c1, v, c0);
	auto           p   = fmadd (c5, v, c4);
	p = fmadd (p, v2, p23);
	p = fmadd (p, v2, p01);

	// i << 23
	// r = (2^i) * (2^v)
	// directly in floating point exponent
#if fstb_ARCHI == fstb_ARCHI_X86
	i = _mm_slli_epi32 (i, 23);
	return _mm_castsi128_ps (_mm_add_epi32 (i, _mm_castps_si128 (p)));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	i = vshlq_n_s32 (i, 23);
	return vreinterpretq_f32_s32 (i + vreinterpretq_s32_f32 (p));
#endif // fstb_ARCHI

#endif // fstb_HAS_SIMD
}



Vf32	ToolsSimd::select (Vf32 cond, Vf32 v_t, Vf32 v_f) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	/*** To do: implement as r = v_f ^ ((v_f ^ v_t) & cond) ***/
	Combo          cc;
	Combo          ct;
	Combo          cf;
	Combo          r;
	cc._vf32 = cond;
	ct._vf32 = v_t;
	cf._vf32 = v_f;
	r._s32 [0] = (ct._s32 [0] & cc._s32 [0]) | (cf._s32 [0] & ~cc._s32 [0]);
	r._s32 [1] = (ct._s32 [1] & cc._s32 [1]) | (cf._s32 [1] & ~cc._s32 [1]);
	r._s32 [2] = (ct._s32 [2] & cc._s32 [2]) | (cf._s32 [2] & ~cc._s32 [2]);
	r._s32 [3] = (ct._s32 [3] & cc._s32 [3]) | (cf._s32 [3] & ~cc._s32 [3]);
	return r._vf32;
#elif fstb_ARCHI == fstb_ARCHI_X86
	const auto     cond_1 = _mm_and_ps (cond, v_t);
	const auto     cond_0 = _mm_andnot_ps (cond, v_f);
	return _mm_or_ps (cond_0, cond_1);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vbslq_f32 (vreinterpretq_u32_f32 (cond), v_t, v_f);
#endif // fstb_ARCHI
}



std::tuple <Vf32, Vf32>	ToolsSimd::swap_cond (Vf32 cond, Vf32 lhs, Vf32 rhs) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	Combo          cc;
	cc._vf32 = cond;
	if (cc._s32 [0] != 0) { std::swap (lhs._x [0], rhs._x [0]); }
	if (cc._s32 [1] != 0) { std::swap (lhs._x [1], rhs._x [1]); }
	if (cc._s32 [2] != 0) { std::swap (lhs._x [2], rhs._x [2]); }
	if (cc._s32 [3] != 0) { std::swap (lhs._x [3], rhs._x [3]); }
	return std::make_tuple (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_X86
	const auto     inv = _mm_and_ps (_mm_xor_ps (lhs, rhs), cond);
	return std::make_tuple (
		_mm_xor_ps (lhs, inv),
		_mm_xor_ps (rhs, inv)
	);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const auto     cu32 = vreinterpretq_u32_f32 (cond);
	return std::make_tuple (
		vbslq_f32 (cu32, rhs, lhs),
		vbslq_f32 (cu32, lhs, rhs)
	);
#endif // fstb_ARCHI
}



int	ToolsSimd::count_bits (Vs32 x) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	// http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
	uint32_t       v0 = x._x [0] - ((x._x [0] >> 1) & 0x55555555);
	uint32_t       v1 = x._x [1] - ((x._x [1] >> 1) & 0x55555555);
	uint32_t       v2 = x._x [2] - ((x._x [2] >> 1) & 0x55555555);
	uint32_t       v3 = x._x [3] - ((x._x [3] >> 1) & 0x55555555);
	v0 = (v0 & 0x33333333) + ((v0 >> 2) & 0x33333333);
	v1 = (v1 & 0x33333333) + ((v1 >> 2) & 0x33333333);
	v2 = (v2 & 0x33333333) + ((v2 >> 2) & 0x33333333);
	v3 = (v3 & 0x33333333) + ((v3 >> 2) & 0x33333333);
	const int      c0 = (((v0 + (v0 >> 4)) & 0xF0F0F0FU) * 0x1010101) >> 24;
	const int      c1 = (((v1 + (v1 >> 4)) & 0xF0F0F0FU) * 0x1010101) >> 24;
	const int      c2 = (((v2 + (v2 >> 4)) & 0xF0F0F0FU) * 0x1010101) >> 24;
	const int      c3 = (((v3 + (v3 >> 4)) & 0xF0F0F0FU) * 0x1010101) >> 24;
	return (c0 + c2) + (c1 + c3);
#elif fstb_ARCHI == fstb_ARCHI_X86
	// https://stackoverflow.com/questions/17354971/fast-counting-the-number-of-set-bits-in-m128i-register
	static const __m128i  popcount_mask1 = _mm_set1_epi8 (0x77);
	static const __m128i  popcount_mask2 = _mm_set1_epi8 (0x0F);
	// Count bits in each 4-bit field.
	__m128i        n = _mm_srli_epi64(x, 1);
	n = _mm_and_si128 (popcount_mask1, n);
	x = _mm_sub_epi8 (x, n);
	n = _mm_srli_epi64 (n, 1);
	n = _mm_and_si128 (popcount_mask1, n);
	x = _mm_sub_epi8 (x, n);
	n = _mm_srli_epi64 (n, 1);
	n = _mm_and_si128 (popcount_mask1, n);
	n = _mm_sub_epi8 (x, n);
	n = _mm_add_epi8 (n, _mm_srli_epi16 (n, 4));
	n = _mm_and_si128 (popcount_mask2, n);
	// Counts the number of bits in the low and high 64-bit parts
	n = _mm_sad_epu8 (n, _mm_setzero_si128 ());
	// Counts the number of bits in the whole 128-bit register
	n = _mm_add_epi32 (n, _mm_unpackhi_epi64 (n, n));
	return _mm_cvtsi128_si32 (n);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const uint8x16_t  cnt_8  = vcntq_u8 (vreinterpretq_u8_s32 (x));
	const uint16x8_t  cnt_16 = vpaddlq_u8 (cnt_8);
	const uint32x4_t  cnt_32 = vpaddlq_u16 (cnt_16);
	const uint64x2_t  cnt_64 = vpaddlq_u32 (cnt_32);
	const int32x4_t   cnt_s  = vreinterpretq_s32_u64 (cnt_64);
	return vgetq_lane_s32 (cnt_s, 0) + vgetq_lane_s32 (cnt_s, 2);
#endif // fstb_ARCHI
}



Vf32	ToolsSimd::cast_f32 (Vs32 x) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return *reinterpret_cast <const Vf32 *> (&x);
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_castsi128_ps (x);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_f32_s32 (x);
#endif
}



Vs32	ToolsSimd::cast_s32 (Vf32 x) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return *reinterpret_cast <const Vs32 *> (&x);
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_castps_si128 (x);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_s32_f32 (x);
#endif
}



// a, b, c, d -> a+c, b+d, a-c, b-d
Vf32	ToolsSimd::butterfly_f32_w64 (Vf32 x) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 { {
		x._x [0] + x._x [2],
		x._x [1] + x._x [3],
		x._x [0] - x._x [2],
		x._x [1] - x._x [3]
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	const auto sign = _mm_castsi128_ps (_mm_setr_epi32 (0, 0, _sign32, _sign32));
	const auto x0   = _mm_shuffle_ps (x, x, (2<<0) + (3<<2) + (0<<4) + (1<<6)); // c, d, a, b
	const auto x1   = _mm_xor_ps (x, sign); // a, b, -c, -d
	return x0 + x1;
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const auto sign = int32x4_t { 0, 0, _sign32, _sign32 };
	const auto x0   = vcombine_f32 (vget_high_f32 (x), vget_low_f32 (x)); // c, d, a, b
	const auto x1   = // a, b, -c, -d
		vreinterpretq_f32_s32 (veorq_s32 (vreinterpretq_s32_f32 (x), sign));
	return x0 + x1;
#endif
}



// a, b, c, d -> a+b, a-b, c+d, c-d
Vf32	ToolsSimd::butterfly_f32_w32 (Vf32 x) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 { {
		x._x [0] + x._x [1],
		x._x [0] + x._x [1],
		x._x [2] - x._x [3],
		x._x [2] - x._x [3]
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	const auto sign = _mm_castsi128_ps (_mm_setr_epi32 (0, _sign32, 0, _sign32));
	const auto x0   = _mm_shuffle_ps (x, x, (1<<0) + (0<<2) + (3<<4) + (2<<6)); // b, a, d, c
	const auto x1   = _mm_xor_ps (x, sign); // a, -b, c, -d
	return x0 + x1;
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const auto sign = int32x4_t { 0, _sign32, 0, _sign32 };
	const auto x0   = vrev64q_f32 (x); // b, a, d, c
	const auto x1   = // a, -b, c, -d
		vreinterpretq_f32_s32 (veorq_s32 (vreinterpretq_s32_f32 (x), sign));
	return x0 + x1;
#endif
}



// p0[0 1] p1[0 1]
Vf32	ToolsSimd::interleave_2f32_lo (Vf32 p0, Vf32 p1) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 { { p0._x [0], p0._x [1], p1._x [0], p1._x [1] } };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_shuffle_ps (p0, p1, (0<<0) + (1<<2) + (0<<4) + (1<<6));
	// return _mm_movelh_ps (p0, p1);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const float32x2_t  p0x = vget_low_f32 (p0);
	const float32x2_t  p1x = vget_low_f32 (p1);
	return vcombine_f32 (p0x, p1x);
#endif // fstb_ARCHI
}



// p0[2 3] p1[2 3]
Vf32	ToolsSimd::interleave_2f32_hi (Vf32 p0, Vf32 p1) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 { { p0._x [2], p0._x [3], p1._x [2], p1._x [3] } };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_shuffle_ps (p0, p1, (2<<0) + (3<<2) + (2<<4) + (3<<6));
	// return _mm_movehl_ps (p1, p0);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const float32x2_t  p0x = vget_high_f32 (p0);
	const float32x2_t  p1x = vget_high_f32 (p1);
	return vcombine_f32 (p0x, p1x);
#endif // fstb_ARCHI
}



void	ToolsSimd::interleave_f32 (Vf32 &i0, Vf32 &i1, Vf32 p0, Vf32 p1) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	i0._x [0] = p0._x [0];
	i0._x [1] = p1._x [0];
	i0._x [2] = p0._x [1];
	i0._x [3] = p1._x [1];
	i1._x [0] = p0._x [2];
	i1._x [1] = p1._x [2];
	i1._x [2] = p0._x [3];
	i1._x [3] = p1._x [3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	i0 = _mm_unpacklo_ps (p0, p1);
	i1 = _mm_unpackhi_ps (p0, p1);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const float32x4x2_t  tmp = vzipq_f32 (p0, p1);
	i0 = tmp.val [0];
	i1 = tmp.val [1];
#endif // fstb_ARCHI
}



void	ToolsSimd::deinterleave_f32 (Vf32 &p0, Vf32 &p1, Vf32 i0, Vf32 i1) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	p0._x [0] = i0._x [0];
	p1._x [0] = i0._x [1];
	p0._x [1] = i0._x [2];
	p1._x [1] = i0._x [3];
	p0._x [2] = i1._x [0];
	p1._x [2] = i1._x [1];
	p0._x [3] = i1._x [2];
	p1._x [3] = i1._x [3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	p0 = _mm_shuffle_ps (i0, i1, (0<<0) | (2<<2) | (0<<4) | (2<<6));
	p1 = _mm_shuffle_ps (i0, i1, (1<<0) | (3<<2) | (1<<4) | (3<<6));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const float32x4x2_t  tmp = vuzpq_f32 (i0, i1);
	p0 = tmp.val [0];
	p1 = tmp.val [1];
#endif // fstb_ARCHI
}



Vf32	ToolsSimd::deinterleave_f32_lo (Vf32 i0, Vf32 i1) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 { { i0._x [0], i0._x [2], i1._x [0], i1._x [2] } };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_shuffle_ps (i0, i1, (0<<0) | (2<<2) | (0<<4) | (2<<6));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vuzpq_f32 (i0, i1).val [0];
#endif // fstb_ARCHI
}



Vf32	ToolsSimd::deinterleave_f32_hi (Vf32 i0, Vf32 i1) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 { { i0._x [1], i0._x [3], i1._x [1], i1._x [3] } };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_shuffle_ps (i0, i1, (1<<0) | (3<<2) | (1<<4) | (3<<6));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vuzpq_f32 (i0, i1).val [1];
#endif // fstb_ARCHI
}



// Sources:
// https://github.com/Maratyszcza/NNPACK/blob/master/src/neon/transpose.h
// https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_MM_TRANSPOSE4_PS&expand=5915,5949
void	ToolsSimd::transpose_f32 (Vf32 &a0, Vf32 &a1, Vf32 &a2, Vf32 &a3) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	Vf32           tmp0;
	Vf32           tmp1;
	Vf32           tmp2;
	Vf32           tmp3;
	interleave_f32 (tmp0, tmp1, a0, a1);
	interleave_f32 (tmp2, tmp3, a2, a3);
	a0 = interleave_2f32_lo (tmp0, tmp2);
	a1 = interleave_2f32_hi (tmp0, tmp2);
	a2 = interleave_2f32_lo (tmp1, tmp3);
	a3 = interleave_2f32_hi (tmp1, tmp3);
#elif fstb_ARCHI == fstb_ARCHI_X86
	const __m128   tmp0 = _mm_unpacklo_ps (a0, a1);
	const __m128   tmp2 = _mm_unpacklo_ps (a2, a3);
	const __m128   tmp1 = _mm_unpackhi_ps (a0, a1);
	const __m128   tmp3 = _mm_unpackhi_ps (a2, a3);
	a0 = _mm_movelh_ps (tmp0, tmp2);
	a1 = _mm_movehl_ps (tmp2, tmp0);
	a2 = _mm_movelh_ps (tmp1, tmp3);
	a3 = _mm_movehl_ps (tmp3, tmp1);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const float32x4x2_t a01 = vtrnq_f32 (a0, a1);
	const float32x4x2_t a23 = vtrnq_f32 (a2, a3);
	a0 = vcombine_f32 (vget_low_f32 (a01.val [0]), vget_low_f32 (a23.val [0]));
	a1 = vcombine_f32 (vget_low_f32 (a01.val [1]), vget_low_f32 (a23.val [1]));
	a2 = vcombine_f32 (vget_high_f32 (a01.val [0]), vget_high_f32 (a23.val [0]));
	a3 = vcombine_f32 (vget_high_f32 (a01.val [1]), vget_high_f32 (a23.val [1]));
#elif
	// Generic form
	Vf32           k0, k1, k2, k3;
	interleave_f32 (k0, k1, a0, a2);
	interleave_f32 (k2, k3, a1, a3);
	interleave_f32 (a0, a1, k0, k2);
	interleave_f32 (a2, a3, k1, k3);
#endif // fstb_ARCHI
}



Vf32	ToolsSimd::monofy_2f32_lo (Vf32 v) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 { { v._x [0], v._x [0], v._x [2], v._x [2] } };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_shuffle_ps (v, v, 0xA0);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vuzpq_f32 (v, v).val [0];
#endif // fstb_ARCHI
}



Vf32	ToolsSimd::monofy_2f32_hi (Vf32 v) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 { { v._x [1], v._x [1], v._x [3], v._x [3] } };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_shuffle_ps (v, v, 0xF5);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vuzpq_f32 (v, v).val [1];
#endif // fstb_ARCHI
}



// Rounding method is unspecified (towards 0 on ARM, depends on MXCSR on x86)
Vs32	ToolsSimd::conv_f32_to_s32 (Vf32 x) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vs32 { {
		conv_int_fast (x._x [0]),
		conv_int_fast (x._x [1]),
		conv_int_fast (x._x [2]),
		conv_int_fast (x._x [3])
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_cvtps_epi32 (x);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vcvtq_s32_f32 (x);
#endif // fstb_ARCHI
}



// Not exact on the rounding boundaries
// Assumes rounding mode is to-nearest on x86
Vs32	ToolsSimd::round_f32_to_s32 (Vf32 x) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vs32 { {
		round_int (x._x [0]),
		round_int (x._x [1]),
		round_int (x._x [2]),
		round_int (x._x [3])
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_cvtps_epi32 (x);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const auto     zero = vdupq_n_f32 ( 0.0f);
	const auto     m    = vdupq_n_f32 (-0.5f);
	const auto     p    = vdupq_n_f32 (+0.5f);
	const auto     gt0  = vcgtq_f32 (x, zero);
	const auto     u    = vbslq_f32 (gt0, p, m);
	x = vaddq_f32 (x, u);
	return vcvtq_s32_f32 (x);
#endif // fstb_ARCHI
}



// Not exact on the rounding boundaries
// Assumes rounding mode is to-nearest on x86
Vs32	ToolsSimd::floor_f32_to_s32 (Vf32 x) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vs32 { {
		floor_int (x._x [0]),
		floor_int (x._x [1]),
		floor_int (x._x [2]),
		floor_int (x._x [3])
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	x = _mm_add_ps (x, _mm_set1_ps (-0.5f));
	return _mm_cvtps_epi32 (x);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const auto     zero = vdupq_n_f32 (0.0f);
	const auto     one  = vdupq_n_f32 (1.0f);
	const auto     gt0  = vcgtq_f32 (x, zero);
	x = vbslq_f32 (gt0, x, vsubq_f32 (one, x));
	auto           i    = vcvtq_s32_f32 (x);
	i = vbslq_s32 (gt0, i, vnegq_s32 (i));
	return i;
#endif // fstb_ARCHI
}



Vf32	ToolsSimd::conv_s32_to_f32 (Vs32 x) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 { {
		float (x._x [0]),
		float (x._x [1]),
		float (x._x [2]),
		float (x._x [3])
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_cvtepi32_ps (x);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vcvtq_f32_s32 (x);
#endif // fstb_ARCHI
}



void	ToolsSimd::start_lerp (Vf32 &val_cur, Vf32 &step, float val_beg, float val_end, int size) noexcept
{
	assert (size > 0);

	const float    dif = val_end - val_beg;
	const float    four_over_size =
		  (size < _inv_table_4_len)
		? _inv_table_4 [size]
		: 4.0f / float (size);
	step    = Vf32 (dif * four_over_size);
	val_cur = Vf32 (val_beg);
	const auto     c0123 = Vf32 (0, 0.25f, 0.5f, 0.75f);
	val_cur.mac (step, c0123);
}



Vs32	ToolsSimd::select (Vs32 cond, Vs32 v_t, Vs32 v_f) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	/*** To do: implement as r = v_f ^ ((v_f ^ v_t) & cond) ***/
	return Vs32 { {
		(cond._x [0] & v_t._x [0]) | (~cond._x [0] & v_f._x [0]),
		(cond._x [1] & v_t._x [1]) | (~cond._x [1] & v_f._x [1]),
		(cond._x [2] & v_t._x [2]) | (~cond._x [2] & v_f._x [2]),
		(cond._x [3] & v_t._x [3]) | (~cond._x [3] & v_f._x [3])
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	const auto     cond_1 = _mm_and_si128 (cond, v_t);
	const auto     cond_0 = _mm_andnot_si128 (cond, v_f);
	return _mm_or_si128 (cond_0, cond_1);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vbslq_s32 (vreinterpretq_u32_s32 (cond), v_t, v_f);
#endif // fstb_ARCHI
}



std::tuple <Vs32, Vs32>	ToolsSimd::swap_cond (Vs32 cond, Vs32 lhs, Vs32 rhs) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	if (cond._x [0] != 0) { std::swap (lhs._x [0], rhs._x [0]); }
	if (cond._x [1] != 0) { std::swap (lhs._x [1], rhs._x [1]); }
	if (cond._x [2] != 0) { std::swap (lhs._x [2], rhs._x [2]); }
	if (cond._x [3] != 0) { std::swap (lhs._x [3], rhs._x [3]); }
	return std::make_tuple (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_X86
	const auto     inv = _mm_and_si128 (_mm_xor_si128 (lhs, rhs), cond);
	return std::make_tuple (
		_mm_xor_si128 (lhs, inv),
		_mm_xor_si128 (rhs, inv)
	);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const auto     cond_u = vreinterpretq_u32_s32 (cond);
	return std::make_tuple (
		vbslq_s32 (cond_u, rhs, lhs),
		vbslq_s32 (cond_u, lhs, rhs)
	);
#endif // fstb_ARCHI
}



Vs32	ToolsSimd::min_s32 (Vs32 lhs, Vs32 rhs) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vs32 { {
		std::min (lhs._x [0], rhs._x [0]),
		std::min (lhs._x [1], rhs._x [1]),
		std::min (lhs._x [2], rhs._x [2]),
		std::min (lhs._x [3], rhs._x [3])
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	const auto     gt = _mm_cmpgt_epi32 (lhs, rhs);
	return select (gt, lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vminq_s32 (lhs, rhs);
#endif // fstb_ARCHI
}



Vs32	ToolsSimd::max_s32 (Vs32 lhs, Vs32 rhs) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vs32 { {
		std::max (lhs._x [0], rhs._x [0]),
		std::max (lhs._x [1], rhs._x [1]),
		std::max (lhs._x [2], rhs._x [2]),
		std::max (lhs._x [3], rhs._x [3])
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	const auto     lt = _mm_cmplt_epi32 (lhs, rhs);
	return select (lt, lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vmaxq_s32 (lhs, rhs);
#endif // fstb_ARCHI
}



template <int N>
Vs32	ToolsSimd::srai_s32 (Vs32 x) noexcept
{
	static_assert (N >= 0, "");
	static_assert (N <= 32, "");
#if ! defined (fstb_HAS_SIMD)
	return Vs32 { {
		x._x [0] >> N,
		x._x [1] >> N,
		x._x [2] >> N,
		x._x [3] >> N
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_srai_epi32 (x, N);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vshrq_n_s32 (x, N);
#endif // fstb_ARCHI
}



template <int N>
Vs32	ToolsSimd::srli_s32 (Vs32 x) noexcept
{
	static_assert (N >= 0, "");
	static_assert (N <= 32, "");
#if ! defined (fstb_HAS_SIMD)
	return Vs32 { {
		int32_t (uint32_t (x._x [0]) >> N),
		int32_t (uint32_t (x._x [1]) >> N),
		int32_t (uint32_t (x._x [2]) >> N),
		int32_t (uint32_t (x._x [3]) >> N)
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_srli_epi32 (x, N);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_s32_u32 (vshrq_n_u32 (vreinterpretq_u32_s32 (x), N));
#endif // fstb_ARCHI
}



template <int N>
Vs32	ToolsSimd::slli_s32 (Vs32 x) noexcept
{
	static_assert (N >= 0, "");
	static_assert (N <= 32, "");
#if ! defined (fstb_HAS_SIMD)
	return Vs32 { {
		x._x [0] << N,
		x._x [1] << N,
		x._x [2] << N,
		x._x [3] << N
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_slli_epi32 (x, N);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vshlq_n_s32 (x, N);
#endif // fstb_ARCHI
}



// Positive = to the left, rotates towards the higher indexes
template <int SHIFT>
Vf32	ToolsSimd::Shift <SHIFT>::rotate (Vf32 a) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 { {
		a._x [(0 - SHIFT) & 3],
		a._x [(1 - SHIFT) & 3],
		a._x [(2 - SHIFT) & 3],
		a._x [(3 - SHIFT) & 3]
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	switch (SHIFT & 3)
	{
	case 1:  return _mm_shuffle_ps (a, a, (2<<6) | (1<<4) | (0<<2) | (3<<0));
	case 2:  return _mm_shuffle_ps (a, a, (1<<6) | (0<<4) | (3<<2) | (2<<0));
	case 3:  return _mm_shuffle_ps (a, a, (0<<6) | (3<<4) | (2<<2) | (1<<0));
	default: return a;
	}
#elif fstb_ARCHI == fstb_ARCHI_ARM
	int32x4_t     aa = vreinterpretq_s32_f32 (a);
	switch (SHIFT & 3)
	{
	case 1:  aa = vextq_s32 (aa, aa, 3); break;
	case 2:  aa = vextq_s32 (aa, aa, 2); break;
	case 3:  aa = vextq_s32 (aa, aa, 1); break;
	default: /* Nothing */               break;
	}
	return vreinterpretq_f32_s32 (aa);
#endif // fstb_ARCHI
}



template <int SHIFT>
float	ToolsSimd::Shift <SHIFT>::extract (Vf32 a) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return a._x [SHIFT & 3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	switch (SHIFT & 3)
	{
	case 1:  a = _mm_shuffle_ps (a, a, 1);	break;
	case 2:  a = _mm_shuffle_ps (a, a, 2);	break;
	case 3:  a = _mm_shuffle_ps (a, a, 3);	break;
	default: /* Nothing */                 break;
	}
	return _mm_cvtss_f32 (a);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vgetq_lane_f32 (a, SHIFT & 3);
#endif // fstb_ARCHI
}



template <int SHIFT>
Vf32	ToolsSimd::Shift <SHIFT>::insert (Vf32 a, float val) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	a._x [SHIFT & 3] = val;
	return a;
#elif fstb_ARCHI == fstb_ARCHI_X86
	a = Shift <(-SHIFT) & 3>::rotate (a);
	a = _mm_move_ss (a, _mm_set_ss (val));
	a = Shift <  SHIFT     >::rotate (a);
	return a;
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vsetq_lane_f32 (val, a, SHIFT & 3);
#endif // fstb_ARCHI
}



template <int SHIFT>
Vf32	ToolsSimd::Shift <SHIFT>::spread (Vf32 a) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 (extract (a));
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_shuffle_ps (a, a, 0x55 * (SHIFT & 3));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vdupq_n_f32 (vgetq_lane_f32 (a, SHIFT & 3));
#endif // fstb_ARCHI
}



// Extracts the vector at the position SHIFT from the double-width vector {a b}
// Concatenates a [SHIFT...3] with b [0...3-SHIFT]
template <int SHIFT>
Vf32	ToolsSimd::Shift <SHIFT>::compose (Vf32 a, Vf32 b) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	switch (SHIFT & 3)
	{
	case 1: a._x [0] = a._x [1]; a._x [1] = a._x [2]; a._x [2] = a._x [3]; a._x [3] = b._x [0]; break;
	case 2: a._x [0] = a._x [2]; a._x [1] = a._x [3]; a._x [2] = b._x [0]; a._x [3] = b._x [1]; break;
	case 3: a._x [0] = a._x [3]; a._x [1] = b._x [0]; a._x [2] = b._x [1]; a._x [3] = b._x [2]; break;
	default: break;
	}
	return a;
#elif fstb_ARCHI == fstb_ARCHI_X86
	switch (SHIFT & 3)
	{
	case 1:
		a = _mm_move_ss (a, b);
		return _mm_shuffle_ps (a, a, (0<<6) | (3<<4) | (2<<2) | (1<<0));
	case 2:
		return _mm_shuffle_ps (a, b, (1<<6) | (0<<4) | (3<<2) | (2<<0));
	case 3:
		a = _mm_shuffle_ps (a, a, (2<<6) | (1<<4) | (0<<2) | (3<<0));
		b = _mm_shuffle_ps (b, b, (2<<6) | (1<<4) | (0<<2) | (3<<0));
		return _mm_move_ss (b, a);
	default:
		return a;
	}
#elif fstb_ARCHI == fstb_ARCHI_ARM
	int32x4_t        aa = vreinterpretq_s32_f32 (a);
	const int32x4_t  bb = vreinterpretq_s32_f32 (b);
	switch (SHIFT & 3)
	{
	case 1:  aa = vextq_s32 (aa, bb, 1); break;
	case 2:  aa = vextq_s32 (aa, bb, 2); break;
	case 3:  aa = vextq_s32 (aa, bb, 3); break;
	default: /* Nothing */               break;
	}
	return vreinterpretq_f32_s32 (aa);
#endif // fstb_ARCHI
}



// Positive = left
template <int SHIFT>
Vs32	ToolsSimd::Shift <SHIFT>::rotate (Vs32 a) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vs32 { {
		a._x [(0 - SHIFT) & 3],
		a._x [(1 - SHIFT) & 3],
		a._x [(2 - SHIFT) & 3],
		a._x [(3 - SHIFT) & 3]
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	switch (SHIFT & 3)
	{
	case 1:  return _mm_shuffle_epi32 (a, (2<<6) | (1<<4) | (0<<2) | (3<<0));
	case 2:  return _mm_shuffle_epi32 (a, (1<<6) | (0<<4) | (3<<2) | (2<<0));
	case 3:  return _mm_shuffle_epi32 (a, (0<<6) | (3<<4) | (2<<2) | (1<<0));
	default: return a;
	}
#elif fstb_ARCHI == fstb_ARCHI_ARM
	switch (SHIFT & 3)
	{
	case 1:  a = vextq_s32 (a, a, 3); break;
	case 2:  a = vextq_s32 (a, a, 2); break;
	case 3:  a = vextq_s32 (a, a, 1); break;
	default: /* Nothing */            break;
	}
	return a;
#endif // fstb_ARCHI
}



template <int SHIFT>
int32_t	ToolsSimd::Shift <SHIFT>::extract (Vs32 a) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return a._x [SHIFT & 3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	switch (SHIFT & 3)
	{
	case 1:  a = _mm_shuffle_epi32 (a, 1);	break;
	case 2:  a = _mm_shuffle_epi32 (a, 2);	break;
	case 3:  a = _mm_shuffle_epi32 (a, 3);	break;
	default: /* Nothing */                 break;
	}
	return _mm_cvtsi128_si32 (a);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vgetq_lane_s32 (a, SHIFT & 3);
#endif // fstb_ARCHI
}



template <int SHIFT>
Vs32	ToolsSimd::Shift <SHIFT>::insert (Vs32 a, int32_t val) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	a._x [SHIFT & 3] = val;
	return a;
#elif fstb_ARCHI == fstb_ARCHI_X86
	a = Shift <(-SHIFT) & 3>::rotate (a);
	a = _mm_castps_si128 (_mm_move_ss (
		_mm_castsi128_ps (a),
		_mm_castsi128_ps (_mm_set1_epi32 (val))
	));
	a = Shift <  SHIFT     >::rotate (a);
	return a;
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vsetq_lane_s32 (val, a, SHIFT & 3);
#endif // fstb_ARCHI
}



template <int SHIFT>
Vs32	ToolsSimd::Shift <SHIFT>::spread (Vs32 a) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vs32 (extract (a));
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_shuffle_epi32 (a, 0x55 * (SHIFT & 3));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vdupq_n_s32 (vgetq_lane_s32 (a, SHIFT & 3));
#endif // fstb_ARCHI
}



// Extracts the vector at the position SHIFT from the double-width vector {a b}
// Concatenates a [SHIFT...3] with b [0...3-SHIFT]
template <int SHIFT>
Vs32	ToolsSimd::Shift <SHIFT>::compose (Vs32 a, Vs32 b) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	switch (SHIFT & 3)
	{
	case 1: a._x [0] = a._x [1]; a._x [1] = a._x [2]; a._x [2] = a._x [3]; a._x [3] = b._x [0]; break;
	case 2: a._x [0] = a._x [2]; a._x [1] = a._x [3]; a._x [2] = b._x [0]; a._x [3] = b._x [1]; break;
	case 3: a._x [0] = a._x [3]; a._x [1] = b._x [0]; a._x [2] = b._x [1]; a._x [3] = b._x [2]; break;
	default: break;
	}
	return a;
#elif fstb_ARCHI == fstb_ARCHI_X86
	switch (SHIFT & 3)
	{
	case 1:
		a = _mm_castps_si128 (_mm_move_ss (
			_mm_castsi128_ps (a), _mm_castsi128_ps (b)
		));
		return _mm_shuffle_epi32 (a, (0<<6) | (3<<4) | (2<<2) | (1<<0));
	case 2:
		return _mm_castps_si128 (_mm_shuffle_ps (
			_mm_castsi128_ps (a),
			_mm_castsi128_ps (b),
			(1<<6) | (0<<4) | (3<<2) | (2<<0)
		));
	case 3:
		a = _mm_shuffle_epi32 (a, (2<<6) | (1<<4) | (0<<2) | (3<<0));
		b = _mm_shuffle_epi32 (b, (2<<6) | (1<<4) | (0<<2) | (3<<0));
		return _mm_castps_si128 (_mm_move_ss (
			_mm_castsi128_ps (b), _mm_castsi128_ps (a)
		));
	default:
		return a;
	}
#elif fstb_ARCHI == fstb_ARCHI_ARM
	switch (SHIFT & 3)
	{
	case 1:  a = vextq_s32 (a, b, 1); break;
	case 2:  a = vextq_s32 (a, b, 2); break;
	case 3:  a = vextq_s32 (a, b, 3); break;
	default: /* Nothing */            break;
	}
	return a;
#endif // fstb_ARCHI
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace fstb



#endif   // fstb_ToolsSimd_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
