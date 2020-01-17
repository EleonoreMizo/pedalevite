/*****************************************************************************

        PresetMenu.cpp
        Author: Laurent de Soras, 2017

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
#include "mfx/uitk/pg/PresetMenu.h"
#include "mfx/uitk/NodeEvt.h"
#include "mfx/uitk/PageMgrInterface.h"
#include "mfx/uitk/PageSwitcher.h"
#include "mfx/ui/Font.h"
#include "mfx/LocEdit.h"
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



PresetMenu::PresetMenu (PageSwitcher &page_switcher, LocEdit &loc_edit)
:	_page_switcher (page_switcher)
,	_loc_edit (loc_edit)
,	_model_ptr (nullptr)
,	_view_ptr (nullptr)
,	_page_ptr (nullptr)
,	_page_size ()
,	_fnt_ptr (nullptr)
,	_menu_sptr (std::make_shared <NWindow> (Entry_WINDOW  ))
,	_load_sptr (std::make_shared <NText  > (Entry_LOAD    ))
,	_brow_sptr (std::make_shared <NText  > (Entry_BROWSE  ))
,	_stor_sptr (std::make_shared <NText  > (Entry_STORE   ))
,	_swap_sptr (std::make_shared <NText  > (Entry_SWAP    ))
,	_renm_sptr (std::make_shared <NText  > (Entry_RENAME  ))
,	_mrph_sptr (std::make_shared <NText  > (Entry_MORPH   ))
,	_dele_sptr (std::make_shared <NText  > (Entry_DELETE  ))
,	_orga_sptr (std::make_shared <NText  > (Entry_ORGANIZE))
,	_lp_param ()
,	_node_id_save (Entry_LOAD)
{
	_load_sptr->set_text ("Load\xE2\x80\xA6");
	_brow_sptr->set_text ("Browse\xE2\x80\xA6");
	_stor_sptr->set_text ("Store\xE2\x80\xA6");
	_swap_sptr->set_text ("Swap\xE2\x80\xA6");
	_renm_sptr->set_text ("Rename\xE2\x80\xA6");
	_mrph_sptr->set_text ("Morph with\xE2\x80\xA6");
	_dele_sptr->set_text ("Delete\xE2\x80\xA6");
	_orga_sptr->set_text ("Organize\xE2\x80\xA6");
	
	_menu_sptr->push_back (_load_sptr);
	_menu_sptr->push_back (_brow_sptr);
	_menu_sptr->push_back (_stor_sptr);
	_menu_sptr->push_back (_swap_sptr);
	_menu_sptr->push_back (_renm_sptr);
	_menu_sptr->push_back (_mrph_sptr);
	_menu_sptr->push_back (_dele_sptr);
	_menu_sptr->push_back (_orga_sptr);
	_menu_sptr->set_autoscroll (true);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	PresetMenu::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt)
{
	fstb::unused (usr_ptr);
	assert (_loc_edit._slot_id >= 0);

	_model_ptr = &model;
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;
	_fnt_ptr   = &fnt._m;

	_page_ptr->clear_all_nodes ();

	_menu_sptr->set_size (_page_size, Vec2d ());
	_menu_sptr->set_disp_pos (Vec2d ());

	const int      scr_w = _page_size [0];
	const int      h_m   = _fnt_ptr->get_char_h ();

	_load_sptr->set_font (*_fnt_ptr);
	_brow_sptr->set_font (*_fnt_ptr);
	_stor_sptr->set_font (*_fnt_ptr);
	_swap_sptr->set_font (*_fnt_ptr);
	_renm_sptr->set_font (*_fnt_ptr);
	_mrph_sptr->set_font (*_fnt_ptr);
	_dele_sptr->set_font (*_fnt_ptr);
	_orga_sptr->set_font (*_fnt_ptr);

	_load_sptr->set_coord (Vec2d (0, h_m * 0));
	_brow_sptr->set_coord (Vec2d (0, h_m * 1));
	_stor_sptr->set_coord (Vec2d (0, h_m * 2));
	_swap_sptr->set_coord (Vec2d (0, h_m * 3));
	_renm_sptr->set_coord (Vec2d (0, h_m * 4));
	_mrph_sptr->set_coord (Vec2d (0, h_m * 5));
	_dele_sptr->set_coord (Vec2d (0, h_m * 6));
	_orga_sptr->set_coord (Vec2d (0, h_m * 7));

	_load_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_brow_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_stor_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_swap_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_renm_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_mrph_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_dele_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_orga_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());

	_page_ptr->push_back (_menu_sptr);

	PageMgrInterface::NavLocList  nav_list;
	NavLoc         nav;
	PageMgrInterface::add_nav (nav_list, Entry_LOAD    );
	PageMgrInterface::add_nav (nav_list, Entry_BROWSE  );
	PageMgrInterface::add_nav (nav_list, Entry_STORE   );
	PageMgrInterface::add_nav (nav_list, Entry_SWAP    );
	PageMgrInterface::add_nav (nav_list, Entry_RENAME  );
	PageMgrInterface::add_nav (nav_list, Entry_MORPH   );
	PageMgrInterface::add_nav (nav_list, Entry_DELETE  );
	PageMgrInterface::add_nav (nav_list, Entry_ORGANIZE);
	_page_ptr->set_nav_layout (nav_list);

	_menu_sptr->invalidate_all ();
	_page_ptr->jump_to (_node_id_save);
}



void	PresetMenu::do_disconnect ()
{
	// Nothing
}



MsgHandlerInterface::EvtProp	PresetMenu::do_handle_evt (const NodeEvt &evt)
{
	EvtProp        ret_val = EvtProp_PASS;

	const int      node_id = evt.get_target ();

	if (evt.is_button_ex ())
	{
		const Button   but = evt.get_button_ex ();
		switch (but)
		{
		case Button_S:
			_node_id_save = node_id;
			ret_val       = EvtProp_CATCH;
			switch (node_id)
			{
			case Entry_LOAD:
				_lp_param._action = PresetList::Action_LOAD;
				_page_switcher.switch_to (pg::PageType_PRESET_LIST, &_lp_param);
				break;
			case Entry_BROWSE:
				_lp_param._action = PresetList::Action_BROWSE;
				_page_switcher.switch_to (pg::PageType_PRESET_LIST, &_lp_param);
				break;
			case Entry_STORE:
				_lp_param._action = PresetList::Action_STORE;
				_page_switcher.switch_to (pg::PageType_PRESET_LIST, &_lp_param);
				break;
			case Entry_SWAP:
				_lp_param._action = PresetList::Action_SWAP;
				_page_switcher.switch_to (pg::PageType_PRESET_LIST, &_lp_param);
				break;
			case Entry_RENAME:
				_lp_param._action = PresetList::Action_RENAME;
				_page_switcher.switch_to (pg::PageType_PRESET_LIST, &_lp_param);
				break;
			case Entry_MORPH:
				_lp_param._action = PresetList::Action_MORPH;
				_page_switcher.switch_to (pg::PageType_PRESET_LIST, &_lp_param);
				break;
			case Entry_DELETE:
				_lp_param._action = PresetList::Action_DELETE;
				_page_switcher.switch_to (pg::PageType_PRESET_LIST, &_lp_param);
				break;
			case Entry_ORGANIZE:
				/*** To do ***/
				_page_switcher.call_page (PageType_NOT_YET, nullptr, node_id);
				break;
			default:
				ret_val = EvtProp_PASS;
				break;
			}
			break;
		case Button_E:
			_page_switcher.switch_to (pg::PageType_SLOT_MENU, nullptr);
			ret_val = EvtProp_CATCH;
			break;
		default:
			// Nothing
			break;
		}
	}

	return ret_val;
}



void	PresetMenu::do_activate_preset (int index)
{
	fstb::unused (index);

	_page_switcher.switch_to (pg::PageType_PROG_EDIT, nullptr);
}



void	PresetMenu::do_remove_slot (int slot_id)
{
	if (slot_id == _loc_edit._slot_id)
	{
		_page_switcher.switch_to (pg::PageType_PROG_EDIT, nullptr);
	}
}



void	PresetMenu::do_set_plugin (int slot_id, const PluginInitData &pi_data)
{
	fstb::unused (pi_data);

	if (slot_id == _loc_edit._slot_id)
	{
		_page_switcher.switch_to (pg::PageType_PROG_EDIT, nullptr);
	}
}



void	PresetMenu::do_remove_plugin (int slot_id)
{
	if (slot_id == _loc_edit._slot_id)
	{
		_page_switcher.switch_to (pg::PageType_PROG_EDIT, nullptr);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
