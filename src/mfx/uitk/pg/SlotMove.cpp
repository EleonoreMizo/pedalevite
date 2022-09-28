/*****************************************************************************

        SlotMove.cpp
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
#include "mfx/uitk/pg/SlotMove.h"
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



SlotMove::SlotMove (PageSwitcher &page_switcher, LocEdit &loc_edit)
:	_page_switcher (page_switcher)
,	_loc_edit (loc_edit)
,	_model_ptr (nullptr)
,	_view_ptr (nullptr)
,	_page_ptr (nullptr)
,	_page_size ()
,	_fnt_ptr (nullptr)
,	_menu_sptr (std::make_shared <NWindow> (1000))
,	_slot_list ()
,	_moving_flag (false)
{
	// Nothing
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	SlotMove::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt)
{
	fstb::unused (usr_ptr);

	_model_ptr = &model;
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;
	_fnt_ptr   = &fnt._m;

	_moving_flag = false;

	_menu_sptr->set_size (_page_size, Vec2d ());
	_menu_sptr->set_disp_pos (Vec2d ());

	_page_ptr->push_back (_menu_sptr);

	update_display ();
}



void	SlotMove::do_disconnect ()
{
	if (_model_ptr != nullptr)
	{
		_model_ptr->reset_all_overridden_param_ctrl ();
	}
}



MsgHandlerInterface::EvtProp	SlotMove::do_handle_evt (const NodeEvt &evt)
{
	EvtProp        ret_val = EvtProp_PASS;

	const int      node_id = evt.get_target ();

	if (evt.is_cursor ())
	{
		if (   ! _moving_flag
		    && _loc_edit._slot_id >= 0
		    && evt.get_cursor () == NodeEvt::Curs_ENTER
		    && node_id >= 0 && node_id < int (_slot_list.size ()))
		{
			move_slot (node_id);
		}
	}

	else if (evt.is_button_ex ())
	{
		const Button   but = evt.get_button_ex ();
		switch (but)
		{
		case Button_S:
		case Button_E:
			_page_switcher.switch_to (PageType_SLOT_MENU, nullptr);
			ret_val = EvtProp_CATCH;
			break;
		default:
			// Nothing
			break;
		}
	}

	return ret_val;
}



void	SlotMove::do_activate_prog (int /*index*/)
{
	_page_switcher.switch_to (PageType_PROG_EDIT, nullptr);
}



void	SlotMove::do_remove_slot (int slot_id)
{
	if (slot_id == _loc_edit._slot_id)
	{
		_page_switcher.switch_to (PageType_PROG_EDIT, nullptr);
	}
}



void	SlotMove::do_set_routing (const doc::Routing &/*routing*/)
{
	update_display ();
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	SlotMove::update_display ()
{
	const int      h_m   = _fnt_ptr->get_char_h ();
	const int      scr_w = _page_size [0];

	const doc::Program & prog = _view_ptr->use_prog_cur ();
	std::vector <Tools::NodeEntry>   entry_list;
	const int      audio_size =
		Tools::extract_slot_list (entry_list, prog, *_model_ptr);
	assert (audio_size <= int (entry_list.size ()));

	PageMgrInterface::NavLocList nav_list;
	_menu_sptr->clear_all_nodes ();
	_slot_list.clear ();

	int            pos_sel = -1;
	for (int pos = 0; pos < audio_size; ++pos)
	{
		const Tools::NodeEntry &   entry = entry_list [pos];
		const int      slot_id = entry._slot_id;
		if (slot_id == _loc_edit._slot_id)
		{
			pos_sel = pos;
		}

		const std::string multilabel (Tools::build_slot_name_with_index (entry));
		TxtSPtr        entry_sptr { std::make_shared <NText> (pos) };
		entry_sptr->set_coord (Vec2d (0, h_m * pos));
		entry_sptr->set_font (*_fnt_ptr);
		entry_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
		std::string    txt = pi::param::Tools::print_name_bestfit (
			scr_w, multilabel.c_str (),
			*entry_sptr, &NText::get_char_width
		);
		entry_sptr->set_text (txt);

		_slot_list.push_back (entry_sptr);
		_menu_sptr->push_back (entry_sptr);
	}

	if (pos_sel >= 0)
	{
		for (int pos = 0; pos < audio_size; ++pos)
		{
			PageMgrInterface::add_nav (nav_list, pos);
		}
	}
	_page_ptr->set_nav_layout (nav_list);

	if (pos_sel >= 0)
	{
		_page_ptr->jump_to (pos_sel);
	}
}



MsgHandlerInterface::EvtProp	SlotMove::move_slot (int pos)
{
	assert (! _moving_flag);

	EvtProp        ret_val = EvtProp_PASS;

	const int      pos_old = conv_loc_edit_to_linear_pos ();
	if (pos_old >= 0 && pos_old != pos)
	{
		_moving_flag = true;

		const doc::Program & prog = _view_ptr->use_prog_cur ();
		doc::Routing   routing    = prog.use_routing (); // Copy
		const ToolsRouting::NodeMap & graph      = _view_ptr->use_graph ();
		const std::vector <int> &  slot_list_aud =
			_view_ptr->use_slot_list_aud ();
		ToolsRouting::move_slot (
			routing._cnx_audio_set,
			_loc_edit._slot_id,
			pos,
			slot_list_aud,
			graph
		);
		_model_ptr->set_routing (routing);

		_moving_flag = false;

		ret_val = EvtProp_CATCH;
	}

	return ret_val;
}



// Returns -1 if not in the graph
int	SlotMove::conv_loc_edit_to_linear_pos () const
{
	int            pos = -1;

	if (_loc_edit._slot_id >= 0)
	{
		pos = Tools::find_linear_index_audio_graph (
			*_view_ptr, _loc_edit._slot_id
		);
	}

	return pos;
}



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
