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



#if ! defined (fstb_VectS32_CODEHEADER_INCLUDED)
#define fstb_VectS32_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

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



Vs32 &   Vs32::operator -= (const Vs32Native &other) noexcept
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



Vs32 &   Vs32::operator *= (const Vs32Native &other) noexcept
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
	_x = vmulq_s32 (_x, other);
#endif // fstb_ARCHI
	return *this;
}



Vs32 &   Vs32::operator <<= (int imm) noexcept
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



Vs32 &   Vs32::operator >>= (int imm) noexcept
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



Vs32	Vs32::zero () noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vs32 { { 0, 0, 0, 0 } };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_setzero_si128 ();
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vdupq_n_s32 (0);
#endif // fstb_ARCHI
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ GLOBAL OPERATORS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



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



}  // namespace fstb



#endif   // fstb_VectS32_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
