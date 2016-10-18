/*****************************************************************************

        BankMenu.cpp
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
#include "mfx/uitk/pg/BankMenu.h"
#include "mfx/uitk/NodeEvt.h"
#include "mfx/uitk/PageMgrInterface.h"
#include "mfx/uitk/PageSwitcher.h"
#include "mfx/ui/Font.h"
#include "mfx/Cst.h"
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



BankMenu::BankMenu (PageSwitcher &page_switcher)
:	_page_switcher (page_switcher)
,	_model_ptr (0)
,	_view_ptr (0)
,	_page_ptr (0)
,	_page_size ()
,	_fnt_ptr (0)
,	_menu_sptr (new NWindow (Entry_WINDOW))
,	_import_sptr (new NText (Entry_IMPORT))
,	_export_sptr (new NText (Entry_EXPORT))
,	_bank_list ()
{
	_import_sptr->set_text ("Import bank");
	_export_sptr->set_text ("Export bank");
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	BankMenu::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const ui::Font &fnt_s, const ui::Font &fnt_m, const ui::Font &fnt_l)
{
	_model_ptr = &model;
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;
	_fnt_ptr   = &fnt_m;

	_import_sptr->set_font (*_fnt_ptr);
	_export_sptr->set_font (*_fnt_ptr);

	const int      h_m   = _fnt_ptr->get_char_h ();
	const int      scr_w = _page_size [0];

	_menu_sptr->set_size (_page_size, Vec2d ());
	_menu_sptr->set_disp_pos (Vec2d ());

	_import_sptr->set_coord (Vec2d (0, 0 * h_m));
	_export_sptr->set_coord (Vec2d (0, 1 * h_m));

	_import_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_export_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());

	_page_ptr->push_back (_menu_sptr);

	update_display ();

	const int      node_id = _view_ptr->get_bank_index ();
	_page_ptr->jump_to (node_id);
}



void	BankMenu::do_disconnect ()
{
	// Nothing
}



MsgHandlerInterface::EvtProp	BankMenu::do_handle_evt (const NodeEvt &evt)
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
			if (node_id == Entry_IMPORT)
			{
				_page_switcher.call_page (PageType_NOT_YET, 0, node_id);
				/*** To do ***/
			}
			else if (node_id == Entry_EXPORT)
			{
				_page_switcher.call_page (PageType_NOT_YET, 0, node_id);
				/*** To do ***/
			}
			else if (node_id >= 0 && node_id < Cst::_nbr_banks)
			{
				_model_ptr->select_bank (node_id);
				_model_ptr->activate_preset (0);
				_page_switcher.call_page (PageType_NOT_YET, 0, node_id);
				/*** To do ***/
			}
			break;
		case Button_E:
			_page_switcher.switch_to (pg::PageType_CUR_PROG, 0);
			ret_val = EvtProp_CATCH;
			break;
		default:
			// Nothing
			break;
		}
	}

	return ret_val;
}



void	BankMenu::do_set_bank (int index, const doc::Bank &bank)
{
	update_display ();
}



void	BankMenu::do_select_bank (int index)
{
	update_display ();
}



void	BankMenu::do_set_bank_name (std::string name)
{
	update_display ();
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	BankMenu::update_display ()
{
	assert (_fnt_ptr != 0);

	const int      h_m   = _fnt_ptr->get_char_h ();
	const int      scr_w = _page_size [0];

	_bank_list.clear ();
	_menu_sptr->clear_all_nodes ();
	PageMgrInterface::NavLocList  nav_list;

	_menu_sptr->push_back (_import_sptr);
	PageMgrInterface::add_nav (nav_list, Entry_IMPORT);

	_menu_sptr->push_back (_export_sptr);
	PageMgrInterface::add_nav (nav_list, Entry_EXPORT);

	const doc::Setup &   setup = _view_ptr->use_setup ();
	const int      bank_index_cur = _view_ptr->get_bank_index ();
	for (int bank_index = 0; bank_index < Cst::_nbr_banks; ++bank_index)
	{
		const int      node_id = bank_index;
		TxtSPtr        name_sptr (new NText (node_id));

		char           txt_0 [255+1];
		const doc::Bank & bank = setup._bank_arr [bank_index];

		fstb::snprintf4all (
			txt_0, sizeof (txt_0),
			"%s %02d %s",
			(bank_index == bank_index_cur) ? "\xE2\x9C\x93" : " ",   // U+2713 CHECK MARK
			bank_index,
			bank._name.c_str ()
		);

		name_sptr->set_font (*_fnt_ptr);
		name_sptr->set_text (txt_0);
		name_sptr->set_coord (Vec2d (0, (bank_index + 2) * h_m));
		name_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());

		_bank_list.push_back (name_sptr);
		_menu_sptr->push_back (name_sptr);
		PageMgrInterface::add_nav (nav_list, node_id);
	}

	_page_ptr->set_nav_layout (nav_list);

	_menu_sptr->invalidate_all ();
}



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
