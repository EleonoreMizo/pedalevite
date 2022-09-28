/*****************************************************************************

        View.cpp
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

#include "mfx/doc/ActionParam.h"
#include "mfx/doc/ActionSettings.h"
#include "mfx/doc/ActionToggleFx.h"
#include "mfx/doc/FxId.h"
#include "mfx/PedalLoc.h"
#include "mfx/View.h"

#include <algorithm>
#include <tuple>

#include <cassert>



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	View::add_observer (ModelObserverInterface &obs)
{
	_obs_set.insert (&obs);
}



void	View::remove_observer (ModelObserverInterface &obs)
{
	auto           it = _obs_set.find (&obs);
	if (it == _obs_set.end ())
	{
		assert (false);
	}
	else
	{
		_obs_set.erase (it);
	}
}



void	View::set_pi_lists (const std::vector <std::string> &pi_aud_list, const std::vector <std::string> &pi_sig_list)
{
	_pi_aud_list = pi_aud_list;
	_pi_sig_list = pi_sig_list;

	_pi_aud_set.clear ();
	_pi_aud_set.insert (_pi_aud_list.begin (), _pi_aud_list.end ());
}



double	View::get_tempo () const
{
	return _tempo;
}



bool	View::is_editing () const
{
	return _edit_flag;
}



bool	View::is_tuner_active () const
{
	return _tuner_flag;
}



bool	View::is_click_active () const
{
	return _click_flag;
}



float	View::get_tuner_freq () const
{
	return _tuner_freq;
}



const doc::Setup &	View::use_setup () const
{
	return _setup;
}



const doc::Program &	View::use_prog_cur () const
{
	return _prog_cur;
}



const ModelObserverInterface::SlotInfoMap &	View::use_slot_info_map () const
{
	return _slot_info_map;
}



int	View::get_bank_index () const
{
	return _bank_index;
}



int	View::get_prog_index () const
{
	return _prog_index;
}



const View::OverrideMap &	View::use_param_ctrl_override_map () const
{
	return _override_map;
}



std::set <std::string>	View::collect_labels (bool cur_prog_flag) const
{
	std::set <std::string>  labels;

	collect_labels (labels, _prog_cur);

	if (! cur_prog_flag)
	{
		for (const auto &bank : _setup._bank_arr)
		{
			for (const auto &prog : bank._prog_arr)
			{
				collect_labels (labels, prog);
			}

			collect_labels (labels, bank._layout);
		}

		collect_labels (labels, _setup._layout);
	}

	assert (labels.find ("") == labels.end ());

	return labels;
}



const std::vector <std::string> &	View::use_pi_aud_list () const
{
	return _pi_aud_list;
}



const std::vector <std::string> &	View::use_pi_sig_list () const
{
	return _pi_sig_list;
}



const ToolsRouting::NodeMap &	View::use_graph () const
{
	return _graph;
}



const std::vector <int> &	View::use_slot_list_aud () const
{
	return _slot_list_aud;
}



const std::vector <int> &	View::use_slot_list_sig () const
{
	return _slot_list_sig;
}



int	View::build_ordered_node_list (std::vector <int> &slot_id_list, bool audio_first_flag) const
{
	slot_id_list.clear ();

	int            second_list_pos = 0;

	if (audio_first_flag)
	{
		slot_id_list    = _slot_list_aud;
		second_list_pos = int (slot_id_list.size ());
		slot_id_list.insert (
			slot_id_list.end (), _slot_list_sig.begin (), _slot_list_sig.end ()
		);
	}
	else
	{
		slot_id_list    = _slot_list_sig;
		second_list_pos = int (slot_id_list.size ());
		slot_id_list.insert (
			slot_id_list.end (), _slot_list_aud.begin (), _slot_list_aud.end ()
		);
	}

	return second_list_pos;
}



void	View::update_parameter (doc::Program &prog, int slot_id, PiType type, int index, float val)
{
	auto           it_slot = prog._slot_map.find (slot_id);
	if (prog.is_slot_empty (it_slot))
	{
		assert (false);
	}
	else
	{
		doc::Slot &    slot = *(it_slot->second);
		doc::PluginSettings &  settings =
				(type == PiType_MIX)
			? slot._settings_mixer
			: slot._settings_all [slot._pi_model];
		if (index >= int (settings._param_list.size ()))
		{
			assert (false);
		}
		else
		{
			settings._param_list [index] = val;
		}
	}
}



float	View::get_param_val (const doc::Program &prog, int slot_id, PiType type, int index)
{
	float          val = 0;

	auto           it_slot = prog._slot_map.find (slot_id);
	assert (it_slot != prog._slot_map.end ());
	if (prog.is_slot_empty (it_slot))
	{
		assert (false);
	}
	else
	{
		doc::Slot &    slot = *(it_slot->second);
		doc::PluginSettings &  settings =
				(type == PiType_MIX)
			? slot._settings_mixer
			: slot._settings_all [slot._pi_model];
		if (index >= int (settings._param_list.size ()))
		{
			assert (false);
		}
		else
		{
			val = settings._param_list [index];
		}
	}

	return val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// We need to copy the observer list because this list may be modified by
// a sub-observer.
/*** To do: turn this macro into a nice template function ***/
#define mfx_View_PROPAGATE(x) \
	do { \
		ObsSet obs_set_copy (_obs_set); \
		for (auto ptr : obs_set_copy) { ptr->x; } \
	} while (false)



void	View::do_set_tempo (double bpm)
{
	_tempo = bpm;
	mfx_View_PROPAGATE (set_tempo (bpm));
}



void	View::do_set_edit_mode (bool edit_flag)
{
	_edit_flag = edit_flag;
	mfx_View_PROPAGATE (set_edit_mode (edit_flag));
}



void	View::do_set_save_mode (doc::Setup::SaveMode mode)
{
	_setup._save_mode = mode;
	mfx_View_PROPAGATE (set_save_mode (mode));
}



void	View::do_set_setup_name (std::string name)
{
	_setup._name = name;
	mfx_View_PROPAGATE (set_setup_name (name));
}



void	View::do_set_pedalboard_layout (const doc::PedalboardLayout &layout)
{
	_setup._layout = layout;
	mfx_View_PROPAGATE (set_pedalboard_layout (layout));
}



void	View::do_set_pedal (const PedalLoc &loc, const doc::PedalActionGroup &content)
{
	doc::PedalboardLayout * layout_ptr = nullptr;

	switch (loc._type)
	{
	case PedalLoc::Type_GLOBAL:
		layout_ptr = &_setup._layout;
		break;

	case PedalLoc::Type_BANK:
		assert (loc._bank_index >= 0);
		assert (loc._bank_index < Cst::_nbr_banks);
		layout_ptr = &_setup._bank_arr [loc._bank_index]._layout;
		break;

	case PedalLoc::Type_PROG:
		assert (loc._bank_index >= 0);
		assert (loc._bank_index < Cst::_nbr_banks);
		assert (loc._prog_index >= 0);
		assert (loc._prog_index < Cst::_nbr_prog_per_bank);
		layout_ptr = &_setup._bank_arr [loc._bank_index]._prog_arr [loc._prog_index]._layout;
		break;

	case PedalLoc::Type_PROG_CUR:
		layout_ptr = &_prog_cur._layout;
		break;
		
	default:
		assert (false);
		break;
	}

	if (layout_ptr != nullptr)
	{
		layout_ptr->_pedal_arr [loc._pedal_index] = content;
	}

	mfx_View_PROPAGATE (set_pedal (loc, content));
}



void	View::do_set_bank (int index, const doc::Bank &bank)
{
	_setup._bank_arr [index] = bank;
	mfx_View_PROPAGATE (set_bank (index, bank));
}



void	View::do_select_bank (int index)
{
	_bank_index = index;
	mfx_View_PROPAGATE (select_bank (index));
}



void	View::do_set_bank_name (std::string name)
{
	_setup._bank_arr [_bank_index]._name = name;
	mfx_View_PROPAGATE (set_bank_name (name));
}



void	View::do_set_prog_name (std::string name)
{
	_prog_cur._name = name;
	mfx_View_PROPAGATE (set_prog_name (name));
}



void	View::do_set_prog (int bank_index, int prog_index, const doc::Program &prog)
{
	_setup._bank_arr [bank_index]._prog_arr [prog_index] = prog;
	mfx_View_PROPAGATE (set_prog (bank_index, prog_index, prog));
}



void	View::do_activate_prog (int index)
{
	_prog_index = index;
	_prog_cur   = _setup._bank_arr [_bank_index]._prog_arr [_prog_index];
	_slot_info_map.clear ();
	update_graph_info ();
	mfx_View_PROPAGATE (activate_prog (index));
}



void	View::do_store_prog (int prog_index, int bank_index)
{
	const int      bank_index2 = (bank_index < 0) ? _bank_index : bank_index;
	_setup._bank_arr [bank_index2]._prog_arr [prog_index] = _prog_cur;
	mfx_View_PROPAGATE (store_prog (prog_index, bank_index));
}



void	View::do_set_prog_switch_mode (doc::ProgSwitchMode mode)
{
	_prog_cur._prog_switch_mode = mode;
	mfx_View_PROPAGATE (set_prog_switch_mode (mode));
}



void	View::do_set_chn_mode (ChnMode mode)
{
	_setup._chn_mode = mode;
	update_graph_info ();
	mfx_View_PROPAGATE (set_chn_mode (mode));
}



void	View::do_set_master_vol (float vol)
{
	_setup._master_vol = vol;
	mfx_View_PROPAGATE (set_master_vol (vol));
}



void	View::do_set_tuner (bool active_flag)
{
	_tuner_flag = active_flag;
	mfx_View_PROPAGATE (set_tuner (active_flag));
}



void	View::do_set_tuner_freq (float freq)
{
	_tuner_freq = freq;
	mfx_View_PROPAGATE (set_tuner_freq (freq));
}



void	View::do_set_slot_info_for_current_prog (const SlotInfoMap &info_map)
{
	_slot_info_map = info_map;
	mfx_View_PROPAGATE (set_slot_info_for_current_prog (info_map));
}



void	View::do_set_param (int slot_id, int index, float val, PiType type)
{
	update_parameter (_prog_cur, slot_id, type, index, val);
	mfx_View_PROPAGATE (set_param (slot_id, index, val, type));
}



void	View::do_set_param_beats (int slot_id, int index, float beats)
{
	doc::Slot &    slot = _prog_cur.use_slot (slot_id);
	doc::PluginSettings &   settings = slot.use_settings (PiType_MAIN);
	assert (index < int (settings._param_list.size ()));

	// Stores the beat value in the document
	auto           it_pres = settings._map_param_pres.find (index);
	assert (it_pres != settings._map_param_pres.end ());
	it_pres->second._ref_beats = beats;

	mfx_View_PROPAGATE (set_param_beats (slot_id, index, beats));
}



void	View::do_add_slot (int slot_id)
{
	assert (_prog_cur._slot_map.find (slot_id) == _prog_cur._slot_map.end ());

	_prog_cur._slot_map.insert (std::make_pair (
		slot_id,
		doc::Program::SlotSPtr ()
	));
	_slot_info_map.clear ();
	update_graph_info ();

	mfx_View_PROPAGATE (add_slot (slot_id));
}



void	View::do_remove_slot (int slot_id)
{
	assert (_prog_cur._slot_map.find (slot_id) != _prog_cur._slot_map.end ());
	assert (! _prog_cur.use_routing ().is_referencing_slot (slot_id));

	auto           it_slot = _prog_cur._slot_map.find (slot_id);
	assert (it_slot != _prog_cur._slot_map.end ());
	_prog_cur._slot_map.erase (it_slot);
	_slot_info_map.clear ();
	update_graph_info ();

	mfx_View_PROPAGATE (remove_slot (slot_id));
}



void	View::do_set_routing (const doc::Routing &routing)
{
	_prog_cur.set_routing (routing);
	update_graph_info ();

	mfx_View_PROPAGATE (set_routing (routing));
}



void	View::do_set_slot_label (int slot_id, std::string name)
{
	auto           it_slot = _prog_cur._slot_map.find (slot_id);
	assert (it_slot != _prog_cur._slot_map.end ());
	doc::Program::SlotSPtr &   slot_sptr = it_slot->second;
	if (slot_sptr.get () == nullptr)
	{
		slot_sptr = std::make_shared <doc::Slot> ();
	}
	slot_sptr->_label = name;

	mfx_View_PROPAGATE (set_slot_label (slot_id, name));
}



void	View::do_set_plugin (int slot_id, const PluginInitData &pi_data)
{
	auto           it_slot = _prog_cur._slot_map.find (slot_id);
	assert (it_slot != _prog_cur._slot_map.end ());
	doc::Program::SlotSPtr &   slot_sptr = it_slot->second;
	if (slot_sptr.get () == nullptr)
	{
		slot_sptr = std::make_shared <doc::Slot> ();
	}
	slot_sptr->_pi_model = pi_data._model;
	const int      nbr_param = pi_data._nbr_param_arr [piapi::ParamCateg_GLOBAL];
	doc::PluginSettings &   settings = slot_sptr->_settings_all [pi_data._model];
	settings._param_list.resize (nbr_param, 0);
	update_graph_info ();

	mfx_View_PROPAGATE (set_plugin (slot_id, pi_data));
}



void	View::do_remove_plugin (int slot_id)
{
	auto           it_slot = _prog_cur._slot_map.find (slot_id);
	assert (it_slot != _prog_cur._slot_map.end ());
	doc::Program::SlotSPtr &   slot_sptr = it_slot->second;
	if (slot_sptr.get () != nullptr)
	{
		slot_sptr->_pi_model.clear ();
	}
	update_graph_info ();

	mfx_View_PROPAGATE (remove_plugin (slot_id));
}



void	View::do_set_plugin_mono (int slot_id, bool mono_flag)
{
	doc::Slot &    slot = _prog_cur.use_slot (slot_id);
	doc::PluginSettings &   settings = slot.use_settings (PiType_MAIN);
	settings._force_mono_flag = mono_flag;
	update_graph_info ();

	mfx_View_PROPAGATE (set_plugin_mono (slot_id, mono_flag));
}



void	View::do_set_plugin_reset (int slot_id, bool reset_flag)
{
	doc::Slot &    slot = _prog_cur.use_slot (slot_id);
	doc::PluginSettings &   settings = slot.use_settings (PiType_MAIN);
	settings._force_reset_flag = reset_flag;

	mfx_View_PROPAGATE (set_plugin_reset (slot_id, reset_flag));
}



void	View::do_set_param_pres (int slot_id, PiType type, int index, const doc::ParamPresentation *pres_ptr)
{
	doc::Slot &    slot = _prog_cur.use_slot (slot_id);
	doc::PluginSettings &   settings = slot.use_settings (type);

	if (pres_ptr == nullptr)
	{
		auto           pres_it = settings._map_param_pres.find (index);
		if (pres_it != settings._map_param_pres.end ())
		{
			settings._map_param_pres.erase (pres_it);
		}
	}
	else if (pres_ptr != nullptr)
	{
		settings._map_param_pres [index] = *pres_ptr;
	}

	mfx_View_PROPAGATE (set_param_pres (slot_id, type, index, pres_ptr));
}



void	View::do_set_param_ctrl (int slot_id, PiType type, int index, const doc::CtrlLinkSet &cls)
{
	doc::Slot &    slot = _prog_cur.use_slot (slot_id);
	doc::PluginSettings &   settings = slot.use_settings (type);
	settings._map_param_ctrl [index] = cls;

	mfx_View_PROPAGATE (set_param_ctrl (slot_id, type, index, cls));
}



void	View::do_override_param_ctrl (int slot_id, PiType type, int index, int rotenc_index)
{
	const OverrideLoc loc { slot_id, type, index };
	if (rotenc_index < 0)
	{
		const auto     it = _override_map.find (loc);
		if (it != _override_map.end ())
		{
			_override_map.erase (it);
		}
	}
	else
	{
		_override_map [loc] = rotenc_index;
	}

	mfx_View_PROPAGATE (override_param_ctrl (slot_id, type, index, rotenc_index));
}



void	View::do_set_signal_port (int port_id, const doc::SignalPort &port)
{
	assert (_prog_cur._slot_map.find (port._slot_id) != _prog_cur._slot_map.end ());

	_prog_cur._port_map [port_id] = port;

	mfx_View_PROPAGATE (set_signal_port (port_id, port));
}



void	View::do_clear_signal_port (int port_id)
{
	const auto     it_port = _prog_cur._port_map.find (port_id);
	assert (it_port != _prog_cur._port_map.end ());
	_prog_cur._port_map.erase (it_port);

	mfx_View_PROPAGATE (clear_signal_port (port_id));
}



void	View::do_add_settings (std::string model, int index, std::string name, const doc::PluginSettings &s_main, const doc::PluginSettings &s_mix)
{
	doc::CatalogPluginSettings &  cat = _setup._map_plugin_settings [model];
	doc::CatalogPluginSettings::Cell cell;
	cell._name  = name;
	cell._main  = s_main;
	cell._mixer = s_mix;
	cat.add_settings (index, cell);

	mfx_View_PROPAGATE (add_settings (model, index, name, s_main, s_mix));
}



void	View::do_remove_settings (std::string model, int index)
{
	auto           it_cat = _setup._map_plugin_settings.find (model);
	assert (it_cat != _setup._map_plugin_settings.end ());

	doc::CatalogPluginSettings &  cat = it_cat->second;
	cat.remove_settings (index);

	mfx_View_PROPAGATE (remove_settings (model, index));
}



void	View::do_clear_all_settings ()
{
	_setup._map_plugin_settings.clear ();

	mfx_View_PROPAGATE (clear_all_settings ());
}



void	View::do_set_click (bool click_flag)
{
	_click_flag = click_flag;

	mfx_View_PROPAGATE (set_click (click_flag));
}



void	View::do_enable_auto_rotenc_override (bool ovr_flag)
{
	_setup._auto_assign_rotenc_flag = ovr_flag;

	mfx_View_PROPAGATE (enable_auto_rotenc_override (ovr_flag));
}



#undef mfx_View_PROPAGATE



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	View::collect_labels (std::set <std::string> &labels, const doc::Program &prog) const
{
	for (const auto &node_slot : prog._slot_map)
	{
		if (node_slot.second.get () != nullptr)
		{
			const doc::Slot & slot = *node_slot.second;
			if (! slot._label.empty ())
			{
				labels.insert (slot._label);
			}
		}
	}

	collect_labels (labels, prog._layout);
}



void	View::collect_labels (std::set <std::string> &labels, const doc::PedalboardLayout &layout) const
{
	for (const auto &pedal : layout._pedal_arr)
	{
		for (const auto &cycle : pedal._action_arr)
		{
			for (const auto &step : cycle._cycle)
			{
				for (const auto &action_sptr : step)
				{
					const doc::PedalActionSingleInterface &   action = *action_sptr;
					const doc::ActionType & type = action.get_type ();
					switch (type)
					{
					// All these actions contain an FxId
					case doc::ActionType_TOGGLE_FX:
						collect_labels (
							labels,
							dynamic_cast <const doc::ActionToggleFx &> (action)._fx_id
						);
						break;
					case doc::ActionType_PARAM:
						collect_labels (
							labels,
							dynamic_cast <const doc::ActionParam &> (action)._fx_id
						);
						break;
					case doc::ActionType_SETTINGS:
						collect_labels (
							labels,
							dynamic_cast <const doc::ActionSettings &> (action)._fx_id
						);
						break;

					// These ones don't
					case doc::ActionType_BANK:
					case doc::ActionType_PROG:
					case doc::ActionType_TOGGLE_TUNER:
					case doc::ActionType_LOOP_REC:
					case doc::ActionType_LOOP_PLAY_STOP:
					case doc::ActionType_LOOP_UNDO:
					case doc::ActionType_TEMPO:
					case doc::ActionType_EVENT:
					case doc::ActionType_TEMPO_SET:
					case doc::ActionType_CLICK:
						// Nothing
						break;

					default:
						assert (false);
						break;
					}
				}
			}
		}
	}
}



void	View::collect_labels (std::set <std::string> &labels, const doc::FxId &fx_id) const
{
	if (fx_id._location_type == doc::FxId::LocType_LABEL)
	{
		if (! fx_id._label_or_model.empty ())
		{
			labels.insert (fx_id._label_or_model);
		}
	}
}



bool	View::OverrideLoc::operator < (const OverrideLoc &rhs) const
{
	return (
		  std::tie (    _slot_id,     _type,     _index)
		< std::tie (rhs._slot_id, rhs._type, rhs._index)
	);
}



void	View::update_graph_info ()
{
	const doc::Routing::CnxSet &  cnx_set =
		_prog_cur.use_routing ()._cnx_audio_set;
	ToolsRouting::build_node_graph (_graph, cnx_set);

	ToolsRouting::build_ordered_node_lists (
		_slot_list_aud,
		_slot_list_sig,
		_prog_cur,
		_graph,
		_pi_aud_set
	);
}



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
