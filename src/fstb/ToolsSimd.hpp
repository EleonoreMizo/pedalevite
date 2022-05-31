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
#if ! defined (fstb_HAS_SIMD)
# include <cstring>
#endif // fstb_HAS_SIMD



namespace fstb
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Vf32	ToolsSimd::cast_f32 (Vs32 x) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	Vf32           result;
	memcpy (&result, &x, sizeof (x));
	return result;
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_castsi128_ps (x);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_f32_s32 (x);
#endif
}



Vs32	ToolsSimd::cast_s32 (Vf32 x) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	Vs32           result;
	memcpy (&result, &x, sizeof (x));
	return result;
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
	return Vs32 {
		conv_int_fast (x._x [0]),
		conv_int_fast (x._x [1]),
		conv_int_fast (x._x [2]),
		conv_int_fast (x._x [3])
	};
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
	return Vs32 {
		round_int (x._x [0]),
		round_int (x._x [1]),
		round_int (x._x [2]),
		round_int (x._x [3])
	};
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
	return Vs32 {
		floor_int (x._x [0]),
		floor_int (x._x [1]),
		floor_int (x._x [2]),
		floor_int (x._x [3])
	};
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
	return Vf32 {
		float (x._x [0]),
		float (x._x [1]),
		float (x._x [2]),
		float (x._x [3])
	};
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
	return Vs32 {
		x._x [0] >> N,
		x._x [1] >> N,
		x._x [2] >> N,
		x._x [3] >> N
	};
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
	return Vs32 {
		int32_t (uint32_t (x._x [0]) >> N),
		int32_t (uint32_t (x._x [1]) >> N),
		int32_t (uint32_t (x._x [2]) >> N),
		int32_t (uint32_t (x._x [3]) >> N)
	};
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
	return Vs32 {
		x._x [0] << N,
		x._x [1] << N,
		x._x [2] << N,
		x._x [3] << N
	};
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
