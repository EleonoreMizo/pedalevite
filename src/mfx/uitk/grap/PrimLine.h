/*****************************************************************************

        PrimLine.h
        Author: Laurent de Soras, 2019

Simple line drawing using the Bresenham's algorithm
https://en.wikipedia.org/wiki/Bresenham's_line_algorithm

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_grap_PrimLine_HEADER_INCLUDED)
#define mfx_uitk_grap_PrimLine_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cstdint>



namespace mfx
{
namespace uitk
{
namespace grap
{



class RenderCtx;

class PrimLine
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static void    draw (RenderCtx &ctx, int x0, int y0, int x1, int y1, uint8_t c, bool term_flag);
	static void    draw_h (RenderCtx &ctx, int x0, int y0, int x1, uint8_t c, bool term_flag);
	static void    draw_v (RenderCtx &ctx, int x0, int y0, int y1, uint8_t c, bool term_flag);

	static void    draw_no_clip (RenderCtx &ctx, int x0, int y0, int x1, int y1, uint8_t c, bool term_flag);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               PrimLine ()                               = delete;
	               PrimLine (const PrimLine &other)          = delete;
	virtual        ~PrimLine ()                              = delete;
	PrimLine &     operator = (const PrimLine &other)        = delete;
	bool           operator == (const PrimLine &other) const = delete;
	bool           operator != (const PrimLine &other) const = delete;

}; // class PrimLine



}  // namespace grap
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/grap/PrimLine.hpp"



#endif   // mfx_uitk_grap_PrimLine_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
