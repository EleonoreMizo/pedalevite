/*****************************************************************************

        RenderCtx.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_uitk_grap_RenderCtx_CODEHEADER_INCLUDED)
#define mfx_uitk_grap_RenderCtx_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace uitk
{
namespace grap
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



uint8_t *	RenderCtx::use_buf ()
{
	return _buf_ptr;
}



const uint8_t*	RenderCtx::use_buf () const
{
	return _buf_ptr;
}



const Vec2d &	RenderCtx::use_size () const
{
	return _sz;
}



int	RenderCtx::get_w () const
{
	return _sz [0];
}



int	RenderCtx::get_h () const
{
	return _sz [1];
}



int	RenderCtx::get_stride () const
{
	return _stride;
}



uint8_t &	RenderCtx::at (int x, int y)
{
	assert (x >= 0);
	assert (x < _sz [0]);
	assert (y >= 0);
	assert (y < _sz [1]);

	return _buf_ptr [y * _stride + x];
}



const uint8_t &	RenderCtx::at (int x, int y) const
{
	assert (x >= 0);
	assert (x < _sz [0]);
	assert (y >= 0);
	assert (y < _sz [1]);

	return _buf_ptr [y * _stride + x];
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace grap
}  // namespace uitk
}  // namespace mfx



#endif   // mfx_uitk_grap_RenderCtx_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
