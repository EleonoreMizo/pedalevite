/*****************************************************************************

        WsAsym13.hpp
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_shape_WsAsym13_CODEHEADER_INCLUDED)
#define mfx_dsp_shape_WsAsym13_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"



namespace mfx
{
namespace dsp
{
namespace shape
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



double	WsAsym13::operator () (double x) const
{
	static const double	limit_h = 1.0954451150103322269139395656016;	// sqrt (6) / sqrt (5)
	static const double	sqrt2_over_sqrt5 = 0.63245553203367586639977870888654;
	static const double	x_slope_max = sqrt2_over_sqrt5;	// sqrt (2) / sqrt (5)
	static const double	y_slope_max = sqrt2_over_sqrt5 * 16.0 / 25.0;
	static const double	slope_max = 8.0 / 5.0;

	x *= -0.5 / slope_max;
	x += x_slope_max;
	x = fstb::limit (x, 0.0, limit_h);

	double			y = fnc (x);

	y -= y_slope_max;
	y *= -2;

	return (y);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// y = 2 * x^3 - x^5
double	WsAsym13::fnc (double x)
{
	const double	x2 = x * x;
	const double	y = x * x2 * (2 - x2);

	return (y);
}



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_shape_WsAsym13_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
