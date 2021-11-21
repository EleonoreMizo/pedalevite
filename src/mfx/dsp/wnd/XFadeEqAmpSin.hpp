/*****************************************************************************

        XFadeEqAmpSin.hpp
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_wnd_XFadeEqAmpSin_CODEHEADER_INCLUDED)
#define mfx_dsp_wnd_XFadeEqAmpSin_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/ToolsSimd.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace wnd
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// Sinus approximation (formula by Nick)
template <typename T>
std::array <T, 2>	XFadeEqAmpSin::compute_gain (T x) noexcept
{
	assert (x >= T (0));
	assert (x <= T (1));

	constexpr double  c0 = 0.224008;
	const T        xb = x + x - T (1);
	const T        y  = (T (1) - T (0.5) * std::abs (xb)) * xb;
	const T        z  = (T (2 * c0) * std::abs (y) + T (1 - c0)) * y + T (0.5);

	return std::array <T, 2> { T (1) - z, z };
}



std::array <fstb::Vf32, 2>	XFadeEqAmpSin::compute_gain (fstb::Vf32 x) noexcept
{
	using TS = fstb::ToolsSimd;

	assert (! TS::or_h (TS::cmp_lt0_f32 (x)));
	assert (! TS::or_h (TS::cmp_gt_f32 (x, TS::set1_f32 (1))));

	constexpr float   c0 = 0.224008f;
	const T        one  = TS::set1_f32 (1);
	const T        half = TS::set1_f32 (0.5f);
	const T        c2   = TS::set1_f32 (2 * c0);
	const T        c1   = TS::set1_f32 (1 - c0);
	const T        xb   = x + x - one;
	const T        y    = (one - half * TS::abs_f32 (xb)) * xb;
	const T        z    = (c2 * std::abs (y) + c1) * y + half;

	return std::array <fstb::Vf32, 2> { one - z, z };
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace wnd
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_wnd_XFadeEqAmpSin_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
