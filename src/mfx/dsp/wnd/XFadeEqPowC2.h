/*****************************************************************************

        XFadeEqPowC2.h
        Author: Laurent de Soras, 2021

This fade function has C2 continuity.

Formula by Andrew Simper, 2021-05-03
https://discord.com/channels/507604115854065674/507630527847596046/838757044625997865

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_wnd_XFadeEqPowC2_HEADER_INCLUDED)
#define mfx_dsp_wnd_XFadeEqPowC2_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/ToolsSimd.h"

#include <array>



namespace mfx
{
namespace dsp
{
namespace wnd
{



class XFadeEqPowC2
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

	               XFadeEqPowC2 ()                               = delete;
	               XFadeEqPowC2 (const XFadeEqPowC2 &other)      = delete;
	               XFadeEqPowC2 (XFadeEqPowC2 &&other)           = delete;
	XFadeEqPowC2 & operator = (const XFadeEqPowC2 &other)        = delete;
	XFadeEqPowC2 & operator = (XFadeEqPowC2 &&other)             = delete;
	bool           operator == (const XFadeEqPowC2 &other) const = delete;
	bool           operator != (const XFadeEqPowC2 &other) const = delete;

}; // class XFadeEqPowC2



}  // namespace wnd
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/wnd/XFadeEqPowC2.hpp"



#endif   // mfx_dsp_wnd_XFadeEqPowC2_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
