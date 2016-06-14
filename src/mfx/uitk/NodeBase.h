/*****************************************************************************

        NodeBase.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_NodeBase_HEADER_INCLUDED)
#define mfx_uitk_NodeBase_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/NodeInterface.h"

#include <cstdint>



namespace mfx
{
namespace uitk
{



class ParentInterface;

class NodeBase
:	public NodeInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       NodeBase (int node_id);
	               NodeBase ()                        = default;
	               NodeBase (const NodeBase &other)   = default;
	virtual        ~NodeBase ()                       = default;

	NodeBase &     operator = (const NodeBase &other) = default;

	void           set_node_id (int node_id);
	ParentInterface *
	               get_parent () const;
	virtual void   set_coord (Vec2d pos);
	virtual void   invalidate_all ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	static void    invert_zone (uint8_t *buf_ptr, int w, int h, int stride);

	// mfx::uitk::NodeInterface (partial)
	virtual void   do_notify_attachment (ParentInterface *cont_ptr);
	virtual int    do_get_id () const;
	virtual Vec2d  do_get_coord () const;
	virtual EvtProp
	               do_handle_evt (const NodeEvt &evt);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	int            _node_id    = -1;    // Negative: not set
	ParentInterface *                   // 0 = not attached.
	               _parent_ptr = 0;
	Vec2d          _coord;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



}; // class NodeBase



}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/NodeBase.hpp"



#endif   // mfx_uitk_NodeBase_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
