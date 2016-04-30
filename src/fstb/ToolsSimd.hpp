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



ToolsSimd::VectF32	ToolsSimd::loadu_f32 (const void *ptr)
{
	assert (is_ptr_align_nz (ptr, 16));

#if fstb_IS (ARCHI, X86)
	return _mm_loadu_ps (reinterpret_cast <const float *> (ptr));
#elif fstb_IS (ARCHI, ARM)
	return vreinterpretq_u8_f32 (
		vld1q_u8 (reinterpret_cast <const uint8_t *> (ptr))
	);
#endif // ff_arch_CPU
}



void	ToolsSimd::storeu_f32 (void *ptr, VectF32 v)
{
	assert (is_ptr_align_nz (ptr, 16));

#if fstb_IS (ARCHI, X86)
	_mm_storeu_ps (reinterpret_cast <float *> (ptr), v);
#elif fstb_IS (ARCHI, ARM)
	vst1q_u8 (reinterpret_cast <uint8_t *> (ptr), vreinterpretq_f32_u8 (v));
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



ToolsSimd::VectF32	ToolsSimd::set1_f32 (float a)
{
#if fstb_IS (ARCHI, X86)
	return _mm_set1_ps (a);
#elif fstb_IS (ARCHI, ARM)
	return vdupq_n_f32 (a);
#endif // ff_arch_CPU
}



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



void	ToolsSimd::mac (VectF32 &s, VectF32 a, VectF32 b)
{
#if fstb_IS (ARCHI, X86)
	s = _mm_add_ps (s, _mm_mul_ps (a, b));
#elif fstb_IS (ARCHI, ARM)
	s = vmlaq_f32 (s, a, b);
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



template <int SHIFT>
ToolsSimd::VectF32	ToolsSimd::rotate (VectF32 a)
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
	int32x4_t      aa = vreinterpretq_f32_i32 (a);
	switch (SHIFT & 3)
	{
	case 1:  aa = vextq_s32 (aa, aa, 3);
	case 2:  aa = vextq_s32 (aa, aa, 2);
	case 3:  aa = vextq_s32 (aa, aa, 1);
	default: aa = aa;
	}
	return vreinterpretq_i32_f32 (aa);
#endif // ff_arch_CPU
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



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
