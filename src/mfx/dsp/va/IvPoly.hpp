/*****************************************************************************

        IvPoly.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_va_IvPoly_CODEHEADER_INCLUDED)
#define mfx_dsp_va_IvPoly_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"



namespace mfx
{
namespace dsp
{
namespace va
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int OP, int ON>
void	IvPoly <OP, ON>::set_atten_p (float a)
{
	assert (a > 0);
	_attn_p = a;
}



template <int OP, int ON>
void	IvPoly <OP, ON>::set_atten_n (float a)
{
	assert (a > 0);
	_attn_n = a;
}



template <int OP, int ON>
void	IvPoly <OP, ON>::eval (float &y, float &dy, float x) const
{
	if (x < 0)
	{
		const float    xnm1 = _attn_n * fstb::ipowpc <ON - 1> (-x);
		y  = x  * xnm1;
		dy = ON * xnm1;
	}
	else
	{
		const float    xnm1 = _attn_p * fstb::ipowpc <OP - 1> (x);
		y  = x  * xnm1;
		dy = OP * xnm1;
	}
}



template <int OP, int ON>
float	IvPoly <OP, ON>::get_max_step (float x) const
{
	fstb::unused (x);

	return 0.1f;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace va
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_va_IvPoly_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
