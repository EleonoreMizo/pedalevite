/*****************************************************************************

        WsSmthMax0.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_shape_WsSmthMax0_CODEHEADER_INCLUDED)
#define mfx_dsp_shape_WsSmthMax0_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/ToolsSimd.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace shape
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename A>
double	WsSmthMax0 <A>::operator () (double x)
{
	const double   a     = double (A::num) / double (A::den);
	const double   a_inv = double (A::den) / double (A::num);
	if (x < a * -0.5)
	{
		x = 0;
	}
	else if (x < a * 0.5)
	{
		x += a * 0.5;
		x  = a_inv * 0.5 * x * x;
	}

	return x;
}



template <typename A>
float	WsSmthMax0 <A>::process_sample (float x)
{
	const float    a     = float (A::num) / float (A::den);
	const float    a_inv = float (A::den) / float (A::num);
	if (x < a * -0.5f)
	{
		x = 0;
	}
	else if (x < a * 0.5f)
	{
		x += a * 0.5f;
		x  = a_inv * 0.5f * x * x;
	}

	return x;
}



template <typename A>
template <typename VD, typename VS>
void  WsSmthMax0 <A>::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (VD::check_ptr (dst_ptr));
	assert (VS::check_ptr (src_ptr));
	assert (nbr_spl > 0);
	assert ((nbr_spl & 3) == 0);

	const auto     zero    = fstb::ToolsSimd::set_f32_zero ();
	const auto     a_2     =
		fstb::ToolsSimd::set1_f32 ( 0.5f * float (A::num) / float (A::den));
	const auto     ma_2    =
		fstb::ToolsSimd::set1_f32 (-0.5f * float (A::num) / float (A::den));
	const auto     a_inv_2 =
		fstb::ToolsSimd::set1_f32 ( 0.5f * float (A::den) / float (A::num));

	for (int pos = 0; pos < nbr_spl; pos += 4)
	{
		auto           x = VS::load_f32 (src_ptr + pos);
		const auto     cmp_p = fstb::ToolsSimd::cmp_lt_f32 (x,  a_2);
		const auto     cmp_m = fstb::ToolsSimd::cmp_lt_f32 (x, ma_2);
		auto           x1 = x + a_2;
		x1 = x1 * x1 * a_inv_2;
		x  = fstb::ToolsSimd::select (cmp_p, x1  , x);
		x  = fstb::ToolsSimd::select (cmp_m, zero, x);
		VD::store_f32 (dst_ptr + pos, x);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_shape_WsSmthMax0_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
