/*****************************************************************************

        NWindow.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_NWindow_HEADER_INCLUDED)
#define mfx_uitk_NWindow_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/ContainerInterface.h"

#include <vector>



namespace mfx
{
namespace uitk
{



class NWindow
:	public NodeInterface
,	public ContainerInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       NWindow (int node_id);
	               NWindow ()  = default;
	               ~NWindow () = default;

	void           set_size (Vec2d disp, Vec2d virt);
	void           set_disp_pos (Vec2d pos);
	void           set_autoscroll (bool flag);

	void           set_node_id (int node_id);
	ParentInterface *
	               get_parent () const;
	virtual void   set_coord (Vec2d pos);
	virtual void   invalidate_all ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::uitk::ContainerInterface
	void           do_push_back (NodeSPtr node_sptr) override;
	void           do_set_node (int pos, NodeSPtr node_sptr) override;
	void           do_insert (int pos, NodeSPtr node_sptr) override;
	void           do_erase (int pos) override;
	int            do_get_nbr_nodes () const override;
	NodeSPtr       do_use_node (int pos) override;

	// mfx::uitk::ParentInterface via mfx::uitk::ContainerInterface
	Vec2d          do_get_coord_abs () const override;
	void           do_invalidate (const Rect &zone) override;

	// mfx::uitk::NodeInterface
	void           do_notify_attachment (ParentInterface *cont_ptr) override;
	int            do_get_id () const override;
	Vec2d          do_get_coord () const override;
	Rect           do_get_bounding_box () const override;
	EvtProp        do_handle_evt (const NodeEvt &evt) override;
	void           do_redraw (ui::DisplayInterface &disp, Rect clipbox, Vec2d node_coord) override;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           keep_node_visible (const NodeInterface &node);

	int            _node_id        = -1;   // Negative: not set
	ParentInterface *                      // 0 = not attached.
	               _parent_ptr     = nullptr;
	Vec2d          _coord;

	Vec2d          _size_disp;
	Vec2d          _size_virt;             // Null component = unlimited (int both directions)
	Vec2d          _pos_virt;

	std::vector <NodeSPtr>
	               _node_list;

	bool           _autoscroll_flag = true;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               NWindow (const NWindow &other)           = delete;
	               NWindow (NWindow &&other)                = delete;
	NWindow &      operator = (const NWindow &other)        = delete;
	NWindow &      operator = (NWindow &&other)             = delete;
	bool           operator == (const NWindow &other) const = delete;
	bool           operator != (const NWindow &other) const = delete;

}; // class NWindow



}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/NWindow.hpp"



#endif   // mfx_uitk_NWindow_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
