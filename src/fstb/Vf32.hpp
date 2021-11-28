/*****************************************************************************

        Vf32.hpp
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_VectF32_CODEHEADER_INCLUDED)
#define fstb_VectF32_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include  "fstb/fnc.h"

#include <algorithm>

#include <cassert>
#include <cmath>



namespace fstb
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// Initialises with a | a | a | a
Vf32::Vf32 (Scalar a) noexcept
#if ! defined (fstb_HAS_SIMD)
:	_x { a, a, a, a }
#elif fstb_ARCHI == fstb_ARCHI_X86
:	_x { _mm_set1_ps (a) }
#elif fstb_ARCHI == fstb_ARCHI_ARM
:	_x { vdupq_n_f32 (a) }
#endif // fstb_ARCHI
{
	// Nothing
}



// Initialises with a | a | a | a
Vf32::Vf32 (double a) noexcept
#if ! defined (fstb_HAS_SIMD)
:	_x { Scalar (a), Scalar (a), Scalar (a), Scalar (a) }
#elif fstb_ARCHI == fstb_ARCHI_X86
:	_x { _mm_set1_ps (Scalar (a)) }
#elif fstb_ARCHI == fstb_ARCHI_ARM
:	_x { vdupq_n_f32 (Scalar (a)) }
#endif // fstb_ARCHI
{
	// Nothing
}



// Initialises with a | a | a | a
Vf32::Vf32 (int a) noexcept
#if ! defined (fstb_HAS_SIMD)
:	_x { Scalar (a), Scalar (a), Scalar (a), Scalar (a) }
#elif fstb_ARCHI == fstb_ARCHI_X86
:	_x { _mm_set1_ps (Scalar (a)) }
#elif fstb_ARCHI == fstb_ARCHI_ARM
:	_x { vdupq_n_f32 (Scalar (a)) }
#endif // fstb_ARCHI
{
	// Nothing
}



// Initialises with a0 | a1 | a2 | a3
Vf32::Vf32 (Scalar a0, Scalar a1, Scalar a2, Scalar a3) noexcept
#if ! defined (fstb_HAS_SIMD)
:	_x { a0, a1, a2, a3 }
#elif fstb_ARCHI == fstb_ARCHI_X86
:	_x { _mm_set_ps (a3, a2, a1, a0) }
#elif fstb_ARCHI == fstb_ARCHI_ARM
:	_x { a0, a1, a2, a3 }
#endif // fstb_ARCHI
{
	// Nothing
}



template <typename MEM>
void	Vf32::store (MEM *ptr) const noexcept
{
	assert (is_ptr_align_nz (ptr, 16));

#if ! defined (fstb_HAS_SIMD)
	*reinterpret_cast <Vf32Native *> (ptr) = _x;
#elif fstb_ARCHI == fstb_ARCHI_X86
	_mm_store_ps (reinterpret_cast <float *> (ptr), _x);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	vst1q_f32 (reinterpret_cast <float32_t *> (ptr), _x);
#endif // fstb_ARCHI
}



// n = number of scalars to store (from the LSB)
template <typename MEM>
void	Vf32::store_part (MEM *ptr, int n) const noexcept
{
	assert (n > 0);

	if (n >= 4)
	{
		store (ptr);
	}
	else
	{
		storeu_part_n13 (ptr, n);
	}
}



template <typename MEM>
void	Vf32::storeu (MEM *ptr) const noexcept
{
	assert (ptr != nullptr);

#if ! defined (fstb_HAS_SIMD)
	*reinterpret_cast <Vf32Native *> (ptr) = _x;
#elif fstb_ARCHI == fstb_ARCHI_X86
	_mm_storeu_ps (reinterpret_cast <float *> (ptr), _x);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	vst1q_u8 (reinterpret_cast <uint8_t *> (ptr), vreinterpretq_u8_f32 (_x));
#endif // fstb_ARCHI
}



// n = number of scalars to store (from the LSB)
template <typename MEM>
void	Vf32::storeu_part (MEM *ptr, int n) const noexcept
{
	assert (n > 0);

	if (n >= 4)
	{
		storeu (ptr);
		return;
	}

	storeu_part_n13 (ptr, n);
}



// ptr [0] = v0
// ptr [1] = v1
template <typename MEM>
void	Vf32::storeu_pair (MEM *ptr) const noexcept
{
	assert (ptr != nullptr);

#if ! defined (fstb_HAS_SIMD)
	auto           p = reinterpret_cast <float *> (ptr);
	p [0] = _x [0];
	p [1] = _x [1];
#elif fstb_ARCHI == fstb_ARCHI_X86
	_mm_store_ss (reinterpret_cast <float *> (ptr)    , _x );
	const auto     v1 = _mm_shuffle_ps (_x, _x, 1 << 0);
	_mm_store_ss (reinterpret_cast <float *> (ptr) + 1, v1);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	vst1_u8 (
		reinterpret_cast <uint8_t *> (ptr),
		vreinterpret_u8_f32 (vget_low_f32 (_x))
	);
#endif // fstb_ARCHI
}



// *ptr = v0
template <typename MEM>
void	Vf32::storeu_scalar (MEM *ptr) const noexcept
{
	assert (ptr != nullptr);

#if ! defined (fstb_HAS_SIMD)
	reinterpret_cast <float *> (ptr) [0] = _x [0];
#elif fstb_ARCHI == fstb_ARCHI_X86
	_mm_store_ss (reinterpret_cast <float *> (ptr), _x);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	vst1q_lane_f32 (reinterpret_cast <float32_t *> (ptr), _x, 0);
#endif // fstb_ARCHI
}



// Works only with well-formed condition results (tested bits depend on the
// implementation).
// For each scalar, true = all bits set, false = all bits cleared
Vf32::operator bool () const noexcept
{
	return and_h ();
}



Vf32 &	Vf32::operator += (const Vf32Native &other) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	_x [0] += other [0];
	_x [1] += other [1];
	_x [2] += other [2];
	_x [3] += other [3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	_x = _mm_add_ps (_x, other);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	_x = vaddq_f32 (_x, other);
#endif // fstb_ARCHI
	return *this;
}



Vf32 &	Vf32::operator -= (const Vf32Native &other) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	_x [0] -= other [0];
	_x [1] -= other [1];
	_x [2] -= other [2];
	_x [3] -= other [3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	_x = _mm_sub_ps (_x, other);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	_x = vsubq_f32 (_x, other);
#endif // fstb_ARCHI
	return *this;
}



Vf32 &	Vf32::operator *= (const Vf32Native &other) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	_x [0] *= other [0];
	_x [1] *= other [1];
	_x [2] *= other [2];
	_x [3] *= other [3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	_x = _mm_mul_ps (_x, other);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	_x = vmulq_f32 (_x, other);
#endif // fstb_ARCHI
	return *this;
}



Vf32 &	Vf32::operator /= (const Vf32Native &other) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	_x [0] /= other [0];
	_x [1] /= other [1];
	_x [2] /= other [2];
	_x [3] /= other [3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	_x = _mm_div_ps (_x, other);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	_x = _x * (Vf32 { other }.rcp_approx2 ())._x;
#endif // fstb_ARCHI
	return *this;
}



Vf32 &	Vf32::operator &= (const Vf32Native &other) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	Combo          al { _x };
	Combo          ar { other };
	al._s32 [0] &= ar._s32 [0];
	al._s32 [1] &= ar._s32 [1];
	al._s32 [2] &= ar._s32 [2];
	al._s32 [3] &= ar._s32 [3];
	_x = al._vf32;
#elif fstb_ARCHI == fstb_ARCHI_X86
	_x = _mm_and_ps (_x, other);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	_x = vreinterpretq_f32_u32 (vandq_u32 (
		vreinterpretq_u32_f32 (_x),
		vreinterpretq_u32_f32 (other)
	));
#endif // fstb_ARCHI
	return *this;
}



Vf32 &	Vf32::operator |= (const Vf32Native &other) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	Combo          al { _x };
	Combo          ar { other };
	al._s32 [0] |= ar._s32 [0];
	al._s32 [1] |= ar._s32 [1];
	al._s32 [2] |= ar._s32 [2];
	al._s32 [3] |= ar._s32 [3];
	_x = al._vf32;
#elif fstb_ARCHI == fstb_ARCHI_X86
	_x = _mm_or_ps (_x, other);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	_x = vreinterpretq_f32_u32 (vorrq_u32 (
		vreinterpretq_u32_f32 (_x),
		vreinterpretq_u32_f32 (other)
	));
#endif // fstb_ARCHI
	return *this;
}



Vf32 &	Vf32::operator ^= (const Vf32Native &other) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	Combo          al { _x };
	Combo          ar { other };
	al._s32 [0] ^= ar._s32 [0];
	al._s32 [1] ^= ar._s32 [1];
	al._s32 [2] ^= ar._s32 [2];
	al._s32 [3] ^= ar._s32 [3];
	_x = al._vf32;
#elif fstb_ARCHI == fstb_ARCHI_X86
	_x = _mm_xor_ps (_x, other);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	_x = vreinterpretq_f32_u32 (veorq_u32 (
		vreinterpretq_u32_f32 (_x),
		vreinterpretq_u32_f32 (other)
	));
#endif // fstb_ARCHI
	return *this;
}



// *this += a * b
Vf32 &	Vf32::mac (Vf32 a, Vf32 b) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	_x [0] += a._x [0] * b._x [0];
	_x [1] += a._x [1] * b._x [1];
	_x [2] += a._x [2] * b._x [2];
	_x [3] += a._x [3] * b._x [3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	_x = _mm_add_ps (_x, _mm_mul_ps (a, b));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	#if defined (__ARM_FEATURE_FMA)
	_x = vfmaq_f32 (_x, a, b);
	#else
	_x = vmlaq_f32 (_x, a, b);
	#endif
#endif // fstb_ARCHI
	return *this;
}



// *this -= a * b
Vf32 &	Vf32::msu (Vf32 a, Vf32 b) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	_x [0] -= a._x [0] * b._x [0];
	_x [1] -= a._x [1] * b._x [1];
	_x [2] -= a._x [2] * b._x [2];
	_x [3] -= a._x [3] * b._x [3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	_x = _mm_sub_ps (_x, _mm_mul_ps (a, b));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	#if defined (__ARM_FEATURE_FMA)
	_x = vfmsq_f32 (_x, a, b);
	#else
	_x = vmlsq_f32 (_x, a, b);
	#endif
#endif // fstb_ARCHI
	return *this;
}



Vf32	Vf32::reverse () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return { { _x [3], _x [2], _x [1], _x [0]	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_shuffle_ps (_x, _x, (3<<0) + (2<<2) + (1<<4) + (0<<6));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vrev64q_f32 (vcombine_f32 (vget_high_f32 (_x), vget_low_f32 (_x)));
#endif // fstb_ARCHI
}



// Assumes "to nearest" rounding mode on x86
Vf32	Vf32::round () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 { {
		roundf (_x [0]),
		roundf (_x [1]),
		roundf (_x [2]),
		roundf (_x [3])
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_cvtepi32_ps (_mm_cvtps_epi32 (_x));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const auto     zero = vdupq_n_f32 ( 0.0f);
	const auto     m    = vdupq_n_f32 (-0.5f);
	const auto     p    = vdupq_n_f32 (+0.5f);
	const auto     gt0  = vcgtq_f32 (_x, zero);
	const auto     u    = vbslq_f32 (gt0, p, m);
	return vaddq_f32 (_x, u);
#endif // fstb_ARCHI
}



Vf32	Vf32::rcp_approx () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return { {
		1.f / _x [0],
		1.f / _x [1],
		1.f / _x [2],
		1.f / _x [3]
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_rcp_ps (_x);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	auto           r = vrecpeq_f32 (_x);
	r = vmulq_f32 (vrecpsq_f32 (_x, r), r);
	return r;
#endif // fstb_ARCHI
}



// With more accuracy
Vf32	Vf32::rcp_approx2 () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return rcp_approx ();
#elif fstb_ARCHI == fstb_ARCHI_X86
	auto           r = _mm_rcp_ps (_x);
	r = r * (_mm_set1_ps (2.f) - r * _x);
	return r;
#elif fstb_ARCHI == fstb_ARCHI_ARM
	auto           r = vrecpeq_f32 (_x);
	r = vmulq_f32 (vrecpsq_f32 (_x, r), r);
	r = vmulq_f32 (vrecpsq_f32 (_x, r), r);
	return r;
#endif // fstb_ARCHI
}



Vf32	Vf32::signbit () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return { {
		copysignf (0.f, _x [0]),
		copysignf (0.f, _x [1]),
		copysignf (0.f, _x [2]),
		copysignf (0.f, _x [3])
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_and_ps (signbit_mask (), _x);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_f32_u32 (vandq_u32 (
		vreinterpretq_u32_f32 (_x),
		vdupq_n_u32 (0x80000000U)
	));
#endif // fstb_ARCHI
}



Vf32   Vf32::is_lt_0 () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	Combo          r;
	r._s32 [0] = (_x [0] < 0) ? -1 : 0;
	r._s32 [1] = (_x [1] < 0) ? -1 : 0;
	r._s32 [2] = (_x [2] < 0) ? -1 : 0;
	r._s32 [3] = (_x [3] < 0) ? -1 : 0;
	return r._vf32;
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_castsi128_ps (_mm_srai_epi32 (_mm_castps_si128 (_x), 31));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_f32_s32 (vshrq_n_s32 (vreinterpretq_s32_f32 (_x), 31));
#endif // fstb_ARCHI
}



std::tuple <float, float, float, float>	Vf32::explode () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return std::make_tuple (_x [0], _x [1], _x [2], _x [3]);
#elif fstb_ARCHI == fstb_ARCHI_X86
	const auto     tmp = _mm_movehl_ps (_x, _x);
	return std::make_tuple (
		_mm_cvtss_f32 (_x),
		_mm_cvtss_f32 (_mm_shuffle_ps (_x, _x, (1<<0))),
		_mm_cvtss_f32 (tmp),
		_mm_cvtss_f32 (_mm_shuffle_ps (tmp, tmp, (1<<0)))
	);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return std::make_tuple (
		vgetq_lane_f32 (_x, 0),
		vgetq_lane_f32 (_x, 1),
		vgetq_lane_f32 (_x, 2),
		vgetq_lane_f32 (_x, 3)
	);
#endif // fstb_ARCHI
}



std::tuple <float, float>	Vf32::extract_pair () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return std::make_tuple (_x [0], _x [1]);
#elif fstb_ARCHI == fstb_ARCHI_X86
	return std::make_tuple (
		_mm_cvtss_f32 (_x),
		_mm_cvtss_f32 (_mm_shuffle_ps (_x, _x, 1))
	);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return std::make_tuple (vgetq_lane_f32 (_x, 0), vgetq_lane_f32 (_x, 1));
#endif // fstb_ARCHI
}



// <0> = v0 | v1 | v0 | v1
// <1> = v2 | v3 | v2 | v3
std::tuple <Vf32, Vf32>	Vf32::spread_pair () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return std::make_tuple (
		Vf32 { _x [0], _x [1], _x [0], _x [1] },
		Vf32 { _x [2], _x [3], _x [2], _x [3] }
	);
#elif fstb_ARCHI == fstb_ARCHI_X86
	return std::make_tuple (
		Vf32 { _mm_shuffle_ps (_x, _x, (0<<0) + (1<<2) + (0<<4) + (1<<6)) },
		Vf32 { _mm_shuffle_ps (_x, _x, (2<<0) + (3<<2) + (2<<4) + (3<<6)) }
	);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const float32x2_t v01 = vget_low_f32 (_x);
	const float32x2_t v23 = vget_high_f32 (_x);
	return std::make_tuple (
		Vf32 { vcombine_f32 (v01, v01) },
		Vf32 { vcombine_f32 (v23, v23) }
	);
#endif // fstb_ARCHI
}



float	Vf32::sum_h () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return (_x [0] + _x [2]) + (_x [1] + _x [3]);
#elif fstb_ARCHI == fstb_ARCHI_X86
	// s = v3,v2,v1,v0
	const auto s = _mm_shuffle_ps (_x, _x, (3 << 0) | (2 << 2) | (1 << 4) | (0 << 6));
	const auto v = _mm_add_ps (_x, s); // v0+v3,v1+v2,v2+v1,v3+v0
	return _mm_cvtss_f32 (_mm_add_ss (v, _mm_movehl_ps (s, v)));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	#if fstb_WORD_SIZE == 64
		return vaddvq_f32 (_x);
	#else
		float32x2_t    v2 = vadd_f32 (vget_high_f32 (_x), vget_low_f32 (_x));
		return vget_lane_f32 (vpadd_f32 (v2, v2), 0);
	#endif
#endif // fstb_ARCHI
}



float	Vf32::min_h () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return std::min (std::min (_x [0], _x [2]), std::min (_x [1], _x [3]));
#elif fstb_ARCHI == fstb_ARCHI_X86
	const auto     v = _mm_min_ps (_x, _mm_shuffle_ps (_x, _x, (3 << 2) | 2));
	return _mm_cvtss_f32 (_mm_min_ss (v, _mm_shuffle_ps (v, v, 1)));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	float32x2_t    v2 = vmin_f32 (vget_high_f32 (_x), vget_low_f32 (_x));
	return vget_lane_f32 (vpmin_f32 (v2, v2), 0);
#endif // fstb_ARCHI
}



float	Vf32::max_h () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return std::max (std::max (_x [0], _x [2]), std::max (_x [1], _x [3]));
#elif fstb_ARCHI == fstb_ARCHI_X86
	const auto     v = _mm_max_ps (_x, _mm_shuffle_ps (_x, _x, (3 << 2) | 2));
	return _mm_cvtss_f32 (_mm_max_ss (v, _mm_shuffle_ps (v, v, 1)));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	float32x2_t    v2 = vmax_f32 (vget_high_f32 (_x), vget_low_f32 (_x));
	return vget_lane_f32 (vpmax_f32 (v2, v2), 0);
#endif // fstb_ARCHI
}



// Works only with well-formed condition results (tested bits depends on the implementation).
// For each scalar, true = all bits set, false = all bits cleared
bool   Vf32::and_h () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	const Combo    c { _x };
	const int32_t  t = (c._s32 [0] & c._s32 [1]) & (c._s32 [2] & c._s32 [3]);
	return (t == -1);
#elif fstb_ARCHI == fstb_ARCHI_X86
	return (_mm_movemask_ps (_x) == 15);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const uint32x2_t  tmp = vreinterpret_u32_u16 (
		vqmovn_u32 (vreinterpretq_u32_f32 (_x))
	);
	return (   vget_lane_u32 (tmp, 0) == 0xFFFFFFFFU
	        && vget_lane_u32 (tmp, 1) == 0xFFFFFFFFU);
#endif // fstb_ARCHI
}



// Works only with well-formed condition results (tested bits depends on the implementation).
// For each scalar, true = all bits set, false = all bits cleared
bool   Vf32::or_h () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	Combo          c;
	c._vf32 = _x;
	const int32_t  t = (c._s32 [0] | c._s32 [1]) | (c._s32 [2] | c._s32 [3]);
	return (t != 0);
#elif fstb_ARCHI == fstb_ARCHI_X86
	return (_mm_movemask_ps (_x) != 0);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const uint32x2_t  tmp = vreinterpret_u32_u16 (
		vqmovn_u32 (vreinterpretq_u32_f32 (_x))
	);
	return (   vget_lane_u32 (tmp, 0) != 0
	        || vget_lane_u32 (tmp, 1) != 0);
#endif // fstb_ARCHI
}



// Moves the boolean content of each 4 scalar into the lower 4 bits of the
// return value.
// Assumes the object is a result of a comparison, with all bits the same
// in each 32-bit element.
unsigned int	Vf32::movemask () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	Combo          c;
	c._vf32 = _x;
	return
		   (c._u32 [0] >> 31)
		| ((c._u32 [1] >> 30) & 2)
		| ((c._u32 [2] >> 29) & 4)
		| ((c._u32 [3] >> 28) & 8);
#elif fstb_ARCHI == fstb_ARCHI_X86
	return static_cast <unsigned int> (_mm_movemask_ps (_x));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	uint64x2_t     tmp1 =
		vreinterpretq_u64_f32 (_x);   // ddd...ddd ccc...ccc bbb...bbb aaa...aaa
	tmp1 = vshrq_n_u64 (tmp1, 31);   // 000...00d ddd...ddc 000...00b bbb...bba
	uint64x1_t     tmp2 = vsli_n_u64 (
		vget_high_u64 (tmp1),
		vget_low_u64 (tmp1),
		2
	);
	return vget_lane_u32 (vreinterpret_u32_u64 (tmp2), 0) & 0xF;
#endif // fstb_ARCHI
}



Vf32	Vf32::zero () noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 { { 0, 0, 0, 0 } };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_setzero_ps ();
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vdupq_n_f32 (0);
#endif // fstb_ARCHI
}



// Returns a0 | a1 | ? | ?
Vf32	Vf32::set_pair (float a0, float a1) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 { { a0, a1 } };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_unpacklo_ps (_mm_set_ss (a0), _mm_set_ss (a1));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vsetq_lane_f32 (a1, vdupq_n_f32 (a0), 1);
#endif // fstb_ARCHI
}



// Returns a02 | a13 | a02 | a13
Vf32	Vf32::set_pair_fill (float a02, float a13) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 { { a02, a13, a02, a13 } };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_unpacklo_ps (_mm_set1_ps (a02), _mm_set1_ps (a13));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const float32x2_t v01 = vset_lane_f32 (a13, vdup_n_f32 (a02), 1);
	return vcombine_f32 (v01, v01);
#endif // fstb_ARCHI
}



// Returns a01 | a01 | a23 | a23
Vf32	Vf32::set_pair_dbl (float a01, float a23) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 { { a01, a01, a23, a23 } };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_shuffle_ps (_mm_set_ss (a01), _mm_set_ss (a23), 0x00);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vcombine_f32 (vdup_n_f32 (a01), vdup_n_f32 (a23));
#endif // fstb_ARCHI
}



// "true" must be 1 and nothing else.
Vf32	Vf32::set_mask (bool m0, bool m1, bool m2, bool m3) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	Combo          c;
	c._s32 [0] = -int32_t (m0);
	c._s32 [1] = -int32_t (m1);
	c._s32 [2] = -int32_t (m2);
	c._s32 [3] = -int32_t (m3);
	return c._vf32;
#elif 1 // Fast version
# if fstb_ARCHI == fstb_ARCHI_X86
	return _mm_castsi128_ps (_mm_sub_epi32 (
		_mm_setzero_si128 (),
		_mm_set_epi32 (m3, m2, m1, m0)
	));
# elif fstb_ARCHI == fstb_ARCHI_ARM
	float32x2_t    v01 = vdup_n_f32 (m0);
	float32x2_t    v23 = vdup_n_f32 (m2);
	v01 = vset_lane_f32 (m1, v01, 1);
	v23 = vset_lane_f32 (m3, v23, 1);
	return vreinterpretq_f32_s32 (vnegq_s32 (vreinterpretq_s32_f32 (
		vcombine_f32 (v01, v23)
	)));
# endif // fstb_ARCHI
#else // Safer but slower version
# if fstb_ARCHI == fstb_ARCHI_X86
	return _mm_castsi128_ps (_mm_sub_epi32 (
		_mm_set_epi32 (!m3, !m2, !m1, !m0),
		_mm_set1_epi32 (1)
	));
# elif fstb_ARCHI == fstb_ARCHI_ARM
	float32x2_t    v01 = vdup_n_f32 (!m0);
	float32x2_t    v23 = vdup_n_f32 (!m2);
	v01 = vset_lane_f32 (!m1, v01, 1);
	v23 = vset_lane_f32 (!m3, v23, 1);
	const auto     one  = vdupq_n_s32 (1);
	return vreinterpretq_f32_s32 (vsubq_s32 (
		vreinterpretq_s32_f32 (vcombine_f32 (v01, v23)),
		one
	));
# endif // fstb_ARCHI
#endif // Versions
}



Vf32Native	Vf32::signbit_mask () noexcept
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
#endif // fstb_ARCHI
}



template <typename MEM>
Vf32	Vf32::load (const MEM *ptr) noexcept
{
	assert (is_ptr_align_nz (ptr, 16));

#if ! defined (fstb_HAS_SIMD)
	return *reinterpret_cast <const Vf32 *> (ptr);
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_load_ps (reinterpret_cast <const float *> (ptr));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vld1q_f32 (reinterpret_cast <const float32_t *> (ptr));
#endif // fstb_ARCHI
}



template <typename MEM>
Vf32	Vf32::loadu (const MEM *ptr) noexcept
{
	assert (ptr != nullptr);

#if ! defined (fstb_HAS_SIMD)
	return *reinterpret_cast <const Vf32 *> (ptr);
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_loadu_ps (reinterpret_cast <const float *> (ptr));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_f32_u8 (
		vld1q_u8 (reinterpret_cast <const uint8_t *> (ptr))
	);
#endif // fstb_ARCHI
}



template <typename MEM>
Vf32	Vf32::loadu_part (const MEM *ptr, int n) noexcept
{
	assert (n > 0);

	if (n >= 4)
	{
		return loadu (ptr);
	}

	const float *  f_ptr = reinterpret_cast <const float *> (ptr);
#if ! defined (fstb_HAS_SIMD)
	Vf32           v;
	v._x [0] = f_ptr [0];
	for (int i = 1; i < n; ++i)
	{
		v._x [i] = f_ptr [i];
	}
	return v;
#elif fstb_ARCHI == fstb_ARCHI_X86
	switch (n)
	{
	case 1:
		return _mm_load_ss (f_ptr);
	case 2:
# if 1
		return _mm_castsi128_ps (_mm_loadl_epi64 (
			reinterpret_cast <const __m128i *> (ptr)
		));
# else // Higher latency from Skylake
		return _mm_unpacklo_ps (_mm_load_ss (f_ptr), _mm_load_ss (f_ptr + 1));
# endif
	case 3:
		return _mm_shuffle_ps (
# if 1
			_mm_castsi128_ps (_mm_loadl_epi64 (
				reinterpret_cast <const __m128i *> (ptr)
			)),
# else // Higher latency from Skylake
			_mm_unpacklo_ps (_mm_load_ss (f_ptr), _mm_load_ss (f_ptr + 1)),
# endif
			_mm_load_ss (f_ptr + 2),
			(0<<0) + (1<<2) + (2<<4)
		);
	default:
		// Keeps the compiler happy with (un)initialisation
		return loadu (ptr);
	}
#elif fstb_ARCHI == fstb_ARCHI_ARM
	auto           v = vmovq_n_f32 (f_ptr [0]);
	if (n >= 2)
	{
		v = vld1q_lane_f32 (f_ptr + 1, v, 1);
		if (n >= 3)
		{
			v = vld1q_lane_f32 (f_ptr + 2, v, 2);
		}
	}
	return v;
#endif // fstb_ARCHI
}



// Returns: ptr [0] | ptr [1] | ? | ?
template <typename MEM>
Vf32	Vf32::loadu_pair (const MEM *ptr) noexcept
{
	assert (ptr != nullptr);

#if ! defined (fstb_HAS_SIMD)
	auto           p = reinterpret_cast <const float *> (ptr);
	return Vf32 { { p [0], p [1] } };
#elif fstb_ARCHI == fstb_ARCHI_X86
# if 1
	return _mm_castsi128_ps (_mm_loadl_epi64 (
		reinterpret_cast <const __m128i *> (ptr)
	));
# else // Higher latency from Skylake
	const auto     x0 = _mm_load_ss (reinterpret_cast <const float *> (ptr)    );
	const auto     x1 = _mm_load_ss (reinterpret_cast <const float *> (ptr) + 1);
	return _mm_unpacklo_ps (x0, x1);
# endif
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const float32x2_t x = vreinterpret_f32_u8 (
		vld1_u8 (reinterpret_cast <const uint8_t *> (ptr))
	);
	return vcombine_f32 (x, x);
#endif // fstb_ARCHI
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// n = number of scalars to store (from the LSB)
template <typename MEM>
void	Vf32::storeu_part_n13 (MEM *ptr, int n) const noexcept
{
	assert (n > 0);
	assert (n < 4);

	float *        f_ptr = reinterpret_cast <float *> (ptr);

#if ! defined (fstb_HAS_SIMD)

	for (int i = 0; i < n; ++i)
	{
		f_ptr [i] = _x [i];
	}

#elif fstb_ARCHI == fstb_ARCHI_X86

	_mm_store_ss (f_ptr, _x);
	if (n >= 2)
	{
		_mm_store_ss (f_ptr + 1, _mm_shuffle_ps (_x, _x, 1 << 0));
		if (n >= 3)
		{
			_mm_store_ss (f_ptr + 2, _mm_movehl_ps (_x, _x));
		}
	}

#elif fstb_ARCHI == fstb_ARCHI_ARM

	vst1q_lane_f32 (f_ptr + 0, _x, 0);
	if (n >= 2)
	{
		vst1q_lane_f32 (f_ptr + 1, _x, 1);
		if (n >= 3)
		{
			vst1q_lane_f32 (f_ptr + 2, _x, 2);
		}
	}

#endif
}



/*\\\ GLOBAL OPERATORS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Vf32 operator + (Vf32 lhs, const Vf32 &rhs) noexcept
{
	lhs += rhs;
	return lhs;
}

Vf32 operator - (Vf32 lhs, const Vf32 &rhs) noexcept
{
	lhs -= rhs;
	return lhs;
}

Vf32 operator * (Vf32 lhs, const Vf32 &rhs) noexcept
{
	lhs *= rhs;
	return lhs;
}

Vf32 operator / (Vf32 lhs, const Vf32 &rhs) noexcept
{
	lhs /= rhs;
	return lhs;
}

Vf32 operator & (Vf32 lhs, const Vf32 &rhs) noexcept
{
	lhs &= rhs;
	return lhs;
}

Vf32 operator | (Vf32 lhs, const Vf32 &rhs) noexcept
{
	lhs |= rhs;
	return lhs;
}

Vf32 operator ^ (Vf32 lhs, const Vf32 &rhs) noexcept
{
	lhs ^= rhs;
	return lhs;
}



Vf32	operator == (const Vf32 &lhs, const Vf32 &rhs) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	Vf32::Combo    r;
	r._s32 [0] = (lhs._x [0] == rhs._x [0]) ? -1 : 0;
	r._s32 [1] = (lhs._x [1] == rhs._x [1]) ? -1 : 0;
	r._s32 [2] = (lhs._x [2] == rhs._x [2]) ? -1 : 0;
	r._s32 [3] = (lhs._x [3] == rhs._x [3]) ? -1 : 0;
	return r._vf32;
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_cmpeq_ps (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_f32_u32 (vceqq_f32 (lhs, rhs));
#endif // fstb_ARCHI
}



Vf32	operator != (const Vf32 &lhs, const Vf32 &rhs) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	Vf32::Combo    r;
	r._s32 [0] = (lhs._x [0] != rhs._x [0]) ? -1 : 0;
	r._s32 [1] = (lhs._x [1] != rhs._x [1]) ? -1 : 0;
	r._s32 [2] = (lhs._x [2] != rhs._x [2]) ? -1 : 0;
	r._s32 [3] = (lhs._x [3] != rhs._x [3]) ? -1 : 0;
	return r._vf32;
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_cmpneq_ps (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_f32_u32 (vmvnq_u32 (vceqq_f32 (lhs, rhs)));
#endif // fstb_ARCHI
}



Vf32	operator <  (const Vf32 &lhs, const Vf32 &rhs) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	Vf32::Combo    r;
	r._s32 [0] = (lhs._x [0] < rhs._x [0]) ? -1 : 0;
	r._s32 [1] = (lhs._x [1] < rhs._x [1]) ? -1 : 0;
	r._s32 [2] = (lhs._x [2] < rhs._x [2]) ? -1 : 0;
	r._s32 [3] = (lhs._x [3] < rhs._x [3]) ? -1 : 0;
	return r._vf32;
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_cmplt_ps (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_f32_u32 (vcltq_f32 (lhs, rhs));
#endif // fstb_ARCHI
}



Vf32	operator <= (const Vf32 &lhs, const Vf32 &rhs) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	Vf32::Combo    r;
	r._s32 [0] = (lhs._x [0] <= rhs._x [0]) ? -1 : 0;
	r._s32 [1] = (lhs._x [1] <= rhs._x [1]) ? -1 : 0;
	r._s32 [2] = (lhs._x [2] <= rhs._x [2]) ? -1 : 0;
	r._s32 [3] = (lhs._x [3] <= rhs._x [3]) ? -1 : 0;
	return r._vf32;
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_cmple_ps (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_f32_u32 (vcleq_f32 (lhs, rhs));
#endif // fstb_ARCHI
}



Vf32	operator >  (const Vf32 &lhs, const Vf32 &rhs) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	Vf32::Combo    r;
	r._s32 [0] = (lhs._x [0] > rhs._x [0]) ? -1 : 0;
	r._s32 [1] = (lhs._x [1] > rhs._x [1]) ? -1 : 0;
	r._s32 [2] = (lhs._x [2] > rhs._x [2]) ? -1 : 0;
	r._s32 [3] = (lhs._x [3] > rhs._x [3]) ? -1 : 0;
	return r._vf32;
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_cmpgt_ps (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_f32_u32 (vcgtq_f32 (lhs, rhs));
#endif // fstb_ARCHI
}



Vf32	operator >= (const Vf32 &lhs, const Vf32 &rhs) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	Vf32::Combo    r;
	r._s32 [0] = (lhs._x [0] >= rhs._x [0]) ? -1 : 0;
	r._s32 [1] = (lhs._x [1] >= rhs._x [1]) ? -1 : 0;
	r._s32 [2] = (lhs._x [2] >= rhs._x [2]) ? -1 : 0;
	r._s32 [3] = (lhs._x [3] >= rhs._x [3]) ? -1 : 0;
	return r._vf32;
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_cmpge_ps (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_f32_u32 (vcgeq_f32 (lhs, rhs));
#endif // fstb_ARCHI
}



Vf32 abs (const Vf32 &v) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 { {
		fabsf (v._x [0]),
		fabsf (v._x [1]),
		fabsf (v._x [2]),
		fabsf (v._x [3])
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_andnot_ps (Vf32::signbit_mask (), v);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vabsq_f32 (v);
#endif // fstb_ARCHI
}



Vf32 round (const Vf32 &v) noexcept
{
	return v.round ();
}



Vf32 min (const Vf32 &lhs, const Vf32 &rhs) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 { {
		std::min (lhs._x [0], rhs._x [0]),
		std::min (lhs._x [1], rhs._x [1]),
		std::min (lhs._x [2], rhs._x [2]),
		std::min (lhs._x [3], rhs._x [3])
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_min_ps (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vminq_f32 (lhs, rhs);
#endif // fstb_ARCHI
}



Vf32 max (const Vf32 &lhs, const Vf32 &rhs) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 { {
		std::max (lhs._x [0], rhs._x [0]),
		std::max (lhs._x [1], rhs._x [1]),
		std::max (lhs._x [2], rhs._x [2]),
		std::max (lhs._x [3], rhs._x [3])
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_max_ps (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vmaxq_f32 (lhs, rhs);
#endif // fstb_ARCHI
}



Vf32 limit (const Vf32 &v, const Vf32 &mi, const Vf32 &ma) noexcept
{
	return min (max (v, mi), ma);
}



}  // namespace fstb



#endif   // fstb_VectF32_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
