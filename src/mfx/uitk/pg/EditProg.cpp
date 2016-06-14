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

#include "mfx/pi/param/Tools.h"
#include "mfx/uitk/pg/EditProg.h"
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



EditProg::EditProg (PageSwitcher &page_switcher, const std::vector <pi::PluginModel> &fx_list)
:	_fx_list (fx_list)
,	_page_switcher (page_switcher)
,	_model_ptr (0)
,	_view_ptr (0)
,	_page_ptr (0)
,	_page_size ()
,	_fnt_m_ptr (0)
,	_menu_sptr (new NWindow (Entry_WINDOW))
,	_fx_list_sptr (new NText (Entry_FX_LIST))
,	_prog_name_sptr (new NText (Entry_PROG_NAME))
,	_controllers_sptr (new NText (Entry_CONTROLLERS))
,	_slot_list ()
{
	_prog_name_sptr  ->set_justification (0.5f, 0, false);
	_controllers_sptr->set_justification (0.5f, 0, false);
	_fx_list_sptr    ->set_justification (0.5f, 1, false);
	_controllers_sptr->set_text ("Controllers");
	_fx_list_sptr    ->set_text ("-----");
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	EditProg::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, const ui::Font &fnt_s, const ui::Font &fnt_m, const ui::Font &fnt_l)
{
	_model_ptr = &model;
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;
	_fnt_m_ptr = &fnt_m;

	_prog_name_sptr  ->set_font (fnt_m);
	_controllers_sptr->set_font (fnt_m);
	_fx_list_sptr    ->set_font (fnt_s);

	const int      scr_w = _page_size [0];
	const int      x_mid =  scr_w >> 1;
	const int      h_m   = _fnt_m_ptr->get_char_h ();

	_menu_sptr->set_size (_page_size, Vec2d ());

	_prog_name_sptr  ->set_coord (Vec2d (x_mid, 0 * h_m));
	_controllers_sptr->set_coord (Vec2d (x_mid, 1 * h_m));
	_fx_list_sptr    ->set_coord (Vec2d (x_mid, 3 * h_m));

	_prog_name_sptr  ->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_controllers_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());

	_page_ptr->push_back (_menu_sptr);

	set_preset_info ();
	_page_ptr->jump_to (0);
}



void	EditProg::do_disconnect ()
{
	// Nothing
}



MsgHandlerInterface::EvtProp	EditProg::do_handle_evt (const NodeEvt &evt)
{
	EvtProp        ret_val = EvtProp_PASS;

	if (evt.is_button_ex ())
	{
		const Button   but     = evt.get_button_ex ();
		const int      node_id = evt.get_target ();
		switch (but)
		{
		case Button_S:
			ret_val = EvtProp_CATCH;
			if (node_id == Entry_PROG_NAME)
			{
				/*** To do ***/
			}
			else if (node_id == Entry_CONTROLLERS)
			{
				/*** To do ***/
			}
			else if (node_id >= 0 && node_id < int (_slot_list.size ()))
			{
				/*** To do ***/
			}
			else
			{
				ret_val = EvtProp_PASS;
			}
			break;
		case Button_E:
			_page_switcher.switch_to (pg::PageType_CUR_PROG);
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
	set_preset_info ();
}



void	EditProg::do_set_nbr_slots (int nbr_slots)
{
	set_preset_info ();
}



void	EditProg::do_set_plugin (int slot_index, const PluginInitData &pi_data)
{
	set_preset_info ();
}



void	EditProg::do_remove_plugin (int slot_index)
{
	set_preset_info ();
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	EditProg::set_preset_info ()
{
	assert (_fnt_m_ptr != 0);

	const doc::Preset &  preset = _view_ptr->use_preset_cur ();
	_prog_name_sptr->set_text (preset._name);

	const int      nbr_slots = preset._slot_list.size ();
	PageMgrInterface::NavLocList  nav_list (nbr_slots + 3);
	_slot_list.resize (nbr_slots + 3);

	_menu_sptr->clear_all_nodes ();
	_menu_sptr->push_back (_prog_name_sptr);
	_menu_sptr->push_back (_controllers_sptr);
	_menu_sptr->push_back (_fx_list_sptr);
	nav_list [0]._node_id = Entry_PROG_NAME;
	nav_list [1]._node_id = Entry_CONTROLLERS;

	for (int slot_index = 0; slot_index < nbr_slots; ++slot_index)
	{
		std::string    multilabel = "<Empty>";

		if (! preset.is_slot_empty (slot_index))
		{
			const doc::Slot & slot = *(preset._slot_list [slot_index]);
			multilabel = mfx::pi::PluginModel_get_name (slot._pi_model);
		}

		set_slot (nav_list, slot_index, multilabel);
	}

	set_slot (nav_list, nbr_slots, "<End>");

	_page_ptr->set_nav_layout (nav_list);

	_menu_sptr->invalidate_all ();
}



void	EditProg::set_slot (PageMgrInterface::NavLocList &nav_list, int slot_index, std::string multilabel)
{
	const int      h_m    = _fnt_m_ptr->get_char_h ();
	const int      scr_w  = _page_size [0];
	const int      pos    = slot_index + 2;

	TxtSPtr        entry_sptr (new NText (slot_index));
	entry_sptr->set_coord (Vec2d (0, h_m * (pos + 1)));
	entry_sptr->set_font (*_fnt_m_ptr);
	entry_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
	std::string    txt = pi::param::Tools::print_name_bestfit (
		scr_w, multilabel.c_str (),
		*entry_sptr, &NText::get_char_width
	);
	entry_sptr->set_text (txt);
	_slot_list [pos] = entry_sptr;
	nav_list [pos]._node_id = slot_index;

	assert (pos + 1 == _menu_sptr->get_nbr_nodes ());
	_menu_sptr->push_back (entry_sptr);
}



MsgHandlerInterface::EvtProp	EditProg::change_effect (int node_id, int dir)
{
	assert (node_id >= 0);
	assert (dir != 0);

	EvtProp        ret_val = EvtProp_PASS;

	const doc::Preset &  preset = _view_ptr->use_preset_cur ();
	const int      nbr_slots = preset._slot_list.size ();

	if (node_id <= nbr_slots)
	{
		const int      nbr_types = int (_fx_list.size ());

		// Index within the official plug-in list. end = empty
		int            pi_index  = nbr_types;
		if (node_id < nbr_slots)
		{
			if (! preset.is_slot_empty (node_id))
			{
				const doc::Slot & slot = *(preset._slot_list [node_id]);
				const pi::PluginModel   type = slot._pi_model;
				auto          type_it =
					std::find (_fx_list.begin (), _fx_list.end (), type);
				assert (type_it != _fx_list.end ());
				pi_index = type_it - _fx_list.begin ();
			}
		}

		const int      mod_len = nbr_types + 1;
		pi_index += dir;
		pi_index = (pi_index + mod_len) % mod_len;

		// We need to add a slot at the end?
		if (node_id == nbr_slots && pi_index != nbr_types)
		{
			_model_ptr->set_nbr_slots (nbr_slots + 1);
		}

		if (pi_index == nbr_types)
		{
			_model_ptr->remove_plugin (node_id);
		}
		else
		{
			_model_ptr->set_plugin (node_id, _fx_list [pi_index]);
		}

		// Last slot needs to be removed?
		if (node_id == nbr_slots - 1 && pi_index == nbr_types)
		{
			int         nbr_slots_new = nbr_slots - 1;
			while (nbr_slots_new > 0 && preset.is_slot_empty (nbr_slots_new - 1))
			{
				-- nbr_slots_new;
			}
			_model_ptr->set_nbr_slots (nbr_slots_new);
		}
	}

	return ret_val;
}



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
