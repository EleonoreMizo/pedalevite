/*****************************************************************************

        WsAsym1.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_shape_WsAsym1_CODEHEADER_INCLUDED)
#define mfx_dsp_shape_WsAsym1_CODEHEADER_INCLUDED



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



constexpr float	WsAsym1::process_sample (float x) noexcept
{
	const float    x2 = x * x;
	if (x <= 0)
	{
		const float    a2 = 1.f / 2;
		x += x2 * a2;
	}
	else
	{
		const float   x4 = x2 * x2;
		const float   x8 = x4 * x4;
		const float   a9 = 1.f / 9;
		x -= x * x8 * a9;
	}

	return x;
}



template <typename VD, typename VS>
void  WsAsym1::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept
{
	assert (VD::check_ptr (dst_ptr));
	assert (VS::check_ptr (src_ptr));
	assert (nbr_spl > 0);
	assert ((nbr_spl & 3) == 0);

	const auto     mi   = fstb::ToolsSimd::set1_f32 (-1.0f);
	const auto     ma   = fstb::ToolsSimd::set1_f32 ( 1.0f);
	const auto     c_9  = fstb::ToolsSimd::set1_f32 (1.f / 9);
	const auto     c_2  = fstb::ToolsSimd::set1_f32 (1.f / 2);
	const auto     bias = fstb::ToolsSimd::set1_f32 ( 0.2f);

	for (int pos = 0; pos < nbr_spl; pos += 4)
	{
		auto           x = VS::load_f32 (src_ptr + pos);

		x += bias;

		x = fstb::ToolsSimd::min_f32 (x, ma);
		x = fstb::ToolsSimd::max_f32 (x, mi);

		const auto     x2  = x  * x;
		const auto     x4  = x2 * x2;
		const auto     x8  = x4 * x4;
		const auto     x9  = x8 * x;
		const auto     x_n = x + x2 * c_2;
		const auto     x_p = x - x9 * c_9;
		const auto     t_0 = fstb::ToolsSimd::cmp_lt0_f32 (x);
		x = fstb::ToolsSimd::select (t_0, x_n, x_p);

		x -= bias;

		VD::store_f32 (dst_ptr + pos, x);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_shape_WsAsym1_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
