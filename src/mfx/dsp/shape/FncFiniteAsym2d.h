/*****************************************************************************

        FncFiniteAsym2d.h
        Author: Laurent de Soras, 2019

Interpolates a function z = f (x, y) at x and y on a given range via a
mapping on a unitary grid.

Template parameters:

- XL, XU: lower and upper bounds of the grid in the X direction

- YL, YU: lower and upper bounds of the grid in the Y direction

- GF: Functor of the function to interpolate. Requires:
	double GF::operator () (double x, double y);

Possible optimisation:

- Use the formula a + x * (b - a) instead of a * (1-x) + b * x to reduce the
number of multiplies. Maybe we could also store a b-a table.
- Use a table with one more element in each dimension to avoid excessive
bound check code.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_shape_FncFiniteAsym2d_HEADER_INCLUDED)
#define mfx_dsp_shape_FncFiniteAsym2d_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace shape
{



template <int XL, int XU, int YL, int YU, class GF>
class FncFiniteAsym2d
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef GF GenFtor;

	               FncFiniteAsym2d ();
	virtual        ~FncFiniteAsym2d () = default;

	static void    init_coef ();

	inline float   operator () (float x, float y) const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  X_LEN = XU - XL + 1;
	static const int  Y_LEN = YU - YL + 1;

	static float	_coef [Y_LEN] [X_LEN];
	static bool		_coef_init_flag;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               FncFiniteAsym2d (const FncFiniteAsym2d &other)   = delete;
	FncFiniteAsym2d &
	               operator = (const FncFiniteAsym2d &other)        = delete;
	bool           operator == (const FncFiniteAsym2d &other) const = delete;
	bool           operator != (const FncFiniteAsym2d &other) const = delete;

}; // class FncFiniteAsym2d



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/shape/FncFiniteAsym2d.hpp"



#endif   // mfx_dsp_shape_FncFiniteAsym2d_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
