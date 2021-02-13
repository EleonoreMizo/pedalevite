/*****************************************************************************

        XFadeEqAmpLin.hpp
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_wnd_XFadeEqAmpLin_CODEHEADER_INCLUDED)
#define mfx_dsp_wnd_XFadeEqAmpLin_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace wnd
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T>
std::array <T, 2>	XFadeEqAmpLin::compute_gain (T x)
{
	assert (x >= T (0));
	assert (x <= T (1));

	return std::array <T, 2> { T (1) - x, x };
}



std::array <fstb::ToolsSimd::VectF32, 2>	XFadeEqAmpLin::compute_gain (fstb::ToolsSimd::VectF32 x)
{
	using TS = fstb::ToolsSimd;

	assert (! TS::or_h (TS::cmp_lt0_f32 (x)));
	assert (! TS::or_h (TS::cmp_gt_f32 (x, TS::set1_f32 (1))));

	return std::array <TS::VectF32, 2> { TS::set1_f32 (1) - x, x };
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace wnd
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_wnd_XFadeEqAmpLin_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
