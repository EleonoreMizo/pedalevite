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

#include "fstb/def.h"
#include "fstb/fnc.h"
#include "mfx/uitk/pg/BankMenu.h"
#include "mfx/uitk/pg/Tools.h"
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



BankMenu::BankMenu (PageSwitcher &page_switcher, PedalEditContext &pedal_ctx)
:	_page_switcher (page_switcher)
,	_pedal_ctx (pedal_ctx)
,	_model_ptr (nullptr)
,	_view_ptr (nullptr)
,	_page_ptr (nullptr)
,	_page_size ()
,	_fnt_ptr (nullptr)
,	_menu_sptr (std::make_shared <NWindow> (Entry_WINDOW  ))
,	_orga_sptr (std::make_shared <NText> (  Entry_ORGANIZE))
,	_import_sptr (std::make_shared <NText> (Entry_IMPORT  ))
,	_export_sptr (std::make_shared <NText> (Entry_EXPORT  ))
,	_layout_sptr (std::make_shared <NText> (Entry_LAYOUT  ))
,	_bank_list ()
{
	_orga_sptr  ->set_text ("Organize\xE2\x80\xA6");
	_import_sptr->set_text ("Import bank\xE2\x80\xA6");
	_export_sptr->set_text ("Export bank\xE2\x80\xA6");
	_layout_sptr->set_text ("Pedal layout\xE2\x80\xA6");
	_orga_sptr  ->set_justification (0.5f, 0.0f, false);
	_import_sptr->set_justification (0.5f, 0.0f, false);
	_export_sptr->set_justification (0.5f, 0.0f, false);
	_layout_sptr->set_justification (0.5f, 0.0f, false);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	BankMenu::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt)
{
	fstb::unused (usr_ptr);

	_model_ptr = &model;
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;
	_fnt_ptr   = &fnt._m;

	_orga_sptr  ->set_font (*_fnt_ptr);
	_import_sptr->set_font (*_fnt_ptr);
	_export_sptr->set_font (*_fnt_ptr);
	_layout_sptr->set_font (*_fnt_ptr);

	const int      h_m   = _fnt_ptr->get_char_h ();
	const int      scr_w = _page_size [0];
	const int      x_mid = scr_w >> 1;

	_menu_sptr->set_size (_page_size, Vec2d ());
	_menu_sptr->set_disp_pos (Vec2d ());
	const int      win_h = _menu_sptr->get_bounding_box ().get_size () [1];
	_page_ptr->set_page_step (win_h / h_m);

	_orga_sptr  ->set_coord (Vec2d (x_mid, 0 * h_m));
	_import_sptr->set_coord (Vec2d (x_mid, 1 * h_m));
	_export_sptr->set_coord (Vec2d (x_mid, 2 * h_m));
	_layout_sptr->set_coord (Vec2d (x_mid, 3 * h_m));

	_orga_sptr  ->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_import_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_export_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_layout_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());

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
			if (node_id == Entry_ORGANIZE)
			{
				_page_switcher.switch_to (PageType_BANK_ORGA, nullptr);
			}
			else if (node_id == Entry_IMPORT)
			{
				_page_switcher.call_page (PageType_NOT_YET, nullptr, node_id);
				/*** To do ***/
			}
			else if (node_id == Entry_EXPORT)
			{
				_page_switcher.call_page (PageType_NOT_YET, nullptr, node_id);
				/*** To do ***/
			}
			else if (node_id == Entry_LAYOUT)
			{
				_pedal_ctx._type     = PedalEditContext::Type_BANK;
				_pedal_ctx._ret_page = PageType_BANK_MENU;
				_page_switcher.switch_to (PageType_PEDALBOARD_CONFIG, nullptr);
			}
			else if (node_id >= 0 && node_id < Cst::_nbr_banks)
			{
				_model_ptr->select_bank (node_id);
				_model_ptr->activate_prog (0);
//				_page_switcher.call_page (PageType_NOT_YET, 0, node_id);
				/*** To do ***/
			}
			break;
		case Button_E:
			_page_switcher.switch_to (pg::PageType_PROG_CUR, nullptr);
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
	fstb::unused (index, bank);

	update_display ();
}



void	BankMenu::do_select_bank (int index)
{
	fstb::unused (index);

	update_display ();
}



void	BankMenu::do_set_bank_name (std::string name)
{
	fstb::unused (name);

	update_display ();
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	BankMenu::update_display ()
{
	assert (_fnt_ptr != nullptr);

	const int      h_m   = _fnt_ptr->get_char_h ();
	const int      scr_w = _page_size [0];

	_bank_list.clear ();
	_menu_sptr->clear_all_nodes ();
	PageMgrInterface::NavLocList  nav_list;

	_menu_sptr->push_back (_orga_sptr);
	PageMgrInterface::add_nav (nav_list, Entry_ORGANIZE);

	_menu_sptr->push_back (_import_sptr);
	PageMgrInterface::add_nav (nav_list, Entry_IMPORT);

	_menu_sptr->push_back (_export_sptr);
	PageMgrInterface::add_nav (nav_list, Entry_EXPORT);

	_menu_sptr->push_back (_layout_sptr);
	PageMgrInterface::add_nav (nav_list, Entry_LAYOUT);

	Tools::create_bank_list (
		_bank_list, *_menu_sptr, nav_list, *_view_ptr, *_fnt_ptr,
		4 * h_m, scr_w, true
	);

	_page_ptr->set_nav_layout (nav_list);

	_menu_sptr->invalidate_all ();
}



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
