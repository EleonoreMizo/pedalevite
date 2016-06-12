/*****************************************************************************

        NodeInterface.cpp
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

#include "mfx/uitk/NodeInterface.h"

#include <cassert>



namespace mfx
{
namespace uitk
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// 0 to detach
void	NodeInterface::notify_attachment (ParentInterface *cont_ptr)
{
	do_notify_attachment (cont_ptr);
}



int	NodeInterface::get_id () const
{
	const int      ret_val = do_get_id ();
	assert (ret_val >= 0);

	return ret_val;
}



// Relative to the container coordinates
Vec2d	NodeInterface::get_coord () const
{
	return do_get_coord ();
}



// Relative to the internal coordinates given by get_coord()
Rect	NodeInterface::get_bounding_box () const
{
	return do_get_bounding_box ();
}



bool	NodeInterface::is_browsable () const
{
	return do_is_browsable ();
}



int	NodeInterface::get_override (Button but) const
{
	assert (but >= 0);
	assert (but < Button_NBR_ELT);

	const int      ret_val = do_get_override (but);
	assert (   ret_val >= 0
	        || ret_val == OvrAction_DEFAULT
	        || ret_val == OvrAction_STOP);

	return ret_val;
}



NodeInterface::EvtProp	NodeInterface::handle_evt (const NodeEvt &evt)
{
	const EvtProp  propag = do_handle_evt (evt);
	assert (propag >= 0);
	assert (propag < EvtProp_NBR_ELT);

	return propag;
}



// Absolute coordinates of the node
void	NodeInterface::redraw (ui::DisplayInterface &disp, Rect clipbox, Vec2d parent_coord)
{
	do_redraw (disp, clipbox, parent_coord);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
