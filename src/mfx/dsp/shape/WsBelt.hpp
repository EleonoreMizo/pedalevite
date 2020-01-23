/*****************************************************************************

        WsBelt.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_shape_WsBelt_CODEHEADER_INCLUDED)
#define mfx_dsp_shape_WsBelt_CODEHEADER_INCLUDED



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
void  WsBelt::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (VD::check_ptr (dst_ptr));
	assert (VS::check_ptr (src_ptr));
	assert (nbr_spl > 0);
	assert ((nbr_spl & 3) == 0);

	const auto     exp_mask = fstb::ToolsSimd::set1_s32 (0x7F800000);
	const auto     exp_lsb  = fstb::ToolsSimd::set1_s32 (0x00800000);
	const auto     exp_add  = fstb::ToolsSimd::set1_s32 (0x3F800000 >> 1);
	for (int pos = 0; pos < nbr_spl; pos += 4)
	{
		auto           x_int   = VS::load_s32 (src_ptr + pos);
		auto           exp_int = fstb::ToolsSimd::and_s32 (x_int, exp_mask);
		auto           cond    = fstb::ToolsSimd::cmp_gt_s32 (exp_int, exp_lsb);
		x_int    -= exp_int;
		exp_int >>= 1;
		exp_int  += exp_add;
		x_int    += exp_int;
		x_int     = fstb::ToolsSimd::and_s32 (x_int, cond);
		VD::store_s32 (dst_ptr + pos, x_int);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_shape_WsBelt_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
