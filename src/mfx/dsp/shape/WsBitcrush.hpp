/*****************************************************************************

        WsBitcrush.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_shape_WsBitcrush_CODEHEADER_INCLUDED)
#define mfx_dsp_shape_WsBitcrush_CODEHEADER_INCLUDED



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



template <class S, bool L>
template <typename T>
T	WsBitcrush <S, L>::operator () (T x) noexcept
{
	return process_sample (x);
}



template <class S, bool L>
template <typename T>
T	WsBitcrush <S, L>::process_sample (T x) noexcept
{
	const T        scale     = T (Scale::num) / T (Scale::den);
	const T        scale_inv = T (Scale::den) / T (Scale::num);

	if (L)
	{
		x = fstb::limit (x, T (-1), T (+1));
	}
	x *= scale;
	x = T (fstb::round_int (x));
	x *= scale_inv;

	return x;
}



template <class S, bool L>
template <typename VD, typename VS>
void  WsBitcrush <S, L>::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept
{
	assert (VD::check_ptr (dst_ptr));
	assert (VS::check_ptr (src_ptr));
	assert (nbr_spl > 0);
	assert ((nbr_spl & 3) == 0);

	const auto     scale     =
		fstb::ToolsSimd::set1_f32 (float (Scale::num) / float (Scale::den));
	const auto     scale_inv =
		fstb::ToolsSimd::set1_f32 (float (Scale::den) / float (Scale::num));
	const auto     m1 = fstb::ToolsSimd::set1_f32 (-1);
	const auto     p1 = fstb::ToolsSimd::set1_f32 (+1);

	for (int pos = 0; pos < nbr_spl; pos += 4)
	{
		auto           x = VS::load_f32 (src_ptr + pos);

		if (L)
		{
			x  = fstb::ToolsSimd::max_f32 (x, m1);
			x  = fstb::ToolsSimd::min_f32 (x, p1);
		}
		x *= scale;
		x  = fstb::ToolsSimd::conv_s32_to_f32 (
			fstb::ToolsSimd::round_f32_to_s32 (x)
		);
		x *= scale_inv;

		VD::store_f32 (dst_ptr + pos, x);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_shape_WsBitcrush_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
