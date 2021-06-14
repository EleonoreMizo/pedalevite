/*****************************************************************************

        WsRcp2.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_shape_WsRcp2_CODEHEADER_INCLUDED)
#define mfx_dsp_shape_WsRcp2_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/ToolsSimd.h"

#include <algorithm>

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace shape
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



float	WsRcp2::process_sample (float x) noexcept
{
	const float    t = 0.5f;
	const float    a = fabsf (x);
	const float    m = std::max (a, t);
	const float    f = t / m;
	const float    g = f * (2 - f);
	x *= g;

	return x;
}



template <typename VD, typename VS>
void	WsRcp2::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept
{
	assert (VD::check_ptr (dst_ptr));
	assert (VS::check_ptr (src_ptr));
	assert (nbr_spl > 0);
	assert ((nbr_spl & 3) == 0);

	const auto        t   = fstb::ToolsSimd::set1_f32 (0.5f);
	const auto        two = fstb::ToolsSimd::set1_f32 (2);

	for (int pos = 0; pos < nbr_spl; pos += 4)
	{
		auto           x = VS::load_f32 (src_ptr + pos);

		const auto     a = fstb::ToolsSimd::abs (x);
		const auto     m = fstb::ToolsSimd::max_f32 (a, t);
		const auto     f = t * fstb::ToolsSimd::rcp_approx (m);
		const auto     g = f * (two - f);
		x *= g;

		VD::store_f32 (dst_ptr + pos, x);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_shape_WsRcp2_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
