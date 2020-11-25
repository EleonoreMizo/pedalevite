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
#include "fstb/fnc.h"

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
// https://www.desmos.com/calculator/t46nefssqd
template <typename T>
inline T	EnvHelper::compute_env_coef_simple (T t, T fs)
{
	assert (fs > 0);
	assert (t >= 0);

#if 1

	// Time error:
	// < 1e-6   for t * fs >= 909 samples
	// < 0.01 % for t * fs >=  25 samples
	// < 1 %    for t * fs >=   3 samples
	// Below, the coefficient is not very accurate but remains consistent.
	const T        p = t * fs;
	const T        u = T (1) / (T (0.501) + p);
	const T        c = std::min (u, T (1));

#else

	// Better accuracy for very low times, but somewhat overkill for simple
	// envelope detection
	constexpr int  o = 4; // Approximation order, >= 1
	const T        p = t * fs;
	const T        u = T (1.0 / o) / (T (0.5 / o) + p);
	const T        v = std::max (1 - u, T (0));
	const T        c = T (1) - fstb::ipowpc <o> (v);

#endif

	return c;
}



// Approximates c = 2 / (1 + k) with k = 1 / tan (1 / (fs * t * 2))
// Coefficient in ]0 ; 1.99], requires putting a zero at nyquist, prefiltering
// the input with { 0.5, 0.5 }
template <typename T>
inline T	EnvHelper::compute_env_coef_w_zero (T t, T fs)
{
	assert (fs > 0);
	assert (t >= 0);

	constexpr T    m = T (1 / (2 * fstb::PI));
	const T        p = std::max (t * fs, m);
	const T        x = 1 / p;
	const T        c = ((T (0.09) * x - T (0.4)) * x + 1) * x;

	return c;
}



// Computes a new time compensating the latency caused by filter orders > 1,
// so the 90% rise time remains more or less constant.
template <typename T>
inline T	EnvHelper::compensate_order (T t, int ord)
{
	assert (ord >= 1);

	return t * 3 / T (ord * 2 + 1);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace dyn
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_dyn_EnvHelper_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
