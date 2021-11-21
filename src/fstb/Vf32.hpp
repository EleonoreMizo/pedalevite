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

#include <cassert>



namespace fstb
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



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



Vf32 &   Vf32::operator -= (const Vf32Native &other) noexcept
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



Vf32 &   Vf32::operator *= (const Vf32Native &other) noexcept
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



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



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



}  // namespace fstb



#endif   // fstb_VectF32_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
