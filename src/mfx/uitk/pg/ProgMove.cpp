/*****************************************************************************

        ProgMove.cpp
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

#include "fstb/fnc.h"
#include "mfx/uitk/pg/ProgMove.h"
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



ProgMove::ProgMove (PageSwitcher &page_switcher)
:	_page_switcher (page_switcher)
,	_model_ptr (0)
,	_view_ptr (0)
,	_page_ptr (0)
,	_page_size ()
,	_fnt_ptr (0)
,	_menu_sptr (new NWindow (Entry_WINDOW))
,	_title_sptr (new NText ( Entry_TITLE))
,	_prog_list ()
,	_state (State_SEL)
,	_moving_flag (false)
,	_moved_flag (false)
,	_prog_index (-1)
{
	_title_sptr->set_justification (0.5f, 0.0f, false);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	ProgMove::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt)
{
	_model_ptr = &model;
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;
	_fnt_ptr   = &fnt._m;

	_state       = State_SEL;
	_moving_flag = false;
	_moved_flag  = false;
	_prog_index  = -1;

	_title_sptr->set_font (*_fnt_ptr);

	const int      h_m   = _fnt_ptr->get_char_h ();
	const int      scr_w = _page_size [0];
	const int      x_mid = scr_w >> 1;

	_title_sptr->set_coord (Vec2d (x_mid, 0 * h_m));

	const Vec2d   menu_pos (0, h_m);
	_menu_sptr->set_coord (menu_pos);
	_menu_sptr->set_size (_page_size - menu_pos, Vec2d ());
	_menu_sptr->set_disp_pos (Vec2d ());

	_page_ptr->push_back (_title_sptr);
	_page_ptr->push_back (_menu_sptr);

	update_display ();

	const int      node_id = _view_ptr->get_preset_index ();
	_page_ptr->jump_to (node_id);
}



void	ProgMove::do_disconnect ()
{
	save_if_req ();
}



MsgHandlerInterface::EvtProp	ProgMove::do_handle_evt (const NodeEvt &evt)
{
	EvtProp        ret_val = EvtProp_PASS;

	const int      node_id = evt.get_target ();

	if (evt.is_cursor ())
	{
		if (   _state == State_MOV
		    && ! _moving_flag
		    && evt.get_cursor () == NodeEvt::Curs_ENTER
		    && node_id >= 0 && node_id < Cst::_nbr_presets_per_bank)
		{
			move_prog (node_id);
		}
	}

	else if (evt.is_button_ex ())
	{
		const Button   but = evt.get_button_ex ();
		switch (but)
		{
		case Button_S:
			if (node_id >= 0 && node_id < Cst::_nbr_presets_per_bank)
			{
				if (_state == State_MOV)
				{
					save_if_req ();
					_state      = State_SEL;
					update_display ();
				}
				else
				{
					_prog_index = node_id;
					_state      = State_MOV;
					update_display ();
				}
				ret_val = EvtProp_CATCH;
			}
			break;
		case Button_E:
			_page_switcher.return_page ();
			ret_val = EvtProp_CATCH;
			break;
		default:
			// Nothing
			break;
		}
	}

	return ret_val;
}



void	ProgMove::do_select_bank (int index)
{
	_page_switcher.return_page ();
}



void	ProgMove::do_set_preset_name (std::string name)
{
	update_display ();
}



void	ProgMove::do_set_preset (int bank_index, int preset_index, const doc::Preset &preset)
{
	if (bank_index == _view_ptr->get_bank_index ())
	{
		update_display ();
	}
}



void	ProgMove::do_store_preset (int preset_index, int bank_index)
{
	if (bank_index < 0 || bank_index == _view_ptr->get_bank_index ())
	{
		update_display ();
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	ProgMove::update_display ()
{
	assert (_fnt_ptr != 0);

	const int      scr_w = _page_size [0];

	if (_state == State_MOV)
	{
		_title_sptr->set_text ("Select a destination");
	}
	else
	{
		_title_sptr->set_text ("Select a prog to move");
	}

	_prog_list.clear ();
	_menu_sptr->clear_all_nodes ();
	PageMgrInterface::NavLocList  nav_list;

	Tools::create_prog_list (
		_prog_list, *_menu_sptr, nav_list, *_view_ptr, *_fnt_ptr,
		0, scr_w
	);

	_page_ptr->set_nav_layout (nav_list);

	_menu_sptr->invalidate_all ();
}



MsgHandlerInterface::EvtProp	ProgMove::move_prog (int index_new)
{
	if (_prog_index >= 0 && index_new != _prog_index)
	{
		_moving_flag = true;

		const doc::Setup &   setup      = _view_ptr->use_setup ();
		const int            bank_index = _view_ptr->get_bank_index ();
		const doc::Bank &    bank       = setup._bank_arr [bank_index];

		// Make a copy
		const doc::Preset prog_old = bank._preset_arr [_prog_index];
		const doc::Preset prog_new = bank._preset_arr [index_new  ];

		_model_ptr->set_preset (bank_index, index_new  , prog_old);
		_model_ptr->set_preset (bank_index, _prog_index, prog_new);

		_moving_flag = false;
		_moved_flag  = true;
	}

	_prog_index = index_new;

	return EvtProp_CATCH;
}



void	ProgMove::save_if_req ()
{
	if (_model_ptr != 0 && _moved_flag)
	{
		const int      ret_val = _model_ptr->save_to_disk ();
		if (ret_val != 0)
		{
			/*** To do ***/
			assert (false);
		}

		_moved_flag = false;
	}
}



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
