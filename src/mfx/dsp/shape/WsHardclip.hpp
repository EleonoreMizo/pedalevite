/*****************************************************************************

        WsHardclip.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_shape_WsHardclip_CODEHEADER_INCLUDED)
#define mfx_dsp_shape_WsHardclip_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"
#include "fstb/ToolsSimd.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace shape
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T>
T	WsHardclip::operator () (T x) noexcept
{
	return process_sample (x);
}



template <typename T>
T	WsHardclip::process_sample (T x) noexcept
{
	return fstb::limit (x, T (-1), T (+1));
}



template <typename VD, typename VS>
void	WsHardclip::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept
{
	assert (VD::check_ptr (dst_ptr));
	assert (VS::check_ptr (src_ptr));
	assert (nbr_spl > 0);
	assert ((nbr_spl & 3) == 0);

	const auto     m1 = fstb::Vf32 (-1);
	const auto     p1 = fstb::Vf32 (+1);

	for (int pos = 0; pos < nbr_spl; pos += 4)
	{
		auto           x = VS::load_f32 (src_ptr + pos);
		x = fstb::limit (x, m1, p1);
		VD::store_f32 (dst_ptr + pos, x);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_shape_WsHardclip_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
