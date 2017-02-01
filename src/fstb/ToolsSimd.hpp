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



namespace fstb
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ToolsSimd::VectF32	ToolsSimd::load_f32 (const void *ptr)
{
	assert (is_ptr_align_nz (ptr, 16));

#if fstb_IS (ARCHI, X86)
	return _mm_load_ps (reinterpret_cast <const float *> (ptr));
#elif fstb_IS (ARCHI, ARM)
	return *reinterpret_cast <const VectF32 *> (ptr);
#endif // ff_arch_CPU
}



void	ToolsSimd::store_f32 (void *ptr, VectF32 v)
{
	assert (is_ptr_align_nz (ptr, 16));

#if fstb_IS (ARCHI, X86)
	_mm_store_ps (reinterpret_cast <float *> (ptr), v);
#elif fstb_IS (ARCHI, ARM)
	*reinterpret_cast <VectF32 *> (ptr) = v;
#endif // ff_arch_CPU
}



// n = number of scalars to store (from the LSB)
void	ToolsSimd::store_f32_part (void *ptr, VectF32 v, int n)
{
	assert (n > 0);

	if (n >= 4)
	{
		store_f32 (ptr, v);
	}
	else
	{
		store_f32_part_n13 (ptr, v, n);
	}
}



ToolsSimd::VectF32	ToolsSimd::loadu_f32 (const void *ptr)
{
	assert (ptr != 0);

#if fstb_IS (ARCHI, X86)
	return _mm_loadu_ps (reinterpret_cast <const float *> (ptr));
#elif fstb_IS (ARCHI, ARM)
	return vreinterpretq_f32_u8 (
		vld1q_u8 (reinterpret_cast <const uint8_t *> (ptr))
	);
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::loadu_f32_part (const void *ptr, int n)
{
	assert (n > 0);

	if (n >= 4)
	{
		return loadu_f32 (ptr);
	}

	return load_f32_part_n13 (ptr, n);
}



void	ToolsSimd::storeu_f32 (void *ptr, VectF32 v)
{
	assert (ptr != 0);

#if fstb_IS (ARCHI, X86)
	_mm_storeu_ps (reinterpret_cast <float *> (ptr), v);
#elif fstb_IS (ARCHI, ARM)
	vst1q_u8 (reinterpret_cast <uint8_t *> (ptr), vreinterpretq_u8_f32 (v));
#endif // ff_arch_CPU
}



void	ToolsSimd::storeu_s32 (void *ptr, VectS32 v)
{
	assert (ptr != 0);

#if fstb_IS (ARCHI, X86)
	_mm_storeu_si128 (reinterpret_cast <__m128i *> (ptr), v);
#elif fstb_IS (ARCHI, ARM)
	vst1q_u8 (reinterpret_cast <uint8_t *> (ptr), vreinterpretq_u8_s32 (v));
#endif // ff_arch_CPU
}



// n = number of scalars to store (from the LSB)
void	ToolsSimd::storeu_f32_part (void *ptr, VectF32 v, int n)
{
	assert (n > 0);

	if (n >= 4)
	{
		storeu_f32 (ptr, v);
	}
	else
	{
		store_f32_part_n13 (ptr, v, n);
	}
}



// n = number of scalars to store (from the LSB)
void	ToolsSimd::storeu_s32_part (void *ptr, VectS32 v, int n)
{
	assert (n > 0);

	if (n >= 4)
	{
		storeu_s32 (ptr, v);
	}
	else
	{
		store_s32_part_n13 (ptr, v, n);
	}
}



// Returns: ptr [0] | ptr [1] | ? | ?
ToolsSimd::VectF32	ToolsSimd::loadu_2f32 (const void *ptr)
{
	assert (ptr != 0);

#if fstb_IS (ARCHI, X86)
	const auto     x_0 = _mm_load_ss (reinterpret_cast <const float *> (ptr)    );
	const auto     x_1 = _mm_load_ss (reinterpret_cast <const float *> (ptr) + 1);
	const auto     x   = _mm_unpacklo_ps (x_0, x_1);
	return x;
#elif fstb_IS (ARCHI, ARM)
	const float32x2_t x = vreinterpret_f32_u8 (
		vld1_u8 (reinterpret_cast <const uint8_t *> (ptr))
	);
	return vcombine_f32 (x, x);
#endif // ff_arch_CPU
}



// ptr [0] = v0
// ptr [1] = v1
void	ToolsSimd::storeu_2f32 (void *ptr, VectF32 v)
{
	assert (ptr != 0);

#if fstb_IS (ARCHI, X86)
	_mm_store_ss (reinterpret_cast <float *> (ptr)    , v );
	const auto     v1 = _mm_shuffle_ps (v, v, 1 << 0);
	_mm_store_ss (reinterpret_cast <float *> (ptr) + 1, v1);
#elif fstb_IS (ARCHI, ARM)
	vst1_u8 (
		reinterpret_cast <uint8_t *> (ptr),
		vreinterpret_u8_f32 (vget_low_f32 (v))
	);
#endif // ff_arch_CPU
}



// *ptr = v0
void	ToolsSimd::storeu_1f32 (void *ptr, VectF32 v)
{
	assert (ptr != 0);

#if fstb_IS (ARCHI, X86)
	_mm_store_ss (reinterpret_cast <float *> (ptr), v);
#elif fstb_IS (ARCHI, ARM)
	*reinterpret_cast <float *> (ptr) = vgetq_lane_f32 (v, 0);
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::set_f32_zero ()
{
#if fstb_IS (ARCHI, X86)
	return _mm_setzero_ps ();
#elif fstb_IS (ARCHI, ARM)
	return vdupq_n_f32 (0);
#endif // ff_arch_CPU
}



// Returns a0 | a0 | a0 | a0
ToolsSimd::VectF32	ToolsSimd::set1_f32 (float a)
{
#if fstb_IS (ARCHI, X86)
	return _mm_set1_ps (a);
#elif fstb_IS (ARCHI, ARM)
	return vdupq_n_f32 (a);
#endif // ff_arch_CPU
}



// Returns a0 | a1 | a2 | a3
ToolsSimd::VectF32	ToolsSimd::set_f32 (float a0, float a1, float a2, float a3)
{
#if fstb_IS (ARCHI, X86)
	return _mm_set_ps (a3, a2, a1, a0);
#elif fstb_IS (ARCHI, ARM)
 #if 1
	float32x2_t    v01 = vdup_n_f32 (a0);
	float32x2_t    v23 = vdup_n_f32 (a2);
	v01 = vset_lane_f32 (a1, v01, 1);
	v23 = vset_lane_f32 (a3, v23, 1);
 #else // Not tested
	const float32x2_t    v01 = vcreate_f32 (
		  (uint64_t (*reinterpret_cast <const uint32_t *> (&a0))      )
		| (uint64_t (*reinterpret_cast <const uint32_t *> (&a1)) << 32)
	);
	const float32x2_t    v23 = vcreate_f32 (
		  (uint64_t (*reinterpret_cast <const uint32_t *> (&a2))      )
		| (uint64_t (*reinterpret_cast <const uint32_t *> (&a3)) << 32)
	);
 #endif
	return vcombine_f32 (v01, v23);
#endif // ff_arch_CPU
}



// Returns a0 | a1 | ? | ?
ToolsSimd::VectF32	ToolsSimd::set_2f32 (float a0, float a1)
{
#if fstb_IS (ARCHI, X86)
	return _mm_unpacklo_ps (_mm_set_ss (a0), _mm_set_ss (a1));
#elif fstb_IS (ARCHI, ARM)
	return vsetq_lane_f32 (a1, vdupq_n_f32 (a0), 1);
#endif // ff_arch_CPU
}



// Returns a0 | a1 | a0 | a1
ToolsSimd::VectF32	ToolsSimd::set_2f32_fill (float a02, float a13)
{
#if fstb_IS (ARCHI, X86)
	return _mm_unpacklo_ps (_mm_set1_ps (a02), _mm_set1_ps (a13));
#elif fstb_IS (ARCHI, ARM)
	const float32x2_t v01 = vset_lane_f32 (a13, vdup_n_f32 (a02), 1);
	return vcombine_f32 (v01, v01);
#endif // ff_arch_CPU
}



// Returns a01 | a01 | a23 | a23
ToolsSimd::VectF32	ToolsSimd::set_2f32_dbl (float a01, float a23)
{
#if fstb_IS (ARCHI, X86)
	return _mm_shuffle_ps (_mm_set_ss (a01), _mm_set_ss (a23), 0x00);
#elif fstb_IS (ARCHI, ARM)
	return vcombine_f32 (vdup_n_f32 (a01), vdup_n_f32 (a23));
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::swap_2f32 (VectF32 v)
{
#if fstb_IS (ARCHI, X86)
	return _mm_shuffle_ps (v, v, (2<<0) + (3<<2) + (0<<4) + (1<<6));
#elif fstb_IS (ARCHI, ARM)
	const float32x2_t v01 = vget_low_f32 (v);
	const float32x2_t v23 = vget_high_f32 (v);
	return vcombine_f32 (v23, v01);
#endif // ff_arch_CPU
}



// ra = v0 | v1 | v0 | v1
// rb = v2 | v3 | v2 | v3
void	ToolsSimd::spread_2f32 (VectF32 &ra, VectF32 &rb, VectF32 v)
{
#if fstb_IS (ARCHI, X86)
	ra = _mm_shuffle_ps (v, v, (0<<0) + (1<<2) + (0<<4) + (1<<6));
	rb = _mm_shuffle_ps (v, v, (2<<0) + (3<<2) + (2<<4) + (3<<6));
#elif fstb_IS (ARCHI, ARM)
	const float32x2_t v01 = vget_low_f32 (v);
	const float32x2_t v23 = vget_high_f32 (v);
	ra = vcombine_f32 (v01, v01);
	rb = vcombine_f32 (v23, v23);
#endif // ff_arch_CPU
}



void	ToolsSimd::mac (VectF32 &s, VectF32 a, VectF32 b)
{
#if fstb_IS (ARCHI, X86)
	s = _mm_add_ps (s, _mm_mul_ps (a, b));
#elif fstb_IS (ARCHI, ARM)
	s = vmlaq_f32 (s, a, b);
#endif // ff_arch_CPU
}



void	ToolsSimd::msu (VectF32 &s, VectF32 a, VectF32 b)
{
#if fstb_IS (ARCHI, X86)
	s = _mm_sub_ps (s, _mm_mul_ps (a, b));
#elif fstb_IS (ARCHI, ARM)
	s = vmlsq_f32 (s, a, b);
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::min_f32 (VectF32 lhs, VectF32 rhs)
{
#if fstb_IS (ARCHI, X86)
	return _mm_min_ps (lhs, rhs);
#elif fstb_IS (ARCHI, ARM)
	return vminq_f32 (lhs, rhs);
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::max_f32 (VectF32 lhs, VectF32 rhs)
{
#if fstb_IS (ARCHI, X86)
	return _mm_max_ps (lhs, rhs);
#elif fstb_IS (ARCHI, ARM)
	return vmaxq_f32 (lhs, rhs);
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::round (VectF32 v)
{
#if fstb_IS (ARCHI, X86)
	return _mm_cvtepi32_ps (_mm_cvtps_epi32 (v));
#elif fstb_IS (ARCHI, ARM)
	return vcvtq_f32_s32 (vcvtq_s32_f32 (v));
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::abs (VectF32 v)
{
#if fstb_IS (ARCHI, X86)
	return _mm_and_ps (v, _mm_castsi128_ps (_mm_set1_epi32 (0x7FFFFFFF)));
#elif fstb_IS (ARCHI, ARM)
	return vabsq_f32 (v);
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::rcp_approx (VectF32 v)
{
#if fstb_IS (ARCHI, X86)
	return _mm_rcp_ps (v);
#elif fstb_IS (ARCHI, ARM)
	float32x4_t    r = vrecpeq_f32 (v);
	r = vmulq_f32 (vrecpsq_f32 (v, r), r);
//	r = vmulq_f32 (vrecpsq_f32 (v, r), r); // Only one step needed?
	return r;
#endif // ff_arch_CPU
}



// With more accuracy
ToolsSimd::VectF32	ToolsSimd::rcp_approx2 (VectF32 v)
{
#if fstb_IS (ARCHI, X86)
	__m128         r = _mm_rcp_ps (v);
	r *= ToolsSimd::set1_f32 (2) - v * r;
	return r;
#elif fstb_IS (ARCHI, ARM)
	float32x4_t    r = vrecpeq_f32 (v);
	r = vmulq_f32 (vrecpsq_f32 (v, r), r);
	r = vmulq_f32 (vrecpsq_f32 (v, r), r);
	return r;
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::sqrt (VectF32 v)
{
#if fstb_IS (ARCHI, X86)
	return _mm_sqrt_ps (v);
#elif fstb_IS (ARCHI, ARM)
	const uint32x4_t  nz_flag = vtstq_u32 (
		vreinterpretq_u32_f32 (v),
		vreinterpretq_u32_f32 (v)
	);
	float32x4_t       rs      = vrsqrteq_f32 (v);
	rs *= vrsqrtsq_f32 (v, rs * rs);
	rs *= vrsqrtsq_f32 (v, rs * rs);
	rs *= vrsqrtsq_f32 (v, rs * rs);
	const float32x4_t sqrt_a  = rs * v;
	return vreinterpretq_f32_u32 (vandq_u32 (
		vreinterpretq_u32_f32 (sqrt_a),
		nz_flag
	));
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::sqrt_approx (VectF32 v)
{
#if fstb_IS (ARCHI, X86)
	const __m128   nz_flag = _mm_cmpgt_ps (v, _mm_setzero_ps ());
	const __m128   sqrt_a  = _mm_mul_ps (v, _mm_rsqrt_ps (v));
	return _mm_and_ps (sqrt_a, nz_flag);
#elif fstb_IS (ARCHI, ARM)
	const uint32x4_t  nz_flag = vtstq_u32 (
		vreinterpretq_u32_f32 (v),
		vreinterpretq_u32_f32 (v)
	);
	float32x4_t       rs      = vrsqrteq_f32 (v);
	rs *= vrsqrtsq_f32 (rs * v, rs);
	const float32x4_t sqrt_a  = rs * v;
	return vreinterpretq_f32_u32 (vandq_u32 (
		vreinterpretq_u32_f32 (sqrt_a),
		nz_flag
	));
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::log2_approx (VectF32 v)
{
#if fstb_IS (ARCHI, X86)

	// Extracts the exponent (actually log2_int = exponent - 1)
	__m128i        xi = _mm_castps_si128 (v);
	xi = _mm_srli_epi32 (xi, 23);
	const __m128i  l2_sub = _mm_set1_epi32 (_log2_sub);
	xi = _mm_sub_epi32 (xi, l2_sub);
	const auto     log2_int = _mm_cvtepi32_ps (xi);

#elif fstb_IS (ARCHI, ARM)

	int32x4_t      xi = vreinterpretq_s32_f32 (v);
	xi = vshrq_n_s32 (xi, 23);
	const int32x4_t   l2_sub = vdupq_n_s32 (_log2_sub);
	xi -= l2_sub;
	const auto     log2_int = vcvtq_f32_s32 (xi);

#endif // ff_arch_CPU

	// Extracts the multiplicative part in [1 ; 2[
	const auto     mask_mantissa = set1_f32 (1.1754942e-38f);  // Binary: (1 << 23) - 1
	auto           part          = and_f32 (v, mask_mantissa);
	const auto     bias          = set1_f32 (1.0f);            // Binary: 127 << 23
	part = or_f32 (part, bias);

	// Computes the polynomial [1 ; 2[ -> [1 ; 2[
	// y = -1/3*x^2 + 2*x - 2/3
	// Ensures the C1 continuity over the whole range.
	// Its exact inverse is:
	// x = 3 - sqrt (7 - 3*y)
	const auto     a = set1_f32 (_log2_coef_a);
	auto           poly = a * part;
	const auto     b = set1_f32 (_log2_coef_b);
	poly += b;
	poly *= part;
	const auto     c = set1_f32 (_log2_coef_c);
	poly += c;

	// Sums the components
	const auto     total = log2_int + poly;

	return total;
}



ToolsSimd::VectF32	ToolsSimd::exp2_approx (VectF32 v)
{

	// Separates integer and fractional parts
#if fstb_IS (ARCHI, X86)
	const auto     round_toward_m_i = set1_f32 (-0.5f);
	auto           x    = v + v + round_toward_m_i;
	auto           xi   = _mm_cvtps_epi32 (x);
	xi = _mm_srai_epi32 (xi, 1);	// We'll use it later
	const auto     val_floor = _mm_cvtepi32_ps (xi);
#elif fstb_IS (ARCHI, ARM)
	const int      round_ofs = 256;
	int32x4_t      xi = vcvtq_s32_f32 (v + set1_f32 (round_ofs));
	xi -= vdupq_n_s32 (round_ofs);
	const auto     val_floor = vcvtq_f32_s32 (xi);
#endif // ff_arch_CPU

	const auto     frac = v - val_floor;

	// Computes the polynomial [0 ; 1[ -> [1 ; 2[
	// y = 1/3*x^2 + 2/3*x + 1
	// Ensures the C1 continuity over the whole range.
	// Its exact inverse is:
	// x = sqrt (3*y - 2) - 1
	const auto     a    = set1_f32 (_exp2_coef_a);
	auto           poly = a * frac;
	const auto     b    = set1_f32 (_exp2_coef_b);
	poly += b;
	poly *= frac;
	const auto     c    = set1_f32 (_exp2_coef_c);
	poly += c;

	// Integer part
#if fstb_IS (ARCHI, X86)
	const __m128i	e2_add = _mm_set1_epi32 (_exp2_add);
	xi  = _mm_add_epi32 (xi, e2_add);
	xi  = _mm_slli_epi32 (xi, 23);
	const auto     int_part = _mm_castsi128_ps (xi);
#elif fstb_IS (ARCHI, ARM)
	xi += vdupq_n_s32 (_exp2_add);
	xi  = vshlq_n_s32 (xi, 23);
	const auto     int_part = vreinterpretq_f32_s32 (xi);
#endif // ff_arch_CPU

	const auto     total = int_part * poly;

	return total;

}



float	ToolsSimd::sum_h_flt (VectF32 v)
{
#if fstb_IS (ARCHI, X86)
	v = _mm_add_ps (v, _mm_shuffle_ps (v, v, (3 << 2) | 2));
	return _mm_cvtss_f32 (_mm_add_ss (v, _mm_shuffle_ps (v, v, 1)));
#elif fstb_IS (ARCHI, ARM)
	float32x2_t    v2 = vadd_f32 (vget_high_f32 (v), vget_low_f32 (v));
	return vget_lane_f32 (vpadd_f32 (v2, v2), 0);
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::select (VectF32 cond, VectF32 v_t, VectF32 v_f)
{
#if fstb_IS (ARCHI, X86)
	const auto     cond_1 = _mm_and_ps (cond, v_t);
	const auto     cond_0 = _mm_andnot_ps (cond, v_f);
	return _mm_or_ps (cond_0, cond_1);
#elif fstb_IS (ARCHI, ARM)
	return vbslq_f32 (vreinterpretq_u32_f32 (cond), v_t, v_f);
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::cmp_gt_f32 (VectF32 lhs, VectF32 rhs)
{
#if fstb_IS (ARCHI, X86)
	return _mm_cmpgt_ps (lhs, rhs);
#elif fstb_IS (ARCHI, ARM)
	return vreinterpretq_f32_u32 (vcgtq_f32 (lhs, rhs));
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::cmp_lt_f32 (VectF32 lhs, VectF32 rhs)
{
#if fstb_IS (ARCHI, X86)
	return _mm_cmplt_ps (lhs, rhs);
#elif fstb_IS (ARCHI, ARM)
	return vreinterpretq_f32_u32 (vcltq_f32 (lhs, rhs));
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::and_f32 (VectF32 lhs, VectF32 rhs)
{
#if fstb_IS (ARCHI, X86)
	return _mm_and_ps (lhs, rhs);
#elif fstb_IS (ARCHI, ARM)
	return vreinterpretq_f32_u32 (vandq_u32 (
		vreinterpretq_u32_f32 (lhs),
		vreinterpretq_u32_f32 (rhs)
	));
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::or_f32 (VectF32 lhs, VectF32 rhs)
{
#if fstb_IS (ARCHI, X86)
	return _mm_or_ps (lhs, rhs);
#elif fstb_IS (ARCHI, ARM)
	return vreinterpretq_f32_u32 (vorrq_u32 (
		vreinterpretq_u32_f32 (lhs),
		vreinterpretq_u32_f32 (rhs)
	));
#endif // ff_arch_CPU
}



// p1[1 0] p0[1 0]
ToolsSimd::VectF32	ToolsSimd::interleave_2f32_low (VectF32 p0, VectF32 p1)
{
#if fstb_IS (ARCHI, X86)
	return _mm_shuffle_ps (p0, p1, (1<<6) + (0<<4) + (1<<2) + (0<<0));
#elif fstb_IS (ARCHI, ARM)
	const float32x2_t  p0x = vget_low_f32 (p0);
	const float32x2_t  p1x = vget_low_f32 (p1);
	return vcombine_f32 (p0x, p1x);
#endif // ff_arch_CPU
}



// p1[3 2] p0[3 2]
ToolsSimd::VectF32	ToolsSimd::interleave_2f32_high (VectF32 p0, VectF32 p1)
{
#if fstb_IS (ARCHI, X86)
	return _mm_shuffle_ps (p0, p1, (3<<6) + (2<<4) + (3<<2) + (2<<0));
#elif fstb_IS (ARCHI, ARM)
	const float32x2_t  p0x = vget_high_f32 (p0);
	const float32x2_t  p1x = vget_high_f32 (p1);
	return vcombine_f32 (p0x, p1x);
#endif // ff_arch_CPU
}



void	ToolsSimd::interleave_f32 (VectF32 &i0, VectF32 &i1, VectF32 p0, VectF32 p1)
{
#if fstb_IS (ARCHI, X86)
	i0 = _mm_unpacklo_ps (p0, p1);
	i1 = _mm_unpackhi_ps (p0, p1);
#elif fstb_IS (ARCHI, ARM)
	const float32x4x2_t  tmp = vzipq_f32 (p0, p1);
	i0 = tmp.val [0];
	i1 = tmp.val [1];
#endif // ff_arch_CPU
}



void	ToolsSimd::deinterleave_f32 (VectF32 &p0, VectF32 &p1, VectF32 i0, VectF32 i1)
{
#if fstb_IS (ARCHI, X86)
	p0 = _mm_shuffle_ps (i0, i1, 0x88);
	p1 = _mm_shuffle_ps (i0, i1, 0xDD);
#elif fstb_IS (ARCHI, ARM)
	const float32x4x2_t  tmp = vuzpq_f32 (i0, i1);
	p0 = tmp.val [0];
	p1 = tmp.val [1];
#endif // ff_arch_CPU
}



void	ToolsSimd::transpose_f32 (VectF32 &a0, VectF32 &a1, VectF32 &a2, VectF32 &a3)
{
	VectF32        k0, k1, k2, k3;
	interleave_f32 (k0, k1, a0, a2);
	interleave_f32 (k2, k3, a1, a3);
	interleave_f32 (a0, a1, k0, k2);
	interleave_f32 (a2, a3, k1, k3);
}



ToolsSimd::VectS32	ToolsSimd::conv_f32_to_s32 (VectF32 x)
{
#if fstb_IS (ARCHI, X86)
	return _mm_cvtps_epi32 (x);
#elif fstb_IS (ARCHI, ARM)
	return vcvtq_s32_f32 (x);
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::conv_s32_to_f32 (VectS32 x)
{
#if fstb_IS (ARCHI, X86)
	return _mm_cvtepi32_ps (x);
#elif fstb_IS (ARCHI, ARM)
	return vcvtq_f32_s32 (x);
#endif // ff_arch_CPU
}



// Positive = left
template <int SHIFT>
ToolsSimd::VectF32	ToolsSimd::Shift <SHIFT>::rotate (VectF32 a)
{
#if fstb_IS (ARCHI, X86)
	switch (SHIFT & 3)
	{
	case 1:  return _mm_shuffle_ps (a, a, (2<<6) | (1<<4) | (0<<2) | (3<<0));
	case 2:  return _mm_shuffle_ps (a, a, (1<<6) | (0<<4) | (3<<2) | (2<<0));
	case 3:  return _mm_shuffle_ps (a, a, (0<<6) | (3<<4) | (2<<2) | (1<<0));
	default: return a;
	}
#elif fstb_IS (ARCHI, ARM)
	int32x4_t     aa = vreinterpretq_s32_f32 (a);
	switch (SHIFT & 3)
	{
	case 1:  aa = vextq_s32 (aa, aa, 3); break;
	case 2:  aa = vextq_s32 (aa, aa, 2); break;
	case 3:  aa = vextq_s32 (aa, aa, 1); break;
	default: aa = aa;
	}
	return vreinterpretq_f32_s32 (aa);
#endif // ff_arch_CPU
}



template <int SHIFT>
float	ToolsSimd::Shift <SHIFT>::extract (VectF32 a)
{
#if fstb_IS (ARCHI, X86)
	switch (SHIFT & 3)
	{
	case 1:  a = _mm_shuffle_ps (a, a, 1);	break;
	case 2:  a = _mm_shuffle_ps (a, a, 2);	break;
	case 3:  a = _mm_shuffle_ps (a, a, 3);	break;
	default: a = a;
	}
	return _mm_cvtss_f32 (a);
#elif fstb_IS (ARCHI, ARM)
	return vgetq_lane_f32 (a, SHIFT & 3);
#endif // ff_arch_CPU
}



template <int SHIFT>
ToolsSimd::VectF32	ToolsSimd::Shift <SHIFT>::insert (VectF32 a, float val)
{
#if fstb_IS (ARCHI, X86)
	a = Shift <(-SHIFT) & 3>::rotate (a);
	a = _mm_move_ss (a, _mm_set_ss (val));
	a = Shift <  SHIFT     >::rotate (a);
	return a;
#elif fstb_IS (ARCHI, ARM)
	return vsetq_lane_f32 (val, a, SHIFT & 3);
#endif // ff_arch_CPU
}



template <int SHIFT>
ToolsSimd::VectF32	ToolsSimd::Shift <SHIFT>::spread (VectF32 a)
{
#if fstb_IS (ARCHI, X86)
	return _mm_shuffle_ps (a, a, 0x55 * SHIFT);
#elif fstb_IS (ARCHI, ARM)
	return vdupq_n_f32 (vgetq_lane_f32 (a, SHIFT));
#endif // ff_arch_CPU
}



// Positive = left
template <int SHIFT>
ToolsSimd::VectS32	ToolsSimd::Shift <SHIFT>::rotate (VectS32 a)
{
#if fstb_IS (ARCHI, X86)
	switch (SHIFT & 3)
	{
	case 1:  return _mm_shuffle_epi32 (a, (2<<6) | (1<<4) | (0<<2) | (3<<0));
	case 2:  return _mm_shuffle_epi32 (a, (1<<6) | (0<<4) | (3<<2) | (2<<0));
	case 3:  return _mm_shuffle_epi32 (a, (0<<6) | (3<<4) | (2<<2) | (1<<0));
	default: return a;
	}
#elif fstb_IS (ARCHI, ARM)
	switch (SHIFT & 3)
	{
	case 1:  a = vextq_s32 (a, a, 3); break;
	case 2:  a = vextq_s32 (a, a, 2); break;
	case 3:  a = vextq_s32 (a, a, 1); break;
	default: a = a;
	}
	return a;
#endif // ff_arch_CPU
}



template <int SHIFT>
int32_t	ToolsSimd::Shift <SHIFT>::extract (VectS32 a)
{
#if fstb_IS (ARCHI, X86)
	switch (SHIFT & 3)
	{
	case 1:  a = _mm_shuffle_epi32 (a, 1);	break;
	case 2:  a = _mm_shuffle_epi32 (a, 2);	break;
	case 3:  a = _mm_shuffle_epi32 (a, 3);	break;
	default: a = a;
	}
	return _mm_cvtsi128_si32 (a);
#elif fstb_IS (ARCHI, ARM)
	return vgetq_lane_s32 (a, SHIFT & 3);
#endif // ff_arch_CPU
}



template <int SHIFT>
ToolsSimd::VectS32	ToolsSimd::Shift <SHIFT>::insert (VectS32 a, int32_t val)
{
#if fstb_IS (ARCHI, X86)
	a = Shift <(-SHIFT) & 3>::rotate (a);
	a = _mm_castps_si128 (_mm_move_ss (
		_mm_castsi128_ps (a),
		_mm_castsi128_ps (_mm_set_ss (val))
	));
	a = Shift <  SHIFT     >::rotate (a);
	return a;
#elif fstb_IS (ARCHI, ARM)
	return vsetq_lane_s32 (val, a, SHIFT & 3);
#endif // ff_arch_CPU
}



template <int SHIFT>
ToolsSimd::VectS32	ToolsSimd::Shift <SHIFT>::spread (VectS32 a)
{
#if fstb_IS (ARCHI, X86)
	return _mm_shuffle_epi32 (a, 0x55 * SHIFT);
#elif fstb_IS (ARCHI, ARM)
	return vdupq_n_s32 (vgetq_lane_s32 (a, SHIFT));
#endif // ff_arch_CPU
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	ToolsSimd::store_f32_part_n13 (void *ptr, VectF32 v, int n)
{
	assert (n > 0);
	assert (n < 4);

	float *        f_ptr = reinterpret_cast <float *> (ptr);

#if fstb_IS (ARCHI, ARM)

	f_ptr [0] = vgetq_lane_f32 (v, 0);
	if (n >= 2)
	{
		f_ptr [1] = vgetq_lane_f32 (v, 1);
		if (n >= 3)
		{
			f_ptr [2] = vgetq_lane_f32 (v, 2);
		}
	}

#else

	for (int i = 0; i < n; ++i)
	{
		f_ptr [i] = Shift < 0>::extract (v);
		v         = Shift <-1>::rotate (v);
	}

#endif
}



void	ToolsSimd::store_s32_part_n13 (void *ptr, VectS32 v, int n)
{
	assert (n > 0);
	assert (n < 4);

	int32_t *      f_ptr = reinterpret_cast <int32_t *> (ptr);

#if fstb_IS (ARCHI, ARM)

	f_ptr [0] = vgetq_lane_s32 (v, 0);
	if (n >= 2)
	{
		f_ptr [1] = vgetq_lane_s32 (v, 1);
		if (n >= 3)
		{
			f_ptr [2] = vgetq_lane_s32 (v, 2);
		}
	}

#else

	for (int i = 0; i < n; ++i)
	{
		f_ptr [i] = Shift < 0>::extract (v);
		v         = Shift <-1>::rotate (v);
	}

#endif
}



ToolsSimd::VectF32	ToolsSimd::load_f32_part_n13 (const void *ptr, int n)
{
	assert (n > 0);
	assert (n < 4);

	const float *  f_ptr = reinterpret_cast <const float *> (ptr);
	VectF32        v;
#if fstb_IS (ARCHI, X86)
	switch (n)
	{
	case 1:
		v = _mm_load_ss (f_ptr);
		break;
	case 2:
		v = _mm_load_ss (f_ptr + 1);
		v = Shift <1>::rotate (v);
		v = Shift <0>::insert (v, f_ptr [0]);
		break;
	case 3:
		v = _mm_load_ss (f_ptr + 2);
		v = Shift <1>::rotate (v);
		v = Shift <0>::insert (v, f_ptr [1]);
		v = Shift <1>::rotate (v);
		v = Shift <0>::insert (v, f_ptr [0]);
		break;
	}
	for (int k = 0; k < n; ++k)
	{
	}
#elif fstb_IS (ARCHI, ARM)
	v = vmovq_n_f32 (f_ptr [0]);
	if (n >= 2)
	{
		v = vsetq_lane_f32 (f_ptr [1], v, 1);
		if (n >= 3)
		{
			v = vsetq_lane_f32 (f_ptr [2], v, 2);
		}
	}
#endif // ff_arch_CPU

	return v;
}



}  // namespace fstb



#if fstb_IS (COMPILER, MSVC)



fstb::ToolsSimd::VectF32 &	operator += (fstb::ToolsSimd::VectF32 &lhs, fstb::ToolsSimd::VectF32 rhs)
{
#if fstb_IS (ARCHI, X86)
	lhs = _mm_add_ps (lhs, rhs);
#elif fstb_IS (ARCHI, ARM)
	lhs = vaddq_f32 (lhs, rhs);
#endif // ff_arch_CPU
	return lhs;
}



fstb::ToolsSimd::VectF32 &	operator -= (fstb::ToolsSimd::VectF32 &lhs, fstb::ToolsSimd::VectF32 rhs)
{
#if fstb_IS (ARCHI, X86)
	lhs = _mm_sub_ps (lhs, rhs);
#elif fstb_IS (ARCHI, ARM)
	lhs = vsubq_f32 (lhs, rhs);
#endif // ff_arch_CPU
	return lhs;
}



fstb::ToolsSimd::VectF32 &	operator *= (fstb::ToolsSimd::VectF32 &lhs, fstb::ToolsSimd::VectF32 rhs)
{
#if fstb_IS (ARCHI, X86)
	lhs = _mm_mul_ps (lhs, rhs);
#elif fstb_IS (ARCHI, ARM)
	lhs = vmulq_f32 (lhs, rhs);
#endif // ff_arch_CPU
	return lhs;
}



fstb::ToolsSimd::VectF32 operator + (fstb::ToolsSimd::VectF32 lhs, fstb::ToolsSimd::VectF32 rhs)
{
#if fstb_IS (ARCHI, X86)
	return _mm_add_ps (lhs, rhs);
#elif fstb_IS (ARCHI, ARM)
	return vaddq_f32 (lhs, rhs);
#endif // ff_arch_CPU
}



fstb::ToolsSimd::VectF32 operator - (fstb::ToolsSimd::VectF32 lhs, fstb::ToolsSimd::VectF32 rhs)
{
#if fstb_IS (ARCHI, X86)
	return _mm_sub_ps (lhs, rhs);
#elif fstb_IS (ARCHI, ARM)
	return vsubq_f32 (lhs, rhs);
#endif // ff_arch_CPU
}



fstb::ToolsSimd::VectF32 operator * (fstb::ToolsSimd::VectF32 lhs, fstb::ToolsSimd::VectF32 rhs)
{
#if fstb_IS (ARCHI, X86)
	return _mm_mul_ps (lhs, rhs);
#elif fstb_IS (ARCHI, ARM)
	return vmulq_f32 (lhs, rhs);
#endif // ff_arch_CPU
}



#endif // MSVC



#endif   // fstb_ToolsSimd_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
