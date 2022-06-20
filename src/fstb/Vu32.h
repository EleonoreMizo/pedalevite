/*****************************************************************************

        Vu32.h
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (fstb_Vu32_HEADER_INCLUDED)
#define fstb_Vu32_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#if ! defined (fstb_HAS_SIMD)
	#include <array>
#elif (fstb_ARCHI == fstb_ARCHI_X86)
	#include <emmintrin.h>
#elif (fstb_ARCHI == fstb_ARCHI_ARM)
	#include <arm_neon.h>
#else
	#error
#endif

#include <tuple>

#include <cstdint>



namespace fstb
{



#if ! defined (fstb_HAS_SIMD)

typedef std::array <uint32_t, 4> Vu32Native;

#elif fstb_ARCHI == fstb_ARCHI_X86

typedef __m128i   Vu32Native;

#elif fstb_ARCHI == fstb_ARCHI_ARM

typedef uint32x4_t Vu32Native;

#else // fstb_ARCHI
#error
#endif // fstb_ARCHI



class Vu32
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static constexpr int _len_l2 = 2;
	static constexpr int _length = 1 << _len_l2;
	typedef uint32_t Scalar;

	               Vu32 ()                        = default;
   fstb_FORCEINLINE
	               Vu32 (Vu32Native a) noexcept : _x { a } {}
   explicit fstb_FORCEINLINE
	               Vu32 (Scalar a) noexcept;
	explicit fstb_FORCEINLINE
	               Vu32 (Scalar a0, Scalar a1, Scalar a2, Scalar a3) noexcept;
	explicit fstb_FORCEINLINE
	               Vu32 (const std::tuple <Scalar, Scalar, Scalar, Scalar> &a) noexcept;
	               Vu32 (const Vu32 &other)       = default;
	               Vu32 (Vu32 &&other)            = default;
	               ~Vu32 ()                       = default;
	Vu32 &         operator = (const Vu32 &other) = default;
	Vu32 &         operator = (Vu32 &&other)      = default;

	template <typename MEM>
	fstb_FORCEINLINE void
	               store (MEM *ptr) const noexcept;
	template <typename MEM>
	fstb_FORCEINLINE void
	               storeu (MEM *ptr) const noexcept;
	template <typename MEM>
	fstb_FORCEINLINE void
	               storeu_part (MEM *ptr, int n) const noexcept;

	fstb_FORCEINLINE
	               operator Vu32Native () const noexcept { return _x; }
	fstb_FORCEINLINE explicit
	               operator bool () const noexcept;

	fstb_FORCEINLINE Vu32 &
	               operator += (const Vu32Native &other) noexcept;
	fstb_FORCEINLINE Vu32 &
	               operator -= (const Vu32Native &other) noexcept;
	fstb_FORCEINLINE Vu32 &
	               operator *= (const Vu32Native &other) noexcept;
	fstb_FORCEINLINE Vu32 &
	               operator *= (const Scalar &other) noexcept;

	fstb_FORCEINLINE Vu32 &
	               operator &= (const Vu32Native &other) noexcept;
	fstb_FORCEINLINE Vu32 &
	               operator |= (const Vu32Native &other) noexcept;
	fstb_FORCEINLINE Vu32 &
	               operator ^= (const Vu32Native &other) noexcept;

	fstb_FORCEINLINE Vu32 &
	               operator <<= (int imm) noexcept;
	fstb_FORCEINLINE Vu32 &
	               operator >>= (int imm) noexcept;

	fstb_FORCEINLINE Vu32
	               operator - () const noexcept;
	fstb_FORCEINLINE Vu32
	               operator ~ () const noexcept;
	fstb_FORCEINLINE Vu32
	               reverse () const noexcept;

	template <int SHIFT>
	fstb_FORCEINLINE Vu32
	               rotate () const noexcept;
	template <int POS>
	fstb_FORCEINLINE uint32_t
	               extract () const noexcept;
	template <int POS>
	fstb_FORCEINLINE Vu32
	               insert (uint32_t val) const noexcept;
	template <int POS>
	fstb_FORCEINLINE Vu32
	               spread () const noexcept;

	fstb_FORCEINLINE std::tuple <uint32_t, uint32_t, uint32_t, uint32_t>
	               explode () const noexcept;

	fstb_FORCEINLINE uint32_t
	               sum_h () const noexcept;
	fstb_FORCEINLINE uint32_t
	               min_h () const noexcept;
	fstb_FORCEINLINE uint32_t
	               max_h () const noexcept;

	fstb_FORCEINLINE bool
	               and_h () const noexcept;
	fstb_FORCEINLINE bool
	               or_h () const noexcept;
	fstb_FORCEINLINE unsigned int
	               movemask () const noexcept;
	fstb_FORCEINLINE int
	               count_bits () const noexcept;

	static fstb_FORCEINLINE Vu32
	               zero () noexcept;
	static fstb_FORCEINLINE Vu32
	               all1 () noexcept;
	static fstb_FORCEINLINE Vu32
	               set_mask (bool m0, bool m1, bool m2, bool m3) noexcept;
	template <int POS>
	static fstb_FORCEINLINE Vu32
	               compose (Vu32 a, Vu32 b) noexcept;
	static fstb_FORCEINLINE Vu32
	               flip_msb (Vu32 x) noexcept;

	template <typename MEM>
	static fstb_FORCEINLINE Vu32
	               load (const MEM *ptr) noexcept;
	template <typename MEM>
	static fstb_FORCEINLINE Vu32
	               loadu (const MEM *ptr) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

#if ! defined (fstb_HAS_SIMD)
public:
#endif
	Vu32Native  _x;
private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

}; // class Vu32



/*\\\ GLOBAL OPERATORS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



fstb_FORCEINLINE Vu32 operator + (Vu32 lhs, const Vu32 &rhs) noexcept;
fstb_FORCEINLINE Vu32 operator - (Vu32 lhs, const Vu32 &rhs) noexcept;
fstb_FORCEINLINE Vu32 operator * (Vu32 lhs, const Vu32 &rhs) noexcept;
fstb_FORCEINLINE Vu32 operator * (Vu32 lhs, const Vu32::Scalar rhs) noexcept;
fstb_FORCEINLINE Vu32 operator & (Vu32 lhs, const Vu32 &rhs) noexcept;
fstb_FORCEINLINE Vu32 operator | (Vu32 lhs, const Vu32 &rhs) noexcept;
fstb_FORCEINLINE Vu32 operator ^ (Vu32 lhs, const Vu32 &rhs) noexcept;

template <typename T>
fstb_FORCEINLINE Vu32 operator << (Vu32 lhs, T rhs) noexcept;
template <typename T>
fstb_FORCEINLINE Vu32 operator >> (Vu32 lhs, T rhs) noexcept;

fstb_FORCEINLINE Vu32 operator == (const Vu32 &lhs, const Vu32 &rhs) noexcept;
fstb_FORCEINLINE Vu32 operator != (const Vu32 &lhs, const Vu32 &rhs) noexcept;
fstb_FORCEINLINE Vu32 operator <  (const Vu32 &lhs, const Vu32 &rhs) noexcept;
fstb_FORCEINLINE Vu32 operator <= (const Vu32 &lhs, const Vu32 &rhs) noexcept;
fstb_FORCEINLINE Vu32 operator >  (const Vu32 &lhs, const Vu32 &rhs) noexcept;
fstb_FORCEINLINE Vu32 operator >= (const Vu32 &lhs, const Vu32 &rhs) noexcept;

fstb_FORCEINLINE Vu32 min (const Vu32 &lhs, const Vu32 &rhs) noexcept;
fstb_FORCEINLINE Vu32 max (const Vu32 &lhs, const Vu32 &rhs) noexcept;
fstb_FORCEINLINE Vu32 limit (const Vu32 &v, const Vu32 &mi, const Vu32 &ma) noexcept;
fstb_FORCEINLINE Vu32 select (const Vu32 &cond, const Vu32 &v_t, const Vu32 &v_f) noexcept;
fstb_FORCEINLINE std::tuple <Vu32, Vu32> swap_if (const Vu32 &cond, Vu32 lhs, Vu32 rhs) noexcept;



}  // namespace fstb



#include "fstb/Vu32.hpp"



#endif   // fstb_Vu32_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
