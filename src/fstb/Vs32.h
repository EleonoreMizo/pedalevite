/*****************************************************************************

        Vs32.h
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (fstb_VectS32_HEADER_INCLUDED)
#define fstb_VectS32_HEADER_INCLUDED



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

#include <cstdint>



namespace fstb
{



#if ! defined (fstb_HAS_SIMD)

typedef std::array <int32_t, 4> Vs32Native;

#elif fstb_ARCHI == fstb_ARCHI_X86

typedef __m128i   Vs32Native;

#elif fstb_ARCHI == fstb_ARCHI_ARM

typedef int32x4_t Vs32Native;

#else // fstb_ARCHI
#error
#endif // fstb_ARCHI



class Vs32
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef int32_t Scalar;

	               Vs32 ()                        = default;
   fstb_FORCEINLINE
	               Vs32 (Vs32Native a) noexcept : _x { a } {}
   explicit fstb_FORCEINLINE
	               Vs32 (Scalar a) noexcept;
	explicit fstb_FORCEINLINE
	               Vs32 (Scalar a0, Scalar a1, Scalar a2, Scalar a3) noexcept;
	               Vs32 (const Vs32 &other)       = default;
	               Vs32 (Vs32 &&other)            = default;
	               ~Vs32 ()                       = default;
	Vs32 &         operator = (const Vs32 &other) = default;
	Vs32 &         operator = (Vs32 &&other)      = default;

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
	               operator Vs32Native () const noexcept { return _x; }

	fstb_FORCEINLINE Vs32 &
	               operator += (const Vs32Native &other) noexcept;
	fstb_FORCEINLINE Vs32 &
	               operator -= (const Vs32Native &other) noexcept;
	fstb_FORCEINLINE Vs32 &
	               operator *= (const Vs32Native &other) noexcept;

	fstb_FORCEINLINE Vs32 &
	               operator <<= (int imm) noexcept;
	fstb_FORCEINLINE Vs32 &
	               operator >>= (int imm) noexcept;

	static fstb_FORCEINLINE Vs32
	               zero () noexcept;

	template <typename MEM>
	static fstb_FORCEINLINE Vs32
	               load (const MEM *ptr) noexcept;
	template <typename MEM>
	static fstb_FORCEINLINE Vs32
	               loadu (const MEM *ptr) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

#if ! defined (fstb_HAS_SIMD)
public:
#endif
	Vs32Native  _x;
private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Vs32 &other) const = delete;
	bool           operator != (const Vs32 &other) const = delete;

}; // class Vs32



/*\\\ GLOBAL OPERATORS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



fstb_FORCEINLINE Vs32 operator + (Vs32 lhs, const Vs32 &rhs) noexcept;
fstb_FORCEINLINE Vs32 operator - (Vs32 lhs, const Vs32 &rhs) noexcept;
fstb_FORCEINLINE Vs32 operator * (Vs32 lhs, const Vs32 &rhs) noexcept;

template <typename T>
fstb_FORCEINLINE Vs32 operator << (Vs32 lhs, T rhs) noexcept;
template <typename T>
fstb_FORCEINLINE Vs32 operator >> (Vs32 lhs, T rhs) noexcept;



}  // namespace fstb



#include "fstb/Vs32.hpp"



#endif   // fstb_VectS32_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
