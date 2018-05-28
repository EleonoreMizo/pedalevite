/*****************************************************************************

        BankOrga.cpp
        Author: Laurent de Soras, 2018

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

#include "mfx/uitk/pg/BankOrga.h"
#include "mfx/uitk/NodeEvt.h"
#include "mfx/uitk/PageMgrInterface.h"
#include "mfx/uitk/PageSwitcher.h"
#include "mfx/ui/Font.h"

#include <cassert>



namespace mfx
{
namespace uitk
{
namespace pg
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



BankOrga::BankOrga (PageSwitcher &page_switcher, PedalEditContext &pedal_ctx)
:	_page_switcher (page_switcher)
,	_pedal_ctx (pedal_ctx)
,	_page_ptr (0)
,	_page_size ()
,	_fnt_ptr (0)
,	_mv_bank_sptr (new NText (Entry_MV_BANK))
,	_mv_prog_sptr (new NText (Entry_MV_PROG))
{
	_mv_bank_sptr->set_justification (0.5f, 0, false);
	_mv_prog_sptr->set_justification (0.5f, 0, false);

	_mv_bank_sptr->set_text ("Move banks\xE2\x80\xA6");
	_mv_prog_sptr->set_text ("Move programs\xE2\x80\xA6");
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	BankOrga::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt)
{
	_page_ptr  = &page;
	_page_size = page_size;
	_fnt_ptr   = &fnt._m;

	_mv_bank_sptr->set_font (fnt._m);
	_mv_prog_sptr->set_font (fnt._m);

	const int      x_mid =  _page_size [0]      >> 1;
	const int      w_sel = (_page_size [0] * 3) >> 2;
	const int      h_m   = fnt._m.get_char_h ();

	_mv_bank_sptr->set_frame (Vec2d (w_sel, 0), Vec2d ());
	_mv_prog_sptr->set_frame (Vec2d (w_sel, 0), Vec2d ());

	_mv_bank_sptr->set_coord (Vec2d (x_mid, h_m * 0));
	_mv_prog_sptr->set_coord (Vec2d (x_mid, h_m * 1));

	_page_ptr->push_back (_mv_bank_sptr);
	_page_ptr->push_back (_mv_prog_sptr);

	PageMgrInterface::NavLocList  nav_list;
	page.add_nav (nav_list, Entry_MV_BANK);
	page.add_nav (nav_list, Entry_MV_PROG);
	page.set_nav_layout (nav_list);
	page.jump_to (nav_list [0]._node_id);
}



void	BankOrga::do_disconnect ()
{
	// Nothing
}



MsgHandlerInterface::EvtProp	BankOrga::do_handle_evt (const NodeEvt &evt)
{
	EvtProp        ret_val = EvtProp_PASS;

	const int      node_id = evt.get_target ();

	if (evt.is_button_ex ())
	{
		const Button   but = evt.get_button_ex ();
		switch (but)
		{
		case Button_S:
			ret_val = EvtProp_CATCH;
			switch (evt.get_target ())
			{
			case Entry_MV_BANK:
				_page_switcher.call_page (PageType_BANK_MOVE, 0, node_id);
				break;
			case Entry_MV_PROG:
				_page_switcher.call_page (PageType_PROG_MOVE, 0, node_id);
				break;
			default:
				ret_val = EvtProp_PASS;
				break;
			}
			break;
		case Button_E:
			_page_switcher.switch_to (pg::PageType_BANK_MENU, 0);
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
