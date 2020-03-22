/*****************************************************************************

        AntisatSinh.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_va_AntisatSinh_CODEHEADER_INCLUDED)
#define mfx_dsp_va_AntisatSinh_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/Approx.h"
#include "fstb/def.h"

#include <cmath>



namespace mfx
{
namespace dsp
{
namespace va
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	AntisatSinh::eval (float &y, float &dy, float x)
{
	const float    xd = 2 * x;
	y  = 0.5f * asinh_fast (xd);
	dy = 1 / sqrt (xd * xd + 1);
}



float AntisatSinh::eval_inv (float y)
{
	return 0.5f * sinh_fast (y * 2);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



float	AntisatSinh::sinh_fast (float x)
{
	const float    ex  = fstb::Approx::exp2 (x * float (fstb::LOG2_E));
	const float    e2x = ex * ex;
	const float    sh  = (e2x - 1) / (2 * ex);

	return sh;
}



float	AntisatSinh::asinh_fast (float x)
{
	const float    u   = x + sqrt (x * x + 1);
	const float    ash = float (fstb::LN2) * fstb::Approx::log2 (u);

	return ash;
}



}  // namespace va
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_va_AntisatSinh_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
