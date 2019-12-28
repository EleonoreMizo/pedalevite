/*****************************************************************************

        PrimBox.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_grap_PrimBox_HEADER_INCLUDED)
#define mfx_uitk_grap_PrimBox_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cstdint>



namespace mfx
{
namespace uitk
{
namespace grap
{



class RenderCtx;

class PrimBox
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static void    draw_outline (RenderCtx &ctx, int x0, int y0, int w, int h, uint8_t c);
	static void    draw_filled (RenderCtx &ctx, int x0, int y0, int w, int h, uint8_t c);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               PrimBox ()                               = delete;
	               PrimBox (const PrimBox &other)           = delete;
	virtual        ~PrimBox ()                              = delete;
	PrimBox &      operator = (const PrimBox &other)        = delete;
	bool           operator == (const PrimBox &other) const = delete;
	bool           operator != (const PrimBox &other) const = delete;

}; // class PrimBox



}  // namespace grap
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/grap/PrimBox.hpp"



#endif   // mfx_uitk_grap_PrimBox_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
