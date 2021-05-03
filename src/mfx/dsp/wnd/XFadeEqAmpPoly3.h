/*****************************************************************************

        XFadeEqAmpPoly3.h
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_wnd_XFadeEqAmpPoly3_HEADER_INCLUDED)
#define mfx_dsp_wnd_XFadeEqAmpPoly3_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/ToolsSimd.h"

#include <array>



namespace mfx
{
namespace dsp
{
namespace wnd
{



class XFadeEqAmpPoly3
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

	               XFadeEqAmpPoly3 ()                               = delete;
	               XFadeEqAmpPoly3 (const XFadeEqAmpPoly3 &other)   = delete;
	               XFadeEqAmpPoly3 (XFadeEqAmpPoly3 &&other)        = delete;
	XFadeEqAmpPoly3 &
	               operator = (const XFadeEqAmpPoly3 &other)        = delete;
	XFadeEqAmpPoly3 &
	               operator = (XFadeEqAmpPoly3 &&other)             = delete;
	bool           operator == (const XFadeEqAmpPoly3 &other) const = delete;
	bool           operator != (const XFadeEqAmpPoly3 &other) const = delete;

}; // class XFadeEqAmpPoly3



}  // namespace wnd
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/wnd/XFadeEqAmpPoly3.hpp"



#endif   // mfx_dsp_wnd_XFadeEqAmpPoly3_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
