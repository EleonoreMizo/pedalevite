/*****************************************************************************

        PrimLine.cpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"
#include "mfx/uitk/grap/PrimLine.h"
#include "mfx/uitk/grap/RenderCtx.h"

#include <algorithm>

#include <cassert>
#include <cstdlib>
#include <cstring>



namespace mfx
{
namespace uitk
{
namespace grap
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	PrimLine::draw (RenderCtx &ctx, int x0, int y0, int x1, int y1, uint8_t c, bool term_flag)
{
	const int      w = ctx.get_w ();
	const int      h = ctx.get_h ();
	if (   x0 >= 0 && x0 < w
	    && x1 >= 0 && x1 < w
	    && y0 >= 0 && y0 < h
	    && y1 >= 0 && y1 < h)
	{
		draw_no_clip (ctx, x0, y0, x1, y1, c, term_flag);
	}
	else
	{
		const int      stride      = ctx.get_stride ();
		const int      dx          =  std::abs (x1 - x0);
		const int      dy          = -std::abs (y1 - y0);
		const int      sx          = (x0 < x1) ? 1 : -1;
		const int      sy          = (y0 < y1) ? 1 : -1;
		const int      sy_buf      = sy * stride;
		int            err         = dx + dy;
		uint8_t *      cur_ptr     = &ctx.at (x0, y0);
		const uint8_t * const end_ptr = &ctx.at (x1, y1);
		bool           clip_h_flag = (x0 >= 0 && x0 < w);
		bool           clip_v_flag = (y0 >= 0 && y0 < h);

		while (cur_ptr != end_ptr)
		{
			if (clip_h_flag && clip_v_flag)
			{
				*cur_ptr = c;
			}
			const int      e2 = err * 2;
			if (e2 >= dy)
			{
				err     += dy;
				x0      += sx;
				cur_ptr += sx;
				clip_h_flag = (x0 >= 0 && x0 < w);
			}
			if (e2 <= dx)
			{
				err     += dx;
				y0      += sy;
				cur_ptr += sy_buf;
				clip_v_flag = (y0 >= 0 && y0 < h);
			}
		}

		if (term_flag)
		{
			assert (end_ptr == cur_ptr);
			if (clip_h_flag && clip_v_flag)
			{
				*cur_ptr = c;
			}
		}
	}
}



void	PrimLine::draw_h (RenderCtx &ctx, int x0, int y0, int x1, uint8_t c, bool term_flag)
{
	if (term_flag)
	{
		x1 += (x1 < x0) ? -1 : 1;
	}
	if (x1 < x0)
	{
		std::swap (x1, x0);
	}
	const int      w = ctx.get_w ();
	const int      h = ctx.get_h ();
	if (   (y0 >= 0 && y0 <  h)
	    && (x0 >= 0 || x1 >  0)
	    && (x0 <  w || x1 <= w))
	{
		x0 = std::max (x0, 0);
		x1 = std::min (x1, w);

		uint8_t *      buf_ptr = &ctx.at (x0, y0);
		memset (buf_ptr, c, x1 - x0);
	}
}



void	PrimLine::draw_v (RenderCtx &ctx, int x0, int y0, int y1, uint8_t c, bool term_flag)
{
	if (term_flag)
	{
		y1 += (y1 < y0) ? -1 : 1;
	}
	if (y1 < y0)
	{
		std::swap (y1, y0);
	}
	const int      w = ctx.get_w ();
	const int      h = ctx.get_h ();
	if (   (x0 >= 0 && x0 <  w)
	    && (y0 >= 0 || y1 >  0)
	    && (y0 <  h || y1 <= h))
	{
		y0 = std::max (y0, 0);
		y1 = std::min (y1, h);

		uint8_t *      buf_ptr = &ctx.at (x0, y0);
		const int      stride  = ctx.get_stride ();
		for (int y = y0; y < y1; ++y)
		{
			*buf_ptr = c;
			buf_ptr += stride;
		}
	}
}



void	PrimLine::draw_no_clip (RenderCtx &ctx, int x0, int y0, int x1, int y1, uint8_t c, bool term_flag)
{
	assert (x0 >= 0);
	assert (x1 >= 0);
	assert (y0 >= 0);
	assert (y1 >= 0);

	const int      w = ctx.get_w ();
	const int      h = ctx.get_h ();
	fstb::unused (w, h);
	assert (x0 < w);
	assert (x1 < w);
	assert (y0 < h);
	assert (y1 < h);

	const int      stride  = ctx.get_stride ();
	const int      dx      =  std::abs (x1 - x0);
	const int      dy      = -std::abs (y1 - y0);
	const int      sx      = (x0 < x1) ? 1      : -1;
	const int      sy_buf  = (y0 < y1) ? stride : -stride;
	int            err     = dx + dy;
	uint8_t *      cur_ptr = &ctx.at (x0, y0);
	const uint8_t * const end_ptr = &ctx.at (x1, y1);

	while (cur_ptr != end_ptr)
	{
		*cur_ptr = c;
		const int      e2 = err * 2;
		if (e2 >= dy)
		{
			err     += dy;
			cur_ptr += sx;
		}
		if (e2 <= dx)
		{
			err     += dx;
			cur_ptr += sy_buf;
		}
	}

	if (term_flag)
	{
		assert (end_ptr == cur_ptr);
		*cur_ptr = c;
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace grap
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
