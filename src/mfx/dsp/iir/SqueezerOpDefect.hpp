/*****************************************************************************

        SqueezerOpDefect.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_iir_SqueezerOpDefect_CODEHEADER_INCLUDED)
#define mfx_dsp_iir_SqueezerOpDefect_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int HA>
void	SqueezerOpDefect <HA>::config (float reso, float p1) noexcept
{
	_param   = 1.25f - p1 - reso * 0.125f;
	assert (_param >= 0);
	_param_i = 1.0f / _param;
	_param_2 = 2 * _param;
}



template <int HA>
float	SqueezerOpDefect <HA>::process_sample (float x) noexcept
{
	const float    xa = fabs (x);
	if (xa > _param)
	{
		x = -x;
	}
	else if (HA < 5)
	{
		float          y  = x  * _param_i;
		float          yy =  y *  y;
		if (HA > 0) {  yy = yy * yy; }
		if (HA > 1) {  yy = yy * yy; }
		if (HA > 2) {  yy = yy * yy; }
		if (HA > 3) {  yy = yy * yy; }
		y *= (1 - yy) * _param_2;
		x = y - x;
	}

	return x;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_iir_SqueezerOpDefect_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
