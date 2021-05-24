/*****************************************************************************

        SlotRoutingAction.cpp
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
#include "mfx/piapi/PluginDescInterface.h"
#include "mfx/ui/Font.h"
#include "mfx/uitk/pg/SlotRoutingAction.h"
#include "mfx/uitk/pg/Tools.h"
#include "mfx/uitk/NodeEvt.h"
#include "mfx/uitk/PageSwitcher.h"
#include "mfx/LocEdit.h"
#include "mfx/Model.h"
#include "mfx/ToolsRouting.h"
#include "mfx/View.h"

#include <cassert>



namespace mfx
{
namespace uitk
{
namespace pg
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



SlotRoutingAction::SlotRoutingAction (PageSwitcher &page_switcher, LocEdit &loc_edit)
:	_page_switcher (page_switcher)
,	_loc_edit (loc_edit)
,	_model_ptr (nullptr)
,	_view_ptr (nullptr)
,	_page_ptr (nullptr)
,	_page_size ()
,	_fnt_ptr (nullptr)
,	_arg_ptr (nullptr)
,	_menu_sptr (std::make_shared <NWindow> (Entry_WINDOW ))
,	_tit_sptr ( std::make_shared <NText  > (Entry_TITLE  ))
,	_del_sptr ( std::make_shared <NText  > (Entry_DEL    ))
,	_rep_sptr ( std::make_shared <NText  > (Entry_REPLACE))
,	_cnx_end_arr ()
{
	_tit_sptr->set_justification (0.5, 0, false);
	_del_sptr->set_text ("Delete");
	_rep_sptr->set_text ("Replace with:");

	_menu_sptr->set_autoscroll (true);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	SlotRoutingAction::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt)
{
	assert (usr_ptr != nullptr);

	_model_ptr = &model;
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;
	_fnt_ptr   = &fnt._m;
	_arg_ptr   = static_cast <const Arg *> (usr_ptr);

	const int      scr_w = _page_size [0];

	_menu_sptr->set_size (_page_size, Vec2d ());
	_menu_sptr->set_disp_pos (Vec2d ());

	switch (_arg_ptr->_ed_type)
	{
	case EdType_CNX:
		_tit_sptr->set_text ("Edit connection");
		break;
	case EdType_PIN:
		_tit_sptr->set_text ("Add connection");
		break;
	default:
		assert (false);
		break;
	}

	_tit_sptr->set_font (fnt._l);
	_tit_sptr->set_coord (Vec2d (scr_w >> 1, 0));
	_tit_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_page_ptr->push_back (_menu_sptr);

	update_display ();
}



void	SlotRoutingAction::do_disconnect ()
{
	_arg_ptr = nullptr;
}



PageInterface::EvtProp	SlotRoutingAction::do_handle_evt (const NodeEvt &evt)
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
			if (   node_id >= Entry_TARGET
			    && node_id < Entry_TARGET + int (_cnx_end_arr.size ()))
			{
				add_or_replace_cnx (node_id);
			}
			else
			{
				switch (node_id)
				{
				case Entry_DEL:
					del_cur_cnx ();
					break;
				default:
					ret_val = EvtProp_PASS;
					break;
				}
			}
			break;
		case Button_E:
			_page_switcher.switch_to (PageType_SLOT_ROUTING, nullptr);
			break;
		default:
			// Nothing
			break;
		}
	}

	return ret_val;
}



void	SlotRoutingAction::do_activate_preset (int index)
{
	fstb::unused (index);

	_page_switcher.switch_to (pg::PageType_PROG_EDIT, nullptr);
}



void	SlotRoutingAction::do_remove_slot (int slot_id)
{
	if (slot_id == _loc_edit._slot_id)
	{
		_loc_edit._slot_id = -1;
		_page_switcher.switch_to (PageType_PROG_EDIT, nullptr);
	}
	else
	{
		update_display ();
	}
}



void	SlotRoutingAction::do_set_routing (const doc::Routing &routing)
{
	fstb::unused (routing);

	_page_switcher.switch_to (pg::PageType_SLOT_ROUTING, nullptr);
}



void	SlotRoutingAction::do_set_plugin (int slot_id, const PluginInitData &pi_data)
{
	fstb::unused (pi_data);

	if (slot_id == _loc_edit._slot_id)
	{
		_page_switcher.switch_to (pg::PageType_SLOT_ROUTING, nullptr);
	}
	else
	{
		update_display ();
	}
}



void	SlotRoutingAction::do_remove_plugin (int slot_id)
{
	if (slot_id == _loc_edit._slot_id)
	{
		_page_switcher.switch_to (pg::PageType_SLOT_ROUTING, nullptr);
	}
	else
	{
		update_display ();
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	SlotRoutingAction::update_display ()
{
	PageMgrInterface::NavLocList  nav_list;

	const int      scr_w = _page_size [0];
	const int      h_m   = _fnt_ptr->get_char_h ();

	_menu_sptr->clear_all_nodes ();
	_menu_sptr->push_back (_tit_sptr);

	int            pos_y = h_m * 2;
	if (_arg_ptr->_ed_type == EdType_CNX)
	{
		_del_sptr->set_font (*_fnt_ptr);
		_rep_sptr->set_font (*_fnt_ptr);

		_del_sptr->set_coord (Vec2d (0, pos_y));
		_rep_sptr->set_coord (Vec2d (0, pos_y + h_m));

		_del_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
		_rep_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());

		_menu_sptr->push_back (_del_sptr);
		_menu_sptr->push_back (_rep_sptr);

		PageMgrInterface::add_nav (nav_list, Entry_DEL);

		pos_y += h_m * 2;
	}

	build_possible_cnx_set (nav_list, pos_y);

	_page_ptr->set_nav_layout (nav_list);

	_menu_sptr->invalidate_all ();
}



void	SlotRoutingAction::build_possible_cnx_set (PageMgrInterface::NavLocList &nav_list, int pos_y)
{
	assert (_loc_edit._slot_id >= 0);
	assert (_arg_ptr != nullptr);

	_cnx_end_arr.clear ();

	// We exclude all the nodes on which a connection may cause a loop.
	// For a downstream connection, they are all the upstream nodes,
	// and vice-versa.
	const ToolsRouting::NodeMap & graph = _view_ptr->use_graph ();
	std::set <ToolsRouting::Node> node_list_u;
	std::set <ToolsRouting::Node> node_list_d;
	ToolsRouting::find_coverage (
		node_list_u, node_list_d, graph, _loc_edit._slot_id
	);
	std::set <ToolsRouting::Node> & node_excl_list (
		  (_arg_ptr->_dir == piapi::Dir_IN) ? node_list_d : node_list_u
	);

	// Subtracts the exclusion list from the overall slot list
	std::vector <int> slot_arr = _view_ptr->use_slot_list_aud (); // Copy
	for (int pos = int (slot_arr.size ()) - 1; pos >= 0; --pos)
	{
		const int      slot_id = slot_arr [pos];
		const auto     it      = node_excl_list.find (
			ToolsRouting::Node (doc::CnxEnd::Type_NORMAL, slot_id)
		);
		if (it != node_excl_list.end ())
		{
			slot_arr.erase (slot_arr.begin () + pos);

			// Removes the element, so next calls to find() will become faster.
			node_excl_list.erase (it);
		}
	}

	// Builds a set of additional exclusion targets: the existing connections
	std::set <doc::CnxEnd>  ce_excl_list;
	ToolsRouting::NodeMap::const_iterator it_node = graph.find (
		ToolsRouting::Node (doc::CnxEnd::Type_NORMAL, _loc_edit._slot_id)
	);
	if (it_node != graph.end ())
	{
		int         pin_idx = _arg_ptr->_pin_idx;
		if (_arg_ptr->_ed_type == EdType_CNX)
		{
			pin_idx = _arg_ptr->_cnx.use_end (_arg_ptr->_dir).get_pin ();
		}

		const auto &   side = it_node->second [_arg_ptr->_dir];

		// Checks the pin index, as the pin may not exist in the graph. For
		// example when trying to add back a connection to a disconnected pin.
		if (pin_idx < int (side.size ()))
		{
			const auto &   pin  = side [pin_idx];
			for (const auto &cnx : pin)
			{
				ce_excl_list.insert (cnx.use_end (_arg_ptr->_dir));
			}
		}
	}

	const piapi::Dir  dir_opp = piapi::Dir_invert (_arg_ptr->_dir);
	const doc::Preset &  prog = _view_ptr->use_preset_cur ();
	std::vector <Tools::NodeEntry>   entry_list;
	Tools::extract_slot_list (entry_list, prog, *_model_ptr);
	int            nid = Entry_TARGET;

	// Builds the list of valid connection targets
	for (const int slot_id : slot_arr)
	{
		std::array <int, piapi::Dir_NBR_ELT> nbr_pins_arr = {{ 1, 1 }};

		// Finds physical ports of the associated plug-in, if any
		int            nbr_s = 0;
		Tools::get_physical_io (
			nbr_pins_arr [piapi::Dir_IN], nbr_pins_arr [piapi::Dir_OUT], nbr_s,
			slot_id, prog, *_model_ptr
		);

		// Creates the connection targets
		for (int pin_idx = 0; pin_idx < nbr_pins_arr [dir_opp]; ++pin_idx)
		{
			list_target (
				nav_list, nid, pos_y,
				doc::CnxEnd (doc::CnxEnd::Type_NORMAL, slot_id, pin_idx),
				ce_excl_list, nbr_pins_arr [dir_opp], entry_list
			);
		}
	}

	// Adds the audio input/output
	const int      nbr_pins = 1; /*** To do: get a value from somewhere ***/
	for (int pin_idx = 0; pin_idx < nbr_pins; ++pin_idx)
	{
		list_target (
			nav_list, nid, pos_y,
			doc::CnxEnd (doc::CnxEnd::Type_IO, 0, pin_idx),
			ce_excl_list, nbr_pins, entry_list
		);
	}

	// Adds the send/return pins
	for (int pin_idx = 0; pin_idx < Cst::_max_nbr_send; ++pin_idx)
	{
		list_target (
			nav_list, nid, pos_y,
			doc::CnxEnd (doc::CnxEnd::Type_RS, 0, pin_idx),
			ce_excl_list, Cst::_max_nbr_send, entry_list
		);
	}
}



void	SlotRoutingAction::list_target (PageMgrInterface::NavLocList &nav_list, int &nid, int &pos_y, const doc::CnxEnd &cnx_end, const std::set <doc::CnxEnd> &ce_excl_list, int nbr_pins, const std::vector <Tools::NodeEntry> &entry_list)
{
	if (ce_excl_list.find (cnx_end) == ce_excl_list.end ())
	{
		const int      scr_w = _page_size [0];
		const int      h_m   = _fnt_ptr->get_char_h ();

		CnxEndData     c_data;
		c_data._cnx_end  = cnx_end;
		c_data._txt_sptr = TxtSPtr (std::make_shared <NText> (nid));
		c_data._txt_sptr->set_font (*_fnt_ptr);
		c_data._txt_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
		c_data._txt_sptr->set_coord (Vec2d (0, pos_y));
		Tools::print_cnx_name (
			*c_data._txt_sptr, scr_w, entry_list,
			_arg_ptr->_dir, c_data._cnx_end, _indent_0, nbr_pins
		);

		_cnx_end_arr.push_back (c_data);
		_menu_sptr->push_back (c_data._txt_sptr);
		PageMgrInterface::add_nav (nav_list, nid);

		++ nid;
		pos_y += h_m;
	}
}



void	SlotRoutingAction::del_cur_cnx ()
{
	assert (_view_ptr  != nullptr);
	assert (_model_ptr != nullptr);
	assert (_arg_ptr   != nullptr);

	const doc::Preset &  prog = _view_ptr->use_preset_cur ();
	doc::Routing   routing = prog.use_routing (); // Copy

	routing._cnx_audio_set.erase (_arg_ptr->_cnx);

	_model_ptr->set_routing (routing);
}



void	SlotRoutingAction::add_or_replace_cnx (int node_id)
{
	assert (_view_ptr  != nullptr);
	assert (_model_ptr != nullptr);
	assert (_arg_ptr   != nullptr);
	assert (_loc_edit._slot_id >= 0);

	// Target end
	const int      cnx_idx = node_id - Entry_TARGET;
	assert (cnx_idx >= 0);
	assert (cnx_idx < int (_cnx_end_arr.size ()));
	const doc::CnxEnd &  cnx_end_tgt = _cnx_end_arr [cnx_idx]._cnx_end;

	const doc::Preset &  prog = _view_ptr->use_preset_cur ();
	doc::Routing   routing = prog.use_routing (); // Copy

	doc::CnxEnd    cnx_end_org;
	if (_arg_ptr->_ed_type == EdType_CNX)
	{
		// Replaces connection
		routing._cnx_audio_set.erase (_arg_ptr->_cnx);

		const piapi::Dir  dir_opp = piapi::Dir_invert (_arg_ptr->_dir);
		cnx_end_org = _arg_ptr->_cnx.use_end (dir_opp);
	}
	else
	{
		// Adds connection
		cnx_end_org.set (
			doc::CnxEnd::Type_NORMAL, _loc_edit._slot_id, _arg_ptr->_pin_idx
		);
	}

	const doc::Cnx cnx (
		(_arg_ptr->_dir == piapi::Dir_IN) ? cnx_end_tgt : cnx_end_org,
		(_arg_ptr->_dir == piapi::Dir_IN) ? cnx_end_org : cnx_end_tgt
	);
	routing._cnx_audio_set.insert (cnx);

	_model_ptr->set_routing (routing);
}



const char *	SlotRoutingAction::_indent_0 = "  ";



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
