/*****************************************************************************

        DiodeClipNR.cpp
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

#include "fstb/def.h"
#include "fstb/fnc.h"
#include "mfx/dsp/va/DiodeClipNR.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace va
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DiodeClipNR::set_sample_freq (double sample_freq)
{
	assert (sample_freq > 0);

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	// Coefficient update
	update_internal_coef ();
}



void	DiodeClipNR::set_d1_is (float is)
{
	assert (is >= 1e-20f);
	assert (is <= 0.1f);

	_is1 = is;
	update_internal_coef ();
}



void	DiodeClipNR::set_d2_is (float is)
{
	assert (is >= 1e-20f);
	assert (is <= 0.1f);

	_is2 = is;
	update_internal_coef ();
}



void	DiodeClipNR::set_d1_n (float n)
{
	assert (n > 0);

	_n1 = n;
	update_internal_coef ();
}



void	DiodeClipNR::set_d2_n (float n)
{
	assert (n > 0);

	_n2 = n;
	update_internal_coef ();
}



void	DiodeClipNR::set_capa (float c)
{
	assert (c > 0);

	_c = c;
	update_internal_coef ();
}



void	DiodeClipNR::set_cutoff_freq (float f)
{
	assert (f > 0);

	set_capa (1.0f / (float (2 * fstb::PI) * _r * f));
}



float	DiodeClipNR::process_sample (float x)
{
	const float    gr    = 1 / _r;
	const float    geqc  =        2 * _c / _inv_fs;
	const float    ieqc  = _v2 * -2 * _c / _inv_fs - _ic;

	// Newton-Raphson iterations
	const float    max_dif_a = 1e-6f;
	const float    max_step  = _vt * (_n1 + _n2) * 2;
	float          v2        = _v2; // Starts with the previous V2 value
	float          v2_old    = v2;
	int            nbr_it    = 0;
	do
	{
		// Check for convergence failure
		if (nbr_it >= _max_it)
		{
			v2 = _v2; // Reuses the previous value
			break;
		}

		const float    m1   = (v2 >= 0) ? _is1            : -_is2;
		const float    m2   = (v2 >= 0) ? 1 / (_vt * _n1) : -1 / (_vt * _n2);
		const float    id   =      m1 * (exp (v2 * m2) - 1);
		const float    geqd = m2 * m1 * (exp (v2 * m2) - 1);
		const float    ieqd = id - geqd * v2;
		v2_old = v2;
		v2 = (gr * x - ieqc - ieqd) / (gr + geqc + geqd);
		v2 = fstb::limit (v2, v2_old - max_step, v2_old + max_step);
		++ nbr_it;
	}
	while (fabs (v2 - v2_old) > max_dif_a);

	// Updates the Ic state (integration) and saves v2
	_ic = geqc * v2 + ieqc;
	_v2 = v2;

	return v2;
}



void	DiodeClipNR::clear_buffers ()
{
	_ic = 0;
	_v2 = 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DiodeClipNR::update_internal_coef ()
{
}



}  // namespace va
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
