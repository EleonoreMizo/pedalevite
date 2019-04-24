/*****************************************************************************

        DisplayVoid.cpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/ui/DisplayVoid.h"

#include <cassert>



namespace mfx
{
namespace ui
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	DisplayVoid::do_get_width () const
{
	return _scr_w;
}



int	DisplayVoid::do_get_height () const
{
	return _scr_h;
}



int	DisplayVoid::do_get_stride () const
{
	return _scr_w;
}



uint8_t *	DisplayVoid::do_use_screen_buf ()
{
	return &_buffer [0];
}



const uint8_t *	DisplayVoid::do_use_screen_buf () const
{
	return &_buffer [0];
}



void	DisplayVoid::do_refresh (int /*x*/, int /*y*/, int /*w*/, int /*h*/)
{
	// Nothing
}



void   DisplayVoid::do_force_reset ()
{
	// Nothing
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace ui
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
