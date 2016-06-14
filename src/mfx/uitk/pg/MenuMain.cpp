/*****************************************************************************

        MenuMain.cpp
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

#include "mfx/uitk/pg/MenuMain.h"
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



MenuMain::MenuMain (PageSwitcher &page_switcher)
:	_page_switcher (page_switcher)
,	_view_ptr (0)
,	_page_ptr (0)
,	_page_size ()
,	_edit_prog_sptr (  new NText (Entry_PROG  ))
,	_edit_bank_sptr (  new NText (Entry_BANKS ))
,	_edit_layout_sptr (new NText (Entry_LAYOUT))
,	_edit_master_sptr (new NText (Entry_MASTER))
,	_reboot_sptr (     new NText (Entry_REBOOT))
{
	_edit_prog_sptr  ->set_justification (0.5f, 0, false);
	_edit_bank_sptr  ->set_justification (0.5f, 0, false);
	_edit_layout_sptr->set_justification (0.5f, 0, false);
	_edit_master_sptr->set_justification (0.5f, 0, false);
	_reboot_sptr     ->set_justification (0.5f, 0, false);

	_edit_prog_sptr  ->set_text ("Edit Program");
	_edit_bank_sptr  ->set_text ("Banks");
	_edit_layout_sptr->set_text ("Layout");
	_edit_master_sptr->set_text ("Master volume");
	_reboot_sptr     ->set_text ("Reboot");
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	MenuMain::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, const ui::Font &fnt_s, const ui::Font &fnt_m, const ui::Font &fnt_l)
{
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;

	_edit_prog_sptr  ->set_font (fnt_m);
	_edit_bank_sptr  ->set_font (fnt_m);
	_edit_layout_sptr->set_font (fnt_m);
	_edit_master_sptr->set_font (fnt_m);
	_reboot_sptr     ->set_font (fnt_m);

	const int      x_mid =  _page_size [0]      >> 1;
	const int      w_sel = (_page_size [0] * 3) >> 2;
	const int      h_m   = fnt_m.get_char_h ();

	_edit_prog_sptr  ->set_frame (Vec2d (w_sel, 0), Vec2d ());
	_edit_bank_sptr  ->set_frame (Vec2d (w_sel, 0), Vec2d ());
	_edit_layout_sptr->set_frame (Vec2d (w_sel, 0), Vec2d ());
	_edit_master_sptr->set_frame (Vec2d (w_sel, 0), Vec2d ());
	_reboot_sptr     ->set_frame (Vec2d (w_sel, 0), Vec2d ());

	_edit_prog_sptr  ->set_coord (Vec2d (x_mid, h_m * 0));
	_edit_bank_sptr  ->set_coord (Vec2d (x_mid, h_m * 1));
	_edit_layout_sptr->set_coord (Vec2d (x_mid, h_m * 2));
	_edit_master_sptr->set_coord (Vec2d (x_mid, h_m * 3));
	_reboot_sptr     ->set_coord (Vec2d (x_mid, h_m * 4));

	_page_ptr->push_back (_edit_prog_sptr  );
	_page_ptr->push_back (_edit_bank_sptr  );
	_page_ptr->push_back (_edit_layout_sptr);
	_page_ptr->push_back (_edit_master_sptr);
	_page_ptr->push_back (_reboot_sptr     );

	PageMgrInterface::NavLocList  nav_list (Entry_NBR_ELT);
	for (int pos = 0; pos < int (nav_list.size ()); ++pos)
	{
		NavLoc &       loc = nav_list [pos];
		loc._node_id = pos;
	}
	page.set_nav_layout (nav_list);
	page.jump_to (nav_list [0]._node_id);
}



void	MenuMain::do_disconnect ()
{
	// Nothing
}



MsgHandlerInterface::EvtProp	MenuMain::do_handle_evt (const NodeEvt &evt)
{
	EvtProp        ret_val = EvtProp_PASS;

	if (evt.is_button_ex ())
	{
		const Button   but = evt.get_button_ex ();
		switch (but)
		{
		case Button_S:
			ret_val = EvtProp_CATCH;
			switch (evt.get_target ())
			{
			case Entry_PROG:   _page_switcher.switch_to (pg::PageType_EDIT_PROG);  break;
/*			case Entry_BANKS:  _page_switcher.switch_to (pg::PageType_EDIT_BANKS); break;
			case Entry_LAYOUT: _page_switcher.switch_to (pg::PageType_PB_LAYOUT);  break;
			case Entry_MASTER: _page_switcher.switch_to (pg::PageType_MASTER_VOL); break;
			case Entry_REBOOT: _page_switcher.switch_to (pg::PageType_REBOOT);     break;
*/			default:
				ret_val = EvtProp_PASS;
				break;
			}
			break;
		case Button_E:
			_page_switcher.switch_to (pg::PageType_CUR_PROG);
			ret_val = EvtProp_CATCH;
			break;
		default:
			// Nothing
			break;
		}
	}

	return ret_val;
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
