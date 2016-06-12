/*****************************************************************************

        NodeInterface.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_NodeInterface_HEADER_INCLUDED)
#define mfx_uitk_NodeInterface_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/Button.h"
#include "mfx/uitk/Rect.h"
#include "mfx/uitk/Vec2d.h"



namespace mfx
{

namespace ui
{
	class DisplayInterface;
}

namespace uitk
{



class ParentInterface;
class NodeEvt;

class NodeInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum OvrAction
	{
		OvrAction_DEFAULT = -1,
		OvrAction_STOP    = -2
	};

	enum EvtProp
	{
		EvtProp_PASS = 0,
		EvtProp_EAT,

		EvtProp_NBR_ELT
	};

	virtual        ~NodeInterface () = default;

	void           notify_attachment (ParentInterface *cont_ptr);
	int            get_id () const;
	Vec2d          get_coord () const;
	Rect           get_bounding_box () const;
	bool           is_browsable () const;
	int            get_override (Button but) const;
	EvtProp        handle_evt (const NodeEvt &evt);
	void           redraw (ui::DisplayInterface &disp, Rect clipbox, Vec2d parent_coord);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual void   do_notify_attachment (ParentInterface *cont_ptr) = 0;
	virtual int    do_get_id () const = 0;
	virtual Vec2d  do_get_coord () const = 0;
	virtual Rect   do_get_bounding_box () const = 0;
	virtual bool   do_is_browsable () const = 0;
	virtual int    do_get_override (Button but) const = 0;
	virtual EvtProp
	               do_handle_evt (const NodeEvt &evt) = 0;
	virtual void   do_redraw (ui::DisplayInterface &disp, Rect clipbox, Vec2d parent_coord) = 0;



}; // class NodeInterface



}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/NodeInterface.hpp"



#endif   // mfx_uitk_NodeInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
