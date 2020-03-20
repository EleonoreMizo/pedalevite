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

#include "fstb/Approx.h"
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
	update_internal_coef_rc ();

	// Required at least once before any use, could be called in a constructor
	update_internal_coef_d ();
}



void	DiodeClipNR::set_d1_is (float is)
{
	assert (is >= 1e-20f);
	assert (is <= 0.1f);

	_is1 = is;
}



void	DiodeClipNR::set_d2_is (float is)
{
	assert (is >= 1e-20f);
	assert (is <= 0.1f);

	_is2 = is;
}



void	DiodeClipNR::set_d1_n (float n)
{
	assert (n > 0);

	_n1 = n;
	update_internal_coef_d ();
}



void	DiodeClipNR::set_d2_n (float n)
{
	assert (n > 0);

	_n2 = n;
	update_internal_coef_d ();
}



void	DiodeClipNR::set_capa (float c)
{
	assert (c > 0);

	_c = c;
	update_internal_coef_rc ();
}



void	DiodeClipNR::set_cutoff_freq (float f)
{
	assert (f > 0);

	set_capa (1.0f / (float (2 * fstb::PI) * _r * f));
}



float	DiodeClipNR::process_sample (float x)
{
	assert (_sample_freq > 0);

	const float    gr_x = _gr * x;
	const float    ieqc = -_v2 * _geqc - _ic;

	// Newton-Raphson iterations
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

		const float    me   = (v2 >= 0) ? _is1 : -_is2;
		const float    mv   = (v2 >= 0) ? _mv1 :  _mv2;
		const float    id   =
			me * (fstb::Approx::exp2 (v2 * mv * float (fstb::LOG2_E)) - 1);
		const float    geqd = mv * id;   // dId/dV
		const float    ieqd = id - geqd * v2;
		v2_old = v2;
		v2 = (gr_x - ieqc - ieqd) / (_gr_p_geqc + geqd);
		v2 = fstb::limit (v2, v2_old - _max_step, v2_old + _max_step);
		++ nbr_it;
	}
	while (fabs (v2 - v2_old) > _max_dif_a);

	// Updates the Ic state (integration) and saves v2
	_ic = _geqc * v2 + ieqc;
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



void	DiodeClipNR::update_internal_coef_d ()
{
	_max_step  = _vt * (_n1 + _n2) * 2;
	_mv1       =  1 / (_vt * _n1);
	_mv2       = -1 / (_vt * _n2);
}



void	DiodeClipNR::update_internal_coef_rc ()
{
	_geqc      = 2 * _c * _sample_freq;
//	_gr        = 1.f / _r;
	_gr_p_geqc = _gr + _geqc;
}



}  // namespace va
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
