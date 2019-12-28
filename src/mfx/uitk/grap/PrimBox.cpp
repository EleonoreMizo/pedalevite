/*****************************************************************************

        PrimBox.cpp
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

#include "mfx/uitk/grap/PrimBox.h"
#include "mfx/uitk/grap/PrimLine.h"
#include "mfx/uitk/grap/RenderCtx.h"

#include <algorithm>

#include <cassert>
#include <cstring>



namespace mfx
{
namespace uitk
{
namespace grap
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	PrimBox::draw_outline (RenderCtx &ctx, int x0, int y0, int w, int h, uint8_t c)
{
	assert (w > 0);
	assert (h > 0);

	PrimLine::draw_h (ctx, x0    , y0    , x0 + w, c, false);
	PrimLine::draw_v (ctx, x0 + w, y0    , y0 + h, c, false);
	PrimLine::draw_h (ctx, x0 + w, y0 + h, x0    , c, false);
	PrimLine::draw_v (ctx, x0    , y0 + h, y0    , c, false);
}



void	PrimBox::draw_filled (RenderCtx &ctx, int x0, int y0, int w, int h, uint8_t c)
{
	assert (w > 0);
	assert (h > 0);

	int            x1 = x0 + w;
	int            y1 = y0 + h;

	const int      cw = ctx.get_w ();
	const int      ch = ctx.get_h ();

	if (   (x0 >= 0  || x1 >  0 )
	    && (x0 <  cw || x1 <= cw)
	    && (y0 >= 0  || y1 >  0 )
	    && (y0 <  ch || y1 <= ch))
	{
		x0 = std::max (x0, 0 );
		x1 = std::min (x1, cw);
		y0 = std::max (y0, 0 );
		y1 = std::min (y1, ch);
		w  = x1 - x0;
		const int      stride  = ctx.get_stride ();
		uint8_t *      cur_ptr = &ctx.at (x0, y0);
		for (int y = y0; y < y1; ++y)
		{
			memset (cur_ptr, c, w);
			cur_ptr += stride;
		}
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace grap
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
