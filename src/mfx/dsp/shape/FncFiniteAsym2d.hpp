/*****************************************************************************

        FncFiniteAsym2d.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_shape_FncFiniteAsym2d_CODEHEADER_INCLUDED)
#define mfx_dsp_shape_FncFiniteAsym2d_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace shape
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int XL, int XU, int YL, int YU, class GF>
FncFiniteAsym2d <XL, XU, YL, YU, GF>::FncFiniteAsym2d () noexcept
{
	init_coef ();
}



template <int XL, int XU, int YL, int YU, class GF>
void	FncFiniteAsym2d <XL, XU, YL, YU, GF>::init_coef () noexcept
{
	if (! _coef_init_flag)
	{
		GenFtor        fnc;

		for (int y_pos = 0; y_pos < Y_LEN; ++y_pos)
		{
			const double   y = y_pos + YL;
			for (int x_pos = 0; x_pos < X_LEN; ++x_pos)
			{
				const double   x = x_pos + XL;
				_coef [y_pos] [x_pos] = fnc (x, y);
			}
		}

		_coef_init_flag = true;
	}
}



template <int XL, int XU, int YL, int YU, class GF>
float	FncFiniteAsym2d <XL, XU, YL, YU, GF>::operator () (float x, float y) const noexcept
{
   assert (_coef_init_flag);

	const int      x_last_index = X_LEN - 1;
	const int      y_last_index = Y_LEN - 1;
	const int      x_max_index  = x_last_index - 1;
	const int      y_max_index  = y_last_index - 1;

	const float    x_pos = fstb::limit (x - XL, 0.0f, float (x_last_index));
	const float    y_pos = fstb::limit (y - YL, 0.0f, float (y_last_index));

	const int      x_offset = std::min (fstb::floor_int (x_pos), x_max_index);
	const int      y_offset = std::min (fstb::floor_int (y_pos), y_max_index);
	assert (x_offset >= 0);	
	assert (y_offset >= 0);	

	const float    x_fact = x_pos - x_offset;
	const float    y_fact = y_pos - y_offset;
	assert (x_fact >= 0 && x_fact <= 1);	// equals 1 only if x_pos == X_LEN 
	assert (y_fact >= 0 && y_fact <= 1);	// equals 1 only if x_pos == X_LEN 

	const int      x_offset_next = x_offset + 1;
	const int      y_offset_next = y_offset + 1;
	assert (x_offset_next < X_LEN);	
	assert (y_offset_next < Y_LEN);	

	const float    x_fact_i = 1 - x_fact;
	const float    y_fact_i = 1 - y_fact;

	const float    a =   x_fact_i * _coef [y_offset     ] [x_offset     ]
	                   + x_fact   * _coef [y_offset     ] [x_offset_next];
	const float    b =   x_fact_i * _coef [y_offset_next] [x_offset     ]
	                   + x_fact   * _coef [y_offset_next] [x_offset_next];

	return y_fact_i * a + y_fact * b;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_shape_FncFiniteAsym2d_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
