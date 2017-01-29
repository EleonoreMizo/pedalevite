/*****************************************************************************

        PedalEditAction.cpp
        Author: Laurent de Soras, 2017

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
#include "mfx/doc/ActionBank.h"
#include "mfx/doc/ActionParam.h"
#include "mfx/doc/ActionPreset.h"
#include "mfx/doc/ActionSettings.h"
#include "mfx/doc/ActionTempo.h"
#include "mfx/doc/ActionToggleFx.h"
#include "mfx/doc/ActionToggleTuner.h"
#include "mfx/piapi/ParamDescInterface.h"
#include "mfx/piapi/PluginDescInterface.h"
#include "mfx/pi/dwm/Param.h"
#include "mfx/pi/param/Tools.h"
#include "mfx/uitk/pg/PedalEditAction.h"
#include "mfx/uitk/pg/Tools.h"
#include "mfx/uitk/NodeEvt.h"
#include "mfx/uitk/PageMgrInterface.h"
#include "mfx/uitk/PageSwitcher.h"
#include "mfx/ui/Font.h"
#include "mfx/Model.h"
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



PedalEditAction::PedalEditAction (PageSwitcher &page_switcher, PedalEditContext &ctx)
:	_page_switcher (page_switcher)
,	_ctx (ctx)
,	_model_ptr (0)
,	_view_ptr (0)
,	_page_ptr (0)
,	_page_size ()
,	_fnt_ptr (0)
,	_type_sptr (    new NText (Entry_TYPE))
,	_index_sptr (   new NText (Entry_INDEX))
,	_value_sptr (   new NText (Entry_VALUE))
,	_label_sptr (   new NText (Entry_LABEL))
,	_state (State_NORMAL)
,	_arg_edit_fxid ()
{
	// Nothing
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	PedalEditAction::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt)
{
	_model_ptr = &model;
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;
	_fnt_ptr   = &fnt._m;
	assert (_ctx._pedal >= 0);
	assert (_ctx._pedal < Cst::_nbr_pedals);
	assert (_ctx._trigger >= 0);
	assert (_ctx._trigger < doc::ActionTrigger_NBR_ELT);
	assert (_ctx._step_index >= 0);
	assert (_ctx._step_index < int (_ctx._content._action_arr [_ctx._trigger]._cycle.size ()));

	check_ctx ();

	doc::PedalActionCycle & cycle =
		_ctx._content._action_arr [_ctx._trigger];
	doc::PedalActionCycle::ActionArray &   step =
		cycle._cycle [_ctx._step_index];
	doc::PedalActionSingleInterface &   action =
		*(step [_ctx._action_index]);
	const doc::ActionType   action_type = action.get_type ();

	if (_state == State_EDIT_FXID)
	{
		_state = State_NORMAL;

		if (_arg_edit_fxid._ok_flag)
		{
			if (action_type == doc::ActionType_PARAM)
			{
				doc::ActionParam &   action_real = 
					dynamic_cast <doc::ActionParam &> (action);
				action_real._fx_id = _arg_edit_fxid._fx_id;
			}
			else if (action_type == doc::ActionType_SETTINGS)
			{
				doc::ActionSettings& action_real = 
					dynamic_cast <doc::ActionSettings &> (action);
				action_real._fx_id = _arg_edit_fxid._fx_id;
			}

			update_model ();
		}
	}

	const int      h_m   = _fnt_ptr->get_char_h ();
	const int      scr_w = _page_size [0];

	_type_sptr    ->set_font (fnt._m);
	_index_sptr   ->set_font (fnt._m);
	_value_sptr   ->set_font (fnt._m);
	_label_sptr   ->set_font (fnt._m);

	_type_sptr    ->set_coord (Vec2d (0, 0 * h_m));

	_type_sptr    ->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_index_sptr   ->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_value_sptr   ->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_label_sptr   ->set_frame (Vec2d (scr_w, 0), Vec2d ());

	update_display ();
}



void	PedalEditAction::do_disconnect ()
{
	// Nothing
}



MsgHandlerInterface::EvtProp	PedalEditAction::do_handle_evt (const NodeEvt &evt)
{
	EvtProp        ret_val = EvtProp_PASS;

	const int      node_id = evt.get_target ();

	if (evt.is_button_ex ())
	{
		const Button   but = evt.get_button_ex ();
		switch (but)
		{
		case Button_S:
			ret_val = change_value (node_id, 0);
			break;
		case Button_E:
			_page_switcher.return_page ();
			ret_val = EvtProp_CATCH;
			break;
		case Button_L:
			ret_val = change_value (node_id, -1);
			break;
		case Button_R:
			ret_val = change_value (node_id, +1);
			break;
		default:
			// Nothing
			break;
		}
	}

	return ret_val;
}



void	PedalEditAction::do_set_pedalboard_layout (const doc::PedalboardLayout &layout)
{
	check_ctx ();
	update_display ();
}



void	PedalEditAction::do_set_pedal (const PedalLoc &loc, const doc::PedalActionGroup &content)
{
	check_ctx ();
	update_display ();
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



const doc::PedalboardLayout &	PedalEditAction::use_layout () const
{
	return _ctx.use_layout (*_view_ptr);
}



void	PedalEditAction::check_ctx ()
{
	assert (_ctx._pedal >= 0);
	assert (_ctx._pedal < Cst::_nbr_pedals);
	assert (_ctx._trigger >= 0);
	assert (_ctx._trigger < doc::ActionTrigger_NBR_ELT);

	const doc::PedalActionCycle & cycle =
		_ctx._content._action_arr [_ctx._trigger];
	const int      nbr_steps   = int (cycle._cycle.size ());
	assert (nbr_steps > 0);
	_ctx._step_index   = fstb::limit (_ctx._step_index, 0, nbr_steps - 1);
	const doc::PedalActionCycle::ActionArray &   step =
		cycle._cycle [_ctx._step_index];
	const int      nbr_actions = int (step.size ());
	assert (nbr_actions > 0);
	_ctx._action_index = fstb::limit (_ctx._action_index, 0, nbr_actions - 1);
}



void	PedalEditAction::update_model ()
{
	_model_ptr->set_pedal (_ctx.conv_to_loc (*_view_ptr), _ctx._content);
}



void	PedalEditAction::update_display ()
{
	_page_ptr->clear_all_nodes ();
	_page_ptr->push_back (_type_sptr);

	PageMgrInterface::NavLocList  nav_list;
	PageMgrInterface::add_nav (nav_list, Entry_TYPE);

	hide_all ();

	const doc::PedalActionCycle & cycle =
		_ctx._content._action_arr [_ctx._trigger];
	const doc::PedalActionCycle::ActionArray &   step =
		cycle._cycle [_ctx._step_index];
	const doc::PedalActionSingleInterface &   action =
		*(step [_ctx._action_index]);

	const doc::ActionType   action_type = action.get_type ();
	switch (action_type)
	{
	case doc::ActionType_BANK:
		display_bank (nav_list, dynamic_cast <const doc::ActionBank &> (action));
		break;
	case doc::ActionType_PRESET:
		display_preset (nav_list, dynamic_cast <const doc::ActionPreset &> (action));
		break;
	case doc::ActionType_TOGGLE_TUNER:
		display_tuner (nav_list);
		break;
	case doc::ActionType_TOGGLE_FX:
		display_fx (nav_list, dynamic_cast <const doc::ActionToggleFx &> (action));
		break;
	case doc::ActionType_LOOP_REC:
		display_loop_rec (nav_list);
		break;
	case doc::ActionType_LOOP_PLAY_STOP:
		display_loop_ps (nav_list);
		break;
	case doc::ActionType_LOOP_UNDO:
		display_loop_undo (nav_list);
		break;
	case doc::ActionType_PARAM:
		display_param (nav_list, dynamic_cast <const doc::ActionParam &> (action));
		break;
	case doc::ActionType_TEMPO:
		display_tempo (nav_list, dynamic_cast <const doc::ActionTempo &> (action));
		break;
	case doc::ActionType_SETTINGS:
		display_settings (nav_list, dynamic_cast <const doc::ActionSettings &> (action));
		break;
	case doc::ActionType_EVENT:
		display_event (nav_list);
		break;
	default:
		assert (false);
		break;
	}

	_page_ptr->invalidate (Rect (Vec2d (), _page_size));

	_page_ptr->set_nav_layout (nav_list);
}



void	PedalEditAction::hide_all ()
{
	_index_sptr   ->show (false);
	_value_sptr   ->show (false);
	_label_sptr   ->show (false);
}



void	PedalEditAction::display_bank (PageMgrInterface::NavLocList &nav_list, const doc::ActionBank &action)
{
	// Action type
	_type_sptr->set_text ("Bank");

	// Absolute or relative index
	if (action._relative_flag)
	{
		_index_sptr->set_text (action._val < 0 ? "Previous" : "Next");
	}
	else
	{
		char           txt_0 [127+1];
		fstb::snprintf4all (txt_0, sizeof (txt_0), "Number: %02d", action._val);
		_index_sptr->set_text (txt_0);
	}

	const int      h_m   = _fnt_ptr->get_char_h ();
	_index_sptr->set_coord (Vec2d (0, 2 * h_m));
	_index_sptr->show (true);
	_page_ptr->push_back (_index_sptr);
	PageMgrInterface::add_nav (nav_list, Entry_INDEX);

	// Bank name if the value is absolute
	if (! action._relative_flag)
	{
		const doc::Setup &   setup = _view_ptr->use_setup ();
		std::string          name       = "Name  : ";
		name += setup._bank_arr [action._val]._name;
		_value_sptr->set_text (name);
		_value_sptr->set_coord (Vec2d (0, 3 * h_m));
		_value_sptr->show (true);
		_page_ptr->push_back (_value_sptr);
	}
}



void	PedalEditAction::display_preset (PageMgrInterface::NavLocList &nav_list, const doc::ActionPreset &action)
{
	// Action type
	_type_sptr->set_text ("Program");

	// Absolute or relative index
	if (action._relative_flag)
	{
		_index_sptr->set_text (action._val < 0 ? "Previous" : "Next");
	}
	else
	{
		char           txt_0 [127+1];
		fstb::snprintf4all (txt_0, sizeof (txt_0), "Number: %02d", action._val);
		_index_sptr->set_text (txt_0);
	}

	const int      h_m   = _fnt_ptr->get_char_h ();
	_index_sptr->set_coord (Vec2d (0, 2 * h_m));
	_index_sptr->show (true);
	_page_ptr->push_back (_index_sptr);
	PageMgrInterface::add_nav (nav_list, Entry_INDEX);

	// Preset name if the value is absolute
	if (! action._relative_flag)
	{
		const doc::Setup &   setup      = _view_ptr->use_setup ();
		const int            bank_index = _view_ptr->get_bank_index ();
		std::string          name       = "Name  : ";
		name += setup._bank_arr [bank_index]._preset_arr [action._val]._name;
		_value_sptr->set_text (name);
		_value_sptr->set_coord (Vec2d (0, 3 * h_m));
		_value_sptr->show (true);
		_page_ptr->push_back (_value_sptr);
	}
}



void	PedalEditAction::display_tuner (PageMgrInterface::NavLocList &nav_list)
{
	// Action type
	_type_sptr->set_text ("Tuner toggle");
}



void	PedalEditAction::display_fx (PageMgrInterface::NavLocList &nav_list, const doc::ActionToggleFx &action)
{
	// Action type
	_type_sptr->set_text ("FX toggle");


	/*** To do ***/

}



void	PedalEditAction::display_loop_rec (PageMgrInterface::NavLocList &nav_list)
{
	// Action type
	_type_sptr->set_text ("Loop record");

	/*** To do ***/

}



void	PedalEditAction::display_loop_ps (PageMgrInterface::NavLocList &nav_list)
{
	// Action type
	_type_sptr->set_text ("Loop play/stop");

	/*** To do ***/

}



void	PedalEditAction::display_loop_undo (PageMgrInterface::NavLocList &nav_list)
{
	// Action type
	_type_sptr->set_text ("Loop undo");

	/*** To do ***/

}



void	PedalEditAction::display_param (PageMgrInterface::NavLocList &nav_list, const doc::ActionParam &action)
{
	// Action type
	_type_sptr->set_text ("Parameter");

	// Collects info
	std::string    model_name;
	std::string    param_name;
	Tools::print_param_action (
		model_name, param_name, action, *_model_ptr, *_view_ptr
	);

	const int      h_m   = _fnt_ptr->get_char_h ();
	const int      scr_w = _page_size [0];

	// Plug-in model
	const std::string title =
		  (action._fx_id._location_type == doc::FxId::LocType_LABEL)
		? "Label: "
		: "Type : ";
	model_name = pi::param::Tools::join_strings_multi (
		model_name.c_str (), '\n', title, ""
	);
	model_name = pi::param::Tools::print_name_bestfit (
		scr_w,
		model_name.c_str (),
		*_label_sptr,
		&NText::get_char_width
	);
	_label_sptr->set_text (model_name);
	_label_sptr->set_coord (Vec2d (0, 2 * h_m));
	_label_sptr->show (true);
	_page_ptr->push_back (_label_sptr);
	PageMgrInterface::add_nav (nav_list, Entry_LABEL);

	// Parameter name
	param_name = pi::param::Tools::join_strings_multi (
		param_name.c_str (), '\n', "Param: ", ""
	);
	param_name = pi::param::Tools::print_name_bestfit (
		scr_w,
		param_name.c_str (),
		*_index_sptr,
		&NText::get_char_width
	);
	_index_sptr->set_text (param_name);
	_index_sptr->set_coord (Vec2d (0, 3 * h_m));
	_index_sptr->show (true);
	_page_ptr->push_back (_index_sptr);
	PageMgrInterface::add_nav (nav_list, Entry_INDEX);

	// Parameter value
	bool           val_flag = false;
	std::string    val_s;

	std::string   pi_id =
		  (action._fx_id._type == PiType_MIX)
		? Cst::_plugin_mix
		: Tools::find_fx_type (action._fx_id, *_view_ptr);
	if (! pi_id.empty ())
	{
		const piapi::PluginDescInterface *   pi_desc_ptr =
			&_model_ptr->get_model_desc (pi_id);
		const int      nbr_param =
			pi_desc_ptr->get_nbr_param (piapi::ParamCateg_GLOBAL);
		if (action._index < nbr_param)
		{
			const piapi::ParamDescInterface & param_desc =
				pi_desc_ptr->get_param_info (piapi::ParamCateg_GLOBAL, action._index);
			const double   val_nat = param_desc.conv_nrm_to_nat (action._val);

			const std::string  unit_s = param_desc.get_unit (999);
			val_s = param_desc.conv_nat_to_str (val_nat, 0);
			val_s = pi::param::Tools::join_strings_multi (
				val_s.c_str (), '\n', "Value: ", " " + unit_s
			);
			size_t         pos_utf8;
			size_t         len_utf8;
			size_t         len_pix;
			pi::param::Tools::cut_str_bestfit (
				pos_utf8, len_utf8, len_pix,
				scr_w, val_s.c_str (), '\n',
				*_value_sptr, &NText::get_char_width
			);
			val_s = val_s.substr (pos_utf8, len_utf8);
			val_flag = true;
		}
	}

	if (! val_flag)
	{
		char           txt_0 [127+1];
		fstb::snprintf4all (
			txt_0, sizeof (txt_0), "Value: %5.1f %%", action._val * 100
		);
		val_s = txt_0;
	}

	_value_sptr->set_text (val_s);
	_value_sptr->set_coord (Vec2d (0, 4 * h_m));
	_value_sptr->show (true);
	_page_ptr->push_back (_value_sptr);
	PageMgrInterface::add_nav (nav_list, Entry_VALUE);
}



void	PedalEditAction::display_tempo (PageMgrInterface::NavLocList &nav_list, const doc::ActionTempo &action)
{
	// Action type
	_type_sptr->set_text ("Tempo tap");
}



void	PedalEditAction::display_settings (PageMgrInterface::NavLocList &nav_list, const doc::ActionSettings &action)
{
	// Action type
	_type_sptr->set_text ("Preset");

	// FX identifier
	const std::string model_id = print_fx_id (action._fx_id);

	const int      h_m   = _fnt_ptr->get_char_h ();
	_label_sptr->set_coord (Vec2d (0, 2 * h_m));
	_label_sptr->show (true);
	_page_ptr->push_back (_label_sptr);
	PageMgrInterface::add_nav (nav_list, Entry_LABEL);

	// Absolute or relative index
	if (action._relative_flag)
	{
		_index_sptr->set_text (action._val < 0 ? "Previous" : "Next");
	}
	else
	{
		char           txt_0 [127+1];
		fstb::snprintf4all (txt_0, sizeof (txt_0), "Number: %02d", action._val);
		_index_sptr->set_text (txt_0);
	}

	_index_sptr->set_coord (Vec2d (0, 3 * h_m));
	_index_sptr->show (true);
	_page_ptr->push_back (_index_sptr);
	PageMgrInterface::add_nav (nav_list, Entry_INDEX);

	// Settings name if the value is absolute
	if (! action._relative_flag && ! model_id.empty ())
	{
		const doc::Setup &   setup = _view_ptr->use_setup ();
		const auto     it = setup._map_plugin_settings.find (model_id);
		if (it != setup._map_plugin_settings.end ())
		{
			const doc::CatalogPluginSettings &  catalog = it->second;
			const int      nbr_settings = int (catalog._cell_arr.size ());
			if (action._val >= 0 && action._val < nbr_settings)
			{
				const doc::CatalogPluginSettings::CellSPtr & cell_sptr =
					catalog._cell_arr [action._val];
				if (cell_sptr.get () != 0)
				{
					std::string    name = "Name  : ";
					name += cell_sptr->_name;
					_value_sptr->set_text (name);
					_value_sptr->set_coord (Vec2d (0, 4 * h_m));
					_value_sptr->show (true);
					_page_ptr->push_back (_value_sptr);
				}
			}
		}
	}
}



void	PedalEditAction::display_event (PageMgrInterface::NavLocList &nav_list)
{
	// Action type
	_type_sptr->set_text ("Event");

	/*** To do ***/

}



// Prints to _label_sptr
// Returns the guessed plug-in type (empty string if not found)
std::string	PedalEditAction::print_fx_id (const doc::FxId &fx_id) const
{
	std::string    model_name;
	const std::string model_id = Tools::find_fx_type (fx_id, *_view_ptr);
	if (! model_id.empty ())
	{
		const piapi::PluginDescInterface & desc_main =
			_model_ptr->get_model_desc (model_id);
		if (fx_id._location_type == doc::FxId::LocType_CATEGORY)
		{
			model_name = desc_main.get_name ();
		}
		else
		{
			model_name = fx_id._label_or_model;
		}
	}
	const std::string title =
		  (fx_id._location_type == doc::FxId::LocType_LABEL)
		? "Label : "
		: "Type  : ";
	model_name = pi::param::Tools::join_strings_multi (
		model_name.c_str (), '\n', title, ""
	);

	const int      scr_w = _page_size [0];
	model_name = pi::param::Tools::print_name_bestfit (
		scr_w,
		model_name.c_str (),
		*_label_sptr,
		&NText::get_char_width
	);
	_label_sptr->set_text (model_name);

	return model_id;
}



// dir == 0 : select
MsgHandlerInterface::EvtProp	PedalEditAction::change_value (int node_id, int dir)
{
	EvtProp        ret_val = EvtProp_PASS;

	if (node_id == Entry_TYPE)
	{
		ret_val = change_type (dir);
	}
	else
	{
		const doc::PedalActionCycle & cycle =
			_ctx._content._action_arr [_ctx._trigger];
		const doc::PedalActionCycle::ActionArray &   step =
			cycle._cycle [_ctx._step_index];
		const doc::PedalActionSingleInterface &   action =
			*(step [_ctx._action_index]);
		const doc::ActionType   action_type = action.get_type ();
		switch (action_type)
		{
		case doc::ActionType_BANK:
			ret_val = change_bank (node_id, dir);
			break;
		case doc::ActionType_PRESET:
			ret_val = change_preset (node_id, dir);
			break;
		case doc::ActionType_PARAM:
			ret_val = change_param (node_id, dir);
			break;
		case doc::ActionType_SETTINGS:
			ret_val = change_settings (node_id, dir);
			break;
		default:
			// Nothing
			break;
		}
	}

	return ret_val;
}



MsgHandlerInterface::EvtProp	PedalEditAction::change_type (int dir)
{
	doc::PedalActionCycle & cycle =
		_ctx._content._action_arr [_ctx._trigger];
	doc::PedalActionCycle::ActionArray &   step =
		cycle._cycle [_ctx._step_index];
	doc::PedalActionCycle::ActionSPtr & action_sptr =
		step [_ctx._action_index];
	doc::ActionType   action_type = action_sptr->get_type ();

	// Currently we only support this set of actions.
	static const std::array <doc::ActionType, 7>  type_list =
	{{
		doc::ActionType_BANK,
		doc::ActionType_PRESET,
		doc::ActionType_TOGGLE_TUNER,
		doc::ActionType_PARAM,
		doc::ActionType_TEMPO,
		doc::ActionType_SETTINGS
	}};
	const int      type_list_size = int (type_list.size ());
	int            type_pos       = 0;
	bool           found_flag     = false;
	while (! found_flag && type_pos < type_list_size)
	{
		if (action_type <= type_list [type_pos])
		{
			found_flag = true;
		}
		else
		{
			++ type_pos;
		}
	}

	bool        ok_flag = false;
	do
	{
		type_pos += dir;
		type_pos += type_list_size;
		type_pos %= type_list_size;

		action_type = type_list [type_pos];
		ok_flag     = true;
		switch (action_type)
		{
		case doc::ActionType_BANK:
			action_sptr = doc::PedalActionCycle::ActionSPtr (
				new doc::ActionBank (false, 0)
			);
			break;
		case doc::ActionType_PRESET:
			action_sptr = doc::PedalActionCycle::ActionSPtr (
				new doc::ActionPreset (false, 0)
			);
			break;
		case doc::ActionType_TOGGLE_TUNER:
			action_sptr = doc::PedalActionCycle::ActionSPtr (
				new doc::ActionToggleTuner
			);
			break;
		case doc::ActionType_PARAM:
			action_sptr = doc::PedalActionCycle::ActionSPtr (
				new doc::ActionParam (doc::FxId (
					doc::FxId::LocType_LABEL, "X", PiType_MIX
				), pi::dwm::Param_BYPASS, 0)
			);
			break;
		case doc::ActionType_TEMPO:
			action_sptr = doc::PedalActionCycle::ActionSPtr (
				new doc::ActionTempo
			);
			break;
		case doc::ActionType_SETTINGS:
			action_sptr = doc::PedalActionCycle::ActionSPtr (
				new doc::ActionSettings (doc::FxId (
					doc::FxId::LocType_LABEL, "X", PiType_MAIN
				), false, 0)
			);
			break;
		default:
			assert (false);
			ok_flag = false;
			break;
		}
	}
	while (! ok_flag);

	update_model ();

	return EvtProp_CATCH;
}



MsgHandlerInterface::EvtProp	PedalEditAction::change_bank (int /*node_id*/, int dir)
{
	doc::PedalActionCycle & cycle =
		_ctx._content._action_arr [_ctx._trigger];
	doc::PedalActionCycle::ActionArray &   step =
		cycle._cycle [_ctx._step_index];
	doc::PedalActionCycle::ActionSPtr & action_sptr =
		step [_ctx._action_index];
	assert (action_sptr->get_type () == doc::ActionType_BANK);
	doc::ActionBank & action = dynamic_cast <doc::ActionBank &> (*action_sptr);

	int            index = action._val;
	if (action._relative_flag)
	{
		index = (action._val < 0) ? 0 : 1;
		index += Cst::_nbr_banks;
	}

	const int      total_size = Cst::_nbr_banks + 2;
	index += dir;
	index += total_size;
	index %= total_size;

	if (index < Cst::_nbr_banks)
	{
		action._val           = index;
		action._relative_flag = false;
	}
	else
	{
		action._val           = (index == Cst::_nbr_banks) ? -1 : 1;
		action._relative_flag = true;
	}

	update_model ();

	return EvtProp_CATCH;
}



MsgHandlerInterface::EvtProp	PedalEditAction::change_preset (int /*node_id*/, int dir)
{
	doc::PedalActionCycle & cycle =
		_ctx._content._action_arr [_ctx._trigger];
	doc::PedalActionCycle::ActionArray &   step =
		cycle._cycle [_ctx._step_index];
	doc::PedalActionCycle::ActionSPtr & action_sptr =
		step [_ctx._action_index];
	assert (action_sptr->get_type () == doc::ActionType_PRESET);
	doc::ActionPreset &  action =
		dynamic_cast <doc::ActionPreset &> (*action_sptr);

	int            index = action._val;
	if (action._relative_flag)
	{
		index = (action._val < 0) ? 0 : 1;
		index += Cst::_nbr_presets_per_bank;
	}

	const int      total_size = Cst::_nbr_presets_per_bank + 2;
	index += dir;
	index += total_size;
	index %= total_size;

	if (index < Cst::_nbr_presets_per_bank)
	{
		action._val           = index;
		action._relative_flag = false;
	}
	else
	{
		action._val           = (index == Cst::_nbr_presets_per_bank) ? -1 : 1;
		action._relative_flag = true;
	}

	update_model ();

	return EvtProp_CATCH;
}



MsgHandlerInterface::EvtProp	PedalEditAction::change_param (int node_id, int dir)
{
	EvtProp        ret_val = EvtProp_PASS;

	doc::PedalActionCycle & cycle =
		_ctx._content._action_arr [_ctx._trigger];
	doc::PedalActionCycle::ActionArray &   step =
		cycle._cycle [_ctx._step_index];
	doc::PedalActionCycle::ActionSPtr & action_sptr =
		step [_ctx._action_index];
	assert (action_sptr->get_type () == doc::ActionType_PARAM);
	doc::ActionParam &   action =
		dynamic_cast <doc::ActionParam &> (*action_sptr);

	// Effect type/label
	if (node_id == Entry_LABEL)
	{
		if (dir == 0)
		{
			_arg_edit_fxid._cur_preset_flag = true;
			_arg_edit_fxid._fx_id           = action._fx_id;
			_state                          = State_EDIT_FXID;
			_page_switcher.call_page (
				PageType_EDIT_FXID, &_arg_edit_fxid, node_id
			);
			ret_val = EvtProp_CATCH;
		}
	}

	// Index
	else if (node_id == Entry_INDEX)
	{
		int            index = action._index;
		index += dir;
		index = std::max (index, 0);

		std::string   pi_id =
			  (action._fx_id._type == PiType_MIX)
			? Cst::_plugin_mix
			: Tools::find_fx_type (action._fx_id, *_view_ptr);
		if (! pi_id.empty ())
		{
			const piapi::PluginDescInterface &   pi_desc =
				_model_ptr->get_model_desc (pi_id);
			const int      nbr_param =
				pi_desc.get_nbr_param (piapi::ParamCateg_GLOBAL);
			index = std::min (index, nbr_param - 1);
		}

		action._index = index;
		update_model ();
		ret_val = EvtProp_CATCH;
	}

	// Value
	else if (node_id == Entry_VALUE)
	{
		if (dir == 0)
		{

			/*** To do: jump to page with fine editing ***/

		}
		else
		{
			bool          done_flag = false;
			std::string   pi_id =
				  (action._fx_id._type == PiType_MIX)
				? Cst::_plugin_mix
				: Tools::find_fx_type (action._fx_id, *_view_ptr);
			if (! pi_id.empty ())
			{
				const piapi::PluginDescInterface &   pi_desc =
					_model_ptr->get_model_desc (pi_id);
				const int      nbr_param =
					pi_desc.get_nbr_param (piapi::ParamCateg_GLOBAL);
				if (action._index >= 0 && action._index < nbr_param)
				{
					action._val = float (Tools::change_param (
						action._val, pi_desc, action._index, Cst::_step_param, dir
					));
					done_flag = true;
				}
			}
			if (! done_flag)
			{
				action._val += float (dir * Cst::_step_param);
				action._val  = fstb::limit (action._val, 0.0f, 1.0f);
			}
			update_model ();
			ret_val = EvtProp_CATCH;
		}
	}
	else
	{
		assert (false);
	}

	return ret_val;
}



MsgHandlerInterface::EvtProp	PedalEditAction::change_settings (int node_id, int dir)
{
	EvtProp        ret_val = EvtProp_PASS;

	doc::PedalActionCycle & cycle =
		_ctx._content._action_arr [_ctx._trigger];
	doc::PedalActionCycle::ActionArray &   step =
		cycle._cycle [_ctx._step_index];
	doc::PedalActionCycle::ActionSPtr & action_sptr =
		step [_ctx._action_index];
	assert (action_sptr->get_type () == doc::ActionType_SETTINGS);
	doc::ActionSettings &   action =
		dynamic_cast <doc::ActionSettings &> (*action_sptr);

	if (node_id == Entry_LABEL)
	{
		if (dir == 0)
		{
			_arg_edit_fxid._cur_preset_flag = true;
			_arg_edit_fxid._fx_id           = action._fx_id;
			_state                          = State_EDIT_FXID;
			_page_switcher.call_page (
				PageType_EDIT_FXID, &_arg_edit_fxid, node_id
			);
			ret_val = EvtProp_CATCH;
		}
	}

	else if (node_id == Entry_INDEX)
	{
		int            index = action._val;
		if (action._relative_flag)
		{
			index = (action._val < 0) ? -2 : -1;
		}

		index += dir;
		index = std::max (index, -2); // No upper limit (INT_MAX will wrap to -2)

		if (index >= 0)
		{
			action._val           = index;
			action._relative_flag = false;
		}
		else
		{
			action._val           = index * 2 + 3;
			action._relative_flag = true;
		}

		update_model ();
		ret_val = EvtProp_CATCH;
	}

	return ret_val;
}



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
