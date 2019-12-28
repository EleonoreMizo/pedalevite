/*****************************************************************************

        RenderCtx.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_grap_RenderCtx_HEADER_INCLUDED)
#define mfx_uitk_grap_RenderCtx_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/Vec2d.h"

#include <cstdint>



namespace mfx
{
namespace uitk
{
namespace grap
{



class RenderCtx
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       RenderCtx (uint8_t *buf_ptr, const Vec2d &sz, int stride);
	               RenderCtx (const RenderCtx &other)  = default;
	virtual        ~RenderCtx ()                       = default;

	inline uint8_t *
	               use_buf ();
	inline const uint8_t *
	               use_buf () const;
	inline const Vec2d &
	               use_size () const;
	inline int     get_w () const;
	inline int     get_h () const;
	inline int     get_stride () const;

	inline uint8_t &
	               at (int x, int y);
	inline const uint8_t &
	               at (int x, int y) const;




/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	uint8_t *const _buf_ptr;
	const Vec2d    _sz;
	const int      _stride;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               RenderCtx ()                               = delete;
	RenderCtx &    operator = (const RenderCtx &other)        = delete;
	bool           operator == (const RenderCtx &other) const = delete;
	bool           operator != (const RenderCtx &other) const = delete;

}; // class RenderCtx



}  // namespace grap
}  // namespace uitk
}  // namespace mfx



#include "mfx/uitk/grap/RenderCtx.hpp"



#endif   // mfx_uitk_grap_RenderCtx_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
