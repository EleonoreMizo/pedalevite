/*****************************************************************************

        WsProgClipper.h
        Author: Laurent de Soras, 2020

Clipping with progressive curve.
https://www.desmos.com/calculator/2abzjtlqhq

Template parameters:

- A: 1st shaper control (closest to 0, fastest curve, slope 1) as a std::ratio
	The smaller A, the longer the first shaper.
	Range: [0 ; 1]

- B: 2nd shaper control (slowest curve, from the end of the first shaper up to
	saturation), as a std::ratio
	The higher B, the slowest the curve
	B should be > 0

- C is the amount of additional ripples at 0, as a std::ratio.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_shape_WsProgClipper_HEADER_INCLUDED)
#define mfx_dsp_shape_WsProgClipper_HEADER_INCLUDED



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



template <typename A, typename B, typename C>
class WsProgClipper
{
	static_assert (
		std::ratio_greater_equal <A, std::ratio <0, 1> >::value,
		"WsProgClipper: A in [0 ; 1]"
	);
	static_assert (
		std::ratio_less_equal <A, std::ratio <1, 1> >::value,
		"WsProgClipper: A in [0 ; 1]"
	);
	static_assert (
		std::ratio_greater <B, std::ratio <0, 1> >::value,
		"WsProgClipper: B > 0"
	);

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	double         operator () (double x) noexcept
	{
		const double   a  = double (A::num) / double (A::den);
		const double   bi = double (B::den) / double (B::num);
		const double   c  = double (C::num) / double (C::den);
		const double   f1 = tanh (x * bi);
		const double   f2 = (2 / fstb::PI) * atan (fstb::PI * 0.5 * x * (1 / (1 - a) - bi));
		const double   x3 = x * x * x;
		const double   z  = fabs (x * 2) + 1;
		const double   z2 = z * z;
		const double   z5 = z2 * z2 * z;
		const double   f3 = 8 * x3 / z5;
		const double   y  = a * f1 + (1 - a) * f2 - c * f3;
		return y;
	}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

}; // class WsProgClipper



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/shape/WsProgClipper.hpp"



#endif   // mfx_dsp_shape_WsProgClipper_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
