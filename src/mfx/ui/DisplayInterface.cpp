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
#include <cstring>



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



void	DisplayInterface::bitblt (int xd, int yd, const uint8_t *src_ptr, int xs, int ys, int ws, int hs, int ss, BlendMode mode)
{
	assert (xd >= 0);
	assert (xd + ws <= get_width ());
	assert (yd >= 0);
	assert (yd + hs <= get_height ());

	const int      sd      = get_stride ();
	uint8_t *      dst_ptr = use_screen_buf ();

	src_ptr += ys * ss + xs;
	dst_ptr += yd * sd + xd;

	if (ss == sd && ws == ss && mode == BlendMode_OPAQUE)
	{
		memcpy (dst_ptr, src_ptr, hs * ws);
	}
	else
	{
		for (int y = 0; y < hs; ++y)
		{
			switch (mode)
			{
			case BlendMode_XOR:
				for (int x = 0; x < ws; ++x)
				{
					dst_ptr [x] ^= src_ptr [x];
				}
				break;
			default:
				memcpy (dst_ptr, src_ptr, ws);
				break;
			}
			src_ptr += ss;
			dst_ptr += sd;
		}
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace ui
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
