/*****************************************************************************

        NotYet.cpp
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
#include "mfx/uitk/pg/NotYet.h"
#include "mfx/uitk/NodeEvt.h"
#include "mfx/uitk/PageMgrInterface.h"
#include "mfx/uitk/PageSwitcher.h"
#include "mfx/ui/Font.h"
#include "mfx/Model.h"
#include "mfx/View.h"

#include <cassert>



namespace mfx
{
namespace uitk
{
namespace pg
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



NotYet::NotYet (PageSwitcher &page_switcher)
:	_page_switcher (page_switcher)
,	_view_ptr (0)
,	_page_ptr (0)
,	_page_size ()
,	_notyet_sptr (new NText (0))
,	_back_sptr (  new NText (1))
{
	_notyet_sptr->set_justification (0.5f, 0.5f, false);
	_back_sptr  ->set_justification (0.5f, 1   , false);

	_back_sptr  ->set_text ("Back");
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	NotYet::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt)
{
	fstb::unused (model, usr_ptr);

	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;

	_notyet_sptr->set_font (fnt._l);
	_back_sptr  ->set_font (fnt._s);

	static const std::string   txt_ny ("NOT YET");
	_notyet_sptr->set_text (txt_ny);

	const int      x_mid =  _page_size [0] >> 1;
	const int      h_s   = fnt._s.get_char_h ();
	const int      h_l   = fnt._l.get_char_h ();
	const int      w_l   = fnt._l.get_char_w ();
	const int      mag_x = _page_size [0] / (w_l * int (txt_ny.length ()));
	const int      mag_y = (_page_size [1] - h_s) / h_l;
	_notyet_sptr->set_mag (mag_x, mag_y);

	_back_sptr->set_frame (Vec2d (_page_size [0] >> 1, 0), Vec2d ());

	_notyet_sptr->set_coord (Vec2d (x_mid, (_page_size [1] - h_s) >> 1));
	_back_sptr  ->set_coord (Vec2d (x_mid,  _page_size [1]            ));

	_page_ptr->push_back (_notyet_sptr);
	_page_ptr->push_back (_back_sptr  );

	PageMgrInterface::NavLocList  nav_list (1);
	nav_list.back ()._node_id = 1;
	page.set_nav_layout (nav_list);
	page.jump_to (nav_list [0]._node_id);
}



void	NotYet::do_disconnect ()
{
	// Nothing
}



MsgHandlerInterface::EvtProp	NotYet::do_handle_evt (const NodeEvt &evt)
{
	EvtProp        ret_val = EvtProp_PASS;

	if (evt.is_button_ex ())
	{
		_page_switcher.return_page ();
		ret_val = EvtProp_CATCH;
	}

	return ret_val;
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
