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
#include "mfx/pi/param/Tools.h"
#include "mfx/uitk/pg/PedalEditAction.h"
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
,	_loc_type_sptr (new NText (Entry_LOC_TYPE))
,	_label_sptr (   new NText (Entry_LABEL))
{
	// Nothing
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	PedalEditAction::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const ui::Font &fnt_s, const ui::Font &fnt_m, const ui::Font &fnt_l)
{
	_model_ptr = &model;
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;
	_fnt_ptr   = &fnt_m;
	assert (_ctx._pedal >= 0);
	assert (_ctx._pedal < Cst::_nbr_pedals);
	assert (_ctx._trigger >= 0);
	assert (_ctx._trigger < doc::ActionTrigger_NBR_ELT);
	assert (_ctx._step_index >= 0);
	assert (_ctx._step_index < int (_ctx._content._action_arr [_ctx._trigger]._cycle.size ()));

	const int      h_m   = _fnt_ptr->get_char_h ();
	const int      scr_w = _page_size [0];
	const int      x_mid =  scr_w >> 1;

	_type_sptr    ->set_font (fnt_m);
	_index_sptr   ->set_font (fnt_m);
	_value_sptr   ->set_font (fnt_m);
	_loc_type_sptr->set_font (fnt_m);
	_label_sptr   ->set_font (fnt_m);

	_type_sptr    ->set_coord (Vec2d (0, 0 * h_m));

	_type_sptr    ->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_index_sptr   ->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_value_sptr   ->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_loc_type_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_label_sptr   ->set_frame (Vec2d (scr_w, 0), Vec2d ());

	check_ctx ();
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
			switch (node_id)
			{
			case Entry_TYPE:
				/*** To do ***/
				break;
			default:
				assert (false);
				break;
			}
			ret_val = EvtProp_CATCH;
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


	/*** To do ***/



	_page_ptr->set_nav_layout (nav_list);
}



void	PedalEditAction::hide_all ()
{
	_index_sptr   ->show (false);
	_value_sptr   ->show (false);
	_loc_type_sptr->show (false);
	_label_sptr   ->show (false);
}



void	PedalEditAction::display_bank (PageMgrInterface::NavLocList &nav_list, const doc::ActionBank &action)
{
	_type_sptr->set_text ("Bank");

	char           txt_0 [127+1];
	fstb::snprintf4all (
		txt_0, sizeof (txt_0),
		action._relative_flag ? "Number: %+d" : "Number: %02d",
		action._val
	);
	_index_sptr->set_text (txt_0);

	const int      h_m   = _fnt_ptr->get_char_h ();
	_index_sptr->set_coord (Vec2d (0, 2 * h_m));
	_index_sptr->show (true);
	_page_ptr->push_back (_index_sptr);
	PageMgrInterface::add_nav (nav_list, Entry_INDEX);
}



void	PedalEditAction::display_preset (PageMgrInterface::NavLocList &nav_list, const doc::ActionPreset &action)
{
	_type_sptr->set_text ("Program");

	char           txt_0 [127+1];
	fstb::snprintf4all (
		txt_0, sizeof (txt_0),
		action._relative_flag ? "Number: %+d" : "Number: %02d",
		action._val
	);
	_index_sptr->set_text (txt_0);

	const int      h_m   = _fnt_ptr->get_char_h ();
	_index_sptr->set_coord (Vec2d (0, 2 * h_m));
	_index_sptr->show (true);
	_page_ptr->push_back (_index_sptr);
	PageMgrInterface::add_nav (nav_list, Entry_INDEX);
}



void	PedalEditAction::display_tuner (PageMgrInterface::NavLocList &nav_list)
{
	_type_sptr->set_text ("Tuner toggle");
}



void	PedalEditAction::display_fx (PageMgrInterface::NavLocList &nav_list, const doc::ActionToggleFx &action)
{
	_type_sptr->set_text ("FX toggle");


	/*** To do ***/

}



void	PedalEditAction::display_loop_rec (PageMgrInterface::NavLocList &nav_list)
{
	_type_sptr->set_text ("Loop record");

	/*** To do ***/

}



void	PedalEditAction::display_loop_ps (PageMgrInterface::NavLocList &nav_list)
{
	_type_sptr->set_text ("Loop play/stop");

	/*** To do ***/

}



void	PedalEditAction::display_loop_undo (PageMgrInterface::NavLocList &nav_list)
{
	_type_sptr->set_text ("Loop undo");

	/*** To do ***/

}



void	PedalEditAction::display_param (PageMgrInterface::NavLocList &nav_list, const doc::ActionParam &action)
{
	_type_sptr->set_text ("Parameter");

	std::string    model_name;
	std::string    param_name;
	Tools::print_param_action (
		model_name, param_name, action, *_model_ptr, *_view_ptr
	);

	const int      h_m   = _fnt_ptr->get_char_h ();
	const int      scr_w = _page_size [0];

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

	std::string   pi_id =
		  (action._fx_id._type == PiType_MIX)
		? Cst::_plugin_mix
		: Tools::find_fx_type (action._fx_id, *_view_ptr);
	if (pi_id.empty ())
	{
		char           txt_0 [127+1];
		fstb::snprintf4all (
			txt_0, sizeof (txt_0), "Value: 5.1f %%", action._val
		);
		_value_sptr->set_text (txt_0);
	}
	else
	{
		const piapi::PluginDescInterface *   pi_desc_ptr =
			&_model_ptr->get_model_desc (pi_id);
		const piapi::ParamDescInterface & param_desc =
			pi_desc_ptr->get_param_info (piapi::ParamCateg_GLOBAL, action._index);
		const double   val_nat = param_desc.conv_nrm_to_nat (action._val);

		std::string    unit_s = param_desc.get_unit (999);
		std::string    val_s  = param_desc.conv_nat_to_str (val_nat, 0);
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
		_value_sptr->set_text (val_s);
	}

	_value_sptr->set_coord (Vec2d (0, 4 * h_m));
	_value_sptr->show (true);
	_page_ptr->push_back (_value_sptr);
	PageMgrInterface::add_nav (nav_list, Entry_VALUE);
}



void	PedalEditAction::display_tempo (PageMgrInterface::NavLocList &nav_list, const doc::ActionTempo &action)
{
	_type_sptr->set_text ("Tempo tap");
}



void	PedalEditAction::display_settings (PageMgrInterface::NavLocList &nav_list, const doc::ActionSettings &action)
{
	_type_sptr->set_text ("Preset");

	char           txt_0 [127+1];
	fstb::snprintf4all (
		txt_0, sizeof (txt_0),
		action._relative_flag ? "Number: %+d" : "Number: %02d",
		action._val
	);
	_index_sptr->set_text (txt_0);

	const int      h_m   = _fnt_ptr->get_char_h ();
	_index_sptr->set_coord (Vec2d (0, 2 * h_m));
	_index_sptr->show (true);
	_page_ptr->push_back (_index_sptr);
	PageMgrInterface::add_nav (nav_list, Entry_INDEX);
}



void	PedalEditAction::display_event (PageMgrInterface::NavLocList &nav_list)
{
	_type_sptr->set_text ("Event");

	/*** To do ***/

}



MsgHandlerInterface::EvtProp	PedalEditAction::change_value (int node_id, int dir)
{
	EvtProp        ret_val = EvtProp_PASS;

	if (node_id == Entry_TYPE)
	{

		/*** To do ***/

	}
	else
	{

		/*** To do ***/

	}

	return ret_val;
}



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
