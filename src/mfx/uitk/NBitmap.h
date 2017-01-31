/*****************************************************************************

        NBitmap.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_NBitmap_HEADER_INCLUDED)
#define mfx_uitk_NBitmap_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/ui/DisplayInterface.h"
#include "mfx/uitk/NodeBase.h"

#include <vector>

#include <cstdint>



namespace mfx
{
namespace uitk
{



class NBitmap
:	public NodeBase
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef NodeBase Inherited;

	explicit       NBitmap (int node_id);
	               NBitmap ()                        = default;
	               NBitmap (const NBitmap &other)    = default;
	virtual        ~NBitmap ()                       = default;

	NBitmap &      operator = (const NBitmap &other) = default;

	void           set_size (Vec2d sz);
	void           show (bool flag);
	void           set_blend_mode (ui::DisplayInterface::BlendMode mode);
	const uint8_t* use_buffer () const;
	uint8_t *      use_buffer ();
	int            get_stride () const;
	bool           has_cursor () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::uitk::NodeInterface via mfx::uitk::NodeBase
	virtual Rect   do_get_bounding_box () const;
	virtual EvtProp
	               do_handle_evt (const NodeEvt &evt);
	virtual void   do_redraw (ui::DisplayInterface &disp, Rect clipbox, Vec2d parent_coord);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	Vec2d          _size;
	std::vector <uint8_t>
	               _buffer;
	ui::DisplayInterface::BlendMode
	               _blend_mode  = ui::DisplayInterface::BlendMode_OPAQUE;
	bool           _cursor_flag = false;
	bool           _show_flag   = true;

	static void    invert_zone (ui::DisplayInterface &disp, Rect zone);



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const NBitmap &other) const = delete;
	bool           operator != (const NBitmap &other) const = delete;

}; // class NBitmap



}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/NBitmap.hpp"



#endif   // mfx_uitk_NBitmap_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
