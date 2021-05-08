/*****************************************************************************

        XFadeEqPowPoly8.h
        Author: Laurent de Soras, 2021

Original formula:
Geraint Luff, A cheap energy-preserving-ish crossfade, 2021-02-12
https://signalsmith-audio.co.uk/writing/2021/cheap-energy-crossfade/

Maximum is 1.003 at 0.1058

Modification:
(0.975 + 1.51 * a) gives a result always <= 1 while keeping the ripples low
for the power sum, although derivative at the top (out = 1) is not 0.

https://www.desmos.com/calculator/pu6axgieum

Template parameters:

- OFLAG: uses the original formula instead of the original one

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_wnd_XFadeEqPowPoly8_HEADER_INCLUDED)
#define mfx_dsp_wnd_XFadeEqPowPoly8_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/ToolsSimd.h"

#include <array>



namespace mfx
{
namespace dsp
{
namespace wnd
{



template <bool OFLAG>
class XFadeEqPowPoly8
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	template <typename T>
	static inline std::array <T, 2>
	               compute_gain (T x) noexcept;

	static inline std::array <fstb::ToolsSimd::VectF32, 2>
	               compute_gain (fstb::ToolsSimd::VectF32 x) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               XFadeEqPowPoly8 ()                               = delete;
	               XFadeEqPowPoly8 (const XFadeEqPowPoly8 &other)   = delete;
	               XFadeEqPowPoly8 (XFadeEqPowPoly8 &&other)        = delete;
	XFadeEqPowPoly8 &
	               operator = (const XFadeEqPowPoly8 &other)        = delete;
	XFadeEqPowPoly8 &
	               operator = (XFadeEqPowPoly8 &&other)             = delete;
	bool           operator == (const XFadeEqPowPoly8 &other) const = delete;
	bool           operator != (const XFadeEqPowPoly8 &other) const = delete;

}; // class XFadeEqPowPoly8



}  // namespace wnd
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/wnd/XFadeEqPowPoly8.hpp"



#endif   // mfx_dsp_wnd_XFadeEqPowPoly8_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
