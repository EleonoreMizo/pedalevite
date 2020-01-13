/*****************************************************************************

        NWindow.cpp
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

#include "fstb/fnc.h"
#include "mfx/uitk/NodeEvt.h"
#include "mfx/uitk/NWindow.h"

#include <cassert>



namespace mfx
{
namespace uitk
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



NWindow::NWindow (int node_id)
:	_node_id (node_id)
{
	assert (node_id >= 0);
}



void	NWindow::set_size (Vec2d disp, Vec2d virt)
{
	assert (virt [0] == 0 || virt [0] >= disp [0]);
	assert (virt [1] == 0 || virt [1] >= disp [1]);

	_size_disp = disp;
	_size_virt = virt;
}



void	NWindow::set_disp_pos (Vec2d pos)
{
	assert (_size_virt [0] == 0 || pos [0] >= 0);
	assert (_size_virt [1] == 0 || pos [1] >= 0);
	assert (_size_virt [0] == 0 || pos [0] <= _size_virt [0] - _size_disp [0]);
	assert (_size_virt [1] == 0 || pos [1] <= _size_virt [1] - _size_disp [1]);

	_pos_virt = pos;
	invalidate_all ();
}



void	NWindow::set_autoscroll (bool flag)
{
	_autoscroll_flag = flag;
}



void	NWindow::set_node_id (int node_id)
{
	assert (node_id >= 0);

	_node_id = node_id;
}



ParentInterface *	NWindow::get_parent () const
{
	return _parent_ptr;
}



void	NWindow::set_coord (Vec2d pos)
{
	const Rect     bbox (get_bounding_box ());
	const Rect     zone_old (bbox + _coord);
	_coord = pos;
	const Rect     zone_new (bbox + _coord);

	if (_parent_ptr != nullptr)
	{
		_parent_ptr->invalidate (zone_old);
		_parent_ptr->invalidate (zone_new);
	}
}



void	NWindow::invalidate_all ()
{
	ParentInterface * parent_ptr = get_parent ();
	if (parent_ptr != nullptr)
	{
		Rect           zone (do_get_bounding_box ());
		zone += get_coord ();
		parent_ptr->invalidate (zone);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	NWindow::do_push_back (NodeSPtr node_sptr)
{
	_node_list.push_back (node_sptr);
	node_sptr->notify_attachment (this);
}



void	NWindow::do_set_node (int pos, NodeSPtr node_sptr)
{
	_node_list [pos]->notify_attachment (nullptr);
	_node_list [pos] = node_sptr;
	node_sptr->notify_attachment (this);
}



void	NWindow::do_insert (int pos, NodeSPtr node_sptr)
{
	_node_list.insert (_node_list.begin () + pos, node_sptr);
	node_sptr->notify_attachment (this);
}



void	NWindow::do_erase (int pos)
{
	_node_list [pos]->notify_attachment (nullptr);
	_node_list.erase (_node_list.begin () + pos);
}



int	NWindow::do_get_nbr_nodes () const
{
	return int (_node_list.size ());
}



ContainerInterface::NodeSPtr	NWindow::do_use_node (int pos)
{
	return _node_list [pos];
}



Vec2d	NWindow::do_get_coord_abs () const
{
	Vec2d          coord;

	coord -= _pos_virt;

	if (_parent_ptr != nullptr)
	{
		coord += _parent_ptr->get_coord_abs ();
	}

	return coord;
}



void	NWindow::do_invalidate (const Rect &zone)
{
	if (_parent_ptr != nullptr)
	{
		Rect           zone_parent (zone + _coord - _pos_virt);
		_parent_ptr->invalidate (zone_parent);
	}
}



void	NWindow::do_notify_attachment (ParentInterface *cont_ptr)
{
	_parent_ptr = cont_ptr;
}



int	NWindow::do_get_id () const
{
	return _node_id;
}



Vec2d	NWindow::do_get_coord () const
{
	return _coord;
}



Rect	NWindow::do_get_bounding_box () const
{
	return Rect (Vec2d (), _size_disp);
}



MsgHandlerInterface::EvtProp	NWindow::do_handle_evt (const NodeEvt &evt)
{
	EvtProp        ret_val = EvtProp_PASS;

	const NodeEvt::Type  type   = evt.get_type ();
	const int            target = evt.get_target ();

	for (auto &node_sptr : _node_list)
	{
		// Ensure that the selected node remains within the visible area
		if (   _autoscroll_flag
		    && type   == NodeEvt::Type_CURSOR
		    && target == node_sptr->get_id ())
		{
			const NodeEvt::Curs  curs = evt.get_cursor ();
			if (curs == NodeEvt::Curs_ENTER)
			{
				keep_node_visible (*node_sptr);
			}
		}

		ret_val = node_sptr->handle_evt (evt);
		if (ret_val == EvtProp_CATCH)
		{
			break;
		}
	}

	return ret_val;
}



void	NWindow::do_redraw (ui::DisplayInterface &disp, Rect clipbox, Vec2d parent_coord)
{
	Vec2d          this_coord (parent_coord + _coord);
	clipbox.intersect (get_bounding_box () + this_coord);

	for (auto &node_sptr : _node_list)
	{
		node_sptr->redraw (disp, clipbox, this_coord - _pos_virt);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	NWindow::keep_node_visible (const NodeInterface &node)
{
	const Rect     node_box_orig (node.get_bounding_box ());

	// Relative to the virtual window top-left corner
	Rect           node_box (node_box_orig + node.get_coord ());

	// Clips the object's box to the largest visible area,
	// keeping at least the top-left part
	Vec2d          box_size (node_box.get_size ());
	for (size_t c = 0; c < box_size.size (); ++c)
	{
		box_size [c] = std::min (box_size [c], _size_disp [c]);
	}
	node_box = Rect (node_box [0], node_box [0] + box_size);

	// Distance of the box corners from the corners of the visible area
	const Vec2d    dist_tl (node_box [0] -  _pos_virt              );
	const Vec2d    dist_br (node_box [1] - (_pos_virt + _size_disp));

	// Finds the required displacement
	Vec2d          shift;
	for (size_t c = 0; c < box_size.size (); ++c)
	{
		if (dist_tl [c] < 0)
		{
			shift [c] = dist_tl [c];
		}
		else if (dist_br [c] > 0)
		{
			shift [c] = dist_br [c];
		}
	}

	const Vec2d    pos_virt_old (_pos_virt);
	_pos_virt += shift;
	const Vec2d    pos_virt_max (_size_virt - _size_disp);
	for (size_t c = 0; c < box_size.size (); ++c)
	{
		if (_size_virt [0] != 0)
		{
			_pos_virt [c] = fstb::limit (_pos_virt [c], 0, pos_virt_max [c]);
		}
	}

	if (_pos_virt != pos_virt_old)
	{
		invalidate_all ();
	}
}



}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
