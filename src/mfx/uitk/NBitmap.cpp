/*****************************************************************************

        NBitmap.cpp
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
#include "mfx/uitk/NBitmap.h"
#include "mfx/uitk/NodeEvt.h"
#include "mfx/uitk/Rect.h"

#include <cassert>



namespace mfx
{
namespace uitk
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



NBitmap::NBitmap (int node_id)
:	Inherited (node_id)
{
	// Nothing
}



void	NBitmap::set_size (Vec2d sz)
{
	assert (sz [0] >= 0);
	assert (sz [1] >= 0);

	_size = sz;
	_buffer.resize (_size [0] * _size [1]);

	// Don't invalidate anything now because the buffer may contain garbage
}



void	NBitmap::show (bool flag)
{
	_show_flag = flag;
	invalidate_all ();
}



void	NBitmap::set_blend_mode (ui::DisplayInterface::BlendMode mode)
{
	assert (mode >= 0);
	assert (mode < ui::DisplayInterface::BlendMode_NBR_ELT);

	_blend_mode = mode;
	invalidate_all ();
}



const uint8_t *	NBitmap::use_buffer () const
{
	assert (_size [0] > 0);
	assert (_size [1] > 0);

	return &_buffer [0];
}



uint8_t *	NBitmap::use_buffer ()
{
	assert (_size [0] > 0);
	assert (_size [1] > 0);

	return &_buffer [0];
}



bool	NBitmap::has_cursor () const
{
	return _cursor_flag;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Rect	NBitmap::do_get_bounding_box () const
{
	return Rect (Vec2d (), _size);
}



MsgHandlerInterface::EvtProp	NBitmap::do_handle_evt (const NodeEvt &evt)
{
	EvtProp        ret_val = EvtProp_PASS;

	if (evt.get_target () == get_id ())
	{
		const NodeEvt::Type  type = evt.get_type ();
		if (type == NodeEvt::Type_CURSOR)
		{
			const NodeEvt::Curs  curs = evt.get_cursor ();
			_cursor_flag = (curs == NodeEvt::Curs_ENTER);
			invalidate_all ();

			ret_val = EvtProp_CATCH;
		}
	}

	return ret_val;
}



void	NBitmap::do_redraw (ui::DisplayInterface &disp, Rect clipbox, Vec2d parent_coord)
{
	if (_show_flag)
	{
		Rect           bitmap_abs (Vec2d (), _size);
		const Vec2d    node_coord (parent_coord + get_coord ());
		bitmap_abs += node_coord;
		bitmap_abs.intersect (clipbox);
		if (! bitmap_abs.empty ())
		{
			assert (! _buffer.empty ());

			const Rect     bitmap_rel (bitmap_abs - node_coord);
			const Vec2d    disp_size (bitmap_abs.get_size ());

			disp.bitblt (
				bitmap_abs [0] [0], bitmap_abs [0] [1],
				&_buffer [0],
				bitmap_rel [0] [0], bitmap_rel [0] [1],
				disp_size [0], disp_size [1],
				_size [0],
				_blend_mode
			);

			if (_cursor_flag)
			{
				invert_zone (disp, bitmap_abs);
			}
		}
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	NBitmap::invert_zone (ui::DisplayInterface &disp, Rect zone)
{
	const Vec2d    disp_size (zone.get_size ());
	const int      stride  = disp.get_stride ();
	uint8_t *      dst_ptr = disp.use_screen_buf ();

	dst_ptr += zone [0] [1] * stride + zone [0] [0];

	NodeBase::invert_zone (dst_ptr, disp_size [0], disp_size [1], stride);
}



}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
