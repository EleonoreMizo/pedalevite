/*****************************************************************************

        FncScale.h
        Author: Laurent de Soras, 2020

Scales a function, keeping the same slope at 0.

Template parameters:

- F: shaper function, F(0) = 0. Requires:
   F::F ()
   ~F::F ()
   double F::operator () (double);

- S: Scaling factor, as a std::ratio.
	Cannot be 0.
	Absolute values greater than 1 make the curve larger
	Negative values invert the curve (quadrants 0 and 2).

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_shape_FncScale_HEADER_INCLUDED)
#define mfx_dsp_shape_FncScale_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"

#include <ratio>



namespace mfx
{
namespace dsp
{
namespace shape
{



template <typename F, typename S>
class FncScale
{
	static_assert (
		std::ratio_not_equal <S, std::ratio <0, 1> >::value,
		"FncScale: S != 0"
	);

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	double         operator () (double x)
	{
		F              f;
		const double   sd = double (S::num) / double (S::den);
		const double   si = double (S::den) / double (S::num);

		return sd * f (si * x);
	}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

}; // class FncScale



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/shape/FncScale.hpp"



#endif   // mfx_dsp_shape_FncScale_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
