/*****************************************************************************

        AntisatAtanh.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_va_AntisatAtanh_CODEHEADER_INCLUDED)
#define mfx_dsp_va_AntisatAtanh_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/Approx.h"
#include "fstb/def.h"



namespace mfx
{
namespace dsp
{
namespace va
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	AntisatAtanh::eval (float &y, float &dy, float x) noexcept
{
	const float    th = tanh_fast (x);
	y  =     th;
	dy = 1 - th * th;
}



float AntisatAtanh::eval_inv (float y) noexcept
{
	assert (y > -1);
	assert (y <  1);

	return atanh_fast (y);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



float	AntisatAtanh::tanh_fast (float x) noexcept
{
	const float    e2x = fstb::Approx::exp2 (x * float (2 * fstb::LOG2_E));
	const float    th  = (e2x - 1) / (e2x + 1);

	return th;
}



float	AntisatAtanh::atanh_fast (float x) noexcept
{
	const float    u     = (1 + x) / (1 - x);
	const float    log2u = fstb::Approx::log2 (u);
	const float    ath   = log2u * float (fstb::LN2 * 0.5f);

	return ath;
}



}  // namespace va
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_va_AntisatAtanh_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

