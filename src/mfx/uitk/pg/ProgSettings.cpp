/*****************************************************************************

        ProgSettings.cpp
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
#include "mfx/doc/Preset.h"
#include "mfx/uitk/pg/ProgSettings.h"
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



ProgSettings::ProgSettings (PageSwitcher &page_switcher, PedalEditContext &pedal_ctx)
:	_page_switcher (page_switcher)
,	_pedal_ctx (pedal_ctx)
,	_model_ptr (nullptr)
,	_view_ptr (nullptr)
,	_page_ptr (nullptr)
,	_page_size ()
,	_fnt_ptr (nullptr)
,	_layout_sptr (std::make_shared <NText> (Entry_LAYOUT))
,	_switch_sptr (std::make_shared <NText> (Entry_SWITCH))
,	_state (State_NONE)
,	_arg_fx_state_modes ()
{
	_layout_sptr->set_text ("Pedal layout\xE2\x80\xA6");

	_arg_fx_state_modes._title = "Set FX states";
	_arg_fx_state_modes._choice_arr.assign ({
		"All \xE2\x86\x92 Keep",   // RIGHTWARDS ARROW U+2192
		"All \xE2\x86\x92 Fresh",
		"Cancel"
	});
	_arg_fx_state_modes._ok_flag = false;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	ProgSettings::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt)
{
	fstb::unused (usr_ptr);

	_model_ptr = &model;
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;
	_fnt_ptr   = &fnt._m;

	switch (_state)
	{
	case State_NONE:
		// Nothing
		break;
	case State_FXSTATE:
		change_all_plugin_state_modes ();
		break;
	default:
		assert (false);
		break;
	}
	_state = State_NONE;

	_layout_sptr->set_font (*_fnt_ptr);
	_switch_sptr->set_font (*_fnt_ptr);

	const int      scr_w = _page_size [0];
	const int      h_m   = _fnt_ptr->get_char_h ();

	_layout_sptr->set_coord (Vec2d (0, 0 * h_m));
	_switch_sptr->set_coord (Vec2d (0, 1 * h_m));

	_layout_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_switch_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());

	_page_ptr->push_back (_layout_sptr);
	_page_ptr->push_back (_switch_sptr);

	PageMgrInterface::NavLocList  nav_list;
	PageMgrInterface::add_nav (nav_list, Entry_LAYOUT);
	PageMgrInterface::add_nav (nav_list, Entry_SWITCH);

	_page_ptr->set_nav_layout (nav_list);

	update_display ();
}



void	ProgSettings::do_disconnect ()
{
	// Nothing
}



MsgHandlerInterface::EvtProp	ProgSettings::do_handle_evt (const NodeEvt &evt)
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
			case Entry_LAYOUT:
				_pedal_ctx._type     = PedalEditContext::Type_PRESET;
				_pedal_ctx._ret_page = pg::PageType_PROG_SETTINGS;
				_page_switcher.switch_to (pg::PageType_PEDALBOARD_CONFIG, nullptr);
				break;
			case Entry_SWITCH:
				_arg_fx_state_modes._selection = int (FxState_CANCEL);
				_state = State_FXSTATE;
				_page_switcher.call_page (
					PageType_QUESTION, &_arg_fx_state_modes, node_id
				);
				break;
			default:
				ret_val = EvtProp_PASS;
				break;
			}
			break;
		case Button_E:
			_page_switcher.switch_to (pg::PageType_PROG_EDIT, nullptr);
			ret_val = EvtProp_CATCH;
			break;
		case Button_L:
			if (node_id == Entry_SWITCH)
			{
				ret_val = change_switch (-1);
			}
			break;
		case Button_R:
			if (node_id == Entry_SWITCH)
			{
				ret_val = change_switch (+1);
			}
			break;
		default:
			// Nothing
			break;
		}
	}

	return ret_val;
}



void	ProgSettings::do_set_prog_switch_mode (doc::ProgSwitchMode mode)
{
	fstb::unused (mode);

	update_display ();
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	ProgSettings::update_display ()
{
	std::string    txt = "Switch: ";
	const doc::Preset &  prog = _view_ptr->use_preset_cur ();
	switch (prog._prog_switch_mode)
	{
	case doc::ProgSwitchMode::DIRECT:
		txt += "Direct";
		break;
	case doc::ProgSwitchMode::FADE_OUT_IN:
		txt += "Fade out+in";
		break;
	default:
		assert (false);
		break;
	}
	_switch_sptr->set_text (txt);
}



MsgHandlerInterface::EvtProp	ProgSettings::change_switch (int dir)
{
	assert (dir == -1 || dir == +1);

	const doc::Preset &  prog = _view_ptr->use_preset_cur ();
	int            mode = int (prog._prog_switch_mode);

	mode += dir;
	mode = fstb::limit (mode, 0, int (doc::ProgSwitchMode::NBR_ELT) - 1);

	_model_ptr->set_prog_switch_mode (doc::ProgSwitchMode (mode));

	return EvtProp_CATCH;
}



void	ProgSettings::change_all_plugin_state_modes ()
{
	if (_arg_fx_state_modes._ok_flag)
	{
		switch (_arg_fx_state_modes._selection)
		{
		case FxState_KEEP:
			change_all_plugin_state_modes (false);
			break;
		case FxState_FRESH:
			change_all_plugin_state_modes (true);
			break;
		case FxState_CANCEL:
			// Nothing
			break;
		default:
			assert (false);
			break;
		}
	}
	_arg_fx_state_modes._ok_flag = false;
}



void	ProgSettings::change_all_plugin_state_modes (bool force_reset_flag)
{
	const doc::Preset &  prog = _view_ptr->use_preset_cur ();
	for (auto it = prog._slot_map.begin (); it != prog._slot_map.end (); ++it)
	{
		if (! prog.is_slot_empty (it))
		{
			const int      slot_id = it->first;
			_model_ptr->set_plugin_reset (slot_id, force_reset_flag);
		}
	}
}



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
