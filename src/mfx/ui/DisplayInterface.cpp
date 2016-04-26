/*****************************************************************************

        DisplayInterface.cpp
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

#include "mfx/ui/DisplayInterface.h"

#include <cassert>



namespace mfx
{
namespace ui
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	DisplayInterface::get_width () const
{
	const int      w = do_get_width ();
	assert (w > 0);

	return w;
}



int	DisplayInterface::get_height () const
{
	const int      h = do_get_height ();
	assert (h > 0);

	return h;
}



// In pixels
int	DisplayInterface::get_stride () const
{
	const int      s = do_get_stride ();
	assert (s >= get_width ());

	return s;
}



uint8_t *	DisplayInterface::use_screen_buf ()
{
	uint8_t *      buf_ptr = do_use_screen_buf ();
	assert (buf_ptr != 0);

	return buf_ptr;
}



const uint8_t *	DisplayInterface::use_screen_buf () const
{
	const uint8_t *buf_ptr = do_use_screen_buf ();
	assert (buf_ptr != 0);

	return buf_ptr;
}



void	DisplayInterface::refresh (int x, int y, int w, int h)
{
	assert (x >= 0);
	assert (y >= 0);
	assert (w > 0);
	assert (h > 0);
	assert (x + w <= get_width ());
	assert (y + h <= get_height ());

	do_refresh (x, y, w, h);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace ui
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
