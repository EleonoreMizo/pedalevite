/*****************************************************************************

        Scale.h
        Author: Laurent de Soras, 2019

Scales a number with another number. Typically this is just a multiplication
(for floating point or similar types), but integers are considered like
fixed-point data. For example, using int32_t with 1 << 30 as nominal "1"
level, the operation would be equivalent to:

	result = int32_t ((int64_t (a) * int64_t (b)) >> 30);

Full precision of the operation is not guaranteed.

Template parameters:

- T: type to scale. May be interger or floating point, or any object with
	a *= operator. Integers may be signed or not.

- LL2: bitdepth of the nominal level (1 << LL2), for integer types.
	Ignored with other types.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (fstb_Scale_HEADER_INCLUDED)
#define fstb_Scale_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"



namespace fstb
{



template <int LL2 = 0>
class Scale
{
public:
	template <typename T>
	static fstb_FORCEINLINE T
	               mul (T a, T b) noexcept;
}; // class Scale



}  // namespace fstb



#include "fstb/Scale.hpp"



#endif   // fstb_Scale_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
