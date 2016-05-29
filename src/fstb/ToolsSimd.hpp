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
	assert (ptr != 0);

#if fstb_IS (ARCHI, X86)
	return _mm_loadu_ps (reinterpret_cast <const float *> (ptr));
#elif fstb_IS (ARCHI, ARM)
	return vreinterpretq_f32_u8 (
		vld1q_u8 (reinterpret_cast <const uint8_t *> (ptr))
	);
#endif // ff_arch_CPU
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



ToolsSimd::VectF32	ToolsSimd::loadu_2f32 (const void *ptr)
{
	assert (ptr != 0);

#if fstb_IS (ARCHI, X86)
	const auto     x_0 = _mm_load_ss (reinterpret_cast <const float *> (ptr)    );
	const auto     x_1 = _mm_load_ss (reinterpret_cast <const float *> (ptr) + 1);
	const auto     x   = _mm_unpacklo_ps (x_0, x_1);
#elif fstb_IS (ARCHI, ARM)
	const float32x2_t x = vreinterpret_f32_u8 (
		vld1_u8 (reinterpret_cast <const uint8_t *> (ptr))
	);
	return vcombine_f32 (x, x);
#endif // ff_arch_CPU
}



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



ToolsSimd::VectF32	ToolsSimd::set_2f32 (float a0, float a1)
{
#if fstb_IS (ARCHI, X86)
	return _mm_unpacklo_ps (_mm_set_ss (a0), _mm_set_ss (a1));
#elif fstb_IS (ARCHI, ARM)
	return vsetq_lane_f32 (a1, vdupq_n_f32 (a0), 1);
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
