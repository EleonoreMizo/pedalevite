/*****************************************************************************

        XFadeEqPowPoly8.h
        Author: Laurent de Soras, 2021

Formula:

Geraint Luff, A cheap energy-preserving-ish crossfade, 2021-02-12
https://signalsmith-audio.co.uk/writing/2021/cheap-energy-crossfade/

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



class XFadeEqPowPoly8
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	template <typename T>
	static inline std::array <T, 2>
	               compute_gain (T x);

	static inline std::array <fstb::ToolsSimd::VectF32, 2>
	               compute_gain (fstb::ToolsSimd::VectF32 x);



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
