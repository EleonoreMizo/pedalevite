/*****************************************************************************

        WsNoisifier.h
        Author: Laurent de Soras, 2021

Turns signal into white noise as the input gain is increased.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_shape_WsNoisifier_HEADER_INCLUDED)
#define mfx_dsp_shape_WsNoisifier_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/Approx.h"
#include "fstb/def.h"
#include "fstb/fnc.h"

#include <algorithm>



namespace mfx
{
namespace dsp
{
namespace shape
{



class WsNoisifier
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	template <typename T>
	fstb_FORCEINLINE T operator () (T x) noexcept
	{
		return process_sample (x);
	}

	template <typename T>
	static fstb_FORCEINLINE T process_sample (T x) noexcept
	{
		// Limits the x^5 factor to make sure we don't overflow the
		// subsequent round_int().
		constexpr auto lim = T (100);
		constexpr auto li2 = lim * lim;
		constexpr auto tp  = T (2 * fstb::PI);
		constexpr auto a1  = T (1   ) / tp;
		constexpr auto a5  = T (0.25) / tp;
		const auto     x2  = std::min (x * x, li2);
		const auto     x4  = x2 * x2;
		const auto     a2p = x * (a1 + a5 * x4);
		const auto     n   = fstb::round_int (a2p);
		const auto     arg = T (a2p - n);
		const auto     y   = fstb::Approx::sin_nick_2pi (arg);

		return y;
	}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

}; // class WsNoisifier



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/shape/WsNoisifier.hpp"



#endif   // mfx_dsp_shape_WsNoisifier_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
