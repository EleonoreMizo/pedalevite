/*****************************************************************************

        WsSmthAbs.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_shape_WsSmthAbs_CODEHEADER_INCLUDED)
#define mfx_dsp_shape_WsSmthAbs_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/ToolsSimd.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace shape
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename A>
template <typename T>
T	WsSmthAbs <A>::operator () (T x)
{
	return process_sample (x);
}



template <typename A>
template <typename T>
T	WsSmthAbs <A>::process_sample (T x)
{
	const T        a     = T (A::num) / T (A::den);
	const T        a_inv = T (A::den) / T (A::num);
	const T        xabs  = T (fabs (x));
	if (xabs < a * T (0.5f))
	{
		x = a * T (0.25f) + a_inv * x * x;
	}
	else
	{
		x = xabs;
	}

	return x;
}



template <typename A>
template <typename VD, typename VS>
void  WsSmthAbs <A>::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (VD::check_ptr (dst_ptr));
	assert (VS::check_ptr (src_ptr));
	assert (nbr_spl > 0);
	assert ((nbr_spl & 3) == 0);

	const auto     a_2   =
		fstb::ToolsSimd::set1_f32 (0.5f  * float (A::num) / float (A::den));
	const auto     a_4   =
		fstb::ToolsSimd::set1_f32 (0.25f * float (A::num) / float (A::den));
	const auto     a_inv =
		fstb::ToolsSimd::set1_f32 (float (A::den) / float (A::num));
	for (int pos = 0; pos < nbr_spl; pos += 4)
	{
		auto           x    = VS::load_f32 (src_ptr + pos);
		const auto     xabs = fstb::ToolsSimd::abs (x);
		const auto     cmp  = fstb::ToolsSimd::cmp_lt_f32 (x, a_2);
		const auto     x1   = x * x * a_inv + a_4;
		x = fstb::ToolsSimd::select (cmp, xabs, x1);
		VD::store_f32 (dst_ptr + pos, x);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_shape_WsSmthAbs_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
