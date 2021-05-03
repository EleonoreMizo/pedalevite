/*****************************************************************************

        WsAsym2.h
        Author: Laurent de Soras, 2020

Minimum input range: [-256; +8]
https://www.musicdsp.org/en/latest/Effects/86-waveshaper-gloubi-boulga.html
https://www.desmos.com/calculator/1ewo3vxqmy

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_shape_WsAsym2_HEADER_INCLUDED)
#define mfx_dsp_shape_WsAsym2_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cmath>



namespace mfx
{
namespace dsp
{
namespace shape
{



class WsAsym2
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

		double         operator () (double x) noexcept
		{
			x *= 2.0 / 3.0;
			const double   a = 1 + exp (-0.75 * sqrt (fabs (x)));
#if 1
			const double   e = exp (x);
			const double   n = e * (e - exp (-x * a));
			const double   d = e * e + 1;
#else
			const double   n = exp (x) - exp (-a * x);
			const double   d = exp (x) + exp (-x);
#endif
			return n / d;
		}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

}; // class WsAsym2



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/shape/WsAsym2.hpp"



#endif   // mfx_dsp_shape_WsAsym2_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
