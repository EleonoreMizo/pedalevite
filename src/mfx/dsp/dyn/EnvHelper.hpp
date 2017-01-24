/*****************************************************************************

        EnvHelper.hpp
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_dyn_EnvHelper_CODEHEADER_INCLUDED)
#define mfx_dsp_dyn_EnvHelper_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace dyn
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// Approximates c = 1 - exp (-1 / (fs * t))
// Coefficient in ]0 ; 1]
double	EnvHelper::compute_env_coef_simple (double t, double fs)
{
	assert (fs > 0);
	assert (t >= 0);

	const double   p = t * fs;
	const double   r = 1.90625;
	const double   u = r / (1 + (3 * r) * p);
	const double   v = std::max (1 - u, 0.0);
	const double   c = 1 - v * v * v;

	return c;
}



// Approximates c = 2 / (1 + k) with k = 1 / tan (1 / (fs * t * 2))
// Coefficient in ]0 ; 1.99], requires putting a zero at nyquist, prefiltering
// the input with { 0.5, 0.5 }
double	EnvHelper::compute_env_coef_w_zero (double t, double fs)
{
	assert (fs > 0);
	assert (t >= 0);

	const double   m = 1 / (2 * double (fstb::PI));
	const double   p = std::max (t * fs, m);
	const double   x = 1 / p;
	const double   c = ((0.09 * x - 0.4) * x + 1) * x;

	return c;
}



// Computes a new time compensating the latency caused by filter orders > 1,
// so the 90% rise time remains more or less constant.
double	EnvHelper::compensate_order (double t, int ord)
{
	assert (ord >= 1);

	return t * 3 / (ord * 2 + 1);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace dyn
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_dyn_EnvHelper_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
