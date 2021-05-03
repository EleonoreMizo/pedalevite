/*****************************************************************************

        DolphCheby.hpp
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_wnd_DolphCheby_CODEHEADER_INCLUDED)
#define mfx_dsp_wnd_DolphCheby_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace wnd
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: set_ripple_ratio
Description:
	Sets the ripple ratio for the window.
Input parameters:
	- r: Ripple ratio (decimal), in ]0 ; 1[.
Throws: Nothing
==============================================================================
*/

template <class T>
void	DolphCheby <T>::set_ripple_ratio (double r) noexcept
{
	assert (r > 0);
	assert (r < 1);

	_r = r;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class T>
void	DolphCheby <T>::do_make_win (T data_ptr [], int len)
{
	const int      m         = (len - 1) / 2;
	const int      n         = m * 2 + 1;
	assert (n <= len);
	const int      center    = len - m - 1;
	const double   theta_mul = 2 * fstb::PI / n;
	const double   x_0       = cosh (acosh (1 / _r) / (m * 2));
	double         scale     = 0;

	data_ptr [     0] = T (0);
	data_ptr [center] = T (1);
	for (int k = 0; k <= m; ++k)
	{
		const double   theta_k = theta_mul * k;
		double         s       = 0;
		for (int p = 1; p <= m; ++p)
		{
			const double	theta_p = theta_mul * p;
			const double	t_2p    = cheby_poly (x_0 * cos (theta_p * 0.5), m * 2);
			s += t_2p * cos (p * theta_k);
		}
		const double   coef = 1 + 2 * _r * s;
		if (k == 0)
		{
			scale = 1 / coef;
		}
		else
		{
			const T        scaled_coef = T (coef * scale);
			data_ptr [center - k] = scaled_coef;
			data_ptr [center + k] = scaled_coef;
		}
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class T>
double	DolphCheby <T>::cheby_poly (double x, int n) noexcept
{
	assert (n >= 0);

	double			y = 0;
	if (fabs (x) <= 1)
	{
		y = cos (n * acos (x));
	}
	else
	{
		y = cosh (n * math::acosh (x));
	}

	return y;
}



}  // namespace wnd
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_wnd_DolphCheby_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

