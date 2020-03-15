/*****************************************************************************

        DiodeClipDAngelo.cpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/Approx.h"
#include "fstb/fnc.h"
#include "mfx/dsp/va/DiodeClipDAngelo.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace va
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DiodeClipDAngelo::set_sample_freq (double sample_freq)
{
	_sample_freq = float (sample_freq);

	// Trapezoidal rule
	// v[n] = v[n-1] + (T/2) * (v'[n] + v'[n-1])
	// v'[n] = (2/T) * v[n] - (2/T) * v[n-1] - v'[n-1]
	_b0 = _sample_freq * 2;
	_b1 = -_b0;
	_a1 = 1;

	// Coefficient update
	update_internal_coef ();
}



void	DiodeClipDAngelo::set_knee_thr (float lvl)
{
	_vt = lvl * 0.04f;
	update_internal_coef ();
}



// It could be interesting to make c proportional to the input gain.
void	DiodeClipDAngelo::set_capa (float c)
{
	assert (c > 0);

	_c = c;
	update_internal_coef ();
}



// Same as setting C, but with a meaningful value.
void	DiodeClipDAngelo::set_cutoff_freq (float f)
{
	assert (f > 0);

	_c = 1.0f / (float (2 * fstb::PI) * _r * f);
	update_internal_coef ();
}



float	DiodeClipDAngelo::process_sample (float x)
{
	assert (_sample_freq > 0);

	const float    q = _k1 * x - _mem_p;      // Eq. 27
	const float    r = float (fstb::sgn (q)); // Eq. 26
	const float    w = _k2 * q + _k3 * r;     // Eq. 25
	const float    u = _k4 * r * w + _k5;                // Eq.24
	const float    o = fstb::Approx::wright_omega_4 (u); // "
	const float    y = w - _vt * r * o;                  // "
	_mem_p = _k6 * y - _a1 * _mem_p;          // Eq. 28

	return y;
}



void	DiodeClipDAngelo::clear_buffers ()
{
	_mem_p = 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DiodeClipDAngelo::update_internal_coef ()
{
	const float    cr        = _c * _r;
	const float    invb0crp1 = 1 / (_b0 * cr + 1);
	_k1 = 1 / cr;
	_k2 = cr * invb0crp1;
	_k3 = _is * _r * invb0crp1;
	_k4 = 1 / _vt;
	_k5 = fstb::Approx::log2 (_k3 * _k4) * float (fstb::LN2);
	_k6 = _b1 - _a1 * _b0;
}



}  // namespace va
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
