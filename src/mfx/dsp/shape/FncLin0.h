/*****************************************************************************

        FncLin0.h
        Author: Laurent de Soras, 2020

Inserts a linear segment at 0 in an existing shaper.
Shaper is assumed to reach unity at high input absolute values.
https://www.desmos.com/calculator/96rydakrcg

Template parameters:

- F: shaper function, C1 at 0, F(0) = 0 and F'(0) = 1. Requires:
   F::F ()
   ~F::F ()
   double F::operator () (double);

- R: amplitude of the linear part, as a std::ratio template.
	Must be in [0 ; 1[

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_shape_FncLin0_HEADER_INCLUDED)
#define mfx_dsp_shape_FncLin0_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"

#include <ratio>



namespace mfx
{
namespace dsp
{
namespace shape
{



template <typename F, typename R>
class FncLin0
{
	static_assert (
		std::ratio_greater_equal <R, std::ratio <0, 1> >::value,
		"FncLin0: R >= 0"
	);
	static_assert (
		std::ratio_less <R, std::ratio <1, 1> >::value,
		"FncLin0: R < 1"
	);

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	double         operator () (double x) noexcept
	{
		F              f;
		const double   r  = double (R::num) / double (R::den);
		const double   xl = fstb::limit (x, -r, r);
		const double   xs = (x - xl) / (1 - r);
		return (1 - r) * f (xs) + xl;
	}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

}; // class FncLin0



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/shape/FncLin0.hpp"



#endif   // mfx_dsp_shape_FncLin0_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
