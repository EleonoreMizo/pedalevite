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
#if ! defined (fstb_VectU32_HEADER_INCLUDED)
#define fstb_VectU32_HEADER_INCLUDED



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
	               Vu32 (const Vu32 &other)       = default;
	               Vu32 (Vu32 &&other)            = default;
	               ~Vu32 ()                       = default;
	Vu32 &         operator = (const Vu32 &other) = default;
	Vu32 &         operator = (Vu32 &&other)      = default;

	fstb_FORCEINLINE
	               operator Vu32Native () const noexcept { return _x; }

	fstb_FORCEINLINE Vu32 &
	               operator += (const Vu32Native &other) noexcept;
	fstb_FORCEINLINE Vu32 &
	               operator -= (const Vu32Native &other) noexcept;
	fstb_FORCEINLINE Vu32 &
	               operator *= (const Vu32Native &other) noexcept;

	fstb_FORCEINLINE Vu32 &
	               operator <<= (int imm) noexcept;
	fstb_FORCEINLINE Vu32 &
	               operator >>= (int imm) noexcept;



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

	bool           operator == (const Vu32 &other) const = delete;
	bool           operator != (const Vu32 &other) const = delete;

}; // class Vu32



/*\\\ GLOBAL OPERATORS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



fstb_FORCEINLINE Vu32 operator + (Vu32 lhs, const Vu32 &rhs) noexcept;
fstb_FORCEINLINE Vu32 operator - (Vu32 lhs, const Vu32 &rhs) noexcept;
fstb_FORCEINLINE Vu32 operator * (Vu32 lhs, const Vu32 &rhs) noexcept;

template <typename T>
fstb_FORCEINLINE Vu32 operator << (Vu32 lhs, T rhs) noexcept;
template <typename T>
fstb_FORCEINLINE Vu32 operator >> (Vu32 lhs, T rhs) noexcept;



}  // namespace fstb



#include "fstb/Vu32.hpp"



#endif   // fstb_VectU32_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
