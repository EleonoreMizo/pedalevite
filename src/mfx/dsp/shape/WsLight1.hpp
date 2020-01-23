/*****************************************************************************

        WsLight1.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_shape_WsLight1_CODEHEADER_INCLUDED)
#define mfx_dsp_shape_WsLight1_CODEHEADER_INCLUDED



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



template <typename VD, typename VS>
void  WsLight1::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (VD::check_ptr (dst_ptr));
	assert (VS::check_ptr (src_ptr));
	assert (nbr_spl > 0);
	assert ((nbr_spl & 3) == 0);

	const auto     mnt_mask = fstb::ToolsSimd::set1_s32 (0x007FFFFF); // 23 bits
	const auto     mnt_invm = fstb::ToolsSimd::set1_s32 (0xFF800000); // exponent and sign
	for (int pos = 0; pos < nbr_spl; pos += 4)
	{
		auto           x_int   = VS::load_s32 (src_ptr + pos);
		auto           mnt_int = fstb::ToolsSimd::and_s32 (x_int, mnt_mask);

		// m^2
		mnt_int >>= 23 - 15; // 0:15
		mnt_int *= mnt_int;  // 0:30
		mnt_int >>= 30 - 23; // 0:23
		assert (! fstb::ToolsSimd::or_h (fstb::ToolsSimd::cmp_lt_s32 (mnt_int, fstb::ToolsSimd::set_s32_zero ())));

		x_int = fstb::ToolsSimd::and_s32 (x_int, mnt_invm);
		x_int = fstb::ToolsSimd::or_s32 (x_int, mnt_int);
		VD::store_s32 (dst_ptr + pos, x_int);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_shape_WsLight1_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
