/*****************************************************************************

        ProgSave.cpp
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
#include "mfx/pi/param/Tools.h"
#include "mfx/piapi/PluginDescInterface.h"
#include "mfx/uitk/pg/ProgSave.h"
#include "mfx/uitk/pg/Tools.h"
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



ProgSave::ProgSave (PageSwitcher &page_switcher)
:	_page_switcher (page_switcher)
,	_model_ptr (nullptr)
,	_view_ptr (nullptr)
,	_page_ptr (nullptr)
,	_page_size ()
,	_fnt_ptr (nullptr)
,	_menu_sptr (std::make_shared <NWindow> (Entry_WINDOW))
,	_bank_sptr (std::make_shared <NText  > (Entry_BANK  ))
,	_prog_list ()
,	_state (State_NORMAL)
,	_save_bank_index (-1)
,	_save_preset_index (-1)
,	_name_param ()
{
	// Nothing
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	ProgSave::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt)
{
	fstb::unused (usr_ptr);

	_model_ptr = &model;
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;
	_fnt_ptr   = &fnt._m;

	const int      cur_preset_index = _view_ptr->get_preset_index ();
	const int      cur_bank_index   = _view_ptr->get_bank_index ();

	if (_state == State_EDIT_NAME)
	{
		_state = State_NORMAL;
		if (_name_param._ok_flag)
		{
			const std::string old_name = _view_ptr->use_preset_cur ()._name;
			_model_ptr->set_preset_name (_name_param._text);
			_model_ptr->store_preset (_save_preset_index, _save_bank_index);
			if (cur_bank_index == _save_bank_index)
			{
				// We need to restore the name because during activate_preset()
				// in edit mode, the current preset is automatically saved into
				// the bank before the new preset activation.
				if (   _view_ptr->is_editing ()
				    && _save_preset_index != cur_preset_index)
				{
					_model_ptr->set_preset_name (old_name);
					// Makes sure the current preset is not overwritten by
					// the activation of the saved preset
					_model_ptr->activate_preset (cur_preset_index);
				}
				_model_ptr->activate_preset (_save_preset_index);
			}

			const int      ret_val = _model_ptr->save_to_disk ();
			if (ret_val != 0)
			{
				/*** To do ***/
				assert (false);
			}

			_page_switcher.switch_to (pg::PageType_PROG_CUR, nullptr);
			return;
		}
	}

	_save_preset_index = cur_preset_index;
	_save_bank_index   = cur_bank_index;

	const int      scr_w = _page_size [0];
	const int      h_m   = _fnt_ptr->get_char_h ();

	_menu_sptr->clear_all_nodes ();
	_menu_sptr->set_size (_page_size, Vec2d ());
	_menu_sptr->set_disp_pos (Vec2d ());
	PageMgrInterface::NavLocList  nav_list (1 + int (_prog_list.size ()));

	_bank_sptr->set_font (fnt._s);
	_bank_sptr->set_coord (Vec2d (0, 0));
	_bank_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
	nav_list [0]._node_id = Entry_BANK;
	_menu_sptr->push_back (_bank_sptr);

	for (int p = 0; p < int (_prog_list.size ()); ++p)
	{
		_prog_list [p] = std::make_shared <NText> (Entry_PROG_LIST + p);
		_prog_list [p]->set_font (*_fnt_ptr);
		_prog_list [p]->set_coord (Vec2d (0, h_m * (p + 1)));
		_prog_list [p]->set_frame (Vec2d (scr_w, 0), Vec2d ());
		nav_list [p + 1]._node_id = Entry_PROG_LIST + p;
		_menu_sptr->push_back (_prog_list [p]);
	}

	_page_ptr->push_back (_menu_sptr);
	_page_ptr->set_nav_layout (nav_list);

	update_display ();
	_page_ptr->jump_to (Entry_PROG_LIST + _save_preset_index);
}



void	ProgSave::do_disconnect ()
{
	// Nothing
}



MsgHandlerInterface::EvtProp	ProgSave::do_handle_evt (const NodeEvt &evt)
{
	EvtProp        ret_val = EvtProp_PASS;

	const int      node_id = evt.get_target ();

	if (evt.is_cursor ())
	{
		if (   evt.get_cursor () == NodeEvt::Curs_ENTER
		    && node_id >= Entry_PROG_LIST
		    && node_id < int (Entry_PROG_LIST + _prog_list.size ()))
		{
			_save_preset_index = node_id - Entry_PROG_LIST;
		}
	}

	else if (evt.is_button_ex ())
	{
		const Button   but = evt.get_button_ex ();
		switch (but)
		{
		case Button_S:
			ret_val = EvtProp_CATCH;
			if (   node_id >= Entry_PROG_LIST
			    && node_id < int (Entry_PROG_LIST + _prog_list.size ()))
			{
				_save_preset_index = node_id - Entry_PROG_LIST;
				const doc::Bank & bank =
					_view_ptr->use_setup ()._bank_arr [_save_bank_index];
				const doc::Preset &  preset_overwritten =
					bank._preset_arr [_save_preset_index];
				char           txt_0 [255+1];
				fstb::snprintf4all (
					txt_0,
					sizeof (txt_0),
					"Save to %02d %s:",
					_save_preset_index,
					preset_overwritten._name.c_str ()
				);
				const doc::Preset &  preset_saved = _view_ptr->use_preset_cur ();
				_name_param._title = txt_0;
				if (preset_saved._name == Cst::_empty_preset_name)
				{
					_name_param._text.clear ();
				}
				else
				{
					_name_param._text  = preset_saved._name;
				}
				_state  = State_EDIT_NAME;
				_page_switcher.call_page (PageType_EDIT_TEXT, &_name_param, node_id);
				ret_val = EvtProp_CATCH;
			}
			else
			{
				ret_val = EvtProp_PASS;
			}
			break;
		case Button_E:
			_page_switcher.switch_to (pg::PageType_PROG_EDIT, nullptr);
			ret_val = EvtProp_CATCH;
			break;
		case Button_L:
			if (node_id == Entry_BANK)
			{
				ret_val = change_bank (-1);
			}
			break;
		case Button_R:
			if (node_id == Entry_BANK)
			{
				ret_val = change_bank (+1);
			}
			break;
		default:
			// Nothing
			break;
		}
	}

	return ret_val;
}



void	ProgSave::do_set_preset_name (std::string name)
{
	fstb::unused (name);

	update_display ();
}



void	ProgSave::do_set_preset (int bank_index, int preset_index, const doc::Preset &preset)
{
	fstb::unused (preset_index, preset);

	if (bank_index == _save_bank_index)
	{
		update_display ();
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	ProgSave::update_display ()
{
	char           txt_0 [255+1];

	const doc::Bank & bank =
		_view_ptr->use_setup ()._bank_arr [_save_bank_index];

	fstb::snprintf4all (
		txt_0, sizeof (txt_0),
		"Bank %02d %s",
		_save_bank_index,
		bank._name.c_str ()
	);
	_bank_sptr->set_text (txt_0);

	const bool     this_bank_flag =
		(_save_bank_index == _view_ptr->get_bank_index ());
	const int      cur_preset_index = _view_ptr->get_preset_index ();
	for (int p = 0; p < int (_prog_list.size ()); ++p)
	{
		const doc::Preset &  preset = bank._preset_arr [p];
		const bool     this_flag = (this_bank_flag && p == cur_preset_index);
		_prog_list [p]->set_bold (this_flag, false);

		fstb::snprintf4all (
			txt_0, sizeof (txt_0),
			"%02d %s",
			p,
			preset._name.c_str ()
		);
		_prog_list [p]->set_text (txt_0);
	}

	_menu_sptr->invalidate_all ();
}



MsgHandlerInterface::EvtProp	ProgSave::change_bank (int dir)
{
	_save_bank_index += dir;
	_save_bank_index += Cst::_nbr_banks;
	_save_bank_index %= Cst::_nbr_banks;
	update_display ();

	return EvtProp_CATCH;
}



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
