/*****************************************************************************

        SlotRouting.cpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/fnc.h"
#include "mfx/pi/param/Tools.h"
#include "mfx/piapi/PluginDescInterface.h"
#include "mfx/ui/Font.h"
#include "mfx/uitk/pg/SlotRouting.h"
#include "mfx/uitk/NodeEvt.h"
#include "mfx/uitk/PageMgrInterface.h"
#include "mfx/uitk/PageSwitcher.h"
#include "mfx/LocEdit.h"
#include "mfx/Model.h"
#include "mfx/ToolsRouting.h"
#include "mfx/View.h"

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace uitk
{
namespace pg
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



SlotRouting::SlotRouting (PageSwitcher &page_switcher, LocEdit &loc_edit)
:	_page_switcher (page_switcher)
,	_loc_edit (loc_edit)
,	_model_ptr (0)
,	_view_ptr (0)
,	_page_ptr (0)
,	_page_size ()
,	_fnt_ptr (0)
,	_menu_sptr (new NWindow (Entry_WINDOW))
,	_mov_sptr (new NText (Entry_MOVE   ))
,	_side_arr ()
{
	_mov_sptr->set_text ("Move\xE2\x80\xA6");

	_menu_sptr->set_autoscroll (true);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	SlotRouting::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt)
{
	fstb::unused (usr_ptr);

	_model_ptr = &model;
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;
	_fnt_ptr   = &fnt._m;

	// Updates _chain_flag, if possible
	_loc_edit.fix_audio_flag (*_view_ptr, *_model_ptr);

	_menu_sptr->set_size (_page_size, Vec2d ());
	_menu_sptr->set_disp_pos (Vec2d ());

	const int      scr_w = _page_size [0];
	const int      h_m   = _fnt_ptr->get_char_h ();

	_mov_sptr->set_font (*_fnt_ptr);

	_mov_sptr->set_coord (Vec2d (0, h_m * 0));

	_mov_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());

	_page_ptr->push_back (_menu_sptr);

	update_display ();
}



void	SlotRouting::do_disconnect ()
{
	// Nothing
}



PageInterface::EvtProp	SlotRouting::do_handle_evt (const NodeEvt &evt)
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
			switch (node_id)
			{
			case Entry_MOVE:
				if (_loc_edit._audio_flag)
				{
					_page_switcher.call_page (PageType_SLOT_MOVE, 0, node_id);
				}
				break;
			default:
				ret_val = EvtProp_PASS;
				break;
			}
			break;
		case Button_E:
			_page_switcher.switch_to (PageType_SLOT_MENU, 0);
			break;
		default:
			// Nothing
			break;
		}
	}

	return ret_val;
}



void	SlotRouting::do_activate_preset (int index)
{
	fstb::unused (index);

	_page_switcher.switch_to (pg::PageType_PROG_EDIT, 0);
}



void	SlotRouting::do_remove_slot (int slot_id)
{
	if (slot_id == _loc_edit._slot_id)
	{
		_loc_edit._slot_id = -1;
		_page_switcher.switch_to (PageType_PROG_EDIT, 0);
	}
}



void	SlotRouting::do_set_routing (const doc::Routing &routing)
{
	fstb::unused (routing);

	update_display ();
}



void	SlotRouting::do_set_plugin (int slot_id, const PluginInitData &pi_data)
{
	fstb::unused (pi_data);

	if (slot_id == _loc_edit._slot_id)
	{
		update_display ();
	}
}



void	SlotRouting::do_remove_plugin (int slot_id)
{
	if (slot_id == _loc_edit._slot_id)
	{
		update_display ();
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	SlotRouting::update_display ()
{
	_menu_sptr->clear_all_nodes ();

	PageMgrInterface::NavLocList  nav_list;

	const bool     select_flag = (_loc_edit._slot_id >= 0);
	const bool     audio_flag  = select_flag && _loc_edit._audio_flag;

	_mov_sptr->show (audio_flag);
	_menu_sptr->push_back (_mov_sptr);
	for (auto &side : _side_arr)
	{
		side.clear ();
	}

	if (audio_flag)
	{
		PageMgrInterface::add_nav (nav_list, Entry_MOVE);

		const doc::Preset &  prog = _view_ptr->use_preset_cur ();
		std::vector <Tools::NodeEntry>   entry_list;
		Tools::extract_slot_list (entry_list, prog, *_model_ptr);

		// Finds physical ports of the associated plug-in, if any
		std::array <int, piapi::Dir_NBR_ELT> nbr_pins_arr_phy = {{ 0, 0 }};
		const bool     exist_flag = ! prog.is_slot_empty (_loc_edit._slot_id);
		if (exist_flag)
		{
			const doc::Slot & slot     = prog.use_slot (_loc_edit._slot_id);
			const std::string pi_model = slot._pi_model;
			const piapi::PluginDescInterface &  pi_desc =
				_model_ptr->get_model_desc (pi_model);
			nbr_pins_arr_phy [piapi::Dir_IN ] = 1;
			nbr_pins_arr_phy [piapi::Dir_OUT] = 1;
			int            nbr_s = 0;
			pi_desc.get_nbr_io (
				nbr_pins_arr_phy [piapi::Dir_IN ],
				nbr_pins_arr_phy [piapi::Dir_OUT],
				nbr_s
			);
		}

		// Graph connections
		const ToolsRouting::NodeMap & graph = _view_ptr->use_graph ();
		ToolsRouting::NodeMap::const_iterator it_node = graph.find (
			ToolsRouting::Node (doc::CnxEnd::Type_NORMAL, _loc_edit._slot_id)
		);
		const int      scr_w = _page_size [0];
		const int      h_m   = _fnt_ptr->get_char_h ();
		int            pos_y = h_m * 2;
		char           txt_0 [255+1];
		char           txt2_0 [255+1];
		for (int dir = 0; dir < piapi::Dir_NBR_ELT; ++dir)
		{
			Side &      side     = _side_arr [dir];
			int         nbr_pins = nbr_pins_arr_phy [dir];
			int         nbr_pins_gra = 0;
			if (it_node != graph.end ())
			{
				const ToolsRouting::CnxPinList &	side_node = it_node->second [dir];
				nbr_pins_gra = int (side_node.size ());
				nbr_pins     = std::max (nbr_pins, nbr_pins_gra);
			}
			side.resize (nbr_pins);
			for (int pin_idx = 0; pin_idx < nbr_pins; ++pin_idx)
			{
				Pin &          pin = side [pin_idx];

				// Name
				const int      nid_name = conv_cnx_to_node_id (
					IoType_NAME, piapi::Dir (dir), pin_idx, -1
				);
				txt2_0 [0] = '\0';
				if (exist_flag)
				{
					/*** To do: add an information about the stereo ***/
					fstb::snprintf4all (txt2_0, sizeof (txt2_0), "/%d", nbr_pins);
				}
				fstb::snprintf4all (
					txt_0, sizeof (txt_0),
					"%s %d%s:%s", _dir_txt_arr [dir], pin_idx + 1, txt2_0,
					(nbr_pins_gra == 0) ? " N.C." : ""
				);
				pin._name_sptr = TxtSPtr (new NText (nid_name));
				pin._name_sptr->set_font (*_fnt_ptr);
				pin._name_sptr->set_coord (Vec2d (0, pos_y));
				pin._name_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
				pin._name_sptr->set_text (txt_0);
				_menu_sptr->push_back (pin._name_sptr);
				PageMgrInterface::add_nav (nav_list, nid_name);

				pos_y += h_m;

				// Connections
				if (it_node != graph.end () && pin_idx < nbr_pins_gra)
				{
					const ToolsRouting::CnxSet &  cnx_set =
						it_node->second [dir] [pin_idx];
					const int      nbr_cnx = int (cnx_set.size ());
					int            cnx_idx = 0;
					ToolsRouting::CnxSet::const_iterator it_cnx = cnx_set.begin ();
					pin._cnx_sptr_arr.resize (nbr_cnx);
					while (it_cnx != cnx_set.end ())
					{
						const doc::CnxEnd &  cnx_end =
							it_cnx->use_end (piapi::Dir (dir));

						const int      nid_cnx  = conv_cnx_to_node_id (
							IoType_CNX, piapi::Dir (dir), pin_idx, cnx_idx
						);
						TxtSPtr &      cnx_sptr = pin._cnx_sptr_arr [cnx_idx];
						cnx_sptr = TxtSPtr (new NText (nid_cnx));
						cnx_sptr->set_font (*_fnt_ptr);
						cnx_sptr->set_coord (Vec2d (0, pos_y));
						cnx_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
						print_cnx_name (
							*cnx_sptr, scr_w, entry_list, piapi::Dir (dir), cnx_end
						);

						_menu_sptr->push_back (cnx_sptr);
						PageMgrInterface::add_nav (nav_list, nid_cnx);

						pos_y += h_m;
						++ it_cnx;
						++ cnx_idx;
					}
				}
			}
		}
	}

	_page_ptr->set_nav_layout (nav_list);

	_menu_sptr->invalidate_all ();
}



void	SlotRouting::print_cnx_name (NText &txtbox, int scr_w, const std::vector <Tools::NodeEntry> &entry_list, piapi::Dir dir, const doc::CnxEnd &cnx_end)
{
	char           txt_0 [255+1];

	const doc::CnxEnd::Type end_type = cnx_end.get_type ();
	std::string    multilabel;
	if (end_type == doc::CnxEnd::Type_IO)
	{
		multilabel = "Audio ";
		multilabel += _dir_txt_arr [dir];
	}
	else
	{
		// Retrieves plug-in name
		const int      end_slot_id = cnx_end.get_slot_id ();
		const auto     it_entry    = std::find_if (
			entry_list.begin (),
			entry_list.end (),
			[end_slot_id] (const auto &entry)
			{
				return (entry._slot_id == end_slot_id);
			}
		);
		assert (it_entry != entry_list.end ());
		multilabel = it_entry->_name_multilabel;
		if (it_entry->_instance_nbr >= 0)
		{
			fstb::snprintf4all (
				txt_0, sizeof (txt_0), " %d", it_entry->_instance_nbr + 1
			);
			multilabel = pi::param::Tools::join_strings_multi (
				multilabel.c_str (), '\n', "", txt_0
			);
		}
	}

	const int      end_pin_idx = cnx_end.get_pin ();
	fstb::snprintf4all (
		txt_0, sizeof (txt_0), " - %d", end_pin_idx + 1
	);
	multilabel = pi::param::Tools::join_strings_multi (
		multilabel.c_str (), '\n', _indent_0, txt_0
	);

	std::string    txt = pi::param::Tools::print_name_bestfit (
		scr_w, multilabel.c_str (),
		txtbox, &NText::get_char_width
	);
	txtbox.set_text (txt);
}



int	SlotRouting::conv_cnx_to_node_id (IoType type, piapi::Dir dir, int pin_idx, int cnx_idx)
{
	assert (type >= 0);
	assert (type < IoType_NBR_ELT);
	assert (dir >= 0);
	assert (dir < piapi::Dir_NBR_ELT);
	assert (pin_idx >= 0);
	assert (type == IoType_NAME || cnx_idx >= 0);

	const int      cnx_val = (type == IoType_NAME) ? _ofs_name : cnx_idx;

	return Entry_IO + (dir << _s_dir) + (pin_idx << _s_pin) + cnx_val;
}



SlotRouting::IoType	SlotRouting::conv_node_id_to_cnx (piapi::Dir &dir, int &pin_idx, int &cnx_idx, int node_id)
{
	IoType         type = IoType_INVALID;

	if (node_id >= Entry_IO)
	{
		node_id -= Entry_IO;

		dir = piapi::Dir (node_id >> _s_dir);
		if (dir >= 0 && dir < piapi::Dir_NBR_ELT)
		{
			pin_idx = (node_id >> _s_pin) & _mask_pin;
			if (pin_idx >= 0)
			{
				cnx_idx = node_id & _mask_cnx;
				if (cnx_idx == _ofs_name)
				{
					cnx_idx = -1;
					type = IoType_NAME;
				}
				else
				{
					type = IoType_CNX;
				}
			}
		}
	}

	return type;
}



std::array <const char *, piapi::Dir_NBR_ELT>	SlotRouting::_dir_txt_arr =
{{
	"In", "Out"
}};

const char *	SlotRouting::_indent_0 = "  ";



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
