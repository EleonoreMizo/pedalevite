/*****************************************************************************

        Overdrive.cpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/ToolsSimd.h"
#include "mfx/Overdrive.h"

#include <algorithm>

#include <cassert>



namespace mfx
{



// 0: 7th order, smooth
// 1: 7th order, slight overshoot
// 2: 3rd order
#define mfx_Overdrive_ALGO     2

// 0: Normal, no foldback
// > 0: Curve is mirrored, N time(s) per polarity
#define mfx_Overdrive_FOLDBACK 0



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Overdrive::set_gain (float g)
{
	assert (g > 0);

	_gain = g;

#if (mfx_Overdrive_ALGO == 0)
	// Smooth distortion
	static const float spec_a3 = -1.0f;
	static const float spec_a5 =  4.0f /  5;
	static const float spec_a7 = -2.0f /  7;
	static const float spec_f1 = 18.0f / 35;

#elif (mfx_Overdrive_ALGO == 1)
	// Slight overshoot
	static const float spec_a3 = -5.0f /   3;
	static const float spec_a5 =  7.0f /   5;
	static const float spec_a7 = -3.0f /   7;
	static const float spec_f1 = 32.0f / 105;

#elif (mfx_Overdrive_ALGO == 2)
	// Simple third order
	static const float spec_a3 = -1.0f /   3;
	static const float spec_f1 =  2.0f /   3;

#else
	#error Wrong value for mfx_Overdrive_ALGO

#endif // mfx_Overdrive_ALGO

	const float    s  = _gain * spec_f1;
	_x_max = 1.0f / s;

	const float    s2 = s * s;
	_a3    = s2 * spec_a3;

#if (mfx_Overdrive_ALGO != 2)
	const float    s4 = s2 * s2;
	const float    s6 = s4 * s2;
	_a5    = s4 * spec_a5;
	_a7    = s6 * spec_a7;
#endif // mfx_Overdrive_ALGO

	const float    ginv = std::max (g * 0.5f, 1.0f);
	_ginv  = ginv;
}



void	Overdrive::process_block (float spl_ptr [], int nbr_spl)
{
	assert (fstb::is_ptr_align_nz (spl_ptr));
	assert (nbr_spl > 0);

	static const float   threshold = 0.125f * 1.01f;

	if (_gain > threshold || _gain > threshold)
	{
#if (mfx_Overdrive_ALGO != 2)
		const auto     a7 = fstb::ToolsSimd::set1_f32 (_a7);
		const auto     a5 = fstb::ToolsSimd::set1_f32 (_a5);
#endif // mfx_Overdrive_ALGO
		const auto     a3 = fstb::ToolsSimd::set1_f32 (_a3);
		const auto     a1 = fstb::ToolsSimd::set1_f32 (1  );

		const auto     mi = fstb::ToolsSimd::set1_f32 (-_x_max);
		const auto     ma = fstb::ToolsSimd::set1_f32 ( _x_max);
		const auto     g  = fstb::ToolsSimd::set1_f32 ( _ginv );

#if (mfx_Overdrive_FOLDBACK != 0)
		const auto     zn = mi + mi;
		const auto     zp = ma + ma;
#endif // mfx_Overdrive_FOLDBACK

		for (int pos = 0; pos < nbr_spl; pos += 4)
		{
			auto           x  = fstb::ToolsSimd::load_f32 (spl_ptr + pos);

#if (mfx_Overdrive_FOLDBACK != 0)
			for (int k = 0; k < mfx_Overdrive_FOLDBACK; ++k)
			{
				const auto     tn = fstb::ToolsSimd::cmp_lt_f32 (x, mi);
				const auto     tp = fstb::ToolsSimd::cmp_gt_f32 (x, ma);
				const auto     xn = zn - x;
				const auto     xp = zp - x;
				x = fstb::ToolsSimd::select (tn, xn, x);
				x = fstb::ToolsSimd::select (tp, xp, x);
			}
#endif // mfx_Overdrive_FOLDBACK

			x = fstb::ToolsSimd::min_f32 (x, ma);
			x = fstb::ToolsSimd::max_f32 (x, mi);

			const auto     x2 = x * x;

#if (mfx_Overdrive_ALGO == 2)
			x = (                       a3  * x2 + a1) * x * g;
#else  // mfx_Overdrive_ALGO
			x = (((a7 * x2 + a5) * x2 + a3) * x2 + a1) * x * g;
#endif // mfx_Overdrive_ALGO

			fstb::ToolsSimd::store_f32 (spl_ptr + pos, x);
		}
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
