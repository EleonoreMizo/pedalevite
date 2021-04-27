/*****************************************************************************

        ParamEdit.cpp
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
#include "mfx/uitk/pg/ParamEdit.h"
#include "mfx/uitk/pg/Tools.h"
#include "mfx/uitk/NodeEvt.h"
#include "mfx/uitk/PageMgrInterface.h"
#include "mfx/uitk/PageSwitcher.h"
#include "mfx/ui/Font.h"
#include "mfx/Cst.h"
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



ParamEdit::ParamEdit (PageSwitcher &page_switcher, LocEdit &loc_edit)
:	_page_switcher (page_switcher)
,	_loc_edit (loc_edit)
,	_model_ptr (nullptr)
,	_view_ptr (nullptr)
,	_page_ptr (nullptr)
,	_page_size ()
,	_fnt_ptr (nullptr)
,	_name_sptr (       std::make_shared <NText> (Entry_NAME   ))
,	_val_unit_sptr (   std::make_shared <NText> (Entry_VALUNIT))
,	_step_sptr_arr ()
,	_controllers_sptr (std::make_shared <NText> (Entry_CTRL   ))
,	_follow_sptr (     std::make_shared <NText> (Entry_FOLLOW ))
,	_step_index (0)
{
	std::string    ratio;
	for (int k = 0; k < _nbr_steps; ++k)
	{
		ratio += "\xE2\x9A\xAB";   // MEDIUM BLACK CIRCLE U+26AB
		_step_sptr_arr [k] = std::make_shared <NText> (Entry_STEP + k);
		_step_sptr_arr [k]->set_text (" " + ratio + " ");
	}

	_controllers_sptr->set_text ("Controllers\xE2\x80\xA6");

	_name_sptr       ->set_justification (0.5f, 0   , false);
	_val_unit_sptr   ->set_justification (0.5f, 0.5f, false);
	_controllers_sptr->set_justification (0.5f, 0   , false);
	_follow_sptr     ->set_justification (0.5f, 0   , false);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	ParamEdit::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt)
{
	fstb::unused (usr_ptr);
	assert (_loc_edit._slot_id >= 0);
	assert (_loc_edit._pi_type >= 0);
	assert (_loc_edit._param_index >= 0);

	_model_ptr = &model;
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;
	_fnt_ptr   = &fnt._m;

	_name_sptr       ->set_font (*_fnt_ptr);
	_val_unit_sptr   ->set_font (fnt._l);
	_controllers_sptr->set_font (*_fnt_ptr);
	_follow_sptr     ->set_font (*_fnt_ptr);

	const int      scr_w = _page_size [0];
	const int      x_mid =  scr_w >> 1;
	const int      h_m   = _fnt_ptr->get_char_h ();

	_name_sptr       ->set_coord (Vec2d (x_mid, h_m * 0));
	_val_unit_sptr   ->set_coord (Vec2d (x_mid, h_m * 2));
	_controllers_sptr->set_coord (Vec2d (x_mid, h_m * 4));
	_follow_sptr     ->set_coord (Vec2d (x_mid, h_m * 5));

	_page_ptr->push_back (_name_sptr);
	_page_ptr->push_back (_val_unit_sptr);

	std::array <int, _nbr_steps>  width_arr;
	int            total_w = 0;
	for (size_t k = 0; k < _nbr_steps; ++k)
	{
		_step_sptr_arr [k]->set_font (*_fnt_ptr);
		width_arr [k] = _step_sptr_arr [k]->get_bounding_box ().get_size () [0];
		total_w += width_arr [k];
	}
	assert (total_w <= scr_w);
	static_assert (_nbr_steps > 1, "_nbr_steps");
	const int      dist   = (scr_w - total_w) / (_nbr_steps - 1);
	int            x_step = 0;
	for (size_t k = 0; k < _nbr_steps; ++k)
	{
		_step_sptr_arr [k]->set_coord (Vec2d (x_step, h_m * 3));
		_page_ptr->push_back (_step_sptr_arr [k]);
		x_step += dist + width_arr [k];
	}

	_page_ptr->push_back (_controllers_sptr);
	_page_ptr->push_back (_follow_sptr);

	update_display ();
	_page_ptr->jump_to (Entry_STEP);
}



void	ParamEdit::do_disconnect ()
{
	// Nothing
}



MsgHandlerInterface::EvtProp	ParamEdit::do_handle_evt (const NodeEvt &evt)
{
	EvtProp        ret_val = EvtProp_PASS;

	const int      node_id = evt.get_target ();

	if (evt.is_cursor ())
	{
		if (   evt.get_cursor () == NodeEvt::Curs_ENTER
		    && node_id >= Entry_STEP && node_id < Entry_STEP + _nbr_steps)
		{
			_step_index = node_id - Entry_STEP;
		}
	}

	else if (evt.is_button_ex ())
	{
		const Button   but = evt.get_button_ex ();
		switch (but)
		{
		case Button_S:
			ret_val = EvtProp_CATCH;
			if (node_id == Entry_CTRL)
			{
				_page_switcher.call_page (
					PageType_PARAM_CONTROLLERS, nullptr, node_id
				);
			}
			else if (node_id == Entry_FOLLOW)
			{
				/*** To do ***/
				_page_switcher.call_page (PageType_NOT_YET, nullptr, node_id);
			}
			else
			{
				ret_val = EvtProp_PASS;
			}
			break;
		case Button_E:
			_page_switcher.switch_to (pg::PageType_PARAM_LIST, nullptr);
			ret_val = EvtProp_CATCH;
			break;
		case Button_L:
			ret_val = change_param (-1);
			break;
		case Button_R:
			ret_val = change_param (+1);
			break;
		default:
			// Nothing
			break;
		}
	}

	return ret_val;
}



void	ParamEdit::do_activate_preset (int index)
{
	fstb::unused (index);

	_page_switcher.switch_to (PageType_PROG_EDIT, nullptr);
}



void	ParamEdit::do_set_param (int slot_id, int index, float val, PiType type)
{
	fstb::unused (val);

	if (   slot_id == _loc_edit._slot_id
	    && type    == _loc_edit._pi_type
	    && index   == _loc_edit._param_index)
	{
		update_param_txt ();
	}
}



void	ParamEdit::do_remove_plugin (int slot_id)
{
	if (slot_id == _loc_edit._slot_id)
	{
		_page_switcher.switch_to (PageType_PROG_EDIT, nullptr);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	ParamEdit::update_display ()
{
	const doc::Preset &  preset     = _view_ptr->use_preset_cur ();
	const int            slot_id    = _loc_edit._slot_id;
	const doc::Slot &    slot       = preset.use_slot (slot_id);
	const PiType         type       = _loc_edit._pi_type;
	assert (slot._settings_all.find (slot._pi_model) != slot._settings_all.end ());
	const doc::PluginSettings &  settings = slot.use_settings (type);
	const int      index = _loc_edit._param_index;

	// Controllers
	const bool     ctrl_flag = settings.has_ctrl (index);
	_controllers_sptr->set_bold (ctrl_flag, true);

	bool           show_sync_flag = false;

	/*** To do: tempo sync ***/


	_follow_sptr->show (show_sync_flag);
	int            nbr_entries = 1 + ((show_sync_flag) ? 1 : 0);

	PageMgrInterface::NavLocList  nav_list (nbr_entries + _nbr_steps);
	for (int k = 0; k < _nbr_steps; ++k)
	{
		nav_list [k]._node_id = Entry_STEP + k;
	}
	nav_list [_nbr_steps + 0]._node_id = Entry_CTRL;
	if (show_sync_flag)
	{
		nav_list [_nbr_steps + 1]._node_id = Entry_FOLLOW;
	}
	_page_ptr->set_nav_layout (nav_list);

	update_param_txt ();
}



void	ParamEdit::update_param_txt ()
{
	const int      slot_id = _loc_edit._slot_id;
	const PiType   type    = _loc_edit._pi_type;
	const int      index   = _loc_edit._param_index;

	Tools::set_param_text (
		*_model_ptr, *_view_ptr, _page_size [0], index, -1, slot_id, type,
		_name_sptr.get (), *_val_unit_sptr, 0, 0, true
	);
}



MsgHandlerInterface::EvtProp	ParamEdit::change_param (int dir)
{
	const int      slot_id = _loc_edit._slot_id;
	const PiType   type    = _loc_edit._pi_type;
	const int      index   = _loc_edit._param_index;
	const int      step_scale =
		_page_ptr->get_shift (PageMgrInterface::Shift::R) ? 1 : 0;
	const float    step    =
		float (Cst::_step_param / pow (10, _step_index + step_scale));

	return Tools::change_param (
		*_model_ptr, *_view_ptr, slot_id, type,
		index, step, _step_index, dir
	);
}



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
