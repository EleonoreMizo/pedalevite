/*****************************************************************************

        Vf32.h
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (fstb_VectF32_HEADER_INCLUDED)
#define fstb_VectF32_HEADER_INCLUDED



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

typedef std::array <float, 4> Vf32Native;

#elif fstb_ARCHI == fstb_ARCHI_X86

typedef __m128      Vf32Native;

#elif fstb_ARCHI == fstb_ARCHI_ARM

typedef float32x4_t Vf32Native;

#else // fstb_ARCHI
#error
#endif // fstb_ARCHI



class Vf32
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef float Scalar;

	               Vf32 ()                        = default;
   fstb_FORCEINLINE
	               Vf32 (Vf32Native a) noexcept : _x { a } {}
   explicit fstb_FORCEINLINE
	               Vf32 (Scalar a) noexcept;
	               Vf32 (const Vf32 &other)       = default;
	               Vf32 (Vf32 &&other)            = default;
	               ~Vf32 ()                       = default;
	Vf32 &         operator = (const Vf32 &other) = default;
	Vf32 &         operator = (Vf32 &&other)      = default;

	fstb_FORCEINLINE
	               operator Vf32Native () const noexcept { return _x; }

	fstb_FORCEINLINE Vf32 &
	               operator += (const Vf32Native &other) noexcept;
	fstb_FORCEINLINE Vf32 &
	               operator -= (const Vf32Native &other) noexcept;
	fstb_FORCEINLINE Vf32 &
	               operator *= (const Vf32Native &other) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

#if ! defined (fstb_HAS_SIMD)
public:
#endif
	Vf32Native  _x;
private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Vf32 &other) const = delete;
	bool           operator != (const Vf32 &other) const = delete;

}; // class Vf32



/*\\\ GLOBAL OPERATORS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



fstb_FORCEINLINE Vf32 operator + (Vf32 lhs, const Vf32 &rhs) noexcept;
fstb_FORCEINLINE Vf32 operator - (Vf32 lhs, const Vf32 &rhs) noexcept;
fstb_FORCEINLINE Vf32 operator * (Vf32 lhs, const Vf32 &rhs) noexcept;



}  // namespace fstb



#include "fstb/Vf32.hpp"



#endif   // fstb_VectF32_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
