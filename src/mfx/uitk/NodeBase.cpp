/*****************************************************************************

        NodeBase.cpp
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

#include "fstb/def.h"
#include "mfx/uitk/ParentInterface.h"
#include "mfx/uitk/NodeBase.h"

#include <cassert>



namespace mfx
{
namespace uitk
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



NodeBase::NodeBase (int node_id)
:	_node_id (node_id)
{
	assert (node_id >= 0);
}



void	NodeBase::set_node_id (int node_id)
{
	assert (node_id >= 0);

	_node_id = node_id;
}



ParentInterface *	NodeBase::get_parent () const
{
	return _parent_ptr;
}



void	NodeBase::set_coord (Vec2d pos)
{
	const Rect     bbox (get_bounding_box ());
	const Rect     zone_old (bbox + _coord);
	_coord = pos;
	const Rect     zone_new (bbox + _coord);

	if (_parent_ptr != 0)
	{
		_parent_ptr->invalidate (zone_old);
		_parent_ptr->invalidate (zone_new);
	}
}



void	NodeBase::invalidate_all ()
{
	ParentInterface * parent_ptr = get_parent ();
	if (parent_ptr != 0)
	{
		Rect           zone (do_get_bounding_box ());
		zone += get_coord ();
		parent_ptr->invalidate (zone);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	NodeBase::invert_zone (uint8_t *buf_ptr, int w, int h, int stride)
{
	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			buf_ptr [x] = ~buf_ptr [x];
		}

		buf_ptr += stride;
	}
}



void	NodeBase::do_notify_attachment (ParentInterface *cont_ptr)
{
	_parent_ptr = cont_ptr;
}



int	NodeBase::do_get_id () const
{
	return _node_id;
}



Vec2d	NodeBase::do_get_coord () const
{
	return _coord;
}



MsgHandlerInterface::EvtProp	NodeBase::do_handle_evt (const NodeEvt &evt)
{
	fstb::unused (evt);

	return EvtProp_PASS;
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
