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

#include <algorithm>

#include <cmath>



namespace fstb
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename MEM>
ToolsSimd::VectF32	ToolsSimd::load_f32 (const MEM *ptr)
{
	assert (is_ptr_align_nz (ptr, 16));

#if ! defined (fstb_HAS_SIMD)
	return *reinterpret_cast <const VectF32 *> (ptr);
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_load_ps (reinterpret_cast <const float *> (ptr));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vld1q_f32 (reinterpret_cast <const float32_t *> (ptr));
#endif // ff_arch_CPU
}



template <typename MEM>
ToolsSimd::VectS32	ToolsSimd::load_s32 (const MEM *ptr)
{
	assert (is_ptr_align_nz (ptr, 16));

#if ! defined (fstb_HAS_SIMD)
	return *reinterpret_cast <const VectS32 *> (ptr);
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_load_si128 (reinterpret_cast <const __m128i *> (ptr));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vld1q_s32 (reinterpret_cast <const int32_t *> (ptr));
#endif // ff_arch_CPU
}



template <typename MEM>
void	ToolsSimd::store_f32 (MEM *ptr, VectF32 v)
{
	assert (is_ptr_align_nz (ptr, 16));

#if ! defined (fstb_HAS_SIMD)
	*reinterpret_cast <VectF32 *> (ptr) = v;
#elif fstb_ARCHI == fstb_ARCHI_X86
	_mm_store_ps (reinterpret_cast <float *> (ptr), v);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	vst1q_f32 (reinterpret_cast <float32_t *> (ptr), v);
#endif // ff_arch_CPU
}



template <typename MEM>
void	ToolsSimd::store_s32 (MEM *ptr, VectS32 v)
{
	assert (is_ptr_align_nz (ptr, 16));

#if ! defined (fstb_HAS_SIMD)
	*reinterpret_cast <VectS32 *> (ptr) = v;
#elif fstb_ARCHI == fstb_ARCHI_X86
	_mm_store_si128 (reinterpret_cast <__m128i *> (ptr), v);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	vst1q_s32 (reinterpret_cast <int32_t *> (ptr), v);
#endif // ff_arch_CPU
}



// n = number of scalars to store (from the LSB)
template <typename MEM>
void	ToolsSimd::store_f32_part (MEM *ptr, VectF32 v, int n)
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



template <typename MEM>
ToolsSimd::VectF32	ToolsSimd::loadu_f32 (const MEM *ptr)
{
	assert (ptr != nullptr);

#if ! defined (fstb_HAS_SIMD)
	return *reinterpret_cast <const VectF32 *> (ptr);
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_loadu_ps (reinterpret_cast <const float *> (ptr));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_f32_u8 (
		vld1q_u8 (reinterpret_cast <const uint8_t *> (ptr))
	);
#endif // ff_arch_CPU
}



template <typename MEM>
ToolsSimd::VectS32	ToolsSimd::loadu_s32 (const MEM *ptr)
{
	assert (ptr != nullptr);

#if ! defined (fstb_HAS_SIMD)
	return *reinterpret_cast <const VectS32 *> (ptr);
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_loadu_si128 (reinterpret_cast <const __m128i *> (ptr));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_s32_u8 (
		vld1q_u8 (reinterpret_cast <const uint8_t *> (ptr))
	);
#endif // ff_arch_CPU
}



template <typename MEM>
ToolsSimd::VectF32	ToolsSimd::loadu_f32_part (const MEM *ptr, int n)
{
	assert (n > 0);

	if (n >= 4)
	{
		return loadu_f32 (ptr);
	}

	return load_f32_part_n13 (ptr, n);
}



template <typename MEM>
void	ToolsSimd::storeu_f32 (MEM *ptr, VectF32 v)
{
	assert (ptr != nullptr);

#if ! defined (fstb_HAS_SIMD)
	*reinterpret_cast <VectF32 *> (ptr) = v;
#elif fstb_ARCHI == fstb_ARCHI_X86
	_mm_storeu_ps (reinterpret_cast <float *> (ptr), v);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	vst1q_u8 (reinterpret_cast <uint8_t *> (ptr), vreinterpretq_u8_f32 (v));
#endif // ff_arch_CPU
}



template <typename MEM>
void	ToolsSimd::storeu_s32 (MEM *ptr, VectS32 v)
{
	assert (ptr != nullptr);

#if ! defined (fstb_HAS_SIMD)
	*reinterpret_cast <VectS32 *> (ptr) = v;
#elif fstb_ARCHI == fstb_ARCHI_X86
	_mm_storeu_si128 (reinterpret_cast <__m128i *> (ptr), v);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	vst1q_u8 (reinterpret_cast <uint8_t *> (ptr), vreinterpretq_u8_s32 (v));
#endif // ff_arch_CPU
}



// n = number of scalars to store (from the LSB)
template <typename MEM>
void	ToolsSimd::storeu_f32_part (MEM *ptr, VectF32 v, int n)
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
template <typename MEM>
void	ToolsSimd::storeu_s32_part (MEM *ptr, VectS32 v, int n)
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
template <typename MEM>
ToolsSimd::VectF32	ToolsSimd::loadu_2f32 (const MEM *ptr)
{
	assert (ptr != nullptr);

#if ! defined (fstb_HAS_SIMD)
	auto           p = reinterpret_cast <const float *> (ptr);
	return VectF32 { { p [0], p [1] } };
#elif fstb_ARCHI == fstb_ARCHI_X86
	const auto     x_0 = _mm_load_ss (reinterpret_cast <const float *> (ptr)    );
	const auto     x_1 = _mm_load_ss (reinterpret_cast <const float *> (ptr) + 1);
	const auto     x   = _mm_unpacklo_ps (x_0, x_1);
	return x;
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const float32x2_t x = vreinterpret_f32_u8 (
		vld1_u8 (reinterpret_cast <const uint8_t *> (ptr))
	);
	return vcombine_f32 (x, x);
#endif // ff_arch_CPU
}



// ptr [0] = v0
// ptr [1] = v1
template <typename MEM>
void	ToolsSimd::storeu_2f32 (MEM *ptr, VectF32 v)
{
	assert (ptr != nullptr);

#if ! defined (fstb_HAS_SIMD)
	auto           p = reinterpret_cast <float *> (ptr);
	p [0] = v._ [0];
	p [1] = v._ [1];
#elif fstb_ARCHI == fstb_ARCHI_X86
	_mm_store_ss (reinterpret_cast <float *> (ptr)    , v );
	const auto     v1 = _mm_shuffle_ps (v, v, 1 << 0);
	_mm_store_ss (reinterpret_cast <float *> (ptr) + 1, v1);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	vst1_u8 (
		reinterpret_cast <uint8_t *> (ptr),
		vreinterpret_u8_f32 (vget_low_f32 (v))
	);
#endif // ff_arch_CPU
}



// *ptr = v0
template <typename MEM>
void	ToolsSimd::storeu_1f32 (MEM *ptr, VectF32 v)
{
	assert (ptr != nullptr);

#if ! defined (fstb_HAS_SIMD)
	reinterpret_cast <float *> (ptr) [0] = v._ [0];
#elif fstb_ARCHI == fstb_ARCHI_X86
	_mm_store_ss (reinterpret_cast <float *> (ptr), v);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	vst1q_lane_f32 (reinterpret_cast <float32_t *> (ptr), v, 0);
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::set_f32_zero ()
{
#if ! defined (fstb_HAS_SIMD)
	return VectF32 { { 0, 0, 0, 0 } };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_setzero_ps ();
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vdupq_n_f32 (0);
#endif // ff_arch_CPU
}



ToolsSimd::VectS32	ToolsSimd::set_s32_zero ()
{
#if ! defined (fstb_HAS_SIMD)
	return VectS32 { { 0, 0, 0, 0 } };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_setzero_si128 ();
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vdupq_n_s32 (0);
#endif // ff_arch_CPU
}



// Returns a0 | a0 | a0 | a0
ToolsSimd::VectF32	ToolsSimd::set1_f32 (float a)
{
#if ! defined (fstb_HAS_SIMD)
	return VectF32 { { a, a, a, a } };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_set1_ps (a);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vdupq_n_f32 (a);
#endif // ff_arch_CPU
}



// Returns a0 | a0 | a0 | a0
ToolsSimd::VectS32	ToolsSimd::set1_s32 (int32_t a)
{
#if ! defined (fstb_HAS_SIMD)
	return VectS32 { { a, a, a, a } };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_set1_epi32 (a);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vdupq_n_s32 (a);
#endif // ff_arch_CPU
}



// Returns a0 | a1 | a2 | a3
ToolsSimd::VectF32	ToolsSimd::set_f32 (float a0, float a1, float a2, float a3)
{
#if ! defined (fstb_HAS_SIMD)
	return VectF32 { { a0, a1, a2, a3 } };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_set_ps (a3, a2, a1, a0);
#elif fstb_ARCHI == fstb_ARCHI_ARM
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



// Returns a0 | a1 | a2 | a3
ToolsSimd::VectS32	ToolsSimd::set_s32 (int32_t a0, int32_t a1, int32_t a2, int32_t a3)
{
#if ! defined (fstb_HAS_SIMD)
	return VectS32 { { a0, a1, a2, a3 } };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_set_epi32 (a3, a2, a1, a0);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	int32x2_t      v01 = vdup_n_s32 (a0);
	int32x2_t      v23 = vdup_n_s32 (a2);
	v01 = vset_lane_s32 (a1, v01, 1);
	v23 = vset_lane_s32 (a3, v23, 1);
	return vcombine_s32 (v01, v23);
#endif // ff_arch_CPU
}



// Returns a0 | a1 | ? | ?
ToolsSimd::VectF32	ToolsSimd::set_2f32 (float a0, float a1)
{
#if ! defined (fstb_HAS_SIMD)
	return VectF32 { { a0, a1 } };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_unpacklo_ps (_mm_set_ss (a0), _mm_set_ss (a1));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vsetq_lane_f32 (a1, vdupq_n_f32 (a0), 1);
#endif // ff_arch_CPU
}



// Returns a02 | a13 | a02 | a13
ToolsSimd::VectF32	ToolsSimd::set_2f32_fill (float a02, float a13)
{
#if ! defined (fstb_HAS_SIMD)
	return VectF32 { { a02, a13, a02, a13 } };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_unpacklo_ps (_mm_set1_ps (a02), _mm_set1_ps (a13));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const float32x2_t v01 = vset_lane_f32 (a13, vdup_n_f32 (a02), 1);
	return vcombine_f32 (v01, v01);
#endif // ff_arch_CPU
}



// Returns a01 | a01 | a23 | a23
ToolsSimd::VectF32	ToolsSimd::set_2f32_dbl (float a01, float a23)
{
#if ! defined (fstb_HAS_SIMD)
	return VectF32 { { a01, a01, a23, a23 } };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_shuffle_ps (_mm_set_ss (a01), _mm_set_ss (a23), 0x00);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vcombine_f32 (vdup_n_f32 (a01), vdup_n_f32 (a23));
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::swap_2f32 (VectF32 v)
{
#if ! defined (fstb_HAS_SIMD)
	return VectF32 { { v._ [2], v._ [3], v._ [0], v._ [1] } };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_shuffle_ps (v, v, (2<<0) + (3<<2) + (0<<4) + (1<<6));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const float32x2_t v01 = vget_low_f32 (v);
	const float32x2_t v23 = vget_high_f32 (v);
	return vcombine_f32 (v23, v01);
#endif // ff_arch_CPU
}



void	ToolsSimd::extract_2f32 (float &a0, float &a1, VectF32 v)
{
#if ! defined (fstb_HAS_SIMD)
	a0 = v._ [0];
	a1 = v._ [1];
#elif fstb_ARCHI == fstb_ARCHI_X86
	a0 = _mm_cvtss_f32 (v);
	a1 = _mm_cvtss_f32 (_mm_shuffle_ps (v, v, 1));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	a0 = vgetq_lane_f32 (v, 0);
	a1 = vgetq_lane_f32 (v, 1);
#endif // ff_arch_CPU
}



// ra = v0 | v1 | v0 | v1
// rb = v2 | v3 | v2 | v3
void	ToolsSimd::spread_2f32 (VectF32 &ra, VectF32 &rb, VectF32 v)
{
#if ! defined (fstb_HAS_SIMD)
	ra._ [0] = v._ [0];
	ra._ [1] = v._ [1];
	ra._ [2] = v._ [0];
	ra._ [3] = v._ [1];
	rb._ [0] = v._ [2];
	rb._ [1] = v._ [3];
	rb._ [2] = v._ [2];
	rb._ [3] = v._ [3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	ra = _mm_shuffle_ps (v, v, (0<<0) + (1<<2) + (0<<4) + (1<<6));
	rb = _mm_shuffle_ps (v, v, (2<<0) + (3<<2) + (2<<4) + (3<<6));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const float32x2_t v01 = vget_low_f32 (v);
	const float32x2_t v23 = vget_high_f32 (v);
	ra = vcombine_f32 (v01, v01);
	rb = vcombine_f32 (v23, v23);
#endif // ff_arch_CPU
}



void	ToolsSimd::mac (VectF32 &s, VectF32 a, VectF32 b)
{
#if ! defined (fstb_HAS_SIMD)
	s._ [0] += a._ [0] * b._ [0];
	s._ [1] += a._ [1] * b._ [1];
	s._ [2] += a._ [2] * b._ [2];
	s._ [3] += a._ [3] * b._ [3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	s = _mm_add_ps (s, _mm_mul_ps (a, b));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	s = vmlaq_f32 (s, a, b);
#endif // ff_arch_CPU
}



void	ToolsSimd::msu (VectF32 &s, VectF32 a, VectF32 b)
{
#if ! defined (fstb_HAS_SIMD)
	s._ [0] -= a._ [0] * b._ [0];
	s._ [1] -= a._ [1] * b._ [1];
	s._ [2] -= a._ [2] * b._ [2];
	s._ [3] -= a._ [3] * b._ [3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	s = _mm_sub_ps (s, _mm_mul_ps (a, b));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	s = vmlsq_f32 (s, a, b);
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::fmadd (VectF32 x, VectF32 a, VectF32 b)
{
#if ! defined (fstb_HAS_SIMD)
	return VectF32 { {
		x._ [0] * a._ [0] + b._ [0],
		x._ [1] * a._ [1] + b._ [1],
		x._ [2] * a._ [2] + b._ [2],
		x._ [3] * a._ [3] + b._ [3]
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_add_ps (_mm_mul_ps (x, a), b);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vmlaq_f32 (b, x, a);
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::fmsub (VectF32 x, VectF32 a, VectF32 b)
{
#if ! defined (fstb_HAS_SIMD)
	return VectF32 { {
		x._ [0] * a._ [0] - b._ [0],
		x._ [1] * a._ [1] - b._ [1],
		x._ [2] * a._ [2] - b._ [2],
		x._ [3] * a._ [3] - b._ [3]
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_sub_ps (_mm_mul_ps (x, a), b);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vmlsq_f32 (b, x, a);
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::min_f32 (VectF32 lhs, VectF32 rhs)
{
#if ! defined (fstb_HAS_SIMD)
	return VectF32 { {
		std::min (lhs._ [0], rhs._ [0]),
		std::min (lhs._ [1], rhs._ [1]),
		std::min (lhs._ [2], rhs._ [2]),
		std::min (lhs._ [3], rhs._ [3])
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_min_ps (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vminq_f32 (lhs, rhs);
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::max_f32 (VectF32 lhs, VectF32 rhs)
{
#if ! defined (fstb_HAS_SIMD)
	return VectF32 { {
		std::max (lhs._ [0], rhs._ [0]),
		std::max (lhs._ [1], rhs._ [1]),
		std::max (lhs._ [2], rhs._ [2]),
		std::max (lhs._ [3], rhs._ [3])
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_max_ps (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vmaxq_f32 (lhs, rhs);
#endif // ff_arch_CPU
}



float	ToolsSimd::sum_h_flt (VectF32 v)
{
#if ! defined (fstb_HAS_SIMD)
	return (v._ [0] + v._ [2]) + (v._ [1] + v._ [3]);
#elif fstb_ARCHI == fstb_ARCHI_X86
	v = _mm_add_ps (v, _mm_shuffle_ps (v, v, (3 << 2) | 2));
	return _mm_cvtss_f32 (_mm_add_ss (v, _mm_shuffle_ps (v, v, 1)));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	float32x2_t    v2 = vadd_f32 (vget_high_f32 (v), vget_low_f32 (v));
	return vget_lane_f32 (vpadd_f32 (v2, v2), 0);
#endif // ff_arch_CPU
}



float	ToolsSimd::min_h_flt (VectF32 v)
{
#if ! defined (fstb_HAS_SIMD)
	return std::min (std::min (v._ [0], v._ [2]), std::min (v._ [1], v._ [3]));
#elif fstb_ARCHI == fstb_ARCHI_X86
	v = _mm_min_ps (v, _mm_shuffle_ps (v, v, (3 << 2) | 2));
	return _mm_cvtss_f32 (_mm_min_ss (v, _mm_shuffle_ps (v, v, 1)));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	float32x2_t    v2 = vmin_f32 (vget_high_f32 (v), vget_low_f32 (v));
	return vget_lane_f32 (vpmin_f32 (v2, v2), 0);
#endif // ff_arch_CPU
}



float	ToolsSimd::max_h_flt (VectF32 v)
{
#if ! defined (fstb_HAS_SIMD)
	return std::max (std::max (v._ [0], v._ [2]), std::max (v._ [1], v._ [3]));
#elif fstb_ARCHI == fstb_ARCHI_X86
	v = _mm_max_ps (v, _mm_shuffle_ps (v, v, (3 << 2) | 2));
	return _mm_cvtss_f32 (_mm_max_ss (v, _mm_shuffle_ps (v, v, 1)));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	float32x2_t    v2 = vmax_f32 (vget_high_f32 (v), vget_low_f32 (v));
	return vget_lane_f32 (vpmax_f32 (v2, v2), 0);
#endif // ff_arch_CPU
}



// Assumes "to nearest" rounding mode on x86
ToolsSimd::VectF32	ToolsSimd::round (VectF32 v)
{
#if ! defined (fstb_HAS_SIMD)
	return VectF32 { {
		roundf (v._ [0]),
		roundf (v._ [1]),
		roundf (v._ [2]),
		roundf (v._ [3])
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_cvtepi32_ps (_mm_cvtps_epi32 (v));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const auto     zero = vdupq_n_f32 ( 0.0f);
	const auto     m    = vdupq_n_f32 (-0.5f);
	const auto     p    = vdupq_n_f32 (+0.5f);
	const auto     gt0  = vcgtq_f32 (v, zero);
	const auto     u    = vbslq_f32 (gt0, p, m);
	v = vaddq_f32 (v, u);
	return v;
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::abs (VectF32 v)
{
#if ! defined (fstb_HAS_SIMD)
	return VectF32 { {
		fabsf (v._ [0]),
		fabsf (v._ [1]),
		fabsf (v._ [2]),
		fabsf (v._ [3])
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_andnot_ps (signbit_mask_f32 (), v);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vabsq_f32 (v);
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::signbit (VectF32 v)
{
#if ! defined (fstb_HAS_SIMD)
	return VectF32 { {
		copysignf (0.f, v._ [0]),
		copysignf (0.f, v._ [1]),
		copysignf (0.f, v._ [2]),
		copysignf (0.f, v._ [3])
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_and_ps (signbit_mask_f32 (), v);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_f32_u32 (vandq_u32 (
		vreinterpretq_u32_f32 (v),
		vdupq_n_u32 (0x80000000U)
	));
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::signbit_mask_f32 ()
{
#if ! defined (fstb_HAS_SIMD)
	Combo          c;
	c._u32 [0] = 0x80000000U;
	c._u32 [1] = 0x80000000U;
	c._u32 [2] = 0x80000000U;
	c._u32 [3] = 0x80000000U;
	return c._vf32;
#elif fstb_ARCHI == fstb_ARCHI_X86
//	return _mm_set1_ps (-0.f);
	return _mm_castsi128_ps (_mm_set1_epi32 (0x80000000));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_f32_u32 (vdupq_n_u32 (0x80000000U)); 
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::rcp_approx (VectF32 v)
{
#if ! defined (fstb_HAS_SIMD)
	return VectF32 { {
		sqrtf (v._ [0]),
		sqrtf (v._ [1]),
		sqrtf (v._ [2]),
		sqrtf (v._ [3])
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_rcp_ps (v);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	float32x4_t    r = vrecpeq_f32 (v);
	r = vmulq_f32 (vrecpsq_f32 (v, r), r);
//	r = vmulq_f32 (vrecpsq_f32 (v, r), r); // Only one step needed?
	return r;
#endif // ff_arch_CPU
}



// With more accuracy
ToolsSimd::VectF32	ToolsSimd::rcp_approx2 (VectF32 v)
{
#if ! defined (fstb_HAS_SIMD)
	return VectF32 { {
		1.f / v._ [0],
		1.f / v._ [1],
		1.f / v._ [2],
		1.f / v._ [3]
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	__m128         r = _mm_rcp_ps (v);
	r *= ToolsSimd::set1_f32 (2) - v * r;
	return r;
#elif fstb_ARCHI == fstb_ARCHI_ARM
	float32x4_t    r = vrecpeq_f32 (v);
	r = vmulq_f32 (vrecpsq_f32 (v, r), r);
	r = vmulq_f32 (vrecpsq_f32 (v, r), r);
	return r;
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::div_approx (VectF32 n, VectF32 d)
{
#if ! defined (fstb_HAS_SIMD)
	return VectF32 { {
		n._ [0] / d._ [0],
		n._ [1] / d._ [1],
		n._ [2] / d._ [2],
		n._ [3] / d._ [3]
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_div_ps (n, d);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return n * rcp_approx (d);
#endif // ff_arch_CPU
}



// With more accuracy
ToolsSimd::VectF32	ToolsSimd::div_approx2 (VectF32 n, VectF32 d)
{
#if ! defined (fstb_HAS_SIMD)
	return VectF32 { {
		n._ [0] / d._ [0],
		n._ [1] / d._ [1],
		n._ [2] / d._ [2],
		n._ [3] / d._ [3]
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_div_ps (n, d);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return n * rcp_approx2 (d);
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::sqrt (VectF32 v)
{
#if ! defined (fstb_HAS_SIMD)
	return VectF32 { {
		sqrtf (v._ [0]),
		sqrtf (v._ [1]),
		sqrtf (v._ [2]),
		sqrtf (v._ [3])
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
	const float32x4_t sqrt_a  = rs * v;
	return vreinterpretq_f32_u32 (vandq_u32 (
		vreinterpretq_u32_f32 (sqrt_a),
		nz_flag
	));
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::sqrt_approx (VectF32 v)
{
#if ! defined (fstb_HAS_SIMD)
	return VectF32 { {
		sqrtf (v._ [0]),
		sqrtf (v._ [1]),
		sqrtf (v._ [2]),
		sqrtf (v._ [3])
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	const __m128   nz_flag = _mm_cmpgt_ps (v, _mm_setzero_ps ());
	const __m128   sqrt_a  = _mm_mul_ps (v, _mm_rsqrt_ps (v));
	return _mm_and_ps (sqrt_a, nz_flag);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const uint32x4_t  nz_flag = vtstq_u32 (
		vreinterpretq_u32_f32 (v),
		vreinterpretq_u32_f32 (v)
	);
	float32x4_t    rs      = vrsqrteq_f32 (v);
	rs *= vrsqrtsq_f32 (rs * v, rs);
	const float32x4_t sqrt_a  = rs * v;
	return vreinterpretq_f32_u32 (vandq_u32 (
		vreinterpretq_u32_f32 (sqrt_a),
		nz_flag
	));
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::rsqrt_approx (VectF32 v)
{
#if ! defined (fstb_HAS_SIMD)
	// Ref: Chris Lomont, Fast Inverse Square Root, 2003-02
	const float    xh0 = v._ [0] * 0.5f;
	const float    xh1 = v._ [1] * 0.5f;
	const float    xh2 = v._ [2] * 0.5f;
	const float    xh3 = v._ [3] * 0.5f;
	Combo          c;
	c._vf32    = v;
	c._s32 [0] = 0x5F375A86 - (c._s32 [0] >> 1);
	c._s32 [1] = 0x5F375A86 - (c._s32 [1] >> 1);
	c._s32 [2] = 0x5F375A86 - (c._s32 [2] >> 1);
	c._s32 [3] = 0x5F375A86 - (c._s32 [3] >> 1);
	float          x0 = c._f32 [0];
	float          x1 = c._f32 [1];
	float          x2 = c._f32 [2];
	float          x3 = c._f32 [3];
	x0 *= 1.5f - xh0 * x0 * x0;
	x1 *= 1.5f - xh1 * x1 * x1;
	x2 *= 1.5f - xh2 * x2 * x2;
	x3 *= 1.5f - xh3 * x3 * x3;
	return VectF32 { { x0, x1, x2, x3 } };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_rsqrt_ps (v);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	float32x4_t    rs = vrsqrteq_f32 (v);
	rs *= vrsqrtsq_f32 (rs * v, rs);
	return rs;
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::rsqrt_approx2 (VectF32 v)
{
#if ! defined (fstb_HAS_SIMD)
	return VectF32 { {
		1.f / sqrtf (v._ [0]),
		1.f / sqrtf (v._ [1]),
		1.f / sqrtf (v._ [2]),
		1.f / sqrtf (v._ [3])
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	__m128         rs = _mm_rsqrt_ps (v);
	rs = _mm_set1_ps (0.5f) * rs * (_mm_set1_ps (3) - v * rs * rs);
	return rs;
#elif fstb_ARCHI == fstb_ARCHI_ARM
	float32x4_t    rs = vrsqrteq_f32 (v);
	rs *= vrsqrtsq_f32 (rs * v, rs);
	rs *= vrsqrtsq_f32 (rs * v, rs);
	return rs;
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::log2_approx (VectF32 v)
{
	const int32_t  log2_sub = 128;
	const float		coef_a   = -1.f / 3;
	const float		coef_b   =  2.f;
	const float		coef_c   = -2.f / 3;

#if ! defined (fstb_HAS_SIMD)

	assert (
	      v._ [0] > 0
		&& v._ [1] > 0
		&& v._ [2] > 0
		&& v._ [3] > 0
	);
	Combo          c;
	c._vf32 = v;
	int            x0 = c._s32 [0];
	int            x1 = c._s32 [1];
	int            x2 = c._s32 [2];
	int            x3 = c._s32 [3];
	const int32_t  log_2_0 = ((x0 >> 23) & 255) - log2_sub;
	const int32_t  log_2_1 = ((x1 >> 23) & 255) - log2_sub;
	const int32_t  log_2_2 = ((x2 >> 23) & 255) - log2_sub;
	const int32_t  log_2_3 = ((x3 >> 23) & 255) - log2_sub;
	x0       &= ~(255 << 23);
	x1       &= ~(255 << 23);
	x2       &= ~(255 << 23);
	x3       &= ~(255 << 23);
	x0       +=   127 << 23;
	x1       +=   127 << 23;
	x2       +=   127 << 23;
	x3       +=   127 << 23;
	c._s32 [0] = x0;
	c._s32 [1] = x1;
	c._s32 [2] = x2;
	c._s32 [3] = x3;
	float          val0 = c._f32 [0];
	float          val1 = c._f32 [1];
	float          val2 = c._f32 [2];
	float          val3 = c._f32 [3];
	val0 = (coef_a * val0 + coef_b) * val0 + coef_c;
	val1 = (coef_a * val1 + coef_b) * val1 + coef_c;
	val2 = (coef_a * val2 + coef_b) * val2 + coef_c;
	val3 = (coef_a * val3 + coef_b) * val3 + coef_c;

	return VectF32 { {
		val0 + log_2_0,
		val1 + log_2_1,
		val2 + log_2_2,
		val3 + log_2_3,
	} };

#else // fstb_HAS_SIMD

#if fstb_ARCHI == fstb_ARCHI_X86

	// Extracts the exponent (actually log2_int = exponent - 1)
	__m128i        xi = _mm_castps_si128 (v);
	xi = _mm_srli_epi32 (xi, 23);
	const __m128i  l2_sub = _mm_set1_epi32 (log2_sub);
	xi = _mm_sub_epi32 (xi, l2_sub);
	const auto     log2_int = _mm_cvtepi32_ps (xi);

#elif fstb_ARCHI == fstb_ARCHI_ARM

	int32x4_t      xi = vreinterpretq_s32_f32 (v);
	xi = vshrq_n_s32 (xi, 23);
	const int32x4_t   l2_sub = vdupq_n_s32 (log2_sub);
	xi -= l2_sub;
	const auto     log2_int = vcvtq_f32_s32 (xi);

#endif // ff_arch_CPU

	// Extracts the multiplicative part in [1 ; 2[
	const auto     mask_mantissa = set1_f32 (1.17549421e-38f); // Binary: (1 << 23) - 1
	auto           part          = and_f32 (v, mask_mantissa);
	const auto     bias          = set1_f32 (1.0f);            // Binary: 127 << 23
	part = or_f32 (part, bias);

	// Computes the polynomial [1 ; 2[ -> [1 ; 2[
	// y = -1/3*x^2 + 2*x - 2/3
	// Ensures the C1 continuity over the whole range.
	// Its exact inverse is:
	// x = 3 - sqrt (7 - 3*y)
	const auto     a = set1_f32 (coef_a);
	const auto     b = set1_f32 (coef_b);
	const auto     c = set1_f32 (coef_c);
	auto           poly = a;
	poly = fmadd (poly, part, b);
	poly = fmadd (poly, part, c);

	// Sums the components
	const auto     total = log2_int + poly;

	return total;

#endif // fstb_HAS_SIMD
}



// Formula by 2DaT
// 12-13 ulp
// https://www.kvraudio.com/forum/viewtopic.php?f=33&t=532048
ToolsSimd::VectF32	ToolsSimd::log2_approx2 (VectF32 v)
{
#if ! defined (fstb_HAS_SIMD)

	assert (
	      v._ [0] > 0
		&& v._ [1] > 0
		&& v._ [2] > 0
		&& v._ [3] > 0
	);
	/*** To do: approximation ***/
	return VectF32 { {
		logf (v._ [0]) * float (LOG2_E),
		logf (v._ [1]) * float (LOG2_E),
		logf (v._ [2]) * float (LOG2_E),
		logf (v._ [3]) * float (LOG2_E),
	} };

#else // fstb_HAS_SIMD

	const auto     c0    = set1_f32 (1.011593342e+01f);
	const auto     c1    = set1_f32 (1.929443550e+01f);
	const auto     d0    = set1_f32 (2.095932245e+00f);
	const auto     d1    = set1_f32 (1.266638851e+01f);
	const auto     d2    = set1_f32 (6.316540241e+00f);
	const auto     one   = set1_f32 (1.0f);
	const auto     multi = set1_f32 (1.41421356237f);
	const auto     mantissa_mask = set1_s32 ((1 << 23) - 1);

#if fstb_ARCHI == fstb_ARCHI_X86

	__m128i        x_i           = _mm_castps_si128 (v);
	__m128i        spl_exp       = _mm_castps_si128 (v * multi);
	spl_exp = spl_exp - _mm_castps_si128 (one);
	spl_exp = _mm_andnot_si128 (mantissa_mask, spl_exp);
	const __m128   spl_mantissa  = _mm_castsi128_ps (x_i - spl_exp);
	spl_exp = _mm_srai_epi32 (spl_exp, 23);
	const __m128   log2_exponent = _mm_cvtepi32_ps (spl_exp);

#elif fstb_ARCHI == fstb_ARCHI_ARM

	const int32x4_t   x_i        = vreinterpretq_s32_f32 (v);
	int32x4_t      spl_exp       = vreinterpretq_s32_f32 (v * multi);
	spl_exp = spl_exp - vreinterpretq_s32_f32 (one);
	spl_exp = vandq_s32 (vmvnq_s32 (mantissa_mask), spl_exp);
	const float32x4_t   spl_mantissa  = vreinterpretq_f32_s32 (x_i - spl_exp);
	spl_exp = vshrq_n_s32 (spl_exp, 23);
	const float32x4_t   log2_exponent = vcvtq_f32_s32 (spl_exp);

#endif // ff_arch_CPU

	auto           num = spl_mantissa + c1;
	num = fmadd (num, spl_mantissa, c0);
	num *= spl_mantissa - one;

	auto           den = d2;
	den = fmadd (den, spl_mantissa, d1);
	den = fmadd (den, spl_mantissa, d0);

	auto           res = div_approx2 (num, den);
	res += log2_exponent;

	return res;

#endif // fstb_HAS_SIMD
}



ToolsSimd::VectF32	ToolsSimd::exp2_approx (VectF32 v)
{
	const float    coef_a   =  1.f / 3;
	const float    coef_b   =  2.f / 3;
	const float    coef_c   =  1.f;

#if ! defined (fstb_HAS_SIMD)

	const int32_t  tx0 = floor_int (v._ [0]);
	const int32_t  tx1 = floor_int (v._ [1]);
	const int32_t  tx2 = floor_int (v._ [2]);
	const int32_t  tx3 = floor_int (v._ [3]);
	const float    v0 = v._ [0] - static_cast <float> (tx0);
	const float    v1 = v._ [1] - static_cast <float> (tx1);
	const float    v2 = v._ [2] - static_cast <float> (tx2);
	const float    v3 = v._ [3] - static_cast <float> (tx3);
	Combo          combo;
	combo._f32 [0] = (coef_a * v0 + coef_b) * v0 + coef_c;
	combo._f32 [1] = (coef_a * v1 + coef_b) * v1 + coef_c;
	combo._f32 [2] = (coef_a * v2 + coef_b) * v2 + coef_c;
	combo._f32 [3] = (coef_a * v3 + coef_b) * v3 + coef_c;
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
	const auto     round_toward_m_i = set1_f32 (-0.5f);
	auto           xi        = _mm_cvtps_epi32 (v + round_toward_m_i);
	const auto     val_floor = _mm_cvtepi32_ps (xi);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const int      round_ofs = 256;
	int32x4_t      xi = vcvtq_s32_f32 (v + set1_f32 (round_ofs));
	xi -= vdupq_n_s32 (round_ofs);
	const auto     val_floor = vcvtq_f32_s32 (xi);
#endif // ff_arch_CPU

	const auto     frac = v - val_floor;

	// Computes the polynomial [0 ; 1] -> [1 ; 2]
	// y = 1/3*x^2 + 2/3*x + 1
	// Ensures the C1 continuity over the whole range.
	// Its exact inverse is:
	// x = sqrt (3*y - 2) - 1
	const auto     a    = set1_f32 (coef_a);
	const auto     b    = set1_f32 (coef_b);
	const auto     c    = set1_f32 (coef_c);
	auto           poly = a;
	poly = fmadd (poly, frac, b);
	poly = fmadd (poly, frac, c);

	// Integer part
#if fstb_ARCHI == fstb_ARCHI_X86
	xi  = _mm_slli_epi32 (xi, 23);
	xi += _mm_castps_si128 (poly);
	return _mm_castsi128_ps (xi);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	xi  = vshlq_n_s32 (xi, 23);
	xi += vreinterpretq_s32_f32 (poly);
	return vreinterpretq_f32_s32 (xi);
#endif // ff_arch_CPU

#endif // fstb_HAS_SIMD
}



// Formula by 2DaT
// Coefficients fixed by Andrew Simper to achieve true C0 continuity
// 3-4 ulp
// https://www.kvraudio.com/forum/viewtopic.php?p=7161124#p7161124
// https://www.kvraudio.com/forum/viewtopic.php?p=7677266#p7677266
ToolsSimd::VectF32	ToolsSimd::exp2_approx2 (VectF32 v)
{
#if ! defined (fstb_HAS_SIMD)

	/*** To do: approximation ***/
	return VectF32 { {
		expf (v._ [0] * float (LN2)),
		expf (v._ [1] * float (LN2)),
		expf (v._ [2] * float (LN2)),
		expf (v._ [3] * float (LN2)),
	} };

#else // fstb_HAS_SIMD

	// [-0.5, 0.5] 2^x approx polynomial ~ 2.4 ulp
	const auto     c0 = set1_f32 (1.000000088673463f);
	const auto     c1 = set1_f32 (0.69314693211407f);
	const auto     c2 = set1_f32 (0.24022037362574f);
	const auto     c3 = set1_f32 (0.0555072548370f);
	const auto     c4 = set1_f32 (0.0096798351988f);
	const auto     c5 = set1_f32 (0.0013285658116f);

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
	const auto     r = set1_f32 (round_ofs + 0.5f);
	auto           i = vcvtq_s32_f32 (v + r);
	i -= set1_s32 (round_ofs);
	v -= vcvtq_f32_s32 (i);
#endif // ff_arch_CPU

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
	return _mm_castsi128_ps (i + _mm_castps_si128 (p));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	i = vshlq_n_s32 (i, 23);
	return vreinterpretq_f32_s32 (i + vreinterpretq_s32_f32 (p));
#endif // ff_arch_CPU

#endif // fstb_HAS_SIMD
}



ToolsSimd::VectF32	ToolsSimd::select (VectF32 cond, VectF32 v_t, VectF32 v_f)
{
#if ! defined (fstb_HAS_SIMD)
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
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::cmp_gt_f32 (VectF32 lhs, VectF32 rhs)
{
#if ! defined (fstb_HAS_SIMD)
	Combo          r;
	r._s32 [0] = (lhs._ [0] > rhs._ [0]) ? -1 : 0;
	r._s32 [1] = (lhs._ [1] > rhs._ [1]) ? -1 : 0;
	r._s32 [2] = (lhs._ [2] > rhs._ [2]) ? -1 : 0;
	r._s32 [3] = (lhs._ [3] > rhs._ [3]) ? -1 : 0;
	return r._vf32;
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_cmpgt_ps (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_f32_u32 (vcgtq_f32 (lhs, rhs));
#endif // ff_arch_CPU
}



ToolsSimd::VectS32	ToolsSimd::cmp_gt_s32 (VectS32 lhs, VectS32 rhs)
{
#if ! defined (fstb_HAS_SIMD)
	return VectS32 { {
		(lhs._ [0] > rhs._ [0]) ? -1 : 0,
		(lhs._ [1] > rhs._ [1]) ? -1 : 0,
		(lhs._ [2] > rhs._ [2]) ? -1 : 0,
		(lhs._ [3] > rhs._ [3]) ? -1 : 0
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_cmpgt_epi32 (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_s32_u32 (vcgtq_s32 (lhs, rhs));
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::cmp_lt_f32 (VectF32 lhs, VectF32 rhs)
{
#if ! defined (fstb_HAS_SIMD)
	Combo          r;
	r._s32 [0] = (lhs._ [0] < rhs._ [0]) ? -1 : 0;
	r._s32 [1] = (lhs._ [1] < rhs._ [1]) ? -1 : 0;
	r._s32 [2] = (lhs._ [2] < rhs._ [2]) ? -1 : 0;
	r._s32 [3] = (lhs._ [3] < rhs._ [3]) ? -1 : 0;
	return r._vf32;
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_cmplt_ps (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_f32_u32 (vcltq_f32 (lhs, rhs));
#endif // ff_arch_CPU
}



ToolsSimd::VectS32	ToolsSimd::cmp_lt_s32 (VectS32 lhs, VectS32 rhs)
{
#if ! defined (fstb_HAS_SIMD)
	return VectS32 { {
		(lhs._ [0] < rhs._ [0]) ? -1 : 0,
		(lhs._ [1] < rhs._ [1]) ? -1 : 0,
		(lhs._ [2] < rhs._ [2]) ? -1 : 0,
		(lhs._ [3] < rhs._ [3]) ? -1 : 0
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_cmplt_epi32 (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_s32_u32 (vcltq_s32 (lhs, rhs));
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::cmp_lt0_f32 (VectF32 lhs)
{
#if ! defined (fstb_HAS_SIMD)
	Combo          r;
	r._s32 [0] = (lhs._ [0] < 0) ? -1 : 0;
	r._s32 [1] = (lhs._ [1] < 0) ? -1 : 0;
	r._s32 [2] = (lhs._ [2] < 0) ? -1 : 0;
	r._s32 [3] = (lhs._ [3] < 0) ? -1 : 0;
	return r._vf32;
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_castsi128_ps (_mm_srai_epi32 (_mm_castps_si128 (lhs), 31));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_f32_s32 (vshrq_n_s32 (vreinterpretq_s32_f32 (lhs), 31));
#endif // ff_arch_CPU
}



ToolsSimd::VectS32	ToolsSimd::cmp_lt0_s32 (VectS32 lhs)
{
#if ! defined (fstb_HAS_SIMD)
	return VectS32 { {
		(lhs._ [0] < 0) ? -1 : 0,
		(lhs._ [1] < 0) ? -1 : 0,
		(lhs._ [2] < 0) ? -1 : 0,
		(lhs._ [3] < 0) ? -1 : 0
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_cmplt_epi32 (lhs, _mm_setzero_si128 ());
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vshrq_n_s32 (lhs, 31));
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::cmp_eq_f32 (VectF32 lhs, VectF32 rhs)
{
#if ! defined (fstb_HAS_SIMD)
	Combo          r;
	r._s32 [0] = (lhs._ [0] == rhs._ [0]) ? -1 : 0;
	r._s32 [1] = (lhs._ [1] == rhs._ [1]) ? -1 : 0;
	r._s32 [2] = (lhs._ [2] == rhs._ [2]) ? -1 : 0;
	r._s32 [3] = (lhs._ [3] == rhs._ [3]) ? -1 : 0;
	return r._vf32;
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_cmpeq_ps (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_f32_u32 (vceqq_f32 (lhs, rhs));
#endif // ff_arch_CPU
}



ToolsSimd::VectS32	ToolsSimd::cmp_eq_s32 (VectS32 lhs, VectS32 rhs)
{
#if ! defined (fstb_HAS_SIMD)
	return VectS32 { {
		(lhs._ [0] == rhs._ [0]) ? -1 : 0,
		(lhs._ [1] == rhs._ [1]) ? -1 : 0,
		(lhs._ [2] == rhs._ [2]) ? -1 : 0,
		(lhs._ [3] == rhs._ [3]) ? -1 : 0
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_cmpeq_epi32 (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_s32_u32 (vceqq_s32 (lhs, rhs));
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::cmp_ne_f32 (VectF32 lhs, VectF32 rhs)
{
#if ! defined (fstb_HAS_SIMD)
	Combo          r;
	r._s32 [0] = (lhs._ [0] != rhs._ [0]) ? -1 : 0;
	r._s32 [1] = (lhs._ [1] != rhs._ [1]) ? -1 : 0;
	r._s32 [2] = (lhs._ [2] != rhs._ [2]) ? -1 : 0;
	r._s32 [3] = (lhs._ [3] != rhs._ [3]) ? -1 : 0;
	return r._vf32;
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_cmpneq_ps (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_f32_u32 (vmvnq_u32 (vceqq_f32 (lhs, rhs)));
#endif // ff_arch_CPU
}



ToolsSimd::VectS32	ToolsSimd::cmp_ne_s32 (VectS32 lhs, VectS32 rhs)
{
#if ! defined (fstb_HAS_SIMD)
	return VectS32 { {
		(lhs._ [0] != rhs._ [0]) ? -1 : 0,
		(lhs._ [1] != rhs._ [1]) ? -1 : 0,
		(lhs._ [2] != rhs._ [2]) ? -1 : 0,
		(lhs._ [3] != rhs._ [3]) ? -1 : 0
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	const auto     eq = _mm_cmpeq_epi32 (lhs, rhs);
	return _mm_xor_si128 (eq, _mm_set1_epi32 (-1));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_s32_u32 (vmvnq_u32 (vceqq_s32 (lhs, rhs)));
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::and_f32 (VectF32 lhs, VectF32 rhs)
{
#if ! defined (fstb_HAS_SIMD)
	Combo          al;
	Combo          ar;
	Combo          r;
	al._vf32   = lhs;
	ar._vf32   = rhs;
	r._s32 [0] = al._s32 [0] & ar._s32 [0];
	r._s32 [1] = al._s32 [1] & ar._s32 [1];
	r._s32 [2] = al._s32 [2] & ar._s32 [2];
	r._s32 [3] = al._s32 [3] & ar._s32 [3];
	return r._vf32;
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_and_ps (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_f32_u32 (vandq_u32 (
		vreinterpretq_u32_f32 (lhs),
		vreinterpretq_u32_f32 (rhs)
	));
#endif // ff_arch_CPU
}



ToolsSimd::VectS32	ToolsSimd::and_s32 (VectS32 lhs, VectS32 rhs)
{
#if ! defined (fstb_HAS_SIMD)
	return VectS32 { {
		lhs._ [0] & rhs._ [0],
		lhs._ [1] & rhs._ [1],
		lhs._ [2] & rhs._ [2],
		lhs._ [3] & rhs._ [3]
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_and_si128 (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vandq_s32 (lhs, rhs);
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::or_f32 (VectF32 lhs, VectF32 rhs)
{
#if ! defined (fstb_HAS_SIMD)
	Combo          al;
	Combo          ar;
	Combo          r;
	al._vf32   = lhs;
	ar._vf32   = rhs;
	r._s32 [0] = al._s32 [0] | ar._s32 [0];
	r._s32 [1] = al._s32 [1] | ar._s32 [1];
	r._s32 [2] = al._s32 [2] | ar._s32 [2];
	r._s32 [3] = al._s32 [3] | ar._s32 [3];
	return r._vf32;
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_or_ps (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_f32_u32 (vorrq_u32 (
		vreinterpretq_u32_f32 (lhs),
		vreinterpretq_u32_f32 (rhs)
	));
#endif // ff_arch_CPU
}



ToolsSimd::VectS32	ToolsSimd::or_s32 (VectS32 lhs, VectS32 rhs)
{
#if ! defined (fstb_HAS_SIMD)
	return VectS32 { {
		lhs._ [0] | rhs._ [0],
		lhs._ [1] | rhs._ [1],
		lhs._ [2] | rhs._ [2],
		lhs._ [3] | rhs._ [3]
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_or_si128 (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vorrq_s32 (lhs, rhs);
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::xor_f32 (VectF32 lhs, VectF32 rhs)
{
#if ! defined (fstb_HAS_SIMD)
	Combo          al;
	Combo          ar;
	Combo          r;
	al._vf32   = lhs;
	ar._vf32   = rhs;
	r._s32 [0] = al._s32 [0] ^ ar._s32 [0];
	r._s32 [1] = al._s32 [1] ^ ar._s32 [1];
	r._s32 [2] = al._s32 [2] ^ ar._s32 [2];
	r._s32 [3] = al._s32 [3] ^ ar._s32 [3];
	return r._vf32;
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_xor_ps (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_f32_u32 (veorq_u32 (
		vreinterpretq_u32_f32 (lhs),
		vreinterpretq_u32_f32 (rhs)
	));
#endif // ff_arch_CPU
}



ToolsSimd::VectS32	ToolsSimd::xor_s32 (VectS32 lhs, VectS32 rhs)
{
#if ! defined (fstb_HAS_SIMD)
	return VectS32 { {
		lhs._ [0] ^ rhs._ [0],
		lhs._ [1] ^ rhs._ [1],
		lhs._ [2] ^ rhs._ [2],
		lhs._ [3] ^ rhs._ [3]
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_xor_si128 (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return veorq_s32 (lhs, rhs);
#endif // ff_arch_CPU
}



// Works only with well-formed condition results (tested bits depends on the implementation).
// For each scalar, true = all bits set, false = all bits cleared
bool	ToolsSimd::and_h (VectF32 cond)
{
#if ! defined (fstb_HAS_SIMD)
	Combo          c;
	c._vf32 = cond;
	const int32_t  t = (c._s32 [0] & c._s32 [1]) & (c._s32 [2] & c._s32 [3]);
	return (t == -1);
#elif fstb_ARCHI == fstb_ARCHI_X86
	return (_mm_movemask_ps (cond) == 15);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const uint32x2_t  tmp = vreinterpret_u32_u16 (
		vqmovn_u32 (vreinterpretq_u32_f32 (cond))
	);
	return (   vget_lane_u32 (tmp, 0) == 0xFFFFFFFFU
	        && vget_lane_u32 (tmp, 1) == 0xFFFFFFFFU);
#endif // ff_arch_CPU
}



// Works only with well-formed condition results (tested bits depends on the implementation).
// For each scalar, true = all bits set, false = all bits cleared
bool	ToolsSimd::or_h (VectF32 cond)
{
#if ! defined (fstb_HAS_SIMD)
	Combo          c;
	c._vf32 = cond;
	const int32_t  t = (c._s32 [0] | c._s32 [1]) | (c._s32 [2] | c._s32 [3]);
	return (t != 0);
#elif fstb_ARCHI == fstb_ARCHI_X86
	return (_mm_movemask_ps (cond) != 0);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const uint32x2_t  tmp = vreinterpret_u32_u16 (
		vqmovn_u32 (vreinterpretq_u32_f32 (cond))
	);
	return (   vget_lane_u32 (tmp, 0) != 0
	        || vget_lane_u32 (tmp, 1) != 0);
#endif // ff_arch_CPU
}



// Works only with well-formed condition results (tested bits depends on the implementation).
// For each scalar, true = all bits set, false = all bits cleared
bool	ToolsSimd::and_h (VectS32 cond)
{
#if ! defined (fstb_HAS_SIMD)
	const int32_t  t = (cond._ [0] & cond._ [1]) & (cond._ [2] & cond._ [3]);
	return (t == -1);
#elif fstb_ARCHI == fstb_ARCHI_X86
	return (_mm_movemask_epi8 (cond) == 0xFFFF);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const uint32x2_t  tmp = vreinterpret_u32_u16 (
		vqmovn_u32 (vreinterpretq_u32_s32 (cond))
	);
	return (   vget_lane_u32 (tmp, 0) == 0xFFFFFFFFU
	        && vget_lane_u32 (tmp, 1) == 0xFFFFFFFFU);
#endif // ff_arch_CPU
}



// Works only with well-formed condition results (tested bits depends on the implementation).
// For each scalar, true = all bits set, false = all bits cleared
bool	ToolsSimd::or_h (VectS32 cond)
{
#if ! defined (fstb_HAS_SIMD)
	const int32_t  t = (cond._ [0] | cond._ [1]) | (cond._ [2] | cond._ [3]);
	return (t != 0);
#elif fstb_ARCHI == fstb_ARCHI_X86
	return (_mm_movemask_epi8 (cond) != 0);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const uint32x2_t  tmp = vreinterpret_u32_u16 (
		vqmovn_u32 (vreinterpretq_u32_s32 (cond))
	);
	return (   vget_lane_u32 (tmp, 0) != 0
	        || vget_lane_u32 (tmp, 1) != 0);
#endif // ff_arch_CPU
}



// "true" must be 1 and nothing else.
ToolsSimd::VectF32	ToolsSimd::set_mask_f32 (bool m0, bool m1, bool m2, bool m3)
{
#if ! defined (fstb_HAS_SIMD)
	Combo          c;
	c._s32 [0] = -int32_t (m0);
	c._s32 [1] = -int32_t (m1);
	c._s32 [2] = -int32_t (m2);
	c._s32 [3] = -int32_t (m3);
	return c._vf32;
#elif 1 // Fast version
#if fstb_ARCHI == fstb_ARCHI_X86
	return _mm_castsi128_ps (_mm_sub_epi32 (
		_mm_setzero_si128 (),
		_mm_set_epi32 (m3, m2, m1, m0)
	));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	float32x2_t    v01 = vdup_n_f32 (m0);
	float32x2_t    v23 = vdup_n_f32 (m2);
	v01 = vset_lane_f32 (m1, v01, 1);
	v23 = vset_lane_f32 (m3, v23, 1);
	return vreinterpretq_f32_s32 (vnegq_s32 (vreinterpretq_s32_f32 (
		vcombine_f32 (v01, v23)
	)));
#endif
#else // Safer but slower version
#if fstb_ARCHI == fstb_ARCHI_X86
	return _mm_castsi128_ps (_mm_sub_epi32 (
		_mm_set_epi32 (!m3, !m2, !m1, !m0),
		_mm_set1_epi32 (1)
	));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	float32x2_t    v01 = vdup_n_f32 (!m0);
	float32x2_t    v23 = vdup_n_f32 (!m2);
	v01 = vset_lane_f32 (!m1, v01, 1);
	v23 = vset_lane_f32 (!m3, v23, 1);
	const auto     one  = vdupq_n_s32 (1);
	return vreinterpretq_f32_s32 (vsubq_s32 (
		vreinterpretq_s32_f32 (vcombine_f32 (v01, v23)),
		one
	));
#endif // ff_arch_CPU
#endif // Versions
}



int	ToolsSimd::count_bits (VectS32 x)
{
#if ! defined (fstb_HAS_SIMD)
	// http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
	uint32_t       v0 = x._ [0] - ((x._ [0] >> 1) & 0x55555555);
	uint32_t       v1 = x._ [1] - ((x._ [1] >> 1) & 0x55555555);
	uint32_t       v2 = x._ [2] - ((x._ [2] >> 1) & 0x55555555);
	uint32_t       v3 = x._ [3] - ((x._ [3] >> 1) & 0x55555555);
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
#endif // ff_arch_CPU
}



// Assumes x is a result of a comparison, with all bits the same
// in each 32-bit element.
unsigned int	ToolsSimd::movemask_f32 (VectF32 x)
{
#if ! defined (fstb_HAS_SIMD)
	Combo          c;
	c._vf32 = x;
	return
		   (c._u32 [0] >> 31)
		| ((c._u32 [0] >> 30) & 2)
		| ((c._u32 [0] >> 29) & 4)
		| ((c._u32 [0] >> 28) & 8);
#elif fstb_ARCHI == fstb_ARCHI_X86
	return (unsigned int) (_mm_movemask_ps (x));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	uint64x2_t     tmp1 =
		vreinterpretq_u64_f32 (x);    // ddd...ddd ccc...ccc bbb...bbb aaa...aaa
	tmp1 = vshrq_n_u64 (tmp1, 31);   // 000...00d ddd...ddc 000...00b bbb...bba
	uint64x1_t     tmp2 = vsli_n_u64 (
		vget_high_u64 (tmp1),
		vget_low_u64 (tmp1),
		2
	);
	return (vget_lane_u32 (vreinterpret_u32_u64 (tmp2), 0) & 0xF);
#endif // ff_arch_CPU
}



// p0[0 1] p1[0 1]
ToolsSimd::VectF32	ToolsSimd::interleave_2f32_lo (VectF32 p0, VectF32 p1)
{
#if ! defined (fstb_HAS_SIMD)
	return VectF32 { { p0._ [0], p0._ [1], p1._ [0], p1._ [1] } };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_shuffle_ps (p0, p1, (1<<6) + (0<<4) + (1<<2) + (0<<0));
	// return _mm_movelh_ps (p0, p1);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const float32x2_t  p0x = vget_low_f32 (p0);
	const float32x2_t  p1x = vget_low_f32 (p1);
	return vcombine_f32 (p0x, p1x);
#endif // ff_arch_CPU
}



// p0[2 3] p1[2 3]
ToolsSimd::VectF32	ToolsSimd::interleave_2f32_hi (VectF32 p0, VectF32 p1)
{
#if ! defined (fstb_HAS_SIMD)
	return VectF32 { { p0._ [2], p0._ [3], p1._ [2], p1._ [3] } };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_shuffle_ps (p0, p1, (3<<6) + (2<<4) + (3<<2) + (2<<0));
	// return _mm_movehl_ps (p1, p0);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const float32x2_t  p0x = vget_high_f32 (p0);
	const float32x2_t  p1x = vget_high_f32 (p1);
	return vcombine_f32 (p0x, p1x);
#endif // ff_arch_CPU
}



void	ToolsSimd::interleave_f32 (VectF32 &i0, VectF32 &i1, VectF32 p0, VectF32 p1)
{
#if ! defined (fstb_HAS_SIMD)
	i0._ [0] = p0._ [0];
	i0._ [1] = p1._ [0];
	i0._ [2] = p0._ [1];
	i0._ [3] = p1._ [1];
	i1._ [0] = p0._ [2];
	i1._ [1] = p1._ [2];
	i1._ [2] = p0._ [3];
	i1._ [3] = p1._ [3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	i0 = _mm_unpacklo_ps (p0, p1);
	i1 = _mm_unpackhi_ps (p0, p1);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const float32x4x2_t  tmp = vzipq_f32 (p0, p1);
	i0 = tmp.val [0];
	i1 = tmp.val [1];
#endif // ff_arch_CPU
}



void	ToolsSimd::deinterleave_f32 (VectF32 &p0, VectF32 &p1, VectF32 i0, VectF32 i1)
{
#if ! defined (fstb_HAS_SIMD)
	p0._ [0] = i0._ [0];
	p1._ [0] = i0._ [1];
	p0._ [1] = i0._ [2];
	p1._ [1] = i0._ [3];
	p0._ [2] = i1._ [0];
	p1._ [2] = i1._ [1];
	p0._ [3] = i1._ [2];
	p1._ [3] = i1._ [3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	p0 = _mm_shuffle_ps (i0, i1, 0x88);
	p1 = _mm_shuffle_ps (i0, i1, 0xDD);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const float32x4x2_t  tmp = vuzpq_f32 (i0, i1);
	p0 = tmp.val [0];
	p1 = tmp.val [1];
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::deinterleave_f32_lo (VectF32 i0, VectF32 i1)
{
#if ! defined (fstb_HAS_SIMD)
	return VectF32 { { i0._ [0], i0._ [2], i1._ [0], i1._ [2] } };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_shuffle_ps (i0, i1, 0x88);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vuzpq_f32 (i0, i1).val [0];
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::deinterleave_f32_hi (VectF32 i0, VectF32 i1)
{
#if ! defined (fstb_HAS_SIMD)
	return VectF32 { { i0._ [1], i0._ [3], i1._ [1], i1._ [3] } };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_shuffle_ps (i0, i1, 0xDD);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vuzpq_f32 (i0, i1).val [1];
#endif // ff_arch_CPU
}



// Sources:
// https://github.com/Maratyszcza/NNPACK/blob/master/src/neon/transpose.h
// https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_MM_TRANSPOSE4_PS&expand=5915,5949
void	ToolsSimd::transpose_f32 (VectF32 &a0, VectF32 &a1, VectF32 &a2, VectF32 &a3)
{
#if ! defined (fstb_HAS_SIMD)
	VectF32        tmp0;
	VectF32        tmp1;
	VectF32        tmp2;
	VectF32        tmp3;
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
	VectF32        k0, k1, k2, k3;
	interleave_f32 (k0, k1, a0, a2);
	interleave_f32 (k2, k3, a1, a3);
	interleave_f32 (a0, a1, k0, k2);
	interleave_f32 (a2, a3, k1, k3);
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::monofy_2f32_lo (VectF32 v)
{
#if ! defined (fstb_HAS_SIMD)
	return VectF32 { { v._ [0], v._ [0], v._ [2], v._ [2] } };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_shuffle_ps (v, v, 0xA0);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vuzpq_f32 (v, v).val [0];
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::monofy_2f32_hi (VectF32 v)
{
#if ! defined (fstb_HAS_SIMD)
	return VectF32 { { v._ [1], v._ [1], v._ [3], v._ [3] } };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_shuffle_ps (v, v, 0xF5);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vuzpq_f32 (v, v).val [1];
#endif // ff_arch_CPU
}



// Rounding method is unspecified (towards 0 on ARM, depends on MXCSR on x86)
ToolsSimd::VectS32	ToolsSimd::conv_f32_to_s32 (VectF32 x)
{
#if ! defined (fstb_HAS_SIMD)
	return VectS32 { {
		conv_int_fast (x._ [0]),
		conv_int_fast (x._ [1]),
		conv_int_fast (x._ [2]),
		conv_int_fast (x._ [3])
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_cvtps_epi32 (x);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vcvtq_s32_f32 (x);
#endif // ff_arch_CPU
}



// Not exact on the rounding boundaries
// Assumes rounding mode is to-nearest on x86
ToolsSimd::VectS32	ToolsSimd::round_f32_to_s32 (VectF32 x)
{
#if ! defined (fstb_HAS_SIMD)
	return VectS32 { {
		round_int (x._ [0]),
		round_int (x._ [1]),
		round_int (x._ [2]),
		round_int (x._ [3])
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
#endif // ff_arch_CPU
}



// Not exact on the rounding boundaries
// Assumes rounding mode is to-nearest on x86
ToolsSimd::VectS32	ToolsSimd::floor_f32_to_s32 (VectF32 x)
{
#if ! defined (fstb_HAS_SIMD)
	return VectS32 { {
		floor_int (x._ [0]),
		floor_int (x._ [1]),
		floor_int (x._ [2]),
		floor_int (x._ [3])
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
#endif // ff_arch_CPU
}



ToolsSimd::VectF32	ToolsSimd::conv_s32_to_f32 (VectS32 x)
{
#if ! defined (fstb_HAS_SIMD)
	return VectF32 { {
		float (x._ [0]),
		float (x._ [1]),
		float (x._ [2]),
		float (x._ [3])
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_cvtepi32_ps (x);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vcvtq_f32_s32 (x);
#endif // ff_arch_CPU
}



void	ToolsSimd::start_lerp (VectF32 &val_cur, VectF32 &step, float val_beg, float val_end, int size)
{
	assert (size > 0);

	const float    dif = val_end - val_beg;
	const float    four_over_size =
		  (size < _inv_table_4_len)
		? _inv_table_4 [size]
		: 4.0f / float (size);
	step    = set1_f32 (dif * four_over_size);
	val_cur = set1_f32 (val_beg);
	const auto     c0123 = set_f32 (0, 0.25f, 0.5f, 0.75f);
	mac (val_cur, step, c0123);
}



ToolsSimd::VectS32	ToolsSimd::select (VectS32 cond, VectS32 v_t, VectS32 v_f)
{
#if ! defined (fstb_HAS_SIMD)
	return VectS32 { {
		(cond._ [0] & v_t._ [0]) | (~cond._ [0] & v_f._ [0]),
		(cond._ [1] & v_t._ [1]) | (~cond._ [1] & v_f._ [1]),
		(cond._ [2] & v_t._ [2]) | (~cond._ [2] & v_f._ [2]),
		(cond._ [3] & v_t._ [3]) | (~cond._ [3] & v_f._ [3])
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	const auto     cond_1 = _mm_and_si128 (cond, v_t);
	const auto     cond_0 = _mm_andnot_si128 (cond, v_f);
	return _mm_or_si128 (cond_0, cond_1);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vbslq_s32 (vreinterpretq_u32_s32 (cond), v_t, v_f);
#endif // ff_arch_CPU
}



ToolsSimd::VectS32	ToolsSimd::min_s32 (VectS32 lhs, VectS32 rhs)
{
#if ! defined (fstb_HAS_SIMD)
	return VectS32 { {
		std::min (lhs._ [0], rhs._ [0]),
		std::min (lhs._ [1], rhs._ [1]),
		std::min (lhs._ [2], rhs._ [2]),
		std::min (lhs._ [3], rhs._ [3])
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	const auto     gt = _mm_cmpgt_epi32 (lhs, rhs);
	return select (gt, lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vminq_s32 (lhs, rhs);
#endif // ff_arch_CPU
}



ToolsSimd::VectS32	ToolsSimd::max_s32 (VectS32 lhs, VectS32 rhs)
{
#if ! defined (fstb_HAS_SIMD)
	return VectS32 { {
		std::max (lhs._ [0], rhs._ [0]),
		std::max (lhs._ [1], rhs._ [1]),
		std::max (lhs._ [2], rhs._ [2]),
		std::max (lhs._ [3], rhs._ [3])
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	const auto     lt = _mm_cmplt_epi32 (lhs, rhs);
	return select (lt, lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vmaxq_s32 (lhs, rhs);
#endif // ff_arch_CPU
}



// Positive = to the left, rotates towards the higher indexes
template <int SHIFT>
ToolsSimd::VectF32	ToolsSimd::Shift <SHIFT>::rotate (VectF32 a)
{
#if ! defined (fstb_HAS_SIMD)
	return VectF32 { {
		a._ [(0 - SHIFT) & 3],
		a._ [(1 - SHIFT) & 3],
		a._ [(2 - SHIFT) & 3],
		a._ [(3 - SHIFT) & 3]
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
#endif // ff_arch_CPU
}



template <int SHIFT>
float	ToolsSimd::Shift <SHIFT>::extract (VectF32 a)
{
#if ! defined (fstb_HAS_SIMD)
	return a._ [SHIFT & 3];
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
#endif // ff_arch_CPU
}



template <int SHIFT>
ToolsSimd::VectF32	ToolsSimd::Shift <SHIFT>::insert (VectF32 a, float val)
{
#if ! defined (fstb_HAS_SIMD)
	a._ [SHIFT & 3] = val;
	return a;
#elif fstb_ARCHI == fstb_ARCHI_X86
	a = Shift <(-SHIFT) & 3>::rotate (a);
	a = _mm_move_ss (a, _mm_set_ss (val));
	a = Shift <  SHIFT     >::rotate (a);
	return a;
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vsetq_lane_f32 (val, a, SHIFT & 3);
#endif // ff_arch_CPU
}



template <int SHIFT>
ToolsSimd::VectF32	ToolsSimd::Shift <SHIFT>::spread (VectF32 a)
{
#if ! defined (fstb_HAS_SIMD)
	return set1_f32 (extract (a));
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_shuffle_ps (a, a, 0x55 * (SHIFT & 3));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vdupq_n_f32 (vgetq_lane_f32 (a, SHIFT & 3));
#endif // ff_arch_CPU
}



// Extracts the vector at the position SHIFT from the double-width vector {a b}
// Concatenates a [SHIFT...3] with b [0...3-SHIFT]
template <int SHIFT>
ToolsSimd::VectF32	ToolsSimd::Shift <SHIFT>::compose (VectF32 a, VectF32 b)
{
#if ! defined (fstb_HAS_SIMD)
	switch (SHIFT & 3)
	{
	case 1: a._ [0] = a._[1]; a._ [1] = a._[2]; a._ [2] = a._[3]; a._ [3] = b._[0]; break;
	case 2: a._ [0] = a._[2]; a._ [1] = a._[3]; a._ [2] = b._[0]; a._ [3] = b._[1]; break;
	case 3: a._ [0] = a._[3]; a._ [1] = b._[0]; a._ [2] = b._[1]; a._ [3] = b._[2]; break;
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
#endif // ff_arch_CPU
}



// Positive = left
template <int SHIFT>
ToolsSimd::VectS32	ToolsSimd::Shift <SHIFT>::rotate (VectS32 a)
{
#if ! defined (fstb_HAS_SIMD)
	return VectS32 { {
		a._ [(0 - SHIFT) & 3],
		a._ [(1 - SHIFT) & 3],
		a._ [(2 - SHIFT) & 3],
		a._ [(3 - SHIFT) & 3]
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
#endif // ff_arch_CPU
}



template <int SHIFT>
int32_t	ToolsSimd::Shift <SHIFT>::extract (VectS32 a)
{
#if ! defined (fstb_HAS_SIMD)
	return a._ [SHIFT & 3];
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
#endif // ff_arch_CPU
}



template <int SHIFT>
ToolsSimd::VectS32	ToolsSimd::Shift <SHIFT>::insert (VectS32 a, int32_t val)
{
#if ! defined (fstb_HAS_SIMD)
	a._ [SHIFT & 3] = val;
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
#endif // ff_arch_CPU
}



template <int SHIFT>
ToolsSimd::VectS32	ToolsSimd::Shift <SHIFT>::spread (VectS32 a)
{
#if ! defined (fstb_HAS_SIMD)
	return set1_s32 (extract (a));
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_shuffle_epi32 (a, 0x55 * (SHIFT & 3));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vdupq_n_s32 (vgetq_lane_s32 (a, SHIFT & 3));
#endif // ff_arch_CPU
}



// Extracts the vector at the position SHIFT from the double-width vector {a b}
// Concatenates a [SHIFT...3] with b [0...3-SHIFT]
template <int SHIFT>
ToolsSimd::VectS32	ToolsSimd::Shift <SHIFT>::compose (VectS32 a, VectS32 b)
{
#if ! defined (fstb_HAS_SIMD)
	switch (SHIFT & 3)
	{
	case 1: a._ [0] = a._[1]; a._ [1] = a._[2]; a._ [2] = a._[3]; a._ [3] = b._[0]; break;
	case 2: a._ [0] = a._[2]; a._ [1] = a._[3]; a._ [2] = b._[0]; a._ [3] = b._[1]; break;
	case 3: a._ [0] = a._[3]; a._ [1] = b._[0]; a._ [2] = b._[1]; a._ [3] = b._[2]; break;
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
#endif // ff_arch_CPU
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename MEM>
void	ToolsSimd::store_f32_part_n13 (MEM *ptr, VectF32 v, int n)
{
	assert (n > 0);
	assert (n < 4);

	float *        f_ptr = reinterpret_cast <float *> (ptr);

#if ! defined (fstb_HAS_SIMD)

	for (int i = 0; i < n; ++i)
	{
		f_ptr [i] = v._ [i];
	}

#elif fstb_ARCHI == fstb_ARCHI_ARM

	vst1q_lane_f32 (f_ptr + 0, v, 0);
	if (n >= 2)
	{
		vst1q_lane_f32 (f_ptr + 1, v, 1);
		if (n >= 3)
		{
			vst1q_lane_f32 (f_ptr + 2, v, 2);
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



template <typename MEM>
void	ToolsSimd::store_s32_part_n13 (MEM *ptr, VectS32 v, int n)
{
	assert (n > 0);
	assert (n < 4);

	int32_t *      f_ptr = reinterpret_cast <int32_t *> (ptr);

#if ! defined (fstb_HAS_SIMD)

	for (int i = 0; i < n; ++i)
	{
		f_ptr [i] = v._ [i];
	}

#elif fstb_ARCHI == fstb_ARCHI_ARM

	vst1q_lane_s32 (f_ptr + 0, v, 0);
	if (n >= 2)
	{
		vst1q_lane_s32 (f_ptr + 1, v, 1);
		if (n >= 3)
		{
			vst1q_lane_s32 (f_ptr + 2, v, 2);
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



template <typename MEM>
ToolsSimd::VectF32	ToolsSimd::load_f32_part_n13 (const MEM *ptr, int n)
{
	assert (n > 0);
	assert (n < 4);

	const float *  f_ptr = reinterpret_cast <const float *> (ptr);
	VectF32        v;
#if ! defined (fstb_HAS_SIMD)
	v._ [0] = f_ptr [0];
	for (int i = 1; i < n; ++i)
	{
		v._ [i] = f_ptr [i];
	}
#elif fstb_ARCHI == fstb_ARCHI_X86
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
	default:
		assert (false);
		// Keeps the compiler happy with (un)initialisation
		v = set1_f32 (f_ptr [0]);
		break;
	}
#elif fstb_ARCHI == fstb_ARCHI_ARM
	v = vmovq_n_f32 (f_ptr [0]);
	if (n >= 2)
	{
		v = vld1q_lane_f32 (f_ptr + 1, v, 1);
		if (n >= 3)
		{
			v = vld1q_lane_f32 (f_ptr + 2, v, 2);
		}
	}
#endif // ff_arch_CPU

	return v;
}



}  // namespace fstb



#if ! defined (fstb_HAS_SIMD) || (fstb_COMPILER == fstb_COMPILER_MSVC)

fstb_ToolsSimd_OPNS_BEG



fstb_ToolsSimd_OPNS (VectF32) &	operator += (fstb_ToolsSimd_OPNS (VectF32) &lhs, const fstb_ToolsSimd_OPNS (VectF32) &rhs)
{
#if ! defined (fstb_HAS_SIMD)
	lhs._ [0] += rhs._ [0];
	lhs._ [1] += rhs._ [1];
	lhs._ [2] += rhs._ [2];
	lhs._ [3] += rhs._ [3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	lhs = _mm_add_ps (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	lhs = vaddq_f32 (lhs, rhs);
#endif // ff_arch_CPU
	return lhs;
}



fstb_ToolsSimd_OPNS (VectF32) &	operator -= (fstb_ToolsSimd_OPNS (VectF32) &lhs, const fstb_ToolsSimd_OPNS (VectF32) &rhs)
{
#if ! defined (fstb_HAS_SIMD)
	lhs._ [0] -= rhs._ [0];
	lhs._ [1] -= rhs._ [1];
	lhs._ [2] -= rhs._ [2];
	lhs._ [3] -= rhs._ [3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	lhs = _mm_sub_ps (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	lhs = vsubq_f32 (lhs, rhs);
#endif // ff_arch_CPU
	return lhs;
}



fstb_ToolsSimd_OPNS (VectF32) &	operator *= (fstb_ToolsSimd_OPNS (VectF32) &lhs, const fstb_ToolsSimd_OPNS (VectF32) &rhs)
{
#if ! defined (fstb_HAS_SIMD)
	lhs._ [0] *= rhs._ [0];
	lhs._ [1] *= rhs._ [1];
	lhs._ [2] *= rhs._ [2];
	lhs._ [3] *= rhs._ [3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	lhs = _mm_mul_ps (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	lhs = vmulq_f32 (lhs, rhs);
#endif // ff_arch_CPU
	return lhs;
}



fstb_ToolsSimd_OPNS (VectF32) operator + (fstb_ToolsSimd_OPNS (VectF32) lhs, const fstb_ToolsSimd_OPNS (VectF32) &rhs)
{
#if ! defined (fstb_HAS_SIMD)
	return fstb_ToolsSimd_OPNS (VectF32) { {
		lhs._ [0] + rhs._ [0],
		lhs._ [1] + rhs._ [1],
		lhs._ [2] + rhs._ [2],
		lhs._ [3] + rhs._ [3]
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_add_ps (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vaddq_f32 (lhs, rhs);
#endif // ff_arch_CPU
}



fstb_ToolsSimd_OPNS (VectF32) operator - (fstb_ToolsSimd_OPNS (VectF32) lhs, const fstb_ToolsSimd_OPNS (VectF32) &rhs)
{
#if ! defined (fstb_HAS_SIMD)
	return fstb_ToolsSimd_OPNS (VectF32) { {
		lhs._ [0] - rhs._ [0],
		lhs._ [1] - rhs._ [1],
		lhs._ [2] - rhs._ [2],
		lhs._ [3] - rhs._ [3]
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_sub_ps (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vsubq_f32 (lhs, rhs);
#endif // ff_arch_CPU
}



fstb_ToolsSimd_OPNS (VectF32) operator * (fstb_ToolsSimd_OPNS (VectF32) lhs, const fstb_ToolsSimd_OPNS (VectF32) &rhs)
{
#if ! defined (fstb_HAS_SIMD)
	return fstb_ToolsSimd_OPNS (VectF32) { {
		lhs._ [0] * rhs._ [0],
		lhs._ [1] * rhs._ [1],
		lhs._ [2] * rhs._ [2],
		lhs._ [3] * rhs._ [3]
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_mul_ps (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vmulq_f32 (lhs, rhs);
#endif // ff_arch_CPU
}



fstb_ToolsSimd_OPNS (VectS32) &	operator += (fstb_ToolsSimd_OPNS (VectS32) &lhs, const fstb_ToolsSimd_OPNS (VectS32) &rhs)
{
#if ! defined (fstb_HAS_SIMD)
	lhs._ [0] += rhs._ [0];
	lhs._ [1] += rhs._ [1];
	lhs._ [2] += rhs._ [2];
	lhs._ [3] += rhs._ [3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	lhs = _mm_add_epi32 (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	lhs = vaddq_s32 (lhs, rhs);
#endif // ff_arch_CPU
	return lhs;
}



fstb_ToolsSimd_OPNS (VectS32) &	operator -= (fstb_ToolsSimd_OPNS (VectS32) &lhs, const fstb_ToolsSimd_OPNS (VectS32) &rhs)
{
#if ! defined (fstb_HAS_SIMD)
	lhs._ [0] -= rhs._ [0];
	lhs._ [1] -= rhs._ [1];
	lhs._ [2] -= rhs._ [2];
	lhs._ [3] -= rhs._ [3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	lhs = _mm_sub_epi32 (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	lhs = vsubq_s32 (lhs, rhs);
#endif // ff_arch_CPU
	return lhs;
}



fstb_ToolsSimd_OPNS (VectS32) &	operator *= (fstb_ToolsSimd_OPNS (VectS32) &lhs, const fstb_ToolsSimd_OPNS (VectS32) &rhs)
{
#if ! defined (fstb_HAS_SIMD)
	lhs._ [0] *= rhs._ [0];
	lhs._ [1] *= rhs._ [1];
	lhs._ [2] *= rhs._ [2];
	lhs._ [3] *= rhs._ [3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	// mullo_epi32
	// Code of this function shamelessly borrowed from tp7
	// https://github.com/tp7/masktools/blob/16bit/masktools/common/simd.h
	const __m128i  lhs13  = _mm_shuffle_epi32 (lhs, 0xF5);       // (-,a3,-,a1)
	const __m128i  rhs13  = _mm_shuffle_epi32 (rhs, 0xF5);       // (-,b3,-,b1)
	const __m128i  prod02 = _mm_mul_epu32 (lhs, rhs);            // (-,a2*b2,-,a0*b0)
	const __m128i  prod13 = _mm_mul_epu32 (lhs13, rhs13);        // (-,a3*b3,-,a1*b1)
	const __m128i  prod01 = _mm_unpacklo_epi32 (prod02, prod13); // (-,-,a1*b1,a0*b0)
	const __m128i  prod23 = _mm_unpackhi_epi32 (prod02, prod13); // (-,-,a3*b3,a2*b2)
	lhs                   = _mm_unpacklo_epi64 (prod01 ,prod23); // (ab3,ab2,ab1,ab0)
#elif fstb_ARCHI == fstb_ARCHI_ARM
	lhs = vmulq_s32 (lhs, rhs);
#endif // ff_arch_CPU
	return lhs;
}



fstb_ToolsSimd_OPNS (VectS32) & operator >>= (fstb_ToolsSimd_OPNS (VectS32) &x, int scalar)
{
	assert (scalar >= 0);
#if ! defined (fstb_HAS_SIMD)
	x._ [0] >>= scalar;
	x._ [1] >>= scalar;
	x._ [2] >>= scalar;
	x._ [3] >>= scalar;
#elif fstb_ARCHI == fstb_ARCHI_X86
	x = _mm_srai_epi32 (x, scalar);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	x = vshrq_n_s32 (x, scalar);
#endif // ff_arch_CPU
	return x;
}



fstb_ToolsSimd_OPNS (VectS32) & operator <<= (fstb_ToolsSimd_OPNS (VectS32) &x, int scalar)
{
	assert (scalar >= 0);
#if ! defined (fstb_HAS_SIMD)
	x._ [0] <<= scalar;
	x._ [1] <<= scalar;
	x._ [2] <<= scalar;
	x._ [3] <<= scalar;
#elif fstb_ARCHI == fstb_ARCHI_X86
	x = _mm_slli_epi32 (x, scalar);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	x = vshlq_n_s32 (x, scalar);
#endif // ff_arch_CPU
	return x;
}



fstb_ToolsSimd_OPNS (VectS32) operator + (fstb_ToolsSimd_OPNS (VectS32) lhs, const fstb_ToolsSimd_OPNS (VectS32) &rhs)
{
#if ! defined (fstb_HAS_SIMD)
	return fstb_ToolsSimd_OPNS (VectS32) { {
		lhs._ [0] + rhs._ [0],
		lhs._ [1] + rhs._ [1],
		lhs._ [2] + rhs._ [2],
		lhs._ [3] + rhs._ [3]
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_add_epi32 (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vaddq_s32 (lhs, rhs);
#endif // ff_arch_CPU
}



fstb_ToolsSimd_OPNS (VectS32) operator - (fstb_ToolsSimd_OPNS (VectS32) lhs, const fstb_ToolsSimd_OPNS (VectS32) &rhs)
{
#if ! defined (fstb_HAS_SIMD)
	return fstb_ToolsSimd_OPNS (VectS32) { {
		lhs._ [0] - rhs._ [0],
		lhs._ [1] - rhs._ [1],
		lhs._ [2] - rhs._ [2],
		lhs._ [3] - rhs._ [3]
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_sub_epi32 (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vsubq_s32 (lhs, rhs);
#endif // ff_arch_CPU
}



fstb_ToolsSimd_OPNS (VectS32) operator * (fstb_ToolsSimd_OPNS (VectS32) lhs, const fstb_ToolsSimd_OPNS (VectS32) &rhs)
{
#if ! defined (fstb_HAS_SIMD)
	return fstb_ToolsSimd_OPNS (VectS32) { {
		lhs._ [0] * rhs._ [0],
		lhs._ [1] * rhs._ [1],
		lhs._ [2] * rhs._ [2],
		lhs._ [3] * rhs._ [3]
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return lhs *= rhs;
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vmulq_s32 (lhs, rhs);
#endif // ff_arch_CPU
}



fstb_ToolsSimd_OPNS (VectS32) operator >> (fstb_ToolsSimd_OPNS (VectS32) x, int scalar)
{
	assert (scalar >= 0);
#if ! defined (fstb_HAS_SIMD)
	return fstb_ToolsSimd_OPNS (VectS32) { {
		x._ [0] >> scalar,
		x._ [1] >> scalar,
		x._ [2] >> scalar,
		x._ [3] >> scalar
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_srai_epi32 (x, scalar);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vshrq_n_s32 (x, scalar);
#endif // ff_arch_CPU
}



fstb_ToolsSimd_OPNS (VectS32) operator << (fstb_ToolsSimd_OPNS (VectS32) x, int scalar)
{
	assert (scalar >= 0);
#if ! defined (fstb_HAS_SIMD)
	return fstb_ToolsSimd_OPNS (VectS32) { {
		x._ [0] << scalar,
		x._ [1] << scalar,
		x._ [2] << scalar,
		x._ [3] << scalar
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_slli_epi32 (x, scalar);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vshlq_n_s32 (x, scalar);
#endif // ff_arch_CPU
}



fstb_ToolsSimd_OPNS_END

#endif // Operators

#undef fstb_ToolsSimd_OPNS_BEG
#undef fstb_ToolsSimd_OPNS
#undef fstb_ToolsSimd_OPNS_END


#endif   // fstb_ToolsSimd_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
