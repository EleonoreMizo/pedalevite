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



class DisplayVoid
:	public DisplayInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _scr_w = 128;
	static const int  _scr_h = 64;

	               DisplayVoid ()  = default;
	virtual        ~DisplayVoid () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::ui::DisplayInterface
	virtual int    do_get_width () const;
	virtual int    do_get_height () const;
	virtual int    do_get_stride () const;
	virtual uint8_t *
	               do_use_screen_buf ();
	virtual const uint8_t *
	               do_use_screen_buf () const;

	virtual void   do_refresh (int x, int y, int w, int h);
	virtual void   do_force_reset ();



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	std::array <uint8_t, _scr_w * _scr_h>
	               _buffer;


/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               DisplayVoid (const DisplayVoid &other)       = delete;
	DisplayVoid &  operator = (const DisplayVoid &other)        = delete;
	bool           operator == (const DisplayVoid &other) const = delete;
	bool           operator != (const DisplayVoid &other) const = delete;

}; // class DisplayVoid



}  // namespace ui
}  // namespace mfx



//#include "mfx/ui/DisplayVoid.hpp"



#endif   // mfx_ui_DisplayVoid_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
