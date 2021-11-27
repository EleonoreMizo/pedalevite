/*****************************************************************************

        WsBadmood.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_shape_WsBadmood_CODEHEADER_INCLUDED)
#define mfx_dsp_shape_WsBadmood_CODEHEADER_INCLUDED



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



float	WsBadmood::process_sample (float x) noexcept
{
	Combo          c;
	c._f = x;
	int32_t        x_int   = c._i;
	int32_t        exp_int = x_int & _e_mask;
	if (exp_int <= _e_lsb)
	{
		x_int = 0;
	}
	else
	{
		const int32_t  rem = x_int & _e_lsb;
		x_int    -= exp_int;
		exp_int >>= 1;
		exp_int  &= _e_mask;
		exp_int  += _e_add;
		x_int    += exp_int;
		x_int    ^= rem << 8;
	}
	c._i = x_int;

	return c._f;
}



template <typename VD, typename VS>
void  WsBadmood::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept
{
	assert (VD::check_ptr (dst_ptr));
	assert (VS::check_ptr (src_ptr));
	assert (nbr_spl > 0);
	assert ((nbr_spl & 3) == 0);

	const auto     exp_mask = fstb::Vs32 (_e_mask);
	const auto     exp_lsb  = fstb::Vs32 (_e_lsb);
	const auto     exp_add  = fstb::Vs32 (_e_add);
	for (int pos = 0; pos < nbr_spl; pos += 4)
	{
		auto           x_int   = VS::load_s32 (src_ptr + pos);
		auto           exp_int = fstb::ToolsSimd::and_s32 (x_int, exp_mask);
		auto           rem     = fstb::ToolsSimd::and_s32 (x_int, exp_lsb);
		auto           cond    = fstb::ToolsSimd::cmp_gt_s32 (exp_int, exp_lsb);
		x_int    -= exp_int;
		exp_int >>= 1;
		exp_int   = fstb::ToolsSimd::and_s32 (exp_int, exp_mask);
		exp_int  += exp_add;
		x_int    += exp_int;
		x_int     = fstb::ToolsSimd::xor_s32 (x_int, rem << 8);
		x_int     = fstb::ToolsSimd::and_s32 (x_int, cond);
		VD::store_s32 (dst_ptr + pos, x_int);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_shape_WsBadmood_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
