/*****************************************************************************

        AntisatSq.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_va_AntisatSq_CODEHEADER_INCLUDED)
#define mfx_dsp_va_AntisatSq_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace va
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	AntisatSq::eval (float &y, float &dy, float x) noexcept
{
	const float    xl = fstb::limit (x, -2.f, 2.f);
	const float    xa = fabsf (xl);
	y  = xl * (1 - 0.25f * xa);
	dy =      (1 - 0.5f  * xa);
}



float AntisatSq::eval_inv (float y) noexcept
{
	assert (y >= -1);
	assert (y <=  1);

	return (2 - 2 * sqrtf (1 - fabsf (y))) * float (fstb::sgn (y));
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace va
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_va_AntisatSq_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

