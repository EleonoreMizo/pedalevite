/*****************************************************************************

        WsPuncherB.h
        Author: Laurent de Soras, 2020

https://www.desmos.com/calculator/2y1tw4wque

Template parameters:

- N: number of additional cycles, >= 0

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_shape_WsPuncherB_HEADER_INCLUDED)
#define mfx_dsp_shape_WsPuncherB_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/fnc.h"

#include <cmath>



namespace mfx
{
namespace dsp
{
namespace shape
{



template <int N>
class WsPuncherB
{
	static_assert ((N >= 0), "WsPuncherB: N >= 0");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	double         operator () (double x) noexcept
	{
		const double   m  = 20; // Range
		const double   z  = log (m + 1);
		const double   a  = ((1.5 + N) * fstb::PI - z) / (z * z);
		const double   xx = fstb::limit (x, -m, +m);
		const double   u  = log (fabs (xx) + 1);
		const double   f  = sin ((a * u + 1) * u);

		return (x < 0) ? -f : f;
	}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

}; // class WsPuncherB



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/shape/WsPuncherB.hpp"



#endif   // mfx_dsp_shape_WsPuncherB_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
