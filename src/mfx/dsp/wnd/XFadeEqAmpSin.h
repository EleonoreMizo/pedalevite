/*****************************************************************************

        XFadeEqAmpSin.h
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_wnd_XFadeEqAmpSin_HEADER_INCLUDED)
#define mfx_dsp_wnd_XFadeEqAmpSin_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/Vf32.h"

#include <array>



namespace mfx
{
namespace dsp
{
namespace wnd
{



class XFadeEqAmpSin
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	template <typename T>
	static inline std::array <T, 2>
	               compute_gain (T x) noexcept;

	static inline std::array <fstb::Vf32, 2>
	               compute_gain (fstb::Vf32 x) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               XFadeEqAmpSin ()                               = delete;
	               XFadeEqAmpSin (const XFadeEqAmpSin &other)     = delete;
	               XFadeEqAmpSin (XFadeEqAmpSin &&other)          = delete;
	XFadeEqAmpSin& operator = (const XFadeEqAmpSin &other)        = delete;
	XFadeEqAmpSin& operator = (XFadeEqAmpSin &&other)             = delete;
	bool           operator == (const XFadeEqAmpSin &other) const = delete;
	bool           operator != (const XFadeEqAmpSin &other) const = delete;

}; // class XFadeEqAmpSin



}  // namespace wnd
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/wnd/XFadeEqAmpSin.hpp"



#endif   // mfx_dsp_wnd_XFadeEqAmpSin_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
