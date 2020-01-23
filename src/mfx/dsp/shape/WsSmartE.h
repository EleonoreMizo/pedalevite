/*****************************************************************************

        WsSmartE.h
        Author: Laurent de Soras, 2020

https://www.desmos.com/calculator/iv27lizdcd

Template parameters:

- A: smoothness/crossover factor as a std::ratio. A > -1

- B: scaling parameter as a std::ratio. B >= 1

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_shape_WsSmartE_HEADER_INCLUDED)
#define mfx_dsp_shape_WsSmartE_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <ratio>

#include <cmath>



namespace mfx
{
namespace dsp
{
namespace shape
{



template <typename A, typename B>
class WsSmartE
{
	static_assert (
		std::ratio_greater <A, std::ratio <-1, 1> >::value,
		"WsSmartE: A > -1"
	);
	static_assert (
		std::ratio_greater_equal <B, std::ratio <1, 1> >::value,
		"WsSmartE: B >= 1"
	);


/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	double         operator () (double x)
	{
		const double   a    = double (A::num) / double (A::den);
		const double   b    = double (B::num) / double (B::den);
		const double   xabs = fabs (x);
		const double   bx   = b * x;
		const double   num  = bx * (xabs + a);
		const double   den  = bx * x + xabs * (a - 1) + 1;

		return num / den;
	}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

}; // class WsSmartE



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/shape/WsSmartE.hpp"



#endif   // mfx_dsp_shape_WsSmartE_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
