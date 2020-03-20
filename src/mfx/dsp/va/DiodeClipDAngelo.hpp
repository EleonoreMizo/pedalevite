/*****************************************************************************

        DiodeClipDAngelo.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_va_DiodeClipDAngelo_CODEHEADER_INCLUDED)
#define mfx_dsp_va_DiodeClipDAngelo_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/Approx.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace va
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



float	DiodeClipDAngelo::process_sample (float x)
{
	assert (_sample_freq > 0);

	if (_dirty_flag)
	{
		update_internal_coef ();
	}

	return process_sample_internal (x, _mem_p);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



float	DiodeClipDAngelo::process_sample_internal (float x, float &mem_p)
{
	const float    q  = _k1 * x - mem_p;                  // Eq. 27
	const bool     neg_flag = (q < 0);
	const float    k3 = (neg_flag) ? _k32 : _k31;         // Eq. 26
	const float    k4 = (neg_flag) ? _k42 : _k41;
	const float    k5 = (neg_flag) ? _k52 : _k51;
	const float    k7 = (neg_flag) ? _k72 : _k71;
	const float    w  = _k2 * q + k3;                     // Eq. 25
	const float    u  =  k4 * w + k5;                     // Eq. 24
	const float    o  = fstb::Approx::wright_omega_4 (u); // "
	const float    y  = w - k7 * o;                       // "
	mem_p = _k6 * y - _a1 * mem_p;                        // Eq. 28

	return y;
}



}  // namespace va
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_va_DiodeClipDAngelo_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

