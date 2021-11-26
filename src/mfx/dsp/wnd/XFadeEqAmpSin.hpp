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

	using std::abs;
	using fstb::abs;

	constexpr double  c0 = 0.224008;
	const auto     half = T (0.5);
	const auto     one  = T (1);
	const T        xb   = x + x - one;
	const T        y    = (one - half * abs (xb)) * xb;
	const T        z    = (T (2 * c0) * abs (y) + T (1 - c0)) * y + half;

	return std::array <T, 2> { T (1) - z, z };
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace wnd
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_wnd_XFadeEqAmpSin_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
