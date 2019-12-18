/*****************************************************************************

        EditProg.cpp
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
#include "mfx/pi/param/Tools.h"
#include "mfx/piapi/PluginDescInterface.h"
#include "mfx/uitk/pg/EditProg.h"
#include "mfx/uitk/pg/Tools.h"
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



EditProg::EditProg (PageSwitcher &page_switcher, LocEdit &loc_edit, const std::vector <std::string> &fx_list, const std::vector <std::string> &ms_list)
:	_fx_list (fx_list)
,	_ms_list (ms_list)
,	_page_switcher (page_switcher)
,	_loc_edit (loc_edit)
,	_model_ptr (0)
,	_view_ptr (0)
,	_page_ptr (0)
,	_page_size ()
,	_fnt_ptr (0)
,	_menu_sptr (new NWindow (Entry_WINDOW))
,	_fx_list_sptr (new NText (Entry_FX_LIST))
,	_ms_list_sptr (new NText (Entry_MS_LIST))
,	_prog_name_sptr (new NText (Entry_PROG_NAME))
,	_controllers_sptr (new NText (Entry_CONTROLLERS))
,	_save_sptr (new NText (Entry_SAVE))
,	_slot_list ()
,	_state (State_NORMAL)
,	_save_bank_index (-1)
,	_save_preset_index (-1)
,	_name_param ()
,	_slot_id_list ()
{
	_prog_name_sptr  ->set_justification (0.5f, 0, false);
	_controllers_sptr->set_justification (0.5f, 0, false);
	_save_sptr       ->set_justification (0.5f, 0, false);
	_fx_list_sptr    ->set_justification (0.5f, 1, false);
	_ms_list_sptr    ->set_justification (0.5f, 1, false);
	_controllers_sptr->set_text ("Controllers\xE2\x80\xA6");
	_save_sptr       ->set_text ("Save to\xE2\x80\xA6");
	_fx_list_sptr    ->set_text ("-----------------");
	_ms_list_sptr    ->set_text ("-----------------");
	_prog_name_sptr  ->set_bold (true, true );
	_fx_list_sptr    ->set_bold (true, false);
	_ms_list_sptr    ->set_bold (true, false);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	EditProg::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt)
{
	fstb::unused (usr_ptr);

	_model_ptr = &model;
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;
	_fnt_ptr   = &fnt._m;

	if (_view_ptr->use_setup ()._save_mode != doc::Setup::SaveMode_MANUAL)
	{
		_model_ptr->set_edit_mode (true);
	}

	if (_state == State_EDIT_NAME)
	{
		if (   _name_param._ok_flag
			 && _view_ptr->get_bank_index ()   == _save_bank_index
		    && _view_ptr->get_preset_index () == _save_preset_index)
		{
			_model_ptr->set_preset_name (_name_param._text);
		}
	}
	_state = State_NORMAL;

	_prog_name_sptr  ->set_font (*_fnt_ptr);
	_controllers_sptr->set_font (*_fnt_ptr);
	_save_sptr       ->set_font (*_fnt_ptr);
	_fx_list_sptr    ->set_font (*_fnt_ptr);
	_ms_list_sptr    ->set_font (*_fnt_ptr);

	const int      scr_w = _page_size [0];
	const int      x_mid =  scr_w >> 1;
	const int      h_m   = _fnt_ptr->get_char_h ();

	_menu_sptr->set_size (_page_size, Vec2d ());
	_menu_sptr->set_disp_pos (Vec2d ());

	_prog_name_sptr  ->set_coord (Vec2d (x_mid, 0 * h_m));
	_controllers_sptr->set_coord (Vec2d (x_mid, 1 * h_m));
	_save_sptr       ->set_coord (Vec2d (x_mid, 2 * h_m));
	_fx_list_sptr    ->set_coord (Vec2d (x_mid, 4 * h_m));

	_prog_name_sptr  ->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_controllers_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_save_sptr       ->set_frame (Vec2d (scr_w, 0), Vec2d ());

	_page_ptr->push_back (_menu_sptr);

	set_preset_info ();
	_page_ptr->jump_to (conv_loc_edit_to_node_id ());
}



void	EditProg::do_disconnect ()
{
	if (_model_ptr != 0)
	{
		_model_ptr->reset_all_overridden_param_ctrl ();
	}
}



MsgHandlerInterface::EvtProp	EditProg::do_handle_evt (const NodeEvt &evt)
{
	EvtProp        ret_val = EvtProp_PASS;

	const int      node_id = evt.get_target ();

	if (evt.is_cursor ())
	{
		if (   evt.get_cursor () == NodeEvt::Curs_ENTER
		    && node_id >= 0 && node_id < int (_slot_list.size ()))
		{
			update_loc_edit (node_id);
			update_rotenc_mapping ();
		}
	}

	else if (evt.is_button_ex ())
	{
		const Button   but = evt.get_button_ex ();
		switch (but)
		{
		case Button_S:
			ret_val = EvtProp_CATCH;
			if (node_id == Entry_PROG_NAME)
			{
				const doc::Preset &  preset = _view_ptr->use_preset_cur ();
				_name_param._title = "Program name:";
				if (preset._name == Cst::_empty_preset_name)
				{
					_name_param._text.clear ();
				}
				else
				{
					_name_param._text  = preset._name;
				}
				_state             = State_EDIT_NAME;
				_save_bank_index   = _view_ptr->get_bank_index ();
				_save_preset_index = _view_ptr->get_preset_index ();
				_page_switcher.call_page (PageType_EDIT_TEXT, &_name_param, node_id);
			}
			else if (node_id == Entry_SAVE)
			{
				_page_switcher.switch_to (PageType_SAVE_PROG, 0);
			}
			else if (node_id == Entry_CONTROLLERS)
			{
				_page_switcher.switch_to (PageType_CTRL_PROG, 0);
			}
			else if (node_id >= 0 && node_id < int (_slot_list.size ()))
			{
				const doc::Preset &  preset  = _view_ptr->use_preset_cur ();
				const int            slot_id = conv_node_id_to_slot_id (node_id);
				if (slot_id >= 0 && ! preset.is_slot_empty (slot_id))
				{
					// Full slot
					update_loc_edit (node_id);
					_page_switcher.switch_to (PageType_PARAM_LIST, 0);
				}
				else
				{
					// Empty slot
					_page_switcher.switch_to (PageType_MENU_SLOT, 0);
				}
			}
			else
			{
				ret_val = EvtProp_PASS;
			}
			break;
		case Button_E:
			_page_switcher.switch_to (pg::PageType_CUR_PROG, 0);
			ret_val = EvtProp_CATCH;
			break;
		case Button_L:
			ret_val = change_effect (node_id, -1);
			break;
		case Button_R:
			ret_val = change_effect (node_id, +1);
			break;
		default:
			// Nothing
			break;
		}
	}

	return ret_val;
}



void	EditProg::do_activate_preset (int index)
{
	fstb::unused (index);

	set_preset_info ();
}



void	EditProg::do_set_preset_name (std::string name)
{
	fstb::unused (name);

	set_preset_info ();
}



void	EditProg::do_add_slot (int slot_id)
{
	fstb::unused (slot_id);

	set_preset_info ();
}



void	EditProg::do_remove_slot (int slot_id)
{
	fstb::unused (slot_id);

	set_preset_info ();
}



void	EditProg::do_insert_slot_in_chain (int index, int slot_id)
{
	fstb::unused (index, slot_id);

	set_preset_info ();
}



void	EditProg::do_erase_slot_from_chain (int index)
{
	fstb::unused (index);

	set_preset_info ();
}



void	EditProg::do_set_plugin (int slot_id, const PluginInitData &pi_data)
{
	fstb::unused (slot_id, pi_data);

	set_preset_info ();
}



void	EditProg::do_remove_plugin (int slot_id)
{
	fstb::unused (slot_id);

	set_preset_info ();
}



void	EditProg::do_set_param_ctrl (int slot_id, PiType type, int index, const doc::CtrlLinkSet &cls)
{
	fstb::unused (slot_id, type, index, cls);

	set_preset_info ();
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	EditProg::set_preset_info ()
{
	assert (_fnt_ptr != 0);

	const doc::Preset &  preset = _view_ptr->use_preset_cur ();
	_slot_id_list = preset.build_ordered_node_list (true);

	update_rotenc_mapping ();

	_prog_name_sptr->set_text (preset._name);

	const int      nbr_slots = int (_slot_id_list.size ());
	PageMgrInterface::NavLocList  nav_list (nbr_slots + 5);
	_slot_list.resize (nbr_slots + 2);

	_menu_sptr->clear_all_nodes ();
	_menu_sptr->push_back (_prog_name_sptr);
	_menu_sptr->push_back (_controllers_sptr);
	_menu_sptr->push_back (_save_sptr);
	_menu_sptr->push_back (_fx_list_sptr);
	_menu_sptr->push_back (_ms_list_sptr);
	nav_list [0]._node_id = Entry_PROG_NAME;
	nav_list [1]._node_id = Entry_CONTROLLERS;
	nav_list [2]._node_id = Entry_SAVE;

	const std::vector <Tools::NodeEntry>   entry_list =
		Tools::extract_slot_list (preset, *_model_ptr);
	assert (nbr_slots == int (entry_list.size ()));
	const int      chain_size = int (preset._routing._chain.size ());

	const int      scr_w = _page_size [0];
	const int      x_mid =  scr_w >> 1;
	const int      h_m   = _fnt_ptr->get_char_h ();
	_ms_list_sptr->set_coord (Vec2d (x_mid, (chain_size + 6) * h_m));

	for (int slot_index = 0; slot_index < nbr_slots; ++slot_index)
	{
		const Tools::NodeEntry &   entry = entry_list [slot_index];
		assert (entry._slot_id == _slot_id_list [slot_index]);

		const int      slot_id    = entry._slot_id;
		std::string    multilabel = "<Empty>";
		bool           ctrl_flag  = false;

		const auto     it_slot    = preset._slot_map.find (slot_id);
		assert (it_slot != preset._slot_map.end ());
		if (! entry._type.empty ())
		{
			const doc::Slot & slot = *(it_slot->second);
			multilabel = entry._name_multilabel;
			ctrl_flag  = slot.has_ctrl ();

			if (entry._instance_nbr >= 0)
			{
				char        txt_0 [127+1];
				fstb::snprintf4all (
					txt_0, sizeof (txt_0), " %d", entry._instance_nbr + 1
				);
				multilabel = pi::param::Tools::join_strings_multi (
					multilabel.c_str (), '\n', "", txt_0
				);
			}
		}

		const int      skip     = (slot_index >= chain_size) ? 1 : 0;
		const int      pos_list = slot_index + skip;
		set_slot (nav_list, pos_list, multilabel, ctrl_flag, chain_size);
	}

	set_slot (nav_list, chain_size   , "<End>", false, chain_size);
	set_slot (nav_list, nbr_slots + 1, "<End>", false, chain_size);

	_page_ptr->set_nav_layout (nav_list);

	_menu_sptr->invalidate_all ();
}



void	EditProg::set_slot (PageMgrInterface::NavLocList &nav_list, int pos_list, std::string multilabel, bool bold_flag, int chain_size)
{
	const int      h_m      = _fnt_ptr->get_char_h ();
	const int      scr_w    = _page_size [0];
	const int      pos_nav  = pos_list + 3; // In the nav_list
	const int      skip     = (pos_list >= chain_size + 1) ? 1 : 0;
	const int      pos_menu = pos_list + 4 + skip;

	TxtSPtr        entry_sptr (new NText (pos_list));
	entry_sptr->set_coord (Vec2d (0, h_m * pos_menu));
	entry_sptr->set_font (*_fnt_ptr);
	entry_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
	entry_sptr->set_bold (bold_flag, true);
	std::string    txt = pi::param::Tools::print_name_bestfit (
		scr_w, multilabel.c_str (),
		*entry_sptr, &NText::get_char_width
	);
	entry_sptr->set_text (txt);
	_slot_list [pos_list] = entry_sptr;
	nav_list [pos_nav]._node_id = pos_list;

	_menu_sptr->push_back (entry_sptr);
}



MsgHandlerInterface::EvtProp	EditProg::change_effect (int node_id, int dir)
{
	assert (node_id >= 0);
	assert (dir != 0);

	_model_ptr->reset_all_overridden_param_ctrl ();

	const doc::Preset &  preset = _view_ptr->use_preset_cur ();
	const int      chain_size   = int (preset._routing._chain.size ());
	_loc_edit._chain_flag = (node_id <= chain_size);
	const int      slot_id      = conv_node_id_to_slot_id (node_id);
	Tools::change_plugin (
		*_model_ptr,
		*_view_ptr,
		slot_id,
		dir,
		(_loc_edit._chain_flag) ? _fx_list : _ms_list,
		_loc_edit._chain_flag
	);

	update_rotenc_mapping ();

	return EvtProp_CATCH;
}



void	EditProg::update_loc_edit (int node_id)
{
	_loc_edit._slot_id = conv_node_id_to_slot_id (
		node_id, _loc_edit._chain_flag
	);
}



void	EditProg::update_rotenc_mapping ()
{
	assert (_model_ptr != 0);
	assert (_view_ptr  != 0);

	if (_loc_edit._slot_id < 0)
	{
		_model_ptr->reset_all_overridden_param_ctrl ();
	}
	else
	{
		Tools::assign_default_rotenc_mapping (
			*_model_ptr, *_view_ptr, _loc_edit._slot_id, 0
		);
	}
}



int	EditProg::conv_node_id_to_slot_id (int node_id) const
{
	bool           chain_flag = true;

	return conv_node_id_to_slot_id (node_id, chain_flag);
}



int	EditProg::conv_node_id_to_slot_id (int node_id, bool &chain_flag) const
{
	int            slot_id = -1;

	const doc::Preset &  preset = _view_ptr->use_preset_cur ();
	const int      chain_size   = int (preset._routing._chain.size ());

	if (node_id >= 0 && node_id <= chain_size)
	{
		if (node_id < chain_size)
		{
			slot_id = _slot_id_list [node_id];
		}
		chain_flag = true;
	}
	else if (node_id > chain_size && node_id <= int (_slot_list.size ()) - 1)
	{
		if (node_id < int (_slot_list.size ()) - 1)
		{
			slot_id = _slot_id_list [node_id - 1];
		}
		chain_flag = false;
	}

	return slot_id;
}



int	EditProg::conv_loc_edit_to_node_id () const
{
	if (_loc_edit._slot_id >= 0)
	{
		const auto     it_slot_id = std::find (
			_slot_id_list.begin (),
			_slot_id_list.end (),
			_loc_edit._slot_id
		);
		if (it_slot_id != _slot_id_list.end ())
		{
			const doc::Preset &  preset = _view_ptr->use_preset_cur ();
			const int      chain_size   = int (preset._routing._chain.size ());

			int         pos = int (it_slot_id - _slot_id_list.begin ());
			if (pos >= chain_size)
			{
				++ pos;
			}

			return pos;
		}
	}

	return 0;
}



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
