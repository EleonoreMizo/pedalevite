/*****************************************************************************

        Poly.h
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (fstb_Poly_HEADER_INCLUDED)
#define fstb_Poly_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/Vf32.h"



namespace fstb
{



class Poly
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	template <class T>
	fstb_FORCEINLINE static constexpr T
	               horner (T x, T c0, T c1) noexcept;
	template <class T>
	fstb_FORCEINLINE static constexpr T
	               horner (T x, T c0, T c1, T c2) noexcept;
	template <class T>
	fstb_FORCEINLINE static constexpr T
	               horner (T x, T c0, T c1, T c2, T c3) noexcept;
	template <class T>
	fstb_FORCEINLINE static constexpr T
	               horner (T x, T c0, T c1, T c2, T c3, T c4) noexcept;
	template <class T>
	fstb_FORCEINLINE static constexpr T
	               horner (T x, T c0, T c1, T c2, T c3, T c4, T c5) noexcept;
	template <class T>
	fstb_FORCEINLINE static constexpr T
	               horner (T x, T c0, T c1, T c2, T c3, T c4, T c5, T c6) noexcept;
	template <class T>
	fstb_FORCEINLINE static constexpr T
	               horner (T x, T c0, T c1, T c2, T c3, T c4, T c5, T c6, T c7) noexcept;

	template <class T>
	fstb_FORCEINLINE static constexpr T
	               estrin (T x, T c0, T c1, T c2, T c3) noexcept;
	template <class T>
	fstb_FORCEINLINE static constexpr T
	               estrin (T x, T c0, T c1, T c2, T c3, T c4) noexcept;
	template <class T>
	fstb_FORCEINLINE static constexpr T
	               estrin (T x, T c0, T c1, T c2, T c3, T c4, T c5) noexcept;
	template <class T>
	fstb_FORCEINLINE static constexpr T
	               estrin (T x, T c0, T c1, T c2, T c3, T c4, T c5, T c6) noexcept;
	template <class T>
	fstb_FORCEINLINE static constexpr T
	               estrin (T x, T c0, T c1, T c2, T c3, T c4, T c5, T c6, T c7) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	template <class T>
	fstb_FORCEINLINE static constexpr T
	               fma (T x, T a, T b) noexcept;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Poly ()                               = delete;
	               Poly (const Poly &other)              = delete;
	               Poly (Poly &&other)                   = delete;
	Poly &         operator = (const Poly &other)        = delete;
	Poly &         operator = (Poly &&other)             = delete;
	bool           operator == (const Poly &other) const = delete;
	bool           operator != (const Poly &other) const = delete;

}; // class Poly



}  // namespace fstb



#include "fstb/Poly.hpp"



#endif   // fstb_Poly_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
