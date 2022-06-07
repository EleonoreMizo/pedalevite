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



#if ! defined (fstb_Vf32_CODEHEADER_INCLUDED)
#define fstb_Vf32_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include  "fstb/fnc.h"

#include <algorithm>

#include <cassert>
#include <cfloat>
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



// Initialises with a0 | a1 | a2 | a3
Vf32::Vf32 (const std::tuple <Scalar, Scalar, Scalar, Scalar> &a) noexcept
#if ! defined (fstb_HAS_SIMD)
:	_x { std::get <0> (a), std::get <1> (a), std::get <2> (a), std::get <3> (a) }
#elif fstb_ARCHI == fstb_ARCHI_X86
:	_x { _mm_set_ps (std::get <3> (a), std::get <2> (a), std::get <1> (a), std::get <0> (a)) }
#elif fstb_ARCHI == fstb_ARCHI_ARM
:	_x { std::get <0> (a), std::get <1> (a), std::get <2> (a), std::get <3> (a) }
#endif // fstb_ARCHI
{
	// Nothing
}



template <typename MEM>
void	Vf32::store (MEM *ptr) const noexcept
{
	assert (is_ptr_align_nz (ptr, fstb_SIMD128_ALIGN));

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



Vf32	Vf32::operator - () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 {
		-_x [0],
		-_x [1],
		-_x [2],
		-_x [3]
	};
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_xor_ps (_x, signbit_mask ());
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vnegq_f32 (_x);
#endif // fstb_ARCHI
}



Vf32	Vf32::reverse () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 { _x [3], _x [2], _x [1], _x [0] };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_shuffle_ps (_x, _x, (3<<0) + (2<<2) + (1<<4) + (0<<6));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vrev64q_f32 (vcombine_f32 (vget_high_f32 (_x), vget_low_f32 (_x)));
#endif // fstb_ARCHI
}



Vf32	Vf32::swap_pairs () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 { _x [2], _x [3], _x [0], _x [1] };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_shuffle_ps (_x, _x, (2<<0) + (3<<2) + (0<<4) + (1<<6));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const float32x2_t v01 = vget_low_f32 (_x);
	const float32x2_t v23 = vget_high_f32 (_x);
	return vcombine_f32 (v23, v01);
#endif // fstb_ARCHI
}



// a, b, c, d -> a, a, c, c
Vf32	Vf32::monofy_pairs_lo () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 { _x [0], _x [0], _x [2], _x [2] };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_shuffle_ps (_x, _x, 0xA0);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vuzpq_f32 (_x, _x).val [0];
#endif // fstb_ARCHI
}



// a, b, c, d -> b, b, d, d
Vf32	Vf32::monofy_pairs_hi () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 { _x [1], _x [1], _x [3], _x [3] };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_shuffle_ps (_x, _x, 0xF5);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vuzpq_f32 (_x, _x).val [1];
#endif // fstb_ARCHI
}



// a, b, c, d -> a+c, b+d, a-c, b-d
Vf32	Vf32::butterfly_w64 () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 {
		_x [0] + _x [2],
		_x [1] + _x [3],
		_x [0] - _x [2],
		_x [1] - _x [3]
	};
#elif fstb_ARCHI == fstb_ARCHI_X86
	const auto sign = _mm_castsi128_ps (_mm_setr_epi32 (0, 0, _sign32, _sign32));
	const auto x0   = _mm_shuffle_ps (_x, _x, (2<<0) + (3<<2) + (0<<4) + (1<<6)); // c, d, a, b
	const auto x1   = _mm_xor_ps (_x, sign); // a, b, -c, -d
	return x0 + x1;
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const auto sign = int32x4_t { 0, 0, _sign32, _sign32 };
	const auto x0   = vcombine_f32 (vget_high_f32 (_x), vget_low_f32 (_x)); // c, d, a, b
	const auto x1   = // a, b, -c, -d
		vreinterpretq_f32_s32 (veorq_s32 (vreinterpretq_s32_f32 (_x), sign));
	return x0 + x1;
#endif
}



// a, b, c, d -> a+b, a-b, c+d, c-d
Vf32	Vf32::butterfly_w32 () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 {
		_x [0] + _x [1],
		_x [0] + _x [1],
		_x [2] - _x [3],
		_x [2] - _x [3]
	};
#elif fstb_ARCHI == fstb_ARCHI_X86
	const auto sign = _mm_castsi128_ps (_mm_setr_epi32 (0, _sign32, 0, _sign32));
	const auto x0   = _mm_shuffle_ps (_x, _x, (1<<0) + (0<<2) + (3<<4) + (2<<6)); // b, a, d, c
	const auto x1   = _mm_xor_ps (_x, sign); // a, -b, c, -d
	return x0 + x1;
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const auto sign = int32x4_t { 0, _sign32, 0, _sign32 };
	const auto x0   = vrev64q_f32 (_x); // b, a, d, c
	const auto x1   = // a, -b, c, -d
		vreinterpretq_f32_s32 (veorq_s32 (vreinterpretq_s32_f32 (_x), sign));
	return x0 + x1;
#endif
}



// Positive = to the left, rotates towards the higher indexes
template <int SHIFT>
Vf32	Vf32::rotate () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 {
		_x [(0 - SHIFT) & 3],
		_x [(1 - SHIFT) & 3],
		_x [(2 - SHIFT) & 3],
		_x [(3 - SHIFT) & 3]
	};
#elif fstb_ARCHI == fstb_ARCHI_X86
	switch (SHIFT & 3)
	{
	case 1:  return _mm_shuffle_ps (_x, _x, (2<<6) | (1<<4) | (0<<2) | (3<<0));
	case 2:  return _mm_shuffle_ps (_x, _x, (1<<6) | (0<<4) | (3<<2) | (2<<0));
	case 3:  return _mm_shuffle_ps (_x, _x, (0<<6) | (3<<4) | (2<<2) | (1<<0));
	default: return *this;
	}
#elif fstb_ARCHI == fstb_ARCHI_ARM
	int32x4_t     aa = vreinterpretq_s32_f32 (_x);
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



template <int POS>
float	Vf32::extract () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return _x [POS & 3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	auto           a = _x;
	switch (POS & 3)
	{
	case 1:  a = _mm_shuffle_ps (a, a, 1);	break;
	case 2:  a = _mm_shuffle_ps (a, a, 2);	break;
	case 3:  a = _mm_shuffle_ps (a, a, 3);	break;
	default: /* Nothing */                 break;
	}
	return _mm_cvtss_f32 (a);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vgetq_lane_f32 (_x, POS & 3);
#endif // fstb_ARCHI
}



template <int POS>
Vf32	Vf32::insert (float val) const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	auto           a = *this;
	a._x [POS & 3] = val;
	return a;
#elif fstb_ARCHI == fstb_ARCHI_X86
	auto           a = rotate <(-POS) & 3> ();
	a._x = _mm_move_ss (a._x, _mm_set_ss (val));
	return a.template rotate <POS> ();
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vsetq_lane_f32 (val, _x, POS & 3);
#endif // fstb_ARCHI
}



template <int POS>
Vf32	Vf32::spread () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 (extract <POS> ());
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_shuffle_ps (_x, _x, 0x55 * (POS & 3));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vdupq_n_f32 (vgetq_lane_f32 (_x, POS & 3));
#endif // fstb_ARCHI
}



// Assumes "to nearest" rounding mode on x86
Vf32	Vf32::round () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 {
		roundf (_x [0]),
		roundf (_x [1]),
		roundf (_x [2]),
		roundf (_x [3])
	};
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_cvtepi32_ps (_mm_cvtps_epi32 (_x));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const auto     zero = vdupq_n_f32 ( 0.0f);
	const auto     m    = vdupq_n_f32 (-0.5f);
	const auto     p    = vdupq_n_f32 (+0.5f);
	const auto     gt0  = vcgtq_f32 (_x, zero);
	const auto     u    = vbslq_f32 (gt0, p, m);
	return vcvtq_f32_s32 (vcvtq_s32_f32 (vaddq_f32 (_x, u)));
#endif // fstb_ARCHI
}



Vf32	Vf32::rcp_approx () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 {
		1.f / _x [0],
		1.f / _x [1],
		1.f / _x [2],
		1.f / _x [3]
	};
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



Vf32	Vf32::div_approx (const Vf32 &d) const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 {
		_x [0] / d._x [0],
		_x [1] / d._x [1],
		_x [2] / d._x [2],
		_x [3] / d._x [3]
	};
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_div_ps (_x, d._x);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return _x * d.rcp_approx ()._x;
#endif // fstb_ARCHI
}



Vf32	Vf32::sqrt_approx () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 {
		sqrtf (_x [0]),
		sqrtf (_x [1]),
		sqrtf (_x [2]),
		sqrtf (_x [3])
	};
#elif fstb_ARCHI == fstb_ARCHI_X86
	// Zero and denormal values will produce INF with _mm_rsqrt_ps(), so
	// we need a mask.
	const __m128   z_flag  = _mm_cmplt_ps (_x, _mm_set1_ps (FLT_MIN));
	const __m128   rsqrt_a = _mm_rsqrt_ps (_x);
	const __m128   sqrt_a  = _mm_mul_ps (_x, rsqrt_a);
	const __m128   sqrt_m  = _mm_andnot_ps (z_flag, sqrt_a);
	return sqrt_m;
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const uint32x4_t  nz_flag = vtstq_u32 (
		vreinterpretq_u32_f32 (_x),
		vreinterpretq_u32_f32 (_x)
	);
	auto           rs      = vrsqrteq_f32 (_x);
	rs *= vrsqrtsq_f32 (rs * float32x4_t (_x), rs);
	const auto     sqrt_a  = rs * float32x4_t (_x);
	return vreinterpretq_f32_u32 (vandq_u32 (
		vreinterpretq_u32_f32 (sqrt_a),
		nz_flag
	));
#endif // fstb_ARCHI
}



Vf32	Vf32::rsqrt () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 {
		1.f / sqrtf (_x [0]),
		1.f / sqrtf (_x [1]),
		1.f / sqrtf (_x [2]),
		1.f / sqrtf (_x [3])
	};
#elif fstb_ARCHI == fstb_ARCHI_X86
	__m128         rs = _mm_rsqrt_ps (_x);
	rs = _mm_set1_ps (0.5f) * rs * (_mm_set1_ps (3) - __m128 (_x) * rs * rs);
	return rs;
#elif fstb_ARCHI == fstb_ARCHI_ARM
	auto           rs = vrsqrteq_f32 (_x);
	rs *= vrsqrtsq_f32 (rs * float32x4_t (_x), rs);
	rs *= vrsqrtsq_f32 (rs * float32x4_t (_x), rs);
	return rs;
#endif // fstb_ARCHI
}



Vf32	Vf32::rsqrt_approx () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	// Ref:
	// Jan Kadlec, http://rrrola.wz.cz/inv_sqrt.html, 2010
	const auto     xh = (*this) * Vf32 (0.703952253f);
	Combo          c { _x };
	c._s32 [0] = 0x5F1FFFF9 - (c._s32 [0] >> 1);
	c._s32 [1] = 0x5F1FFFF9 - (c._s32 [1] >> 1);
	c._s32 [2] = 0x5F1FFFF9 - (c._s32 [2] >> 1);
	c._s32 [3] = 0x5F1FFFF9 - (c._s32 [3] >> 1);
	auto           rs = Vf32 { c._vf32 };
	rs *= Vf32 (1.681914091f) - xh * rs * rs;
	return rs;
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_rsqrt_ps (_x);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	auto           rs = vrsqrteq_f32 (_x);
	rs *= vrsqrtsq_f32 (rs * float32x4_t (_x), rs);
	return rs;
#endif // fstb_ARCHI
}



// poly is a user-provided Vf32 log2 approximation from [1 ; 2[ to [0 ; 1[
template <typename P>
Vf32	Vf32::log2_base (P poly) const noexcept
{
	const int32_t  log2_sub = 127;

#if ! defined (fstb_HAS_SIMD)

	assert (
	      _x [0] > 0
		&& _x [1] > 0
		&& _x [2] > 0
		&& _x [3] > 0
	);
	Combo          c { _x };
	const int      x0 = c._s32 [0];
	const int      x1 = c._s32 [1];
	const int      x2 = c._s32 [2];
	const int      x3 = c._s32 [3];
	const Vf32     log2_int {
		float (((x0 >> 23) & 255) - log2_sub),
		float (((x1 >> 23) & 255) - log2_sub),
		float (((x2 >> 23) & 255) - log2_sub),
		float (((x3 >> 23) & 255) - log2_sub)
	};
	c._s32 [0] = (x0 & ~(255 << 23)) + (127 << 23);
	c._s32 [1] = (x1 & ~(255 << 23)) + (127 << 23);
	c._s32 [2] = (x2 & ~(255 << 23)) + (127 << 23);
	c._s32 [3] = (x3 & ~(255 << 23)) + (127 << 23);
	Vf32           part { c._vf32 };

#else // fstb_HAS_SIMD

#if fstb_ARCHI == fstb_ARCHI_X86

	// Extracts the exponent
	__m128i        xi = _mm_castps_si128 (_x);
	xi = _mm_srli_epi32 (xi, 23);
	const __m128i  l2_sub = _mm_set1_epi32 (log2_sub);
	xi = _mm_sub_epi32 (xi, l2_sub);
	const auto     log2_int = Vf32 { _mm_cvtepi32_ps (xi) };

#elif fstb_ARCHI == fstb_ARCHI_ARM

	int32x4_t      xi = vreinterpretq_s32_f32 (_x);
	xi = vshrq_n_s32 (xi, 23);
	const int32x4_t   l2_sub = vdupq_n_s32 (log2_sub);
	xi -= l2_sub;
	const auto     log2_int = Vf32 { vcvtq_f32_s32 (xi) };

#endif // fstb_ARCHI

	// Extracts the multiplicative part in [1 ; 2[
	const auto     mask_mantissa = Vf32 (1.17549421e-38f); // Binary: (1 << 23) - 1
	auto           part          = _x & mask_mantissa;
	const auto     bias          = Vf32 (1.0f);            // Binary: 127 << 23
	part |= bias;

#endif // fstb_HAS_SIMD

	// Computes the log2 approximation [1 ; 2[ -> [0 ; 1[
	part = poly (part);

	// Sums the components
	const auto     total = log2_int + part;

	return total;
}



// poly is a user-provided Vf32 exp2 approximation from [0 ; 1[ to [1 ; 2[
template <typename P>
Vf32	Vf32::exp2_base (P poly) const noexcept
{
#if ! defined (fstb_HAS_SIMD)

	const int32_t  tx0 = floor_int (_x [0]);
	const int32_t  tx1 = floor_int (_x [1]);
	const int32_t  tx2 = floor_int (_x [2]);
	const int32_t  tx3 = floor_int (_x [3]);
	const Vf32     frac {
		_x [0] - static_cast <float> (tx0),
		_x [1] - static_cast <float> (tx1),
		_x [2] - static_cast <float> (tx2),
		_x [3] - static_cast <float> (tx3)
	};

	Combo          combo { poly (frac) };

	combo._s32 [0] += tx0 << 23;
	combo._s32 [1] += tx1 << 23;
	combo._s32 [2] += tx2 << 23;
	combo._s32 [3] += tx3 << 23;
	assert (
	      combo._vf32 [0] >= 0
		&& combo._vf32 [1] >= 0
		&& combo._vf32 [2] >= 0
		&& combo._vf32 [3] >= 0
	);
	return combo._vf32;

#else // fstb_HAS_SIMD

	// Separates the integer and fractional parts
# if fstb_ARCHI == fstb_ARCHI_X86
	const auto     round_toward_m_i = _mm_set1_ps (-0.5f);
	auto           xi        = _mm_cvtps_epi32 (_mm_add_ps (_x, round_toward_m_i));
	const auto     val_floor = Vf32 { _mm_cvtepi32_ps (xi) };
# elif fstb_ARCHI == fstb_ARCHI_ARM
	const int      round_ofs = 256;
	int32x4_t      xi = vcvtq_s32_f32 (_x + vdupq_n_f32 (float (round_ofs)));
	xi -= vdupq_n_s32 (round_ofs);
	const auto     val_floor = Vf32 { vcvtq_f32_s32 (xi) };
# endif // fstb_ARCHI

	auto           frac = *this - val_floor;

	// Computes the exp2 approximation [0 ; 1] -> [1 ; 2]
	frac = poly (frac);

	// Integer part
# if fstb_ARCHI == fstb_ARCHI_X86
	xi = _mm_slli_epi32 (xi, 23);
	xi = _mm_add_epi32 (xi, _mm_castps_si128 (frac));
	return _mm_castsi128_ps (xi);
# elif fstb_ARCHI == fstb_ARCHI_ARM
	xi = vshlq_n_s32 (xi, 23);
	xi = xi + vreinterpretq_s32_f32 (frac);
	return vreinterpretq_f32_s32 (xi);
# endif // fstb_ARCHI

#endif // fstb_HAS_SIMD
}



Vf32	Vf32::signbit () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 {
		copysignf (0.f, _x [0]),
		copysignf (0.f, _x [1]),
		copysignf (0.f, _x [2]),
		copysignf (0.f, _x [3])
	};
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
std::tuple <Vf32, Vf32>	Vf32::spread_pairs () const noexcept
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
	return Vf32 { 0, 0, 0, 0 };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_setzero_ps ();
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vdupq_n_f32 (0);
#endif // fstb_ARCHI
}



Vf32	Vf32::all1 () noexcept
{
#if ! defined (fstb_HAS_SIMD)
	Combo          c;
	c._s32 [0] = -1;
	c._s32 [1] = -1;
	c._s32 [2] = -1;
	c._s32 [3] = -1;
	return Vf32 { c._vf32 };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_castsi128_ps (_mm_set1_epi32 (-1));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_f32_s32 (vdupq_n_s32 (-1));
#endif // fstb_ARCHI
}



// Returns a0 | a1 | ? | ?
Vf32	Vf32::set_pair (float a0, float a1) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 { a0, a1, 0, 0 };
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
	return Vf32 { a02, a13, a02, a13 };
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
	return Vf32 { a01, a01, a23, a23 };
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



// returns { p0 [0 1], p1 [0 1] }
Vf32	Vf32::interleave_pair_lo (Vf32 p0, Vf32 p1) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 { p0._x [0], p0._x [1], p1._x [0], p1._x [1] };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_shuffle_ps (p0._x, p1._x, (0<<0) + (1<<2) + (0<<4) + (1<<6));
	// return _mm_movelh_ps (p0, p1);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const float32x2_t  p0x = vget_low_f32 (p0._x);
	const float32x2_t  p1x = vget_low_f32 (p1._x);
	return vcombine_f32 (p0x, p1x);
#endif // fstb_ARCHI
}



// returns { p0 [2 3], p1 [2 3] }
Vf32	Vf32::interleave_pair_hi (Vf32 p0, Vf32 p1) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 { p0._x [2], p0._x [3], p1._x [2], p1._x [3] };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_shuffle_ps (p0._x, p1._x, (2<<0) + (3<<2) + (2<<4) + (3<<6));
	// return _mm_movehl_ps (p1, p0);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const float32x2_t  p0x = vget_high_f32 (p0._x);
	const float32x2_t  p1x = vget_high_f32 (p1._x);
	return vcombine_f32 (p0x, p1x);
#endif // fstb_ARCHI
}



std::tuple <Vf32, Vf32>	Vf32::interleave (Vf32 p0, Vf32 p1) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return std::make_tuple (
		Vf32 {
			p0._x [0],
			p1._x [0],
			p0._x [1],
			p1._x [1]
		}, Vf32 {
			p0._x [2],
			p1._x [2],
			p0._x [3],
			p1._x [3]
		}
	);
#elif fstb_ARCHI == fstb_ARCHI_X86
	return std::make_tuple (
		Vf32 { _mm_unpacklo_ps (p0._x, p1._x) },
		Vf32 { _mm_unpackhi_ps (p0._x, p1._x) }
	);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const float32x4x2_t  tmp = vzipq_f32 (p0._x, p1._x);
	return std::make_tuple (
		Vf32 { tmp.val [0] },
		Vf32 { tmp.val [1] }
	);
#endif // fstb_ARCHI
}



std::tuple <Vf32, Vf32>	Vf32::deinterleave (Vf32 i0, Vf32 i1) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return std::make_tuple (
		Vf32 {
			i0._x [0],
			i0._x [2],
			i1._x [0],
			i1._x [2]
		}, Vf32 {
			i0._x [1],
			i0._x [3],
			i1._x [1],
			i1._x [3]
		}
	);
#elif fstb_ARCHI == fstb_ARCHI_X86
	return std::make_tuple (
		Vf32 { _mm_shuffle_ps (i0._x, i1._x, (0<<0) | (2<<2) | (0<<4) | (2<<6)) },
		Vf32 { _mm_shuffle_ps (i0._x, i1._x, (1<<0) | (3<<2) | (1<<4) | (3<<6)) }
	);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const float32x4x2_t  tmp = vuzpq_f32 (i0._x, i1._x);
	return std::make_tuple (
		Vf32 { tmp.val [0] },
		Vf32 { tmp.val [1] }
	);
#endif // fstb_ARCHI
}



Vf32	Vf32::deinterleave_lo (Vf32 i0, Vf32 i1) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 { i0._x [0], i0._x [2], i1._x [0], i1._x [2] };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_shuffle_ps (i0._x, i1._x, (0<<0) | (2<<2) | (0<<4) | (2<<6));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vuzpq_f32 (i0._x, i1._x).val [0];
#endif // fstb_ARCHI
}



Vf32	Vf32::deinterleave_hi (Vf32 i0, Vf32 i1) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 { i0._x [1], i0._x [3], i1._x [1], i1._x [3] };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_shuffle_ps (i0._x, i1._x, (1<<0) | (3<<2) | (1<<4) | (3<<6));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vuzpq_f32 (i0._x, i1._x).val [1];
#endif // fstb_ARCHI
}



// Extracts the vector at the position POS from the double-width vector {a b}
// Concatenates a [POS...3] with b [0...3-POS]
template <int POS>
Vf32	Vf32::compose (Vf32 a, Vf32 b) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	switch (POS & 3)
	{
	case 1:  return Vf32 { a._x [1], a._x [2], a._x [3], b._x [0] };
	case 2:  return Vf32 { a._x [2], a._x [3], b._x [0], b._x [1] };
	case 3:  return Vf32 { a._x [3], b._x [0], b._x [1], b._x [2] };
	default: return a;
	}
#elif fstb_ARCHI == fstb_ARCHI_X86
	switch (POS & 3)
	{
	case 1:
		{
			const auto     tmp = _mm_move_ss (a._x, b._x);
			return _mm_shuffle_ps (tmp, tmp, (0<<6) | (3<<4) | (2<<2) | (1<<0));
		}
	case 2:
		return _mm_shuffle_ps (a._x, b._x, (1<<6) | (0<<4) | (3<<2) | (2<<0));
	case 3:
		return _mm_move_ss (
			_mm_shuffle_ps (b._x, b._x, (2<<6) | (1<<4) | (0<<2) | (3<<0)),
			_mm_shuffle_ps (a._x, a._x, (2<<6) | (1<<4) | (0<<2) | (3<<0))
		);
	default:
		return a;
	}
#elif fstb_ARCHI == fstb_ARCHI_ARM
	if (POS == 0)
	{
		return a;
	}
	else
	{
		const auto     aa = vreinterpretq_s32_f32 (a._x);
		const auto     bb = vreinterpretq_s32_f32 (b._x);
		return vreinterpretq_f32_s32 (vextq_s32 (aa, bb, POS));
	}
#endif // fstb_ARCHI
}



template <typename MEM>
Vf32	Vf32::load (const MEM *ptr) noexcept
{
	assert (is_ptr_align_nz (ptr, fstb_SIMD128_ALIGN));

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
	return Vf32 { p [0], p [1], 0, 0 };
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



/*\\\ GLOBAL OPERATORS AND FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



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
	return Vf32 {
		fabsf (v._x [0]),
		fabsf (v._x [1]),
		fabsf (v._x [2]),
		fabsf (v._x [3])
	};
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_andnot_ps (Vf32::signbit_mask (), v);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vabsq_f32 (v);
#endif // fstb_ARCHI
}



// Returns x * a + b
Vf32 fma (const Vf32 &x, const Vf32 &a, const Vf32 &b) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 {
		x._x [0] * a._x [0] + b._x [0],
		x._x [1] * a._x [1] + b._x [1],
		x._x [2] * a._x [2] + b._x [2],
		x._x [3] * a._x [3] + b._x [3]
	};
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
Vf32 fms (const Vf32 &x, const Vf32 &a, const Vf32 &b) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 {
		x._x [0] * a._x [0] - b._x [0],
		x._x [1] * a._x [1] - b._x [1],
		x._x [2] * a._x [2] - b._x [2],
		x._x [3] * a._x [3] - b._x [3]
	};
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_sub_ps (_mm_mul_ps (x, a), b);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	#if defined (__ARM_FEATURE_FMA)
	return -vfmsq_f32 (b, x, a);
	#else
	return -vmlsq_f32 (b, x, a);
	#endif
#endif // fstb_ARCHI
}



// Returns - x * a + b
Vf32 fnma (const Vf32 &x, const Vf32 &a, const Vf32 &b) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 {
		b._x [0] - x._x [0] * a._x [0],
		b._x [1] - x._x [1] * a._x [1],
		b._x [2] - x._x [2] * a._x [2],
		b._x [3] - x._x [3] * a._x [3]
	};
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_sub_ps (b, _mm_mul_ps (x, a));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	#if defined (__ARM_FEATURE_FMA)
	return vfmsq_f32 (b, x, a);
	#else
	return vmlsq_f32 (b, x, a);
	#endif
#endif // fstb_ARCHI
}



Vf32 round (const Vf32 &v) noexcept
{
	return v.round ();
}



Vf32 min (const Vf32 &lhs, const Vf32 &rhs) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 {
		std::min (lhs._x [0], rhs._x [0]),
		std::min (lhs._x [1], rhs._x [1]),
		std::min (lhs._x [2], rhs._x [2]),
		std::min (lhs._x [3], rhs._x [3])
	};
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_min_ps (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vminq_f32 (lhs, rhs);
#endif // fstb_ARCHI
}



Vf32 max (const Vf32 &lhs, const Vf32 &rhs) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 {
		std::max (lhs._x [0], rhs._x [0]),
		std::max (lhs._x [1], rhs._x [1]),
		std::max (lhs._x [2], rhs._x [2]),
		std::max (lhs._x [3], rhs._x [3])
	};
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



Vf32 select (Vf32 cond, Vf32 v_t, Vf32 v_f) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	/*** To do: implement as r = v_f ^ ((v_f ^ v_t) & cond) ***/
	const Vf32::Combo cc { cond };
	Vf32::Combo    ct { v_t };
	Vf32::Combo    cf { v_f };
	Vf32::Combo    r;
	r._s32 [0] = (ct._s32 [0] & cc._s32 [0]) | (cf._s32 [0] & ~cc._s32 [0]);
	r._s32 [1] = (ct._s32 [1] & cc._s32 [1]) | (cf._s32 [1] & ~cc._s32 [1]);
	r._s32 [2] = (ct._s32 [2] & cc._s32 [2]) | (cf._s32 [2] & ~cc._s32 [2]);
	r._s32 [3] = (ct._s32 [3] & cc._s32 [3]) | (cf._s32 [3] & ~cc._s32 [3]);
	return r._vf32;
#elif fstb_ARCHI == fstb_ARCHI_X86
	const auto     cond_1 = _mm_and_ps (   cond, v_t);
	const auto     cond_0 = _mm_andnot_ps (cond, v_f);
	return _mm_or_ps (cond_0, cond_1);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vbslq_f32 (vreinterpretq_u32_f32 (cond), v_t, v_f);
#endif // fstb_ARCHI
}



std::tuple <Vf32, Vf32> swap_if (Vf32 cond, Vf32 lhs, Vf32 rhs) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	const Vf32::Combo cc { cond };
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



Vf32 sqrt (Vf32 v) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 {
		sqrtf (v._x [0]),
		sqrtf (v._x [1]),
		sqrtf (v._x [2]),
		sqrtf (v._x [3])
	};
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



// Formula by 2DaT
// 12-13 ulp
// https://www.kvraudio.com/forum/viewtopic.php?f=33&t=532048
Vf32 log2 (Vf32 v) noexcept
{
#if ! defined (fstb_HAS_SIMD)

	assert (v > Vf32 (0));
	/*** To do: actual approximation matching the SIMD formula ***/
	return Vf32 {
		logf (v._x [0]) * float (LOG2_E),
		logf (v._x [1]) * float (LOG2_E),
		logf (v._x [2]) * float (LOG2_E),
		logf (v._x [3]) * float (LOG2_E),
	};

#else // fstb_HAS_SIMD

	// Rational fraction approximating log2 (x)
	// [sqrt (0.5) ; sqrt (2)] -> [-0.5 ; 0.5]
	// f: x -> (x - 1) * (x^2 + c1*x + c0) / (d2*x^2 + d1*x + d0)
	// No analytic continuity on the full range, although this is "almost" C0
	// (good enough for single precision).
	const auto     c0    = Vf32 (1.011593342e+01f);
	const auto     c1    = Vf32 (1.929443550e+01f);
	const auto     d0    = Vf32 (2.095932245e+00f);
	const auto     d1    = Vf32 (1.266638851e+01f);
	const auto     d2    = Vf32 (6.316540241e+00f);
	const auto     one   = Vf32 (1.0f);
	const auto     multi = Vf32 (1.41421356237f);
	const auto     mmask = ~((1 << 23) - 1);

#if fstb_ARCHI == fstb_ARCHI_X86

	__m128i        x_i           = _mm_castps_si128 (v);
	__m128i        spl_exp       = _mm_castps_si128 (v * multi);
	spl_exp = _mm_sub_epi32 (spl_exp, _mm_castps_si128 (one));
	spl_exp = _mm_and_si128 (spl_exp, _mm_set1_epi32 (mmask));
	const auto     spl_mantissa  =
		Vf32 { _mm_castsi128_ps (_mm_sub_epi32 (x_i, spl_exp)) };
	spl_exp = _mm_srai_epi32 (spl_exp, 23);
	const auto     log2_exponent = Vf32 { _mm_cvtepi32_ps (spl_exp) };

#elif fstb_ARCHI == fstb_ARCHI_ARM

	const int32x4_t   x_i        = vreinterpretq_s32_f32 (v);
	int32x4_t      spl_exp       = vreinterpretq_s32_f32 (v * multi);
	spl_exp = spl_exp - vreinterpretq_s32_f32 (one);
	spl_exp = vandq_s32 (spl_exp, vdupq_n_s32 (mmask));
	const auto     spl_mantissa  = Vf32 { vreinterpretq_f32_s32 (x_i - spl_exp) };
	spl_exp = vshrq_n_s32 (spl_exp, 23);
	const auto     log2_exponent = Vf32 { vcvtq_f32_s32 (spl_exp) };

#endif // fstb_ARCHI

	auto           num = spl_mantissa + c1;
	num = fma (num, spl_mantissa, c0);
	num = fms (num, spl_mantissa, num);

	auto           den = d2;
	den = fma (den, spl_mantissa, d1);
	den = fma (den, spl_mantissa, d0);

	auto           res = num / den;
	res += log2_exponent;

	return res;

#endif // fstb_HAS_SIMD
}



// Formula by 2DaT
// Coefficients fixed by Andrew Simper to achieve true C0 continuity
// 3-4 ulp
// https://www.kvraudio.com/forum/viewtopic.php?p=7161124#p7161124
// https://www.kvraudio.com/forum/viewtopic.php?p=7677266#p7677266
Vf32 exp2 (Vf32 v) noexcept
{
#if ! defined (fstb_HAS_SIMD)

	/*** To do: actual approximation matching the SIMD formula ***/
	return Vf32 {
		exp2f (v._x [0]),
		exp2f (v._x [1]),
		exp2f (v._x [2]),
		exp2f (v._x [3]),
	};

#else // fstb_HAS_SIMD

	// [-0.5, 0.5] 2^x approx polynomial ~ 2.4 ulp
	const auto     c0 = Vf32 (1.000000088673463);
	const auto     c1 = Vf32 (0.69314693211407);
	const auto     c2 = Vf32 (0.24022037362574);
	const auto     c3 = Vf32 (0.0555072548370);
	const auto     c4 = Vf32 (0.0096798351988);
	const auto     c5 = Vf32 (0.0013285658116);

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
	i -= vdupq_n_s32 (round_ofs);
	v -= vcvtq_f32_s32 (i);
#endif // fstb_ARCHI

	// Estrin-Horner evaluation scheme
	const auto     v2  = v * v;
	const auto     p23 = fma (c3, v, c2);
	const auto     p01 = fma (c1, v, c0);
	auto           p   = fma (c5, v, c4);
	p = fma (p, v2, p23);
	p = fma (p, v2, p01);

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



}  // namespace fstb



#endif   // fstb_Vf32_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
