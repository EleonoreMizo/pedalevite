/*****************************************************************************

        XFadeEqPowPoly8.hpp
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_wnd_XFadeEqPowPoly8_CODEHEADER_INCLUDED)
#define mfx_dsp_wnd_XFadeEqPowPoly8_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace wnd
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <bool OFLAG>
template <typename T>
std::array <T, 2>	XFadeEqPowPoly8 <OFLAG>::compute_gain (T x) noexcept
{
	assert (x >= T (0));
	assert (x <= T (1));

	const T        xi = T (1) - x;
	const T        a  = x * xi;
	const T        b0 = T (OFLAG ? 1      : 0.975);
	const T        b1 = T (OFLAG ? 1.4186 : 1.51 );
	const T        b  = a * (b0 + b1 * a);
	const T        c  = b + x;
	const T        d  = b + xi;
	const T        fi = c * c;
	const T        fo = d * d;

	return std::array <T, 2> { fo, fi };
}



template <bool OFLAG>
std::array <fstb::ToolsSimd::VectF32, 2>	XFadeEqPowPoly8 <OFLAG>::compute_gain (fstb::ToolsSimd::VectF32 x) noexcept
{
	using TS = fstb::ToolsSimd;

	assert (! TS::or_h (TS::cmp_lt0_f32 (x)));
	assert (! TS::or_h (TS::cmp_gt_f32 (x, TS::set1_f32 (1))));

	const auto     one = TS::set1_f32 (1);
	const auto     xi  = one - x;
	const auto     a   = x * xi;
	const auto     b0  = OFLAG ? one : TS::set1_f32 (0.975f);
	const auto     b1  = TS::set1_f32 (OFLAG ? 1.4186f : 1.51f);
	const auto     b   = a * (b0 + b1 * a);
	const auto     c   = b + x;
	const auto     d   = b + xi;
	const auto     fi  = c * c;
	const auto     fo  = d * d;

	return std::array <TS::VectF32, 2> { fo, fi };
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace wnd
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_wnd_XFadeEqPowPoly8_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
