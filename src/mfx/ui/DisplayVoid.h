/*****************************************************************************

        DisplayVoid.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_ui_DisplayVoid_HEADER_INCLUDED)
#define mfx_ui_DisplayVoid_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/ui/DisplayInterface.h"

#include <array>

#include <cstdint>



namespace mfx
{
namespace ui
{



class DisplayVoid final
:	public DisplayInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _scr_w = 128;
	static const int  _scr_h = 64;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::ui::DisplayInterface
	int            do_get_width () const final;
	int            do_get_height () const final;
	int            do_get_stride () const final;
	uint8_t *      do_use_screen_buf () final;
	const uint8_t* do_use_screen_buf () const final;

	void           do_refresh (int x, int y, int w, int h) final;
	void           do_force_reset () final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	std::array <uint8_t, _scr_w * _scr_h>
	               _buffer;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

}; // class DisplayVoid



}  // namespace ui
}  // namespace mfx



//#include "mfx/ui/DisplayVoid.hpp"



#endif   // mfx_ui_DisplayVoid_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
