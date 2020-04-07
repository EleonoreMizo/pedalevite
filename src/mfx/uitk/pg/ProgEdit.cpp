/*****************************************************************************

        ProgEdit.cpp
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
#include "mfx/uitk/pg/ProgEdit.h"
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



ProgEdit::ProgEdit (PageSwitcher &page_switcher, LocEdit &loc_edit)
:	_page_switcher (page_switcher)
,	_loc_edit (loc_edit)
,	_model_ptr (nullptr)
,	_view_ptr (nullptr)
,	_page_ptr (nullptr)
,	_page_size ()
,	_fnt_ptr (nullptr)
,	_menu_sptr (     std::make_shared <NWindow> (Entry_WINDOW   ))
,	_fx_list_sptr (  std::make_shared <NText  > (Entry_FX_LIST  ))
,	_ms_list_sptr (  std::make_shared <NText  > (Entry_MS_LIST  ))
,	_prog_name_sptr (std::make_shared <NText  > (Entry_PROG_NAME))
,	_settings_sptr ( std::make_shared <NText  > (Entry_SETTINGS ))
,	_save_sptr (     std::make_shared <NText  > (Entry_SAVE     ))
,	_slot_list ()
,	_rout_list ()
,	_state (State_NORMAL)
,	_save_bank_index (-1)
,	_save_preset_index (-1)
,	_name_param ()
,	_slot_id_list ()
,	_audio_list_len (0)
,	_spi_flag (false)
{
	_prog_name_sptr->set_justification (0.5f, 0, false);
	_settings_sptr ->set_justification (0.5f, 0, false);
	_save_sptr     ->set_justification (0.5f, 0, false);
	_fx_list_sptr  ->set_justification (0.5f, 1, false);
	_ms_list_sptr  ->set_justification (0.5f, 1, false);
	_settings_sptr ->set_text ("Settings\xE2\x80\xA6");
	_save_sptr     ->set_text ("Save to\xE2\x80\xA6");
	_ms_list_sptr  ->set_text ("---------------------");
	_prog_name_sptr->set_bold (true, true );
	_fx_list_sptr  ->set_bold (true, false);
	_ms_list_sptr  ->set_bold (true, false);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	ProgEdit::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt)
{
	fstb::unused (usr_ptr);

	_model_ptr = &model;
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;
	_fnt_ptr   = &fnt._m;

	_spi_flag = false;

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

	_prog_name_sptr->set_font (*_fnt_ptr);
	_settings_sptr ->set_font (*_fnt_ptr);
	_save_sptr     ->set_font (*_fnt_ptr);
	_fx_list_sptr  ->set_font (*_fnt_ptr);
	_ms_list_sptr  ->set_font (*_fnt_ptr);

	const int      scr_w = _page_size [0];
	const int      x_mid =  scr_w >> 1;
	const int      h_m   = _fnt_ptr->get_char_h ();

	_menu_sptr->set_size (_page_size, Vec2d ());
	_menu_sptr->set_disp_pos (Vec2d ());

	_prog_name_sptr->set_coord (Vec2d (x_mid, 0 * h_m));
	_settings_sptr ->set_coord (Vec2d (x_mid, 1 * h_m));
	_save_sptr     ->set_coord (Vec2d (x_mid, 2 * h_m));
	_fx_list_sptr  ->set_coord (Vec2d (x_mid, 4 * h_m));

	_prog_name_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_settings_sptr ->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_save_sptr     ->set_frame (Vec2d (scr_w, 0), Vec2d ());

	_page_ptr->push_back (_menu_sptr);

	// We need to save the slot_id because it will be reset during
	// the set_nav_layout () in the set_preset_info ().
	/*** To do: remove this infamous hack after the cleaning of
	set_preset_info () ***/
	const int      slot_id_save = _loc_edit._slot_id;
	set_preset_info ();

	_loc_edit._slot_id = slot_id_save;
	_page_ptr->jump_to (conv_loc_edit_to_node_id ());
}



void	ProgEdit::do_disconnect ()
{
	if (_model_ptr != nullptr)
	{
		_model_ptr->reset_all_overridden_param_ctrl ();
	}
}



MsgHandlerInterface::EvtProp	ProgEdit::do_handle_evt (const NodeEvt &evt)
{
	EvtProp        ret_val = EvtProp_PASS;

	const int      node_id = evt.get_target ();

	if (evt.is_cursor ())
	{
		if (evt.get_cursor () == NodeEvt::Curs_ENTER)
		{
		   if (node_id >= 0 && node_id < int (_slot_list.size ()))
			{
				update_loc_edit (node_id);
				set_preset_info ();
			}
			else if (node_id >= Entry_WINDOW && _loc_edit._slot_id >= 0)
			{
				_loc_edit._slot_id = -1;
				set_preset_info ();
			}
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
				_page_switcher.switch_to (PageType_PROG_SAVE, nullptr);
			}
			else if (node_id == Entry_SETTINGS)
			{
				_page_switcher.switch_to (PageType_PROG_SETTINGS, nullptr);
			}
			else if (node_id >= 0 && node_id < int (_slot_list.size ()))
			{
				const doc::Preset &  preset  = _view_ptr->use_preset_cur ();
				const int            slot_id = conv_node_id_to_slot_id (node_id);
				if (slot_id >= 0 && ! preset.is_slot_empty (slot_id))
				{
					// Full slot
					update_loc_edit (node_id);
					_page_switcher.switch_to (PageType_PARAM_LIST, nullptr);
				}
				else
				{
					// Empty slot
					_page_switcher.switch_to (PageType_SLOT_MENU, nullptr);
				}
			}
			else
			{
				ret_val = EvtProp_PASS;
			}
			break;
		case Button_E:
			_page_switcher.switch_to (pg::PageType_PROG_CUR, nullptr);
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



void	ProgEdit::do_activate_preset (int index)
{
	fstb::unused (index);

	set_preset_info ();
}



void	ProgEdit::do_set_preset_name (std::string name)
{
	fstb::unused (name);

	set_preset_info ();
}



void	ProgEdit::do_add_slot (int slot_id)
{
	fstb::unused (slot_id);

	set_preset_info ();
}



void	ProgEdit::do_remove_slot (int slot_id)
{
	fstb::unused (slot_id);

	set_preset_info ();
}



void	ProgEdit::do_set_routing (const doc::Routing &routing)
{
	fstb::unused (routing);

	set_preset_info ();
}



void	ProgEdit::do_set_plugin (int slot_id, const PluginInitData &pi_data)
{
	fstb::unused (slot_id, pi_data);

	set_preset_info ();
}



void	ProgEdit::do_remove_plugin (int slot_id)
{
	fstb::unused (slot_id);

	set_preset_info ();
}



void	ProgEdit::do_set_param_ctrl (int slot_id, PiType type, int index, const doc::CtrlLinkSet &cls)
{
	fstb::unused (slot_id, type, index, cls);

	set_preset_info ();
}



void	ProgEdit::do_enable_auto_rotenc_override (bool ovr_flag)
{
	if (! ovr_flag)
	{
		_model_ptr->reset_all_overridden_param_ctrl ();
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*** To do:
This code is wrong. We should split this function in two distinct ones:
- Update of cached data after a model change.
- Update of displayed information after user navigation.
set_nav_layout () should belong to the first function, and
update_rotenc_mapping () should belong to the second function.
***/

void	ProgEdit::set_preset_info ()
{
	if (_spi_flag)
	{
		return;
	}

	_spi_flag = true;

	assert (_fnt_ptr != nullptr);

	update_cached_pi_list ();
	update_rotenc_mapping ();

	const doc::Preset &  preset = _view_ptr->use_preset_cur ();

	_prog_name_sptr->set_text (preset._name);

	const int      nbr_slots = int (_slot_id_list.size ());
	PageMgrInterface::NavLocList  nav_list (nbr_slots + 5);
	_slot_list.resize (nbr_slots + 2);
	_rout_list.resize (nbr_slots + 2);

	_menu_sptr->clear_all_nodes ();
	_menu_sptr->push_back (_prog_name_sptr);
	_menu_sptr->push_back (_settings_sptr);
	_menu_sptr->push_back (_save_sptr);
	_menu_sptr->push_back (_fx_list_sptr);
	_menu_sptr->push_back (_ms_list_sptr);
	nav_list [0]._node_id = Entry_PROG_NAME;
	nav_list [1]._node_id = Entry_SETTINGS;
	nav_list [2]._node_id = Entry_SAVE;

	std::vector <Tools::NodeEntry>   entry_list;
	Tools::extract_slot_list (entry_list, preset, *_model_ptr);
	assert (nbr_slots == int (entry_list.size ()));

	std::vector <Link>   link_list (find_chain_links (entry_list));
	find_broken_links (link_list, entry_list);

	const int      scr_w = _page_size [0];
	const int      x_mid =  scr_w >> 1;
	const int      h_m   = _fnt_ptr->get_char_h ();
	_ms_list_sptr->set_coord (Vec2d (x_mid, (_audio_list_len + 6) * h_m));

	for (int slot_index = 0; slot_index < nbr_slots; ++slot_index)
	{
		const Tools::NodeEntry &   entry = entry_list [slot_index];
		assert (entry._slot_id == _slot_id_list [slot_index]);

		const int      slot_id    = entry._slot_id;
		std::string    multilabel = entry._name_multilabel;
		bool           ctrl_flag  = false;

		const auto     it_slot    = preset._slot_map.find (slot_id);
		assert (it_slot != preset._slot_map.end ());
		if (! entry._type.empty ())
		{
			const doc::Slot & slot = *(it_slot->second);
			ctrl_flag  = slot.has_ctrl ();
		}

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

		const char *   link_0 = "";
		switch (link_list [slot_index])
		{
		case Link_NONE:   /* Nothing */            break;
		case Link_CHAIN:  link_0 = "\xE2\x9A\xAB"; break; // U+26AB MEDIUM BLACK CIRCLE
		case Link_BRANCH: link_0 = "\xE2\x9A\xAC"; break; // U+26AC MEDIUM SMALL WHITE CIRCLE
		case Link_BROKEN: link_0 = "\xC3\x97";     break; // U+00D7 MULTIPLICATION SIGN
		default:          assert (false);          break;
		}

		const int      skip     = (slot_index >= _audio_list_len) ? 1 : 0;
		const int      pos_list = slot_index + skip;
		set_slot (
			nav_list, pos_list, multilabel, link_0, ctrl_flag, _audio_list_len
		);
	}

	set_slot (nav_list, _audio_list_len, "<End>", "", false, _audio_list_len);
	set_slot (nav_list, nbr_slots + 1  , "<End>", "", false, _audio_list_len);

	if (ToolsRouting::are_audio_io_connected (_view_ptr->use_graph ()))
	{
		_fx_list_sptr->set_text ("---------------------");
	}
	else
	{
		_fx_list_sptr->set_text ("---I/O not linked!---");
	}

	_page_ptr->set_nav_layout (nav_list);

	_menu_sptr->invalidate_all ();

	_spi_flag = false;
}



std::vector <ProgEdit::Link>	ProgEdit::find_chain_links (const std::vector <Tools::NodeEntry> &entry_list) const
{
	std::vector <ProgEdit::Link>  link_list (entry_list.size (), Link_NONE);

	if (_loc_edit._slot_id >= 0)
	{
		const ToolsRouting::NodeMap & graph = _view_ptr->use_graph ();
		ToolsRouting::NodeMap::const_iterator  it = graph.find (
			ToolsRouting::Node (doc::CnxEnd::Type_NORMAL, _loc_edit._slot_id)
		);
		if (it != graph.end ())
		{
			// The starting point
			set_link (link_list, _loc_edit._slot_id, Link_CHAIN, entry_list);

			for (int dir = 0; dir < piapi::Dir_NBR_ELT; ++dir)
			{
				find_chain_links_dir (
					link_list,
					_loc_edit._slot_id,
					piapi::Dir (dir),
					entry_list
				);
			}
		}
	}

	return link_list;
}



/*** To do:
To be decided: should we include in the chain the nodes that merge paths
(current behaviour) or should we stop the chain at these nodes?
Actual use-cases will probably tell us on the long term.
***/

void	ProgEdit::find_chain_links_dir (std::vector <Link> &link_list, int slot_id, piapi::Dir dir, const std::vector <Tools::NodeEntry> &entry_list) const
{
	const ToolsRouting::NodeMap & graph = _view_ptr->use_graph ();

	bool        cont_flag = true;
	do
	{
		ToolsRouting::NodeMap::const_iterator  it = graph.find (
			ToolsRouting::Node (doc::CnxEnd::Type_NORMAL, slot_id)
		);
		if (it == graph.end ())
		{
			cont_flag = false;
		}
		else
		{
			std::set <int> slot_id_set;
			int            nbr_cnx = 0;
			const auto &   side = it->second [dir];
			for (auto &pin : side)
			{
				for (auto &cnx : pin)
				{
					const doc::CnxEnd &  cnx_end = cnx.use_end (dir);
					const doc::CnxEnd::Type type = cnx_end.get_type ();
					if (type == doc::CnxEnd::Type_NORMAL)
					{
						slot_id = cnx_end.get_slot_id ();
						slot_id_set.insert (slot_id);
						++ nbr_cnx;
					}
				}
			}

			Link           link (Link_CHAIN);
			if (nbr_cnx != 1)
			{
				link      = Link_BRANCH;
				cont_flag = false;
			}

			for (auto sid_mark : slot_id_set)
			{
				set_link (link_list, sid_mark, link, entry_list);
			}
		}
	}
	while (cont_flag);
}



void	ProgEdit::set_link (std::vector <Link> &link_list, int slot_id, Link link, const std::vector <Tools::NodeEntry> &entry_list) const
{
	const auto     it = std::find_if (
		entry_list.begin (),
		entry_list.end (),
		[slot_id] (const Tools::NodeEntry &entry)
		{
			return (entry._slot_id == slot_id);
		}
	);
	assert (it != entry_list.end ());
	const int      pos = int (it - entry_list.begin ());
	link_list [pos] = link;
}



void	ProgEdit::find_broken_links (std::vector <Link> &link_list, const std::vector <Tools::NodeEntry> &entry_list) const
{
	assert (_view_ptr != 0);
	assert (_model_ptr != 0);
	assert (link_list.size () == entry_list.size ());
	assert (_audio_list_len <= int (entry_list.size ()));

	const ToolsRouting::NodeMap & graph = _view_ptr->use_graph ();
	const doc::Preset &           prog  = _view_ptr->use_preset_cur ();

	for (int slot_cnt = 0; slot_cnt < _audio_list_len; ++slot_cnt)
	{
		const int      slot_id     = entry_list [slot_cnt]._slot_id;
		bool           broken_flag = false;

		// Strategy should be synchronized to cmd::Router::visit_node (),
		// buffer allocation part

		// Graph connections
		ToolsRouting::NodeMap::const_iterator it_node = graph.find (
			ToolsRouting::Node (doc::CnxEnd::Type_NORMAL, slot_id)
		);
		const int      nbr_i_g = int (it_node->second [piapi::Dir_IN ].size ());
		const int      nbr_o_g = int (it_node->second [piapi::Dir_OUT].size ());

		// Finds physical ports of the associated plug-in, if any
		std::array <int, piapi::Dir_NBR_ELT> nbr_pins_arr_phy =
		{{
			// Default values are the known graph connections
			nbr_i_g, nbr_o_g
		}};
		int            nbr_s      = 0;
		const bool     exist_flag = Tools::get_physical_io (
			nbr_pins_arr_phy [piapi::Dir_IN], nbr_pins_arr_phy [piapi::Dir_OUT],
			nbr_s, slot_id, prog, *_model_ptr
		);

		// There is a plug-in: checks the connections.
		// We assume here that extra input pins on the plug-in are side-chains
		// and leaving them unconnected does not break the signal path.
		// Same for extra output pins, they could be auxiliary outputs.
		if (exist_flag)
		{
			broken_flag = (
				   nbr_i_g > nbr_pins_arr_phy [piapi::Dir_IN ]
				|| nbr_o_g > nbr_pins_arr_phy [piapi::Dir_OUT]
			);
		}

		// No plug-in
		else
		{
			// May not be relevant, for example for potential side-chains,
			// but it's better to report it anyway.
			broken_flag = (nbr_o_g > nbr_i_g);
		}

		if (broken_flag)
		{
			link_list [slot_cnt] = Link_BROKEN;
		}
	}
}



void	ProgEdit::set_slot (PageMgrInterface::NavLocList &nav_list, int pos_list, std::string multilabel, std::string link_txt, bool bold_flag, int chain_size)
{
	const int      w_m      = _fnt_ptr->get_char_w ();
	const int      h_m      = _fnt_ptr->get_char_h ();
	const int      scr_w    = _page_size [0];
	const int      pos_nav  = pos_list + 3; // In the nav_list
	const int      skip     = (pos_list >= chain_size + 1) ? 1 : 0;
	const int      pos_menu = pos_list + 4 + skip;

	// Main text
	const int      txt_x    = w_m;
	const int      margin   = w_m;
	TxtSPtr        entry_sptr { std::make_shared <NText> (pos_list) };
	entry_sptr->set_frame (Vec2d (scr_w - txt_x, 0), Vec2d (margin, 0));
	entry_sptr->set_coord (Vec2d (txt_x, h_m * pos_menu));
	entry_sptr->set_font (*_fnt_ptr);
	entry_sptr->set_bold (bold_flag, true);
	std::string    txt = pi::param::Tools::print_name_bestfit (
		scr_w - txt_x - margin, multilabel.c_str (),
		*entry_sptr, &NText::get_char_width
	);
	entry_sptr->set_text (txt);
	_slot_list [pos_list] = entry_sptr;
	nav_list [pos_nav]._node_id = pos_list;

	_menu_sptr->push_back (entry_sptr);

	// Link information
	if (link_txt.empty ())
	{
		_rout_list [pos_list].reset ();
	}
	else
	{
		TxtSPtr        link_sptr {
			std::make_shared <NText> (Entry_LINKS + pos_list) };
		link_sptr->set_coord (Vec2d (0, h_m * pos_menu));
		link_sptr->set_font (*_fnt_ptr);
		link_sptr->set_text (link_txt);
		_rout_list [pos_list] = link_sptr;

		_menu_sptr->push_back (link_sptr);
	}
}



MsgHandlerInterface::EvtProp	ProgEdit::change_effect (int node_id, int dir)
{
	assert (node_id >= 0);
	assert (dir != 0);

	_model_ptr->reset_all_overridden_param_ctrl ();

	_loc_edit._audio_flag = (node_id <= _audio_list_len);

	const std::vector <std::string> &   pi_list =
		  (_loc_edit._audio_flag)
		? _view_ptr->use_pi_aud_list ()
		: _view_ptr->use_pi_sig_list ();
	const int      slot_id = conv_node_id_to_slot_id (node_id);
	Tools::change_plugin (
		*_model_ptr,
		*_view_ptr,
		slot_id,
		dir,
		pi_list,
		_loc_edit._audio_flag
	);

	update_cached_pi_list ();
	update_rotenc_mapping ();

	return EvtProp_CATCH;
}



void	ProgEdit::update_loc_edit (int node_id)
{
	_loc_edit._slot_id = conv_node_id_to_slot_id (
		node_id, _loc_edit._audio_flag
	);
}



void	ProgEdit::update_cached_pi_list ()
{
	_audio_list_len = _view_ptr->build_ordered_node_list (_slot_id_list, true);
}



void	ProgEdit::update_rotenc_mapping ()
{
	assert (_model_ptr != nullptr);
	assert (_view_ptr  != nullptr);

	const doc::Setup &   setup = _view_ptr->use_setup ();

	if (_loc_edit._slot_id < 0)
	{
		_model_ptr->reset_all_overridden_param_ctrl ();
	}
	else if (setup._auto_assign_rotenc_flag)
	{
		Tools::assign_default_rotenc_mapping (
			*_model_ptr, *_view_ptr, _loc_edit._slot_id, 0
		);
	}
}



int	ProgEdit::conv_node_id_to_slot_id (int node_id) const
{
	bool           chain_flag = true;

	return conv_node_id_to_slot_id (node_id, chain_flag);
}



int	ProgEdit::conv_node_id_to_slot_id (int node_id, bool &chain_flag) const
{
	int            slot_id = -1;

	if (node_id >= 0 && node_id <= _audio_list_len)
	{
		if (node_id < _audio_list_len)
		{
			slot_id = _slot_id_list [node_id];
		}
		chain_flag = true;
	}
	else if (node_id > _audio_list_len && node_id <= int (_slot_list.size ()) - 1)
	{
		if (node_id < int (_slot_list.size ()) - 1)
		{
			slot_id = _slot_id_list [node_id - 1];
		}
		chain_flag = false;
	}

	return slot_id;
}



int	ProgEdit::conv_loc_edit_to_node_id () const
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
			int         pos = int (it_slot_id - _slot_id_list.begin ());
			if (pos >= _audio_list_len)
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
