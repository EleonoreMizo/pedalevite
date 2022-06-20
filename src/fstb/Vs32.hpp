/*****************************************************************************

        Vs32.hpp
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_Vs32_CODEHEADER_INCLUDED)
#define fstb_Vs32_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"

#include <algorithm>

#include <cassert>



namespace fstb
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// Returns a0 | a0 | a0 | a0
Vs32::Vs32 (Scalar a) noexcept
#if ! defined (fstb_HAS_SIMD)
:	_x { a, a, a, a }
#elif fstb_ARCHI == fstb_ARCHI_X86
:	_x { _mm_set1_epi32 (a) }
#elif fstb_ARCHI == fstb_ARCHI_ARM
:	_x { vdupq_n_s32 (a) }
#endif // fstb_ARCHI
{
	// Nothing
}



// Returns a0 | a1 | a2 | a3
Vs32::Vs32 (Scalar a0, Scalar a1, Scalar a2, Scalar a3) noexcept
#if ! defined (fstb_HAS_SIMD)
:	_x { a0, a1, a2, a3 }
#elif fstb_ARCHI == fstb_ARCHI_X86
:	_x { _mm_set_epi32 (a3, a2, a1, a0) }
#elif fstb_ARCHI == fstb_ARCHI_ARM
:	_x { a0, a1, a2, a3 }
#endif // fstb_ARCHI
{
	// Nothing
}



// Returns a0 | a1 | a2 | a3
Vs32::Vs32 (const std::tuple <Scalar, Scalar, Scalar, Scalar> &a) noexcept
#if ! defined (fstb_HAS_SIMD)
:	_x { std::get <0> (a), std::get <1> (a), std::get <2> (a), std::get <3> (a) }
#elif fstb_ARCHI == fstb_ARCHI_X86
:	_x { _mm_set_epi32 (std::get <3> (a), std::get <2> (a), std::get <1> (a), std::get <0> (a)) }
#elif fstb_ARCHI == fstb_ARCHI_ARM
:	_x { std::get <0> (a), std::get <1> (a), std::get <2> (a), std::get <3> (a) }
#endif // fstb_ARCHI
{
	// Nothing
}



template <typename MEM>
void	Vs32::store (MEM *ptr) const noexcept
{
	assert (is_ptr_align_nz (ptr, fstb_SIMD128_ALIGN));

#if ! defined (fstb_HAS_SIMD)
	*reinterpret_cast <Vs32Native *> (ptr) = _x;
#elif fstb_ARCHI == fstb_ARCHI_X86
	_mm_store_si128 (reinterpret_cast <__m128i *> (ptr), _x);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	vst1q_s32 (reinterpret_cast <int32_t *> (ptr), _x);
#endif // fstb_ARCHI
}



template <typename MEM>
void	Vs32::storeu (MEM *ptr) const noexcept
{
	assert (ptr != nullptr);

#if ! defined (fstb_HAS_SIMD)
	*reinterpret_cast <Vs32Native *> (ptr) = _x;
#elif fstb_ARCHI == fstb_ARCHI_X86
	_mm_storeu_si128 (reinterpret_cast <__m128i *> (ptr), _x);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	vst1q_u8 (reinterpret_cast <uint8_t *> (ptr), vreinterpretq_u8_s32 (_x));
#endif // fstb_ARCHI
}



// n = number of scalars to store (from the LSB)
template <typename MEM>
void	Vs32::storeu_part (MEM *ptr, int n) const noexcept
{
	assert (n > 0);

	if (n >= _length)
	{
		storeu (ptr);
		return;
	}

	int32_t *      f_ptr = reinterpret_cast <int32_t *> (ptr);

#if ! defined (fstb_HAS_SIMD)

	for (int i = 0; i < n; ++i)
	{
		f_ptr [i] = _x [i];
	}

#elif fstb_ARCHI == fstb_ARCHI_X86

	f_ptr [0] = _mm_cvtsi128_si32 (_x);
	if (n >= 2)
	{
		f_ptr [1] = _mm_cvtsi128_si32 (_mm_shuffle_epi32 (_x, 1 << 0));
		if (n >= 3)
		{
			f_ptr [1] = _mm_cvtsi128_si32 (_mm_shuffle_epi32 (_x, 2 << 0));
		}
	}

#elif fstb_ARCHI == fstb_ARCHI_ARM

	vst1q_lane_s32 (f_ptr + 0, _x, 0);
	if (n >= 2)
	{
		vst1q_lane_s32 (f_ptr + 1, _x, 1);
		if (n >= 3)
		{
			vst1q_lane_s32 (f_ptr + 2, _x, 2);
		}
	}

#endif
}



// Works only with well-formed condition results (tested bits depend on the
// implementation).
// For each scalar, true = all bits set, false = all bits cleared
Vs32::operator bool () const noexcept
{
	return and_h ();
}



Vs32 &	Vs32::operator += (const Vs32Native &other) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	_x [0] += other [0];
	_x [1] += other [1];
	_x [2] += other [2];
	_x [3] += other [3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	_x = _mm_add_epi32 (_x, other);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	_x = vaddq_s32 (_x, other);
#endif // fstb_ARCHI
	return *this;
}



Vs32 &	Vs32::operator -= (const Vs32Native &other) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	_x [0] -= other [0];
	_x [1] -= other [1];
	_x [2] -= other [2];
	_x [3] -= other [3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	_x = _mm_sub_epi32 (_x, other);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	_x = vsubq_s32 (_x, other);
#endif // fstb_ARCHI
	return *this;
}



Vs32 &	Vs32::operator *= (const Vs32Native &other) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	_x [0] *= other [0];
	_x [1] *= other [1];
	_x [2] *= other [2];
	_x [3] *= other [3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	// Emulation of _mm_mullo_epi32 (SSE4.1)
# if fstb_COMPILER == fstb_COMPILER_MSVC
	// For some reason this code is slightly faster on MSVC
	auto           p02_64 = _mm_mul_epu32 (_x, other);
	auto           p13_64 = _mm_mul_epu32 (
		_mm_srli_si128 (_x   , 4),
		_mm_srli_si128 (other, 4)
	);
	p02_64 = _mm_shuffle_epi32 (p02_64, (0 << 0) | (2 << 2));
	p13_64 = _mm_shuffle_epi32 (p13_64, (0 << 0) | (2 << 2));
	_x     = _mm_unpacklo_epi32 (p02_64, p13_64);
# else
	// Code of this function shamelessly borrowed from tp7
	// https://github.com/tp7/masktools/blob/16bit/masktools/common/simd.h
	// This code is faster on GCC/Clang
	const __m128i  lhs13  = _mm_shuffle_epi32 (_x, 0xF5);        // (-,a3,-,a1)
	const __m128i  rhs13  = _mm_shuffle_epi32 (other, 0xF5);     // (-,b3,-,b1)
	const __m128i  prod02 = _mm_mul_epu32 (_x, other);           // (-,a2*b2,-,a0*b0)
	const __m128i  prod13 = _mm_mul_epu32 (lhs13, rhs13);        // (-,a3*b3,-,a1*b1)
	const __m128i  prod01 = _mm_unpacklo_epi32 (prod02, prod13); // (-,-,a1*b1,a0*b0)
	const __m128i  prod23 = _mm_unpackhi_epi32 (prod02, prod13); // (-,-,a3*b3,a2*b2)
	_x                    = _mm_unpacklo_epi64 (prod01 ,prod23); // (ab3,ab2,ab1,ab0)
# endif // fstb_COMPILER
#elif fstb_ARCHI == fstb_ARCHI_ARM
	_x = vmulq_s32 (_x, other);
#endif // fstb_ARCHI
	return *this;
}



Vs32 &	Vs32::operator &= (const Vs32Native &other) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	_x [0] &= other [0];
	_x [1] &= other [1];
	_x [2] &= other [2];
	_x [3] &= other [3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	_x = _mm_and_si128 (_x, other);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	_x = vandq_s32 (_x, other);
#endif // fstb_ARCHI
	return *this;
}



Vs32 &	Vs32::operator |= (const Vs32Native &other) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	_x [0] |= other [0];
	_x [1] |= other [1];
	_x [2] |= other [2];
	_x [3] |= other [3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	_x = _mm_or_si128 (_x, other);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	_x = vorrq_s32 (_x, other);
#endif // fstb_ARCHI
	return *this;
}



Vs32 &	Vs32::operator ^= (const Vs32Native &other) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	_x [0] ^= other [0];
	_x [1] ^= other [1];
	_x [2] ^= other [2];
	_x [3] ^= other [3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	_x = _mm_xor_si128 (_x, other);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	_x = veorq_s32 (_x, other);
#endif // fstb_ARCHI
	return *this;
}



Vs32 &	Vs32::operator <<= (int imm) noexcept
{
	assert (imm >= 0);
	assert (imm <= 32);
#if ! defined (fstb_HAS_SIMD)
	_x [0] <<= imm;
	_x [1] <<= imm;
	_x [2] <<= imm;
	_x [3] <<= imm;
#elif fstb_ARCHI == fstb_ARCHI_X86
	_x = _mm_slli_epi32 (_x, imm);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	_x <<= imm;
#endif // fstb_ARCHI
	return *this;
}



Vs32 &	Vs32::operator >>= (int imm) noexcept
{
	assert (imm >= 0);
	assert (imm <= 32);
#if ! defined (fstb_HAS_SIMD)
	_x [0] >>= imm;
	_x [1] >>= imm;
	_x [2] >>= imm;
	_x [3] >>= imm;
#elif fstb_ARCHI == fstb_ARCHI_X86
	_x = _mm_srai_epi32 (_x, imm);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	_x >>= imm;
#endif // fstb_ARCHI
	return *this;
}



// -(1<<31) stays constant
Vs32	Vs32::operator - () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vs32 {
		-_x [0],
		-_x [1],
		-_x [2],
		-_x [3]
	};
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_sub_epi32 (_mm_setzero_si128 (), _x);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vnegq_s32 (_x);
#endif // fstb_ARCHI
}



Vs32 	Vs32::operator ~ () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vs32 {
		~(_x [0]),
		~(_x [1]),
		~(_x [2]),
		~(_x [3])
	};
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_xor_si128 (_x, _mm_set1_epi32 (-1));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vmvnq_s32 (_x);
#endif // fstb_ARCHI
}



Vs32	Vs32::is_lt_0 () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vs32 {
		(_x [0] < 0) ? -1 : 0,
		(_x [1] < 0) ? -1 : 0,
		(_x [2] < 0) ? -1 : 0,
		(_x [3] < 0) ? -1 : 0
	};
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_cmplt_epi32 (_x, _mm_setzero_si128 ());
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vshrq_n_s32 (_x, 31);
#endif // fstb_ARCHI
}



Vs32	Vs32::reverse () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vs32 { _x [3], _x [2], _x [1], _x [0] };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_shuffle_epi32 (_x, (3<<0) + (2<<2) + (1<<4) + (0<<6));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vrev64q_s32 (vcombine_s32 (vget_high_s32 (_x), vget_low_s32 (_x)));
#endif // fstb_ARCHI
}



// Positive = left
template <int SHIFT>
Vs32	Vs32::rotate () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vs32 {
		_x [(0 - SHIFT) & 3],
		_x [(1 - SHIFT) & 3],
		_x [(2 - SHIFT) & 3],
		_x [(3 - SHIFT) & 3]
	};
#elif fstb_ARCHI == fstb_ARCHI_X86
	switch (SHIFT & 3)
	{
	case 1:  return _mm_shuffle_epi32 (_x, (2<<6) | (1<<4) | (0<<2) | (3<<0));
	case 2:  return _mm_shuffle_epi32 (_x, (1<<6) | (0<<4) | (3<<2) | (2<<0));
	case 3:  return _mm_shuffle_epi32 (_x, (0<<6) | (3<<4) | (2<<2) | (1<<0));
	default: return *this;
	}
#elif fstb_ARCHI == fstb_ARCHI_ARM
	switch (SHIFT & 3)
	{
	case 1:  return vextq_s32 (_x, _x, 3);
	case 2:  return vextq_s32 (_x, _x, 2);
	case 3:  return vextq_s32 (_x, _x, 1);
	default: return *this;
	}
#endif // fstb_ARCHI
}



template <int POS>
int32_t	Vs32::extract () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return _x [POS & 3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	auto           a = _x;
	switch (POS & 3)
	{
	case 1:  a = _mm_shuffle_epi32 (a, 1); break;
	case 2:  a = _mm_shuffle_epi32 (a, 2); break;
	case 3:  a = _mm_shuffle_epi32 (a, 3); break;
	default: /* Nothing */                 break;
	}
	return _mm_cvtsi128_si32 (a);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vgetq_lane_s32 (_x, POS & 3);
#endif // fstb_ARCHI
}



template <int POS>
Vs32	Vs32::insert (int32_t val) const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	auto           a = *this;
	a._x [POS & 3] = val;
	return a;
#elif fstb_ARCHI == fstb_ARCHI_X86
	auto           a = rotate <(-POS) & 3> ();
	a._x = _mm_castps_si128 (_mm_move_ss (
		_mm_castsi128_ps (a._x),
		_mm_castsi128_ps (_mm_set1_epi32 (val))
	));
	return a.template rotate <POS> ();
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vsetq_lane_s32 (val, _x, POS & 3);
#endif // fstb_ARCHI
}



template <int POS>
Vs32	Vs32::spread () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vs32 (extract <POS> ());
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_shuffle_epi32 (_x, 0x55 * (POS & 3));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vdupq_n_s32 (vgetq_lane_s32 (_x, POS & 3));
#endif // fstb_ARCHI
}



int32_t	Vs32::sum_h () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return (_x [0] + _x [2]) + (_x [1] + _x [3]);
#elif fstb_ARCHI == fstb_ARCHI_X86
	// s = v3,v2,v1,v0
	const auto s = _mm_shuffle_epi32 (_x, (3 << 0) | (2 << 2) | (1 << 4) | (0 << 6));
	const auto v = _mm_add_epi32 (_x, s); // v0+v3,v1+v2,v2+v1,v3+v0
	return _mm_cvtsi128_si32 (_mm_add_epi32 (v, _mm_shuffle_epi32 (v, 1 << 0)));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	#if fstb_WORD_SIZE == 64
		return vaddvq_s32 (_x);
	#else
		int32x2_t      v2 = vadd_s32 (vget_high_s32 (_x), vget_low_s32 (_x));
		return vget_lane_s32 (vpadd_s32 (v2, v2), 0);
	#endif
#endif // fstb_ARCHI
}



int32_t	Vs32::min_h () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return std::min (std::min (_x [0], _x [2]), std::min (_x [1], _x [3]));
#elif fstb_ARCHI == fstb_ARCHI_X86
	const auto     v0 = min (*this, _mm_shuffle_epi32 (_x, (3 << 2) | 2));
	const auto     v1 = _mm_shuffle_epi32 (v0, 1);
	return std::min (_mm_cvtsi128_si32 (v0), _mm_cvtsi128_si32 (v1));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	int32x2_t      v2 = vmin_s32 (vget_high_s32 (_x), vget_low_s32 (_x));
	return vget_lane_s32 (vpmin_s32 (v2, v2), 0);
#endif // fstb_ARCHI
}



int32_t	Vs32::max_h () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return std::max (std::max (_x [0], _x [2]), std::max (_x [1], _x [3]));
#elif fstb_ARCHI == fstb_ARCHI_X86
	const auto     v0 = max (*this, _mm_shuffle_epi32 (_x, (3 << 2) | 2));
	const auto     v1 = _mm_shuffle_epi32 (v0, 1);
	return std::max (_mm_cvtsi128_si32 (v0), _mm_cvtsi128_si32 (v1));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	int32x2_t      v2 = vmax_s32 (vget_high_s32 (_x), vget_low_s32 (_x));
	return vget_lane_s32 (vpmax_s32 (v2, v2), 0);
#endif // fstb_ARCHI
}



// Works only with well-formed condition results (tested bits depends on the implementation).
// For each scalar, true = all bits set, false = all bits cleared
bool	Vs32::and_h () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	const int32_t  t = (_x [0] & _x [1]) & (_x [2] & _x [3]);
	return (t == -1);
#elif fstb_ARCHI == fstb_ARCHI_X86
	return (_mm_movemask_epi8 (_x) == 0xFFFF);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const uint32x2_t  tmp = vreinterpret_u32_u16 (
		vqmovn_u32 (vreinterpretq_u32_s32 (_x))
	);
	return (   vget_lane_u32 (tmp, 0) == 0xFFFFFFFFU
	        && vget_lane_u32 (tmp, 1) == 0xFFFFFFFFU);
#endif // fstb_ARCHI
}



// Works only with well-formed condition results (tested bits depends on the implementation).
// For each scalar, true = all bits set, false = all bits cleared
bool	Vs32::or_h () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	const int32_t  t = (_x [0] | _x [1]) | (_x [2] | _x [3]);
	return (t != 0);
#elif fstb_ARCHI == fstb_ARCHI_X86
	return (_mm_movemask_epi8 (_x) != 0);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const uint32x2_t  tmp = vreinterpret_u32_u16 (
		vqmovn_u32 (vreinterpretq_u32_s32 (_x))
	);
	return (   vget_lane_u32 (tmp, 0) != 0
	        || vget_lane_u32 (tmp, 1) != 0);
#endif // fstb_ARCHI
}



// Moves the boolean content of each 4 scalar into the lower 4 bits of the
// return value.
// Assumes the object is a result of a comparison, with all bits the same
// in each 32-bit element.
unsigned int	Vs32::movemask () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return
		   (_x [0] >> 31)
		| ((_x [1] >> 30) & 2)
		| ((_x [2] >> 29) & 4)
		| ((_x [3] >> 28) & 8);
#elif fstb_ARCHI == fstb_ARCHI_X86
	return static_cast <unsigned int> (_mm_movemask_ps (_mm_castsi128_ps (_x)));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	uint64x2_t     tmp1 =
		vreinterpretq_u64_s32 (_x);   // ddd...ddd ccc...ccc bbb...bbb aaa...aaa
	tmp1 = vshrq_n_u64 (tmp1, 31);   // 000...00d ddd...ddc 000...00b bbb...bba
	uint64x1_t     tmp2 = vsli_n_u64 (
		vget_high_u64 (tmp1),
		vget_low_u64 (tmp1),
		2
	);
	return vget_lane_u32 (vreinterpret_u32_u64 (tmp2), 0) & 0xF;
#endif // fstb_ARCHI
}



int	Vs32::count_bits () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	// http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
	uint32_t       v0 = _x [0] - ((_x [0] >> 1) & 0x55555555);
	uint32_t       v1 = _x [1] - ((_x [1] >> 1) & 0x55555555);
	uint32_t       v2 = _x [2] - ((_x [2] >> 1) & 0x55555555);
	uint32_t       v3 = _x [3] - ((_x [3] >> 1) & 0x55555555);
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
	auto           x = _x;
	auto           n = _mm_srli_epi64 (x, 1);
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
	const uint8x16_t  cnt_8  = vcntq_u8 (vreinterpretq_u8_s32 (_x));
	const uint16x8_t  cnt_16 = vpaddlq_u8 (cnt_8);
	const uint32x4_t  cnt_32 = vpaddlq_u16 (cnt_16);
	const uint64x2_t  cnt_64 = vpaddlq_u32 (cnt_32);
	const int32x4_t   cnt_s  = vreinterpretq_s32_u64 (cnt_64);
	return vgetq_lane_s32 (cnt_s, 0) + vgetq_lane_s32 (cnt_s, 2);
#endif // fstb_ARCHI
}



Vs32	Vs32::zero () noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vs32 { 0, 0, 0, 0 };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_setzero_si128 ();
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vdupq_n_s32 (0);
#endif // fstb_ARCHI
}



Vs32	Vs32::all1 () noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vs32 { -1, -1, -1, -1 };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_set1_epi32 (-1);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vdupq_n_s32 (-1);
#endif // fstb_ARCHI
}



// "true" must be 1 and nothing else.
Vs32	Vs32::set_mask (bool m0, bool m1, bool m2, bool m3) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vs32 {
		-int32_t (m0),
		-int32_t (m1),
		-int32_t (m2),
		-int32_t (m3),
	};
#elif 1 // Fast version
# if fstb_ARCHI == fstb_ARCHI_X86
	return _mm_sub_epi32 (
		_mm_setzero_si128 (),
		_mm_set_epi32 (m3, m2, m1, m0)
	);
# elif fstb_ARCHI == fstb_ARCHI_ARM
	float32x2_t    v01 = vdup_n_f32 (m0);
	float32x2_t    v23 = vdup_n_f32 (m2);
	v01 = vset_lane_f32 (m1, v01, 1);
	v23 = vset_lane_f32 (m3, v23, 1);
	return vnegq_s32 (vreinterpretq_s32_f32 (
		vcombine_f32 (v01, v23)
	));
# endif // fstb_ARCHI
#else // Safer but slower version
# if fstb_ARCHI == fstb_ARCHI_X86
	return _mm_sub_epi32 (
		_mm_set_epi32 (!m3, !m2, !m1, !m0),
		_mm_set1_epi32 (1)
	);
# elif fstb_ARCHI == fstb_ARCHI_ARM
	float32x2_t    v01 = vdup_n_f32 (!m0);
	float32x2_t    v23 = vdup_n_f32 (!m2);
	v01 = vset_lane_f32 (!m1, v01, 1);
	v23 = vset_lane_f32 (!m3, v23, 1);
	const auto     one  = vdupq_n_s32 (1);
	return vsubq_s32 (
		vreinterpretq_s32_f32 (vcombine_f32 (v01, v23)),
		one
	);
# endif // fstb_ARCHI
#endif // Versions
}



// Extracts the vector at the position SHIFT from the double-width vector {a b}
// Concatenates a [SHIFT...3] with b [0...3-SHIFT]
template <int POS>
Vs32	Vs32::compose (Vs32 a, Vs32 b) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	switch (POS & 3)
	{
	case 1:  return Vs32 { a._x [1], a._x [2], a._x [3], b._x [0] };
	case 2:  return Vs32 { a._x [2], a._x [3], b._x [0], b._x [1] };
	case 3:  return Vs32 { a._x [3], b._x [0], b._x [1], b._x [2] };
	default: return a;
	}
	return a;
#elif fstb_ARCHI == fstb_ARCHI_X86
	switch (POS & 3)
	{
	case 1:
		{
			const auto     tmp = _mm_castps_si128 (_mm_move_ss (
				_mm_castsi128_ps (a._x), _mm_castsi128_ps (b._x)
			));
			return _mm_shuffle_epi32 (tmp, (0<<6) | (3<<4) | (2<<2) | (1<<0));
		}
	case 2:
		return _mm_castps_si128 (_mm_shuffle_ps (
			_mm_castsi128_ps (a._x),
			_mm_castsi128_ps (b._x),
			(1<<6) | (0<<4) | (3<<2) | (2<<0)
		));
	case 3:
		return _mm_castps_si128 (_mm_move_ss (
			_mm_castsi128_ps (
				_mm_shuffle_epi32 (b._x, (2<<6) | (1<<4) | (0<<2) | (3<<0))
			),
			_mm_castsi128_ps (
				_mm_shuffle_epi32 (a._x, (2<<6) | (1<<4) | (0<<2) | (3<<0))
			)
		));
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
		return vextq_s32 (a._x, b._x, POS);
	}
#endif // fstb_ARCHI
}



template <typename MEM>
Vs32	Vs32::load (const MEM *ptr) noexcept
{
	assert (is_ptr_align_nz (ptr, fstb_SIMD128_ALIGN));

#if ! defined (fstb_HAS_SIMD)
	return *reinterpret_cast <const Vs32 *> (ptr);
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_load_si128 (reinterpret_cast <const __m128i *> (ptr));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vld1q_s32 (reinterpret_cast <const int32_t *> (ptr));
#endif // fstb_ARCHI
}



template <typename MEM>
Vs32	Vs32::loadu (const MEM *ptr) noexcept
{
	assert (ptr != nullptr);

#if ! defined (fstb_HAS_SIMD)
	return *reinterpret_cast <const Vs32 *> (ptr);
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_loadu_si128 (reinterpret_cast <const __m128i *> (ptr));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_s32_u8 (
		vld1q_u8 (reinterpret_cast <const uint8_t *> (ptr))
	);
#endif // fstb_ARCHI
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ GLOBAL OPERATORS AND FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Vs32 operator + (Vs32 lhs, const Vs32 &rhs) noexcept
{
	lhs += rhs;
	return lhs;
}

Vs32 operator - (Vs32 lhs, const Vs32 &rhs) noexcept
{
	lhs -= rhs;
	return lhs;
}

Vs32 operator * (Vs32 lhs, const Vs32 &rhs) noexcept
{
	lhs *= rhs;
	return lhs;
}

Vs32 operator & (Vs32 lhs, const Vs32 &rhs) noexcept
{
	lhs &= rhs;
	return lhs;
}

Vs32 operator | (Vs32 lhs, const Vs32 &rhs) noexcept
{
	lhs |= rhs;
	return lhs;
}

Vs32 operator ^ (Vs32 lhs, const Vs32 &rhs) noexcept
{
	lhs ^= rhs;
	return lhs;
}



template <typename T>
Vs32 operator << (Vs32 lhs, T rhs) noexcept
{
	lhs <<= rhs;
	return lhs;
}

template <typename T>
Vs32 operator >> (Vs32 lhs, T rhs) noexcept
{
	lhs >>= rhs;
	return lhs;
}



Vs32 operator == (const Vs32 &lhs, const Vs32 &rhs) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vs32 {
		(lhs._x [0] == rhs._x [0]) ? -1 : 0,
		(lhs._x [1] == rhs._x [1]) ? -1 : 0,
		(lhs._x [2] == rhs._x [2]) ? -1 : 0,
		(lhs._x [3] == rhs._x [3]) ? -1 : 0
	};
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_cmpeq_epi32 (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_s32_u32 (vceqq_s32 (lhs, rhs));
#endif // fstb_ARCHI
}



Vs32 operator != (const Vs32 &lhs, const Vs32 &rhs) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vs32 {
		(lhs._x [0] != rhs._x [0]) ? -1 : 0,
		(lhs._x [1] != rhs._x [1]) ? -1 : 0,
		(lhs._x [2] != rhs._x [2]) ? -1 : 0,
		(lhs._x [3] != rhs._x [3]) ? -1 : 0
	};
#elif fstb_ARCHI == fstb_ARCHI_X86
	const auto     eq = _mm_cmpeq_epi32 (lhs, rhs);
	return _mm_xor_si128 (eq, _mm_set1_epi32 (-1));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_s32_u32 (vmvnq_u32 (vceqq_s32 (lhs, rhs)));
#endif // fstb_ARCHI
}



Vs32 operator <  (const Vs32 &lhs, const Vs32 &rhs) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vs32 {
		(lhs._x [0] < rhs._x [0]) ? -1 : 0,
		(lhs._x [1] < rhs._x [1]) ? -1 : 0,
		(lhs._x [2] < rhs._x [2]) ? -1 : 0,
		(lhs._x [3] < rhs._x [3]) ? -1 : 0
	};
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_cmplt_epi32 (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_s32_u32 (vcltq_s32 (lhs, rhs));
#endif // fstb_ARCHI
}



Vs32 operator <= (const Vs32 &lhs, const Vs32 &rhs) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vs32 {
		(lhs._x [0] <= rhs._x [0]) ? -1 : 0,
		(lhs._x [1] <= rhs._x [1]) ? -1 : 0,
		(lhs._x [2] <= rhs._x [2]) ? -1 : 0,
		(lhs._x [3] <= rhs._x [3]) ? -1 : 0
	};
#elif fstb_ARCHI == fstb_ARCHI_X86
# if 1
	return (lhs < rhs) | (lhs == rhs);
# else
	return ~(lhs > rhs);
# endif
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_s32_u32 (vcleq_s32 (lhs, rhs));
#endif // fstb_ARCHI
}



Vs32 operator >  (const Vs32 &lhs, const Vs32 &rhs) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vs32 {
		(lhs._x [0] > rhs._x [0]) ? -1 : 0,
		(lhs._x [1] > rhs._x [1]) ? -1 : 0,
		(lhs._x [2] > rhs._x [2]) ? -1 : 0,
		(lhs._x [3] > rhs._x [3]) ? -1 : 0
	};
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_cmpgt_epi32 (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_s32_u32 (vcgtq_s32 (lhs, rhs));
#endif // fstb_ARCHI
}



Vs32 operator >= (const Vs32 &lhs, const Vs32 &rhs) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vs32 {
		(lhs._x [0] >= rhs._x [0]) ? -1 : 0,
		(lhs._x [1] >= rhs._x [1]) ? -1 : 0,
		(lhs._x [2] >= rhs._x [2]) ? -1 : 0,
		(lhs._x [3] >= rhs._x [3]) ? -1 : 0
	};
#elif fstb_ARCHI == fstb_ARCHI_X86
# if 1
	return (lhs > rhs) | (lhs == rhs);
# else
	return ~(lhs < rhs);
# endif
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_s32_u32 (vcgeq_s32 (lhs, rhs));
#endif // fstb_ARCHI
}



// Result is undefined for -(1<<31).
Vs32 abs (const Vs32 &v) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vs32 {
		std::abs (v._x [0]),
		std::abs (v._x [1]),
		std::abs (v._x [2]),
		std::abs (v._x [3])
	};
#elif fstb_ARCHI == fstb_ARCHI_X86
	const auto     v_neg = _mm_sub_epi32 (_mm_setzero_si128 (), v);
	return max (v, v_neg);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vqabsq_s32 (v);
#endif // fstb_ARCHI
}



Vs32 min (const Vs32 &lhs, const Vs32 &rhs) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vs32 {
		std::min (lhs._x [0], rhs._x [0]),
		std::min (lhs._x [1], rhs._x [1]),
		std::min (lhs._x [2], rhs._x [2]),
		std::min (lhs._x [3], rhs._x [3])
	};
#elif fstb_ARCHI == fstb_ARCHI_X86
	const auto     gt = (lhs > rhs);
	return _mm_or_si128 (
		_mm_and_si128 (   gt, rhs),
		_mm_andnot_si128 (gt, lhs)
	);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vminq_s32 (lhs, rhs);
#endif // fstb_ARCHI
}



Vs32 max (const Vs32 &lhs, const Vs32 &rhs) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vs32 {
		std::max (lhs._x [0], rhs._x [0]),
		std::max (lhs._x [1], rhs._x [1]),
		std::max (lhs._x [2], rhs._x [2]),
		std::max (lhs._x [3], rhs._x [3])
	};
#elif fstb_ARCHI == fstb_ARCHI_X86
	const auto     lt = (lhs < rhs);
	return _mm_or_si128 (
		_mm_and_si128 (   lt, rhs),
		_mm_andnot_si128 (lt, lhs)
	);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vmaxq_s32 (lhs, rhs);
#endif // fstb_ARCHI
}



Vs32 limit (const Vs32 &v, const Vs32 &mi, const Vs32 &ma) noexcept
{
	return min (max (v, mi), ma);
}



Vs32 select (const Vs32 &cond, const Vs32 &v_t, const Vs32 &v_f) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	/*** To do: implement as r = v_f ^ ((v_f ^ v_t) & cond) ***/
	return Vs32 {
		(cond._x [0] & v_t._x [0]) | (~cond._x [0] & v_f._x [0]),
		(cond._x [1] & v_t._x [1]) | (~cond._x [1] & v_f._x [1]),
		(cond._x [2] & v_t._x [2]) | (~cond._x [2] & v_f._x [2]),
		(cond._x [3] & v_t._x [3]) | (~cond._x [3] & v_f._x [3])
	};
#elif fstb_ARCHI == fstb_ARCHI_X86
	const auto     cond_1 = _mm_and_si128 (cond, v_t);
	const auto     cond_0 = _mm_andnot_si128 (cond, v_f);
	return _mm_or_si128 (cond_0, cond_1);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vbslq_s32 (vreinterpretq_u32_s32 (cond), v_t, v_f);
#endif // fstb_ARCHI
}



std::tuple <Vs32, Vs32> swap_if (const Vs32 &cond, Vs32 lhs, Vs32 rhs) noexcept
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
		Vs32 (_mm_xor_si128 (lhs, inv)),
		Vs32 (_mm_xor_si128 (rhs, inv))
	);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const auto     cond_u = vreinterpretq_u32_s32 (cond);
	return std::make_tuple (
		Vs32 (vbslq_s32 (cond_u, rhs, lhs)),
		Vs32 (vbslq_s32 (cond_u, lhs, rhs))
	);
#endif // fstb_ARCHI
}



}  // namespace fstb



#endif   // fstb_Vs32_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
