/*****************************************************************************

        SlotMenu.cpp
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
#include "mfx/ui/Font.h"
#include "mfx/uitk/pg/SlotMenu.h"
#include "mfx/uitk/pg/Tools.h"
#include "mfx/uitk/NodeEvt.h"
#include "mfx/uitk/PageMgrInterface.h"
#include "mfx/uitk/PageSwitcher.h"
#include "mfx/LocEdit.h"
#include "mfx/Model.h"
#include "mfx/PluginDetails.h"
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



SlotMenu::SlotMenu (PageSwitcher &page_switcher, LocEdit &loc_edit)
:	_page_switcher (page_switcher)
,	_loc_edit (loc_edit)
,	_model_ptr (nullptr)
,	_view_ptr (nullptr)
,	_page_ptr (nullptr)
,	_page_size ()
,	_fnt_ptr (nullptr)
,	_state (State_NORMAL)
,	_save_bank_index (-1)
,	_save_preset_index (-1)
,	_save_slot_id (-1)
,	_menu_sptr (std::make_shared <NWindow> (Entry_WINDOW ))
,	_typ_sptr ( std::make_shared <NText  > (Entry_TYPE   ))
,	_inb_sptr ( std::make_shared <NText  > (Entry_INS_BFR))
,	_ina_sptr ( std::make_shared <NText  > (Entry_INS_AFT))
,	_del_sptr ( std::make_shared <NText  > (Entry_DELETE ))
,	_rtn_sptr ( std::make_shared <NText  > (Entry_ROUTING))
,	_prs_sptr ( std::make_shared <NText  > (Entry_PRESETS))
,	_rst_sptr ( std::make_shared <NText  > (Entry_RESET  ))
,	_chn_sptr ( std::make_shared <NText  > (Entry_CHN    ))
,	_frs_sptr ( std::make_shared <NText  > (Entry_FRESH  ))
,	_lbl_sptr ( std::make_shared <NText  > (Entry_LABEL  ))
,	_cpu_sptr ( std::make_shared <NText  > (Entry_MET_CPU))
,	_label_param ()
,	_selfx_param ()
{
	_ina_sptr->set_text ("Insert after");
	_del_sptr->set_text ("Delete");
	_rtn_sptr->set_text ("Routing\xE2\x80\xA6");
	_prs_sptr->set_text ("Presets\xE2\x80\xA6");
	_rst_sptr->set_text ("Reset");

	_menu_sptr->push_back (_typ_sptr);
	_menu_sptr->push_back (_inb_sptr);
	_menu_sptr->push_back (_ina_sptr);
	_menu_sptr->push_back (_del_sptr);
	_menu_sptr->push_back (_rtn_sptr);
	_menu_sptr->push_back (_prs_sptr);
	_menu_sptr->push_back (_rst_sptr);
	_menu_sptr->push_back (_chn_sptr);
	_menu_sptr->push_back (_frs_sptr);
	_menu_sptr->push_back (_lbl_sptr);
	_menu_sptr->push_back (_cpu_sptr);
	_menu_sptr->set_autoscroll (true);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	SlotMenu::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt)
{
	fstb::unused (usr_ptr);

	_model_ptr = &model;
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;
	_fnt_ptr   = &fnt._m;

	if (_state == State_EDIT_LABEL)
	{
		if (   _label_param._ok_flag
		    && _view_ptr->get_bank_index ()   == _save_bank_index
		    && _view_ptr->get_preset_index () == _save_preset_index
		    && _loc_edit._slot_id             == _save_slot_id)
		{
			_model_ptr->set_slot_label (_loc_edit._slot_id, _label_param._label);
		}
	}
	else if (_state == State_SELECT_PLUGIN)
	{
		if (   _selfx_param._ok_flag
		    && _view_ptr->get_bank_index ()   == _save_bank_index
		    && _view_ptr->get_preset_index () == _save_preset_index
		    && _loc_edit._slot_id             == _save_slot_id)
		{
			const int      slot_id_new = Tools::change_plugin (
				*_model_ptr,
				*_view_ptr,
				_loc_edit._slot_id,
				_selfx_param._model_id,
				_loc_edit._audio_flag
			);

			if (slot_id_new != _loc_edit._slot_id)
			{
				_loc_edit._slot_id = slot_id_new;
				_loc_edit.fix_audio_flag (*_view_ptr, *_model_ptr);
			}
		}
	}
	_state = State_NORMAL;

	// Updates _chain_flag, if possible
	_loc_edit.fix_audio_flag (*_view_ptr, *_model_ptr);

	_menu_sptr->set_size (_page_size, Vec2d ());
	_menu_sptr->set_disp_pos (Vec2d ());

	const int      scr_w = _page_size [0];
	const int      h_m   = _fnt_ptr->get_char_h ();

	int            y     = 0;
	for (auto &sptr : {
		_typ_sptr, _inb_sptr, _ina_sptr, _del_sptr, _rtn_sptr,
		_prs_sptr, _rst_sptr, _chn_sptr, _frs_sptr,
#if defined (mfx_PluginDetails_USE_TIMINGS)
# if PV_VERSION == 1
		// In v1, the display is too small to show all the lines at once, so one
		// has to scroll to see the last lines. However the scrolling is limited
		// to the browsable parts of the UI, so if the CPU line (not browsable)
		// is last, it is never displayed. Therefore as a quick fix, we have to
		// reorder the lines so the CPU line is not the last one.
		_cpu_sptr, _lbl_sptr
# else
		_lbl_sptr, _cpu_sptr
# endif // PV_VERSION
#endif // mfx_PluginDetails_USE_TIMINGS
	})
	{
		sptr->set_font (*_fnt_ptr);
		sptr->set_coord (Vec2d (0, y));
		sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
		y += h_m;
	}

	_page_ptr->push_back (_menu_sptr);
#if defined (mfx_PluginDetails_USE_TIMINGS)
	_page_ptr->set_timer (0, true);
#endif // mfx_PluginDetails_USE_TIMINGS

	update_display ();
}



void	SlotMenu::do_disconnect ()
{
#if defined (mfx_PluginDetails_USE_TIMINGS)
	_page_ptr->set_timer (0, false);
#endif // mfx_PluginDetails_USE_TIMINGS
}



MsgHandlerInterface::EvtProp	SlotMenu::do_handle_evt (const NodeEvt &evt)
{
	EvtProp        ret_val = EvtProp_PASS;

	const int      node_id = evt.get_target ();

	if (evt.is_timer ())
	{
		refresh_display ();
		ret_val = EvtProp_CATCH;
	}

	else if (evt.is_button_ex ())
	{
		const Button   but = evt.get_button_ex ();
		switch (but)
		{
		case Button_S:
			ret_val = EvtProp_CATCH;
			switch (node_id)
			{
			case Entry_TYPE:
				select_plugin (node_id);
				break;
			case Entry_INS_BFR:
				insert_slot_before ();
				break;
			case Entry_INS_AFT:
				insert_slot_after ();
				break;
			case Entry_DELETE:
				delete_slot ();
				break;
			case Entry_ROUTING:
				_page_switcher.switch_to (PageType_SLOT_ROUTING, nullptr);
				break;
			case Entry_PRESETS:
				_page_switcher.switch_to (PageType_PRESET_MENU, nullptr);
				break;
			case Entry_RESET:
				reset_plugin ();
				break;
			case Entry_CHN:
				change_chn_pref ();
				break;
			case Entry_FRESH:
				change_state_mode ();
				break;
			case Entry_LABEL:
				set_label (node_id);
				break;
			default:
				ret_val = EvtProp_PASS;
				break;
			}
			break;
		case Button_E:
			{
				const doc::Preset &  preset = _view_ptr->use_preset_cur ();
				if (   _loc_edit._slot_id < 0
				    || preset.is_slot_empty (_loc_edit._slot_id))
				{
					_page_switcher.switch_to (pg::PageType_PROG_EDIT, nullptr);
				}
				else
				{
					_page_switcher.switch_to (pg::PageType_PARAM_LIST, nullptr);
				}
			}
			ret_val = EvtProp_CATCH;
			break;
		case Button_L:
			if (node_id == Entry_TYPE)
			{
				change_type (-1);
			}
			break;
		case Button_R:
			if (node_id == Entry_TYPE)
			{
				change_type (+1);
			}
			break;
		default:
			// Nothing
			break;
		}
	}

	return ret_val;
}



void	SlotMenu::do_activate_preset (int index)
{
	fstb::unused (index);

	_page_switcher.switch_to (pg::PageType_PROG_EDIT, nullptr);
}



void	SlotMenu::do_remove_slot (int slot_id)
{
	if (slot_id == _loc_edit._slot_id)
	{
		_loc_edit._slot_id = -1;
		update_display ();
	}
}



void	SlotMenu::do_set_routing (const doc::Routing &routing)
{
	fstb::unused (routing);

	update_display ();
}



void	SlotMenu::do_set_slot_label (int slot_id, std::string name)
{
	fstb::unused (name);

	if (slot_id == _loc_edit._slot_id)
	{
		update_display ();
	}
}



void	SlotMenu::do_set_plugin (int slot_id, const PluginInitData &pi_data)
{
	fstb::unused (pi_data);

	if (slot_id == _loc_edit._slot_id)
	{
		update_display ();
	}
}



void	SlotMenu::do_remove_plugin (int slot_id)
{
	if (slot_id == _loc_edit._slot_id)
	{
		update_display ();
	}
}



void	SlotMenu::do_set_plugin_mono (int slot_id, bool mono_flag)
{
	fstb::unused (mono_flag);

	if (slot_id == _loc_edit._slot_id)
	{
		update_display ();
	}
}



void	SlotMenu::do_set_plugin_reset (int slot_id, bool reset_flag)
{
	fstb::unused (reset_flag);

	if (slot_id == _loc_edit._slot_id)
	{
		update_display ();
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	SlotMenu::update_display ()
{
	const int      scr_w = _page_size [0];

	PageMgrInterface::NavLocList  nav_list;
	NavLoc         nav;

	const doc::Preset &  preset = _view_ptr->use_preset_cur ();
	const int      slot_id      = _loc_edit._slot_id;
	const bool     exist_flag   = (slot_id >= 0);
	const bool     full_flag    =
		(exist_flag && ! preset.is_slot_empty (slot_id));

	std::string    multilabel = "<Empty>";
	if (full_flag)
	{
		const doc::Slot & slot = preset.use_slot (slot_id);
		const piapi::PluginDescInterface &  desc =
			_model_ptr->get_model_desc (slot._pi_model);
		const piapi::PluginInfo pi_info { desc.get_info () };
		multilabel = pi_info._name;
	}
	std::string    txt = pi::param::Tools::print_name_bestfit (
		scr_w, multilabel.c_str (),
		*_typ_sptr, &NText::get_char_width
	);
	_typ_sptr->set_text (txt);
	PageMgrInterface::add_nav (nav_list, Entry_TYPE);

	_inb_sptr->set_text (_loc_edit._audio_flag ? "Insert before" : "Insert");
	PageMgrInterface::add_nav (nav_list, Entry_INS_BFR);

	_ina_sptr->show (exist_flag && _loc_edit._audio_flag);
	_del_sptr->show (exist_flag);
	_rtn_sptr->show (exist_flag);
	if (exist_flag)
	{
		if (_loc_edit._audio_flag)
		{
			PageMgrInterface::add_nav (nav_list, Entry_INS_AFT);
		}
		PageMgrInterface::add_nav (nav_list, Entry_DELETE);
		PageMgrInterface::add_nav (nav_list, Entry_ROUTING);
	}

	_prs_sptr->show (full_flag);
	_rst_sptr->show (full_flag);
	_chn_sptr->show (full_flag);
	_frs_sptr->show (full_flag);
	_lbl_sptr->show (full_flag);
	_cpu_sptr->show (full_flag);
	if (full_flag)
	{
		const doc::Slot & slot = preset.use_slot (slot_id);

		PageMgrInterface::add_nav (nav_list, Entry_PRESETS);
		PageMgrInterface::add_nav (nav_list, Entry_RESET);

		const doc::PluginSettings &   settings = slot.use_settings (PiType_MAIN);
		const bool     fm_flag = settings._force_mono_flag;
		_chn_sptr->set_text (
			  fm_flag
			? "Chan : prefer mono"
			: "Chan : auto"
		);
		PageMgrInterface::add_nav (nav_list, Entry_CHN);

		const bool     fresh_flag = settings._force_reset_flag;
		_frs_sptr->set_text (
			  fresh_flag
			? "State: fresh"
			: "State: keep"
		);
		PageMgrInterface::add_nav (nav_list, Entry_FRESH);

		_lbl_sptr->set_text ("Name : " + slot._label);
		PageMgrInterface::add_nav (nav_list, Entry_LABEL);
	}

	_page_ptr->set_nav_layout (nav_list);

	refresh_display ();

	_menu_sptr->invalidate_all ();
}



// Called reguarly, for moving things
void	SlotMenu::refresh_display ()
{
#if defined (mfx_PluginDetails_USE_TIMINGS)
	const doc::Preset &  preset = _view_ptr->use_preset_cur ();
	const int      slot_id      = _loc_edit._slot_id;
	const bool     exist_flag   = (slot_id >= 0);
	const bool     full_flag    =
		(exist_flag && ! preset.is_slot_empty (slot_id));
	if (full_flag)
	{
		const Model::CpuMeter   res = _model_ptr->get_plugin_cpu_meter (slot_id);
		const float    cpu_avg = std::min (res._rms  * 100, 99.99f);
		const float    cpu_max = std::min (res._peak * 100, 99.99f);
		char           txt_0 [255+1];
		fstb::snprintf4all (txt_0, sizeof (txt_0),
			"CPU  : %5.2f%%/%5.2f%%", cpu_avg, cpu_max
		);
		_cpu_sptr->set_text (txt_0);
	}
	else
	{
		_cpu_sptr->set_text ("CPU  : N.A.");
	}
#endif // mfx_PluginDetails_USE_TIMINGS
}



MsgHandlerInterface::EvtProp	SlotMenu::change_type (int dir)
{
	const std::vector <std::string> &   pi_list =
		  (_loc_edit._audio_flag)
		? _view_ptr->use_pi_aud_list ()
		: _view_ptr->use_pi_sig_list ();
	const int      slot_id_new = Tools::change_plugin (
		*_model_ptr,
		*_view_ptr,
		_loc_edit._slot_id,
		dir,
		pi_list,
		_loc_edit._audio_flag
	);

	if (slot_id_new != _loc_edit._slot_id)
	{
		_loc_edit._slot_id = slot_id_new;
		_loc_edit.fix_audio_flag (*_view_ptr, *_model_ptr);
		update_display ();
	}

	return EvtProp_CATCH;
}



void	SlotMenu::reset_plugin ()
{
	const doc::Preset &  preset = _view_ptr->use_preset_cur ();
	const int      slot_id      = _loc_edit._slot_id;
	const doc::Slot & slot      = preset.use_slot (slot_id);
	
	const doc::CtrlLinkSet  cls {};
	for (int type_cnt = 0; type_cnt < PiType_NBR_ELT; ++type_cnt)
	{
		const PiType   pi_type = static_cast <PiType> (type_cnt);
		const std::string pi_model =
			  (pi_type == PiType_MIX)
			? Cst::_plugin_dwm
			: slot._pi_model;
		const piapi::PluginState & def =
			_model_ptr->use_default_settings (pi_model);
		const int      nbr_param    = int (def._param_list.size ());
		for (int index = 0; index < nbr_param; ++index)
		{
			const float    val = float (def._param_list [index]);
			_model_ptr->set_param (slot_id, pi_type, index, val);
			_model_ptr->set_param_ctrl (slot_id, pi_type, index, cls);
			_model_ptr->set_param_pres (slot_id, pi_type, index, nullptr);
		}

		/*** To do: add the default controllers ***/

	}

	update_display ();
}



void	SlotMenu::select_plugin (int node_id)
{
	_selfx_param._title         = "Category";
	_selfx_param._audio_flag    = _loc_edit._audio_flag;
	_selfx_param._auto_loc_flag = true;
	_selfx_param._model_id.clear ();
	if (_loc_edit._slot_id >= 0)
	{
		const doc::Preset &  preset = _view_ptr->use_preset_cur ();
		if (! preset.is_slot_empty (_loc_edit._slot_id))
		{
			const doc::Slot & slot = preset.use_slot (_loc_edit._slot_id);
			_selfx_param._model_id = slot._pi_model;
		}
	}

	_save_bank_index   = _view_ptr->get_bank_index ();
	_save_preset_index = _view_ptr->get_preset_index ();
	_save_slot_id      = _loc_edit._slot_id;
	_state             = State_SELECT_PLUGIN;

	_page_switcher.call_page (PageType_SELECT_FX, &_selfx_param, node_id);
}



// Works also with non-audio slots
void	SlotMenu::insert_slot_before ()
{
	const int      slot_id_new  = _model_ptr->add_slot ();
	if (_loc_edit._audio_flag)
	{
		const int      slot_id_old  = _loc_edit._slot_id;
		const doc::Preset &  preset = _view_ptr->use_preset_cur ();
		doc::Routing   routing      = preset.use_routing (); // Copy
		ToolsRouting::insert_slot_before (
			routing._cnx_audio_set, slot_id_new, slot_id_old
		);
		_model_ptr->set_routing (routing);
	}
	_loc_edit._slot_id = slot_id_new;

	update_display ();
}



void	SlotMenu::insert_slot_after ()
{
	const int      slot_id_new  = _model_ptr->add_slot ();
	assert (_loc_edit._audio_flag);
	const int      slot_id_old  = _loc_edit._slot_id;
	const doc::Preset &  preset = _view_ptr->use_preset_cur ();
	doc::Routing   routing      = preset.use_routing (); // Copy
	ToolsRouting::insert_slot_after (
		routing._cnx_audio_set, slot_id_new, slot_id_old
	);
	_model_ptr->set_routing (routing);
	_loc_edit._slot_id = slot_id_new;

	update_display ();
}



void	SlotMenu::delete_slot ()
{
	if (_loc_edit._slot_id >= 0)
	{
		if (_loc_edit._audio_flag)
		{
			// Find the current editing position in the linear audio slot list
			const auto &   slot_list_aud = _view_ptr->use_slot_list_aud ();
			int            pos = ToolsRouting::find_linear_index_audio_graph (
				slot_list_aud, _loc_edit._slot_id
			);

			// Removes the slot
			const doc::Preset &  preset = _view_ptr->use_preset_cur ();
			doc::Routing   routing      = preset.use_routing (); // Copy
			ToolsRouting::disconnect_slot (
				routing._cnx_audio_set, _loc_edit._slot_id
			);
			_model_ptr->set_routing (routing);
			_model_ptr->remove_slot (_loc_edit._slot_id);

			// Grabs the new slot_id at the saved position
			if (pos < int (slot_list_aud.size ()))
			{
				_loc_edit._slot_id = slot_list_aud [pos];
			}
			else
			{
				_loc_edit._slot_id = -1;
			}
		}
		else
		{
			_model_ptr->remove_slot (_loc_edit._slot_id);
		}
	}
	else
	{
		assert (false);
	}

	update_display ();
}



void	SlotMenu::change_chn_pref ()
{
	if (_loc_edit._slot_id >= 0)
	{
		const doc::Preset &  preset = _view_ptr->use_preset_cur ();
		const auto     it_slot =
			preset._slot_map.find (_loc_edit._slot_id);
		assert (it_slot != preset._slot_map.end ());
		if (! preset.is_slot_empty (it_slot))
		{
			const doc::Slot & slot = *(it_slot->second);
			const doc::PluginSettings &   settings =
				slot.use_settings (PiType_MAIN);
			bool           fm_flag = settings._force_mono_flag;
			fm_flag = ! fm_flag;
			_model_ptr->set_plugin_mono (_loc_edit._slot_id, fm_flag);
		}
	}
}



void	SlotMenu::change_state_mode ()
{
	if (_loc_edit._slot_id >= 0)
	{
		const doc::Preset &  preset = _view_ptr->use_preset_cur ();
		const auto     it_slot =
			preset._slot_map.find (_loc_edit._slot_id);
		assert (it_slot != preset._slot_map.end ());
		if (! preset.is_slot_empty (it_slot))
		{
			const doc::Slot & slot = *(it_slot->second);
			const doc::PluginSettings &   settings =
				slot.use_settings (PiType_MAIN);
			bool           fresh_flag = settings._force_reset_flag;
			fresh_flag = ! fresh_flag;
			_model_ptr->set_plugin_reset (_loc_edit._slot_id, fresh_flag);
		}
	}
}



void	SlotMenu::set_label (int node_id)
{
	assert (_loc_edit._slot_id >= 0);
	const doc::Preset &  preset = _view_ptr->use_preset_cur ();
	const doc::Slot &    slot   = preset.use_slot (_loc_edit._slot_id);
	_label_param._label         = slot._label;
	_label_param._sep_cur_flag  = true;
	_state                      = State_EDIT_LABEL;
	_save_bank_index            = _view_ptr->get_bank_index ();
	_save_preset_index          = _view_ptr->get_preset_index ();
	_save_slot_id               = _loc_edit._slot_id;
	_page_switcher.call_page (PageType_EDIT_LABEL, &_label_param, node_id);
}



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
