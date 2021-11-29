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
	std::tie (tmp0, tmp1) = Vf32::interleave (a0, a1);
	std::tie (tmp2, tmp3) = Vf32::interleave (a2, a3);
	a0 = Vf32::interleave_pair_lo (tmp0, tmp2);
	a1 = Vf32::interleave_pair_hi (tmp0, tmp2);
	a2 = Vf32::interleave_pair_lo (tmp1, tmp3);
	a3 = Vf32::interleave_pair_hi (tmp1, tmp3);
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
#else
	// Generic form
	Vf32           k0, k1, k2, k3;
	std::tie (k0, k1) = Vf32::interleave (a0, a2);
	std::tie (k2, k3) = Vf32::interleave (a1, a3);
	std::tie (a0, a1) = Vf32::interleave (k0, k2);
	std::tie (a2, a3) = Vf32::interleave (k1, k3);
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



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace fstb



#endif   // fstb_ToolsSimd_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
