/*****************************************************************************

        Vu32.hpp
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_Vu32_CODEHEADER_INCLUDED)
#define fstb_Vu32_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace fstb
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Vu32::Vu32 (Scalar a) noexcept
#if ! defined (fstb_HAS_SIMD)
:	_x { a, a, a, a }
#elif fstb_ARCHI == fstb_ARCHI_X86
:	_x { _mm_set1_epi32 (int32_t (a)) }
#elif fstb_ARCHI == fstb_ARCHI_ARM
:	_x { vdupq_n_u32 (a) }
#endif // fstb_ARCHI
{
	// Nothing
}



// Returns a0 | a1 | a2 | a3
Vu32::Vu32 (Scalar a0, Scalar a1, Scalar a2, Scalar a3) noexcept
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
Vu32::Vu32 (const std::tuple <Scalar, Scalar, Scalar, Scalar> &a) noexcept
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
void	Vu32::store (MEM *ptr) const noexcept
{
	assert (is_ptr_align_nz (ptr, fstb_SIMD128_ALIGN));

#if ! defined (fstb_HAS_SIMD)
	*reinterpret_cast <Vu32Native *> (ptr) = _x;
#elif fstb_ARCHI == fstb_ARCHI_X86
	_mm_store_si128 (reinterpret_cast <__m128i *> (ptr), _x);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	vst1q_u32 (reinterpret_cast <uint32_t *> (ptr), _x);
#endif // fstb_ARCHI
}



template <typename MEM>
void	Vu32::storeu (MEM *ptr) const noexcept
{
	assert (ptr != nullptr);

#if ! defined (fstb_HAS_SIMD)
	*reinterpret_cast <Vu32Native *> (ptr) = _x;
#elif fstb_ARCHI == fstb_ARCHI_X86
	_mm_storeu_si128 (reinterpret_cast <__m128i *> (ptr), _x);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	vst1q_u8 (reinterpret_cast <uint8_t *> (ptr), vreinterpretq_u8_u32 (_x));
#endif // fstb_ARCHI
}



// n = number of scalars to store (from the LSB)
template <typename MEM>
void	Vu32::storeu_part (MEM *ptr, int n) const noexcept
{
	assert (n > 0);

	if (n >= _length)
	{
		storeu (ptr);
		return;
	}

	uint32_t *      f_ptr = reinterpret_cast <uint32_t *> (ptr);

#if ! defined (fstb_HAS_SIMD)

	for (int i = 0; i < n; ++i)
	{
		f_ptr [i] = _x [i];
	}

#elif fstb_ARCHI == fstb_ARCHI_X86

	f_ptr [0] = uint32_t (_mm_cvtsi128_si32 (_x));
	if (n >= 2)
	{
		f_ptr [1] = uint32_t (_mm_cvtsi128_si32 (_mm_shuffle_epi32 (_x, 1 << 0)));
		if (n >= 3)
		{
			f_ptr [1] = uint32_t (_mm_cvtsi128_si32 (_mm_shuffle_epi32 (_x, 2 << 0)));
		}
	}

#elif fstb_ARCHI == fstb_ARCHI_ARM

	vst1q_lane_u32 (f_ptr + 0, _x, 0);
	if (n >= 2)
	{
		vst1q_lane_u32 (f_ptr + 1, _x, 1);
		if (n >= 3)
		{
			vst1q_lane_u32 (f_ptr + 2, _x, 2);
		}
	}

#endif
}



Vu32 &	Vu32::operator += (const Vu32Native &other) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	_x [0] += other [0];
	_x [1] += other [1];
	_x [2] += other [2];
	_x [3] += other [3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	_x = _mm_add_epi32 (_x, other);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	_x = vaddq_u32 (_x, other);
#endif // fstb_ARCHI
	return *this;
}



Vu32 &   Vu32::operator -= (const Vu32Native &other) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	_x [0] -= other [0];
	_x [1] -= other [1];
	_x [2] -= other [2];
	_x [3] -= other [3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	_x = _mm_sub_epi32 (_x, other);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	_x = vsubq_u32 (_x, other);
#endif // fstb_ARCHI
	return *this;
}



Vu32 &   Vu32::operator *= (const Vu32Native &other) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	_x [0] *= other [0];
	_x [1] *= other [1];
	_x [2] *= other [2];
	_x [3] *= other [3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	// mullo_epi32
	// Code of this function shamelessly borrowed from tp7
	// https://github.com/tp7/masktools/blob/16bit/masktools/common/simd.h
	const __m128i  lhs13  = _mm_shuffle_epi32 (_x, 0xF5);        // (-,a3,-,a1)
	const __m128i  rhs13  = _mm_shuffle_epi32 (other, 0xF5);     // (-,b3,-,b1)
	const __m128i  prod02 = _mm_mul_epu32 (_x, other);           // (-,a2*b2,-,a0*b0)
	const __m128i  prod13 = _mm_mul_epu32 (lhs13, rhs13);        // (-,a3*b3,-,a1*b1)
	const __m128i  prod01 = _mm_unpacklo_epi32 (prod02, prod13); // (-,-,a1*b1,a0*b0)
	const __m128i  prod23 = _mm_unpackhi_epi32 (prod02, prod13); // (-,-,a3*b3,a2*b2)
	_x                    = _mm_unpacklo_epi64 (prod01 ,prod23); // (ab3,ab2,ab1,ab0)
#elif fstb_ARCHI == fstb_ARCHI_ARM
	_x = vmulq_u32 (_x, other);
#endif // fstb_ARCHI
	return *this;
}



Vu32 &   Vu32::operator *= (const Scalar &other) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	_x [0] *= other;
	_x [1] *= other;
	_x [2] *= other;
	_x [3] *= other;
#elif fstb_ARCHI == fstb_ARCHI_X86
	const auto     vb = _mm_set1_epi32 (int32_t (other));
	auto           v0 = _mm_shuffle_epi32 (_x, (0<<0) | (1<<4));
	auto           v1 = _mm_shuffle_epi32 (_x, (2<<0) | (3<<4));
	v0 = _mm_mul_epu32 (v0, vb);
	v1 = _mm_mul_epu32 (v1, vb);
	_x = _mm_castps_si128 (_mm_shuffle_ps (
		_mm_castsi128_ps (v0),
		_mm_castsi128_ps (v1),
		(0<<0) | (2<<2) | (0<<4) | (2<<6)
	));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	_x = vmulq_u32 (_x, vdupq_n_u32 (other));
#endif // fstb_ARCHI
	return *this;
}



Vu32 &   Vu32::operator &= (const Vu32Native &other) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	_x [0] &= other [0];
	_x [1] &= other [1];
	_x [2] &= other [2];
	_x [3] &= other [3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	_x = _mm_and_si128 (_x, other);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	_x = vandq_u32 (_x, other);
#endif // fstb_ARCHI
	return *this;
}



Vu32 &   Vu32::operator |= (const Vu32Native &other) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	_x [0] |= other [0];
	_x [1] |= other [1];
	_x [2] |= other [2];
	_x [3] |= other [3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	_x = _mm_or_si128 (_x, other);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	_x = vorrq_u32 (_x, other);
#endif // fstb_ARCHI
	return *this;
}



Vu32 &   Vu32::operator ^= (const Vu32Native &other) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	_x [0] ^= other [0];
	_x [1] ^= other [1];
	_x [2] ^= other [2];
	_x [3] ^= other [3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	_x = _mm_xor_si128 (_x, other);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	_x = veorq_u32 (_x, other);
#endif // fstb_ARCHI
	return *this;
}



Vu32 &   Vu32::operator <<= (int imm) noexcept
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



Vu32 &   Vu32::operator >>= (int imm) noexcept
{
	assert (imm >= 0);
	assert (imm <= 32);
#if ! defined (fstb_HAS_SIMD)
	_x [0] >>= imm;
	_x [1] >>= imm;
	_x [2] >>= imm;
	_x [3] >>= imm;
#elif fstb_ARCHI == fstb_ARCHI_X86
	_x = _mm_srli_epi32 (_x, imm);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	_x >>= imm;
#endif // fstb_ARCHI
	return *this;
}



std::tuple <uint32_t, uint32_t, uint32_t, uint32_t>	Vu32::explode () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return std::make_tuple (_x [0], _x [1], _x [2], _x [3]);
#elif fstb_ARCHI == fstb_ARCHI_X86
	return std::make_tuple (
		uint32_t (_mm_cvtsi128_si32 (_x                            )),
		uint32_t (_mm_cvtsi128_si32 (_mm_shuffle_epi32 (_x, (1<<0)))),
		uint32_t (_mm_cvtsi128_si32 (_mm_shuffle_epi32 (_x, (2<<0)))),
		uint32_t (_mm_cvtsi128_si32 (_mm_shuffle_epi32 (_x, (3<<0))))
	);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return std::make_tuple (
		vgetq_lane_u32 (_x, 0),
		vgetq_lane_u32 (_x, 1),
		vgetq_lane_u32 (_x, 2),
		vgetq_lane_u32 (_x, 3)
	);
#endif // fstb_ARCHI
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ GLOBAL OPERATORS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Vu32 operator + (Vu32 lhs, const Vu32 &rhs) noexcept
{
	lhs += rhs;
	return lhs;
}

Vu32 operator - (Vu32 lhs, const Vu32 &rhs) noexcept
{
	lhs -= rhs;
	return lhs;
}

Vu32 operator * (Vu32 lhs, const Vu32 &rhs) noexcept
{
	lhs *= rhs;
	return lhs;
}

Vu32 operator * (Vu32 lhs, const Vu32::Scalar rhs) noexcept
{
	lhs *= rhs;
	return lhs;
}

Vu32 operator & (Vu32 lhs, const Vu32 &rhs) noexcept
{
	lhs &= rhs;
	return lhs;
}

Vu32 operator | (Vu32 lhs, const Vu32 &rhs) noexcept
{
	lhs |= rhs;
	return lhs;
}

Vu32 operator ^ (Vu32 lhs, const Vu32 &rhs) noexcept
{
	lhs ^= rhs;
	return lhs;
}



template <typename T>
Vu32 operator << (Vu32 lhs, T rhs) noexcept
{
	lhs <<= rhs;
	return lhs;
}

template <typename T>
Vu32 operator >> (Vu32 lhs, T rhs) noexcept
{
	lhs >>= rhs;
	return lhs;
}



}  // namespace fstb



#endif   // fstb_Vu32_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
