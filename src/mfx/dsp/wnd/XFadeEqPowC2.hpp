/*****************************************************************************

        XFadeEqPowC2.hpp
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_wnd_XFadeEqPowC2_CODEHEADER_INCLUDED)
#define mfx_dsp_wnd_XFadeEqPowC2_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace wnd
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T>
std::array <T, 2>	XFadeEqPowC2::compute_gain (T x) noexcept
{
	assert (x >= T (0));
	assert (x <= T (1));

	const T        xi  = T (1) - x;
	const T        x2  = x   * x;
	const T        xi2 = xi  * xi;
	const T        x4  = x2  * x2;
	const T        xi4 = xi2 * xi2;
	const T        den = T (sqrt (xi4 + x4));
	const T        mul = T (1) / den;
	const T        fi  = x2  * mul;
	const T        fo  = xi2 * mul;

	return std::array <T, 2> { fo, fi };
}



std::array <fstb::Vf32, 2>	XFadeEqPowC2::compute_gain (fstb::Vf32 x) noexcept
{
	assert (x >= fstb::Vf32 (0));
	assert (x <= fstb::Vf32 (1));

	const auto     one = fstb::Vf32 (1);
	const auto     xi  = one - x;
	const auto     x2  = x   * x;
	const auto     xi2 = xi  * xi;
	const auto     x4  = x2  * x2;
	const auto     xi4 = xi2 * xi2;
	const auto     sum = xi4 + x4;
	const auto     mul = sum.rsqrt_approx ();
	const auto     fi  = x2  * mul;
	const auto     fo  = xi2 * mul;

	return std::array <fstb::Vf32, 2> { fo, fi };
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace wnd
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_wnd_XFadeEqPowC2_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
