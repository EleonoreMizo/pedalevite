/*****************************************************************************

        WsNegCond.h
        Author: Laurent de Soras, 2020

Template parameters:

- LB: lower bound of the negated range as std::ratio

- UB: upper bound of the negated range as std::ratio

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_shape_WsNegCond_HEADER_INCLUDED)
#define mfx_dsp_shape_WsNegCond_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#include <ratio>

#include <cmath>



namespace mfx
{
namespace dsp
{
namespace shape
{



template <class LB, class UB>
class WsNegCond
{
	static_assert (
		std::ratio_greater_equal <LB, std::ratio <0, 1> >::value,
		"WsNegCond: LB >= 0"
	);
	static_assert (
		std::ratio_greater_equal <UB, LB>::value,
		"WsNegCond: LB <= UB"
	);

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	template <typename T>
	fstb_FORCEINLINE T operator () (T x)
	{
		const T        lb = T (LB::num) / T (LB::den);
		const T        ub = T (UB::num) / T (UB::den);
		const T        xa = T (fabs (x));
		if (xa >= lb && xa < ub)
		{
			x = -x;
		}
		return x;
	}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

}; // class WsNegCond



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/shape/WsNegCond.hpp"



#endif   // mfx_dsp_shape_WsNegCond_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
