/*****************************************************************************

        Model.cpp
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

#include "mfx/pi/dwm/DryWetDesc.h"
#include "mfx/pi/dwm/Param.h"
#include "mfx/pi/tuner/Tuner.h"
#include "mfx/doc/ActionBank.h"
#include "mfx/doc/ActionParam.h"
#include "mfx/doc/ActionPreset.h"
#include "mfx/doc/ActionToggleFx.h"
#include "mfx/doc/ActionToggleTuner.h"
#include "mfx/Model.h"
#include "mfx/ModelObserverInterface.h"

#include <algorithm>

#include <cassert>



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



const std::array <int, Cst::_nbr_pedals>	Model::_pedal_to_switch_map =
{{
	2, 3, 4, 5, 6, 7, 8, 9,
	14, 15, 16, 17
}};



Model::Model (ui::UserInputInterface::MsgQueue &queue_input_to_cmd, ui::UserInputInterface::MsgQueue &queue_input_to_audio, ui::UserInputInterface &input_device)
:	_central (queue_input_to_audio, input_device)
,	_setup ()
,	_bank_index (0)
,	_preset_index (0)
,	_preset_cur ()
,	_layout_cur ()
,	_pi_id_list ()
,	_pedal_state_arr ()
,	_hold_time (2 * 1000*1000) // 2 s
,	_edit_flag (false)
,	_edit_preset_flag (false)
,	_tuner_flag (false)
,	_tuner_pi_id (-1)
,	_tuner_ptr (0)
,	_input_device (input_device)
,	_queue_input_to_cmd (queue_input_to_cmd)
,	_obs_ptr (0)
,	_dummy_mix_id (_central.get_dummy_mix_id ())
,	_slot_info ()
{
	_central.set_callback (this);
}



Model::~Model ()
{
	// Flush the queue
	ui::UserInputInterface::MsgCell * cell_ptr = 0;
	do
	{
		cell_ptr = _queue_input_to_cmd.dequeue ();
		if (cell_ptr != 0)
		{
			_input_device.return_cell (*cell_ptr);
		}
	}
	while (cell_ptr != 0);
}



void	Model::set_process_info (double sample_freq, int max_block_size)
{
	_central.set_process_info (sample_freq, max_block_size);
}



void	Model::process_block (float * const * dst_arr, const float * const * src_arr, int nbr_spl)
{
	_central.process_block (dst_arr, src_arr, nbr_spl);
}



bool	Model::check_signal_clipping ()
{
	return _central.check_signal_clipping ();
}



// obs_ptr can be 0 to remove the observer.
void	Model::set_observer (ModelObserverInterface *obs_ptr)
{
	_obs_ptr = obs_ptr;
}



// Plug-in must have been instantiated at least once.
const piapi::PluginState &	Model::use_default_settings (std::string model) const
{
	return _central.use_default_settings (model);
}



void	Model::process_messages ()
{
	_central.process_queue_audio_to_cmd ();
	process_msg_ui ();

	// Checks hold state for pedals
	const int64_t  date_us = _central.get_cur_date ();
	for (int ped_cnt = 0; ped_cnt < int (_pedal_state_arr.size ()); ++ped_cnt)
	{
		PedalState &   state = _pedal_state_arr [ped_cnt];
		if (   state._press_flag && ! state._hold_flag
		    && date_us >= state._press_ts + _hold_time)
		{
			state._hold_flag = true;
			process_pedal_event (ped_cnt, doc::ActionTrigger_HOLD);
		}
	}

	// Frequency detected by the tuner
	if (_obs_ptr != 0 && _tuner_flag && _tuner_ptr != 0)
	{
		const float    freq = _tuner_ptr->get_freq ();
		_obs_ptr->set_tuner_freq (freq);
	}
}



void	Model::set_edit_mode (bool edit_flag)
{
	if (edit_flag != _edit_flag)
	{
		if (! edit_flag && _edit_flag && _edit_preset_flag)
		{
			store_preset (_preset_index);
		}

		_edit_flag = edit_flag;

		if (_obs_ptr != 0)
		{
			_obs_ptr->set_edit_mode (edit_flag);
		}
	}
}



void	Model::set_pedalboard_layout (const doc::PedalboardLayout &layout)
{
	_setup._layout = layout;

	if (_obs_ptr != 0)
	{
		_obs_ptr->set_pedalboard_layout (layout);
	}

	update_layout ();
}



void	Model::set_bank (int index, const doc::Bank &bank)
{
	assert (index >= 0);
	assert (index < Cst::_nbr_banks);

	_setup._bank_arr [index] = bank;

	if (_obs_ptr != 0)
	{
		_obs_ptr->set_bank (index, bank);
	}

	if (_edit_flag && index == _bank_index)
	{
		_edit_preset_flag = false;
		activate_preset (_preset_index);
	}
}



// In edit mode, current preset is automatically activated upon bank change.
void	Model::select_bank (int index)
{
	assert (index >= 0);
	assert (index < Cst::_nbr_banks);

	if (index != _bank_index)
	{
		if (_edit_flag && _edit_preset_flag)
		{
			store_preset (_preset_index);
			_edit_preset_flag = false;
		}

		_bank_index = index;

		if (_obs_ptr != 0)
		{
			_obs_ptr->select_bank (index);
		}

		if (_edit_flag)
		{
			activate_preset (_preset_index);
		}
	}
}



void	Model::set_bank_name (std::string name)
{
	_setup._bank_arr [_bank_index]._name = name;
	if (_obs_ptr != 0)
	{
		_obs_ptr->set_bank_name (name);
	}
}



void	Model::set_preset_name (std::string name)
{
	_preset_cur._name = name;
	if (_obs_ptr != 0)
	{
		_obs_ptr->set_preset_name (name);
	}
}



void	Model::activate_preset (int index)
{
	assert (index >= 0);
	assert (index < Cst::_nbr_presets_per_bank);

	if (_edit_flag && _edit_preset_flag)
	{
		store_preset (_preset_index);
	}

	_preset_index = index;
	_preset_cur   = _setup._bank_arr [_bank_index]._preset_arr [_preset_index];
	if (_obs_ptr != 0)
	{
		_obs_ptr->activate_preset (index);
	}

	_edit_preset_flag = true;

	update_layout ();
}



void	Model::store_preset (int index)
{
	assert (index >= 0);
	assert (index < Cst::_nbr_presets_per_bank);

	_setup._bank_arr [_bank_index]._preset_arr [index] = _preset_cur;

	if (_obs_ptr != 0)
	{
		_obs_ptr->store_preset (index);
	}
}



void	Model::set_nbr_slots (int nbr_slots)
{
	assert (nbr_slots >= 0);

	const int         nbr_slots_old = int (_preset_cur._slot_list.size ());
	for (int slot_index = nbr_slots_old - 1
	;	slot_index >= nbr_slots
	;	-- slot_index)
	{
		remove_plugin (slot_index);
	}

	_preset_cur._slot_list.resize (nbr_slots);

	if (_obs_ptr != 0)
	{
		_obs_ptr->set_nbr_slots (nbr_slots);
	}
}



void	Model::insert_slot (int slot_index)
{
	assert (slot_index >= 0);
	assert (slot_index <= int (_preset_cur._slot_list.size ()));

	_preset_cur._slot_list.insert (
		_preset_cur._slot_list.begin () + slot_index,
		doc::Preset::SlotSPtr ()
	);

	if (_obs_ptr != 0)
	{
		_obs_ptr->insert_slot (slot_index);
	}

	update_layout ();
}



void	Model::erase_slot (int slot_index)
{
	assert (slot_index >= 0);
	assert (slot_index < int (_preset_cur._slot_list.size ()));

	remove_plugin (slot_index);

	_preset_cur._slot_list.erase (
		_preset_cur._slot_list.begin () + slot_index
	);

	if (_obs_ptr != 0)
	{
		_obs_ptr->erase_slot (slot_index);
	}

	update_layout ();
}



void	Model::set_plugin (int slot_index, std::string model)
{
	assert (slot_index >= 0);
	assert (slot_index < int (_preset_cur._slot_list.size ()));
	assert (! model.empty ());

	doc::Preset::SlotSPtr &	slot_sptr = _preset_cur._slot_list [slot_index];
	if (slot_sptr.get () == 0)
	{
		slot_sptr = doc::Preset::SlotSPtr (new doc::Slot);
	}
	slot_sptr->_pi_model = model;

	apply_settings ();

	if (_obs_ptr != 0)
	{
		ModelObserverInterface::PluginInitData pi_data;
		fill_pi_init_data (slot_index, pi_data);

		_obs_ptr->set_plugin (slot_index, pi_data);

		// Parameter values
		const int      nbr_param =
			pi_data._nbr_param_arr [piapi::ParamCateg_GLOBAL];
		const int      pi_id = _pi_id_list [slot_index]._pi_id_arr [PiType_MAIN];
		for (int p = 0; p < nbr_param; ++p)
		{
			const float    val = slot_sptr->_settings_all [model]._param_list [p];
			_obs_ptr->set_param (pi_id, p, val, slot_index, PiType_MAIN);
		}
	}
}



void	Model::remove_plugin (int slot_index)
{
	assert (slot_index >= 0);
	assert (slot_index < int (_preset_cur._slot_list.size ()));

	doc::Preset::SlotSPtr &	slot_sptr = _preset_cur._slot_list [slot_index];
	if (slot_sptr.get () != 0)
	{
		slot_sptr->_pi_model.clear ();
	}

	if (_obs_ptr != 0)
	{
		_obs_ptr->remove_plugin (slot_index);
	}

	apply_settings ();
}



void	Model::set_param (int slot_index, PiType type, int index, float val)
{
	assert (slot_index >= 0);
	assert (slot_index < int (_preset_cur._slot_list.size ()));
	assert (! _preset_cur.is_slot_empty (slot_index));
	assert (type >= 0);
	assert (type < PiType_NBR_ELT);
	assert (index >= 0);
	assert (val >= 0);
	assert (val <= 1);

	int            pi_id = _pi_id_list [slot_index]._pi_id_arr [type];
	assert (pi_id >= 0);

	if (! _tuner_flag && pi_id != _dummy_mix_id)
	{
		_central.set_param (pi_id, index, val);
	}

	update_parameter (_preset_cur, slot_index, type, index, val);

	// Add the mixer plug-in if necessary but don't remove it if not.
	// This avoids clicks when switching back and forth between configurations
	// with and without mixer. The removal will be effective at the next call
	// to apply_settings.
	if (pi_id == _dummy_mix_id && has_mixer_plugin (_preset_cur, slot_index))
	{
		apply_settings ();
		pi_id = _pi_id_list [slot_index]._pi_id_arr [type];
		assert (pi_id >= 0);
	}

	if (_obs_ptr != 0)
	{
		_obs_ptr->set_param (pi_id, index, val, slot_index, type);
	}
}



void	Model::set_param_ctrl (int slot_index, PiType type, int index, const doc::CtrlLinkSet &cls)
{
	assert (slot_index >= 0);
	assert (slot_index < int (_preset_cur._slot_list.size ()));
	assert (! _preset_cur.is_slot_empty (slot_index));
	assert (type >= 0);
	assert (type < PiType_NBR_ELT);
	assert (index >= 0);

	doc::Slot &    slot = *(_preset_cur._slot_list [slot_index]);
	doc::PluginSettings &   settings = slot.use_settings (type);
	assert (index < int (settings._param_list.size ()));

	settings._map_param_ctrl [index] = cls;

	int            pi_id = _pi_id_list [slot_index]._pi_id_arr [type];
	assert (pi_id >= 0);
	const bool     cls_empty_flag =
		(cls._bind_sptr.get () == 0 && cls._mod_arr.empty ());

	if (pi_id == _dummy_mix_id)
	{
		if (! cls_empty_flag)
		{
			apply_settings ();
		}
	}
	else if (! has_mixer_plugin (_preset_cur, slot_index))
	{
		apply_settings ();
	}
	else if (! _tuner_flag)
	{
		_central.set_mod (pi_id, index, cls);
		_central.commit ();
	}

	if (_obs_ptr != 0)
	{
		// apply_settings() could have changed the list
		pi_id = _pi_id_list [slot_index]._pi_id_arr [type];

		_obs_ptr->set_param_ctrl (slot_index, type, index, cls);
	}
}



std::vector <std::string>	Model::list_plugin_models () const
{
	return std::move (_central.use_pi_pool ().list_models ());
}



const piapi::PluginDescInterface &	Model::get_model_desc (std::string model_id) const
{
	return _central.use_pi_pool ().get_model_desc (model_id);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Model::do_process_msg_audio_to_cmd (const Msg &msg)
{
	if (msg._type == mfx::Msg::Type_PARAM)
	{
		const int      pi_id = msg._content._param._plugin_id;
		const int      index = msg._content._param._index;
		const float    val   = msg._content._param._val;

		int            slot_index;
		PiType         type;
		find_slot_type_cur_preset (slot_index, type, pi_id);
		if (slot_index >= 0)
		{
			const bool     ok_flag =
				update_parameter (_preset_cur, slot_index, type, index, val);

			if (ok_flag && _obs_ptr != 0)
			{
				_obs_ptr->set_param (pi_id, index, val, slot_index, type);
			}
		}
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Model::SlotPiId::SlotPiId ()
:	_pi_id_arr ({{ -1, -1 }})
{
	// Nothing
}



void	Model::update_layout ()
{
	_layout_cur = _setup._layout;
	_layout_cur.merge_layout (_setup._bank_arr [_bank_index]._layout);
	_layout_cur.merge_layout (_preset_cur._layout);

	apply_settings ();
}



void	Model::preinstantiate_all_plugins_from_bank ()
{
	const doc::Bank & bank = _setup._bank_arr [_bank_index];

	// Counts all the plug-ins used in the bank

	// [model] = count
	std::map <std::string, int>   pi_cnt_bank;

	for (size_t preset_index = 0
	;	preset_index < bank._preset_arr.size ()
	;	++preset_index)
	{
		// Count for this preset
		const doc::Preset &  preset = bank._preset_arr [preset_index];
		std::map <std::string, int>  pi_cnt_preset;
		for (const auto &slot_sptr : preset._slot_list)
		{
			if (slot_sptr.get () != 0 && ! slot_sptr->is_empty ())
			{
				const doc::Slot & slot = *slot_sptr;
				auto           it = pi_cnt_preset.find (slot._pi_model);
				if (it == pi_cnt_preset.end ())
				{
					pi_cnt_preset [slot._pi_model] = 1;
				}
				else
				{
					++ it->second;
				}
			}
		}

		// Merge with bank results
		for (const auto &elt : pi_cnt_preset)
		{
			auto           it = pi_cnt_bank.find (elt.first);
			if (it == pi_cnt_bank.end ())
			{
				pi_cnt_bank.insert (*it);
			}
			else
			{
				it->second = std::max (it->second, elt.second);
			}
		}
	}

	// Instantiate all the plug-ins

	/*** To do ***/


}



// Transmit to _central a preset built from scratch,
// based on _preset_cur and _layout_cur
void	Model::apply_settings ()
{
	_central.clear ();

	if (_tuner_flag)
	{
		apply_settings_tuner ();
	}
	else
	{
		apply_settings_normal ();
	}

	// Done.
	_central.commit ();

	if (! _tuner_flag)
	{
		build_slot_info ();

		if (_obs_ptr != 0)
		{
			notify_slot_info ();
		}
	}
}



void	Model::apply_settings_normal ()
{
	_pi_id_list.clear ();
	_slot_info.clear ();

	const int      nbr_slots = _preset_cur._slot_list.size ();

	int            slot_index_central = 0;
	for (int slot_index = 0; slot_index < nbr_slots; ++slot_index)
	{
		_pi_id_list.push_back (SlotPiId ());

		// Full slot
		if (! _preset_cur.is_slot_empty (slot_index))
		{
			_central.insert_slot (slot_index_central);
			doc::Slot &    slot = *(_preset_cur._slot_list [slot_index]);

			// Check first if we need a mixer plug-in.
			// Updates the parameters
			check_mixer_plugin (slot_index, slot_index_central);

			// Now the main plug-in
			auto           it_s  = slot._settings_all.find (slot._pi_model);
			int            pi_id = -1;
			if (it_s == slot._settings_all.end ())
			{
				// Probably the first creation on this slot.
				// Creates the plug-ins and collects the parameter list
				pi_id = _central.set_plugin (
					slot_index_central,
					slot._pi_model,
					false
				);
				doc::PluginSettings &	settings =
					slot._settings_all [slot._pi_model];
				settings._param_list =
					_central.use_pi_pool ().use_plugin (pi_id)._param_arr;
				_pi_id_list [slot_index]._pi_id_arr [PiType_MAIN] = pi_id;
			}
			else
			{
				// Creates the plug-in and apply the stored settings
				_central.force_mono (
					slot_index_central,
					it_s->second._force_mono_flag
				);
				pi_id = _central.set_plugin (
					slot_index_central,
					slot._pi_model,
					it_s->second._force_reset_flag
				);
				_pi_id_list [slot_index]._pi_id_arr [PiType_MAIN] = pi_id;
				send_effect_settings (pi_id, it_s->second);
			}

			++ slot_index_central;
		}
	}
}



void	Model::apply_settings_tuner ()
{
	assert (_tuner_flag);

	_central.insert_slot (0);
	_central.remove_mixer (0);

	_tuner_pi_id = _central.set_plugin (0, Cst::_plugin_tuner, false);

	const PluginPool::PluginDetails &   details =
		_central.use_pi_pool ().use_plugin (_tuner_pi_id);
	_tuner_ptr = dynamic_cast <pi::tuner::Tuner *> (details._pi_uptr.get ());
	assert (_tuner_ptr != 0);
}



// Adds or removes the mixer plug-in, depending on the current
// settings.
// We need it if there is an automation or if it is set to something
// different of 100% wet at 0 dB.
// Does not commit anything
// The slot should exist in the current preset.
void	Model::check_mixer_plugin (int slot_index, int slot_index_central)
{
	assert (_preset_cur._slot_list [slot_index].get () != 0);

	const bool        use_flag = has_mixer_plugin (_preset_cur, slot_index);	
	const doc::Slot & slot     = *(_preset_cur._slot_list [slot_index]);
	int &             id_ref   = _pi_id_list [slot_index]._pi_id_arr [PiType_MIX];

	// Instantiation and setting update
	if (use_flag)
	{
		if (id_ref < 0 || id_ref == _dummy_mix_id)
		{
			const int      pi_id = _central.set_mixer (slot_index_central);
			id_ref = pi_id;
			send_effect_settings (pi_id, slot._settings_mixer);
		}
	}

	// Removal
	else
	{
		if (id_ref >= 0 && id_ref != _dummy_mix_id)
		{
			_central.remove_mixer (slot_index_central);
		}
		id_ref = _dummy_mix_id;
	}
}



// Slot must exist
bool	Model::has_mixer_plugin (const doc::Preset &preset, int slot_index)
{
	assert (slot_index >= 0);
	assert (slot_index < int (preset._slot_list.size ()));
	assert (! preset.is_slot_empty (slot_index));

	const doc::Slot & slot = *(preset._slot_list [slot_index]);
	const doc::PluginSettings::ParamList & plist =
		slot._settings_mixer._param_list;

	bool           use_flag = (
		   ! slot._settings_mixer._map_param_ctrl.empty ()
		|| plist [pi::dwm::Param_BYPASS] != 0
		|| plist [pi::dwm::Param_WET   ] != 1
		|| plist [pi::dwm::Param_GAIN  ] != pi::dwm::DryWetDesc::_gain_neutral
	);

	// Check if it is referenced in the pedals
	for (int p = 0; p < Cst::_nbr_pedals && ! use_flag; ++p)
	{
		const doc::PedalActionGroup & group = _layout_cur._pedal_arr [p];
		for (size_t t = 0; t < group._action_arr.size () && ! use_flag; ++t)
		{
			const doc::PedalActionCycle & cycle = group._action_arr [t];
			for (size_t a = 0; a < cycle._cycle.size () && ! use_flag; ++a)
			{
				const doc::PedalActionCycle::ActionArray &   aa = cycle._cycle [a];
				for (size_t u = 0; u < aa.size () && ! use_flag; ++u)
				{
					const doc::PedalActionSingleInterface &   action = *(aa [u]);
					if (action.get_type () == doc::ActionType_PARAM)
					{
						const doc::ActionParam &   ap =
							dynamic_cast <const doc::ActionParam &> (action);
						if (ap._fx_id._type == PiType_MIX)
						{
							if (ap._fx_id._location_type == doc::FxId::LocType_CATEGORY)
							{
								use_flag = (ap._fx_id._label_or_model == slot._pi_model);
							}
							else
							{
								use_flag = (ap._fx_id._label_or_model == slot._label);
							}
						}
					}
				}
			}
		}
	}

	return use_flag;
}



// Does not commit anything
void	Model::send_effect_settings (int pi_id, const doc::PluginSettings &settings)
{
	// Parameters
	PluginPool::PluginDetails &   details =
		_central.use_pi_pool ().use_plugin (pi_id);
	const int      nbr_param =
		details._desc_ptr->get_nbr_param (piapi::ParamCateg_GLOBAL);
	for (int p_index = 0; p_index < nbr_param; ++p_index)
	{
		const float    val = settings._param_list [p_index];
		_central.set_param (pi_id, p_index, val);
	}

	// Modulations and automations
	for (const auto &x : settings._map_param_ctrl)
	{
		const int   p_index = x.first;
		_central.set_mod (pi_id, p_index, x.second);
	}
}



void	Model::process_msg_ui ()
{
	ui::UserInputInterface::MsgCell * cell_ptr = 0;
	do
	{
		cell_ptr = _queue_input_to_cmd.dequeue ();
		if (cell_ptr != 0)
		{
			const ui::UserInputType type = cell_ptr->_val.get_type ();
			if (type == ui::UserInputType_SW)
			{
				const int      switch_index = cell_ptr->_val.get_index ();
				const int      pedal_index  = find_pedal (switch_index);
				if (pedal_index < 0)
				{
					// We shouldn't have been connected to this switch
					assert (false);
				}
				else
				{
					const float    val  = cell_ptr->_val.get_val ();
					const int64_t  date = cell_ptr->_val.get_date ();
					process_pedal (pedal_index, (val >= 0.5f), date);
				}
			}
			else
			{
				// We shouldn't have been connected to this input
				assert (false);
			}

			_input_device.return_cell (*cell_ptr);
		}
	}
	while (cell_ptr != 0);
}



// -1 if not found
int	Model::find_pedal (int switch_index) const
{
	assert (switch_index >= 0);

	int            pedal_index = -1;

	const int      nbr_pedals = int (_pedal_to_switch_map.size ());
	for (int k = 0; k < nbr_pedals && pedal_index < 0; ++ k)
	{
		if (_pedal_to_switch_map [k] == switch_index)
		{
			pedal_index = k;
		}
	}

	return pedal_index;
}



void	Model::process_pedal (int pedal_index, bool set_flag, int64_t date)
{
	assert (pedal_index >= 0);
	assert (pedal_index < Cst::_nbr_pedals);

	PedalState &   state = _pedal_state_arr [pedal_index];

	if (set_flag)
	{
		state._hold_flag  = false;
		state._press_flag = true;
		state._press_ts   = date;
		process_pedal_event (pedal_index, doc::ActionTrigger_PRESS);
	}
	else
	{
		state._hold_flag  = false;
		state._press_flag = false;
		state._press_ts   = date;
		process_pedal_event (pedal_index, doc::ActionTrigger_RELEASE);
	}
}



// Commits modifications
void	Model::process_pedal_event (int pedal_index, doc::ActionTrigger trigger)
{
	const doc::PedalActionGroup & group = _layout_cur._pedal_arr [pedal_index];
	const doc::PedalActionCycle & cycle = group._action_arr [trigger];
	const int      nbr_actions = int (cycle._cycle.size ());

	PedalState &   state = _pedal_state_arr [pedal_index];

	if (nbr_actions > 0)
	{
		int            cycle_pos = 0;
		if (trigger == doc::ActionTrigger_PRESS)
		{
			cycle_pos = state._cycle_pos;
			if (cycle_pos >= nbr_actions)
			{
				cycle_pos = 0;
			}
		}

		// Make a copy instead of taking a reference.
		// The copy is quick because the array is made of shared_ptr.
		// We need a copy because the current layout may be modified
		// by the processed action(s).
		const doc::PedalActionCycle::ActionArray  action_list =
			cycle._cycle [cycle_pos];

		for (const auto &action_sptr : action_list)
		{
			assert (action_sptr.get () != 0);
			const doc::PedalActionSingleInterface &   action = *action_sptr;
			process_action (action);
		}

		if (trigger == doc::ActionTrigger_PRESS)
		{
			++ cycle_pos;
			if (cycle_pos >= nbr_actions)
			{
				cycle_pos = 0;
			}
			state._cycle_pos = cycle_pos;
		}
	}

	_central.commit ();
}



void	Model::process_action (const doc::PedalActionSingleInterface &action)
{
	const doc::ActionType   type = action.get_type ();

	switch (type)
	{
	case doc::ActionType_BANK:
		process_action_bank (dynamic_cast <const doc::ActionBank &> (action));
		break;

	case doc::ActionType_PARAM:
		process_action_param (dynamic_cast <const doc::ActionParam &> (action));
		break;

	case doc::ActionType_PRESET:
		process_action_preset (dynamic_cast <const doc::ActionPreset &> (action));
		break;

	case doc::ActionType_TOGGLE_FX:
		process_action_toggle_fx (dynamic_cast <const doc::ActionToggleFx &> (action));
		break;

	case doc::ActionType_TOGGLE_TUNER:
		process_action_toggle_tuner (dynamic_cast <const doc::ActionToggleTuner &> (action));
		break;

	default:
		assert (false);
	}
}



void	Model::process_action_bank (const doc::ActionBank &action)
{
	int            new_index = action._val;
	if (action._relative_flag)
	{
		new_index += _bank_index;
		new_index += Cst::_nbr_banks;
		assert (new_index >= 0);
		new_index %= Cst::_nbr_banks;
	}

	select_bank (new_index);
}



void	Model::process_action_param (const doc::ActionParam &action)
{
	const int      slot_pos = find_slot_cur_preset (action._fx_id);
	if (slot_pos >= 0)
	{
		const int      pi_id =
			_pi_id_list [slot_pos]._pi_id_arr [action._fx_id._type];
		assert (pi_id >= 0);

		if (pi_id != _dummy_mix_id)
		{
			_central.set_param (pi_id, action._index, action._val);
		}
	}
}



void	Model::process_action_preset (const doc::ActionPreset &action)
{
	int            new_index = action._val;
	if (action._relative_flag)
	{
		new_index += _preset_index;
		new_index += Cst::_nbr_presets_per_bank;
		assert (new_index >= 0);
		new_index %= Cst::_nbr_presets_per_bank;
	}

	activate_preset (new_index);
}



void	Model::process_action_toggle_fx (const doc::ActionToggleFx &action)
{

	/*** To do ***/

}



void	Model::process_action_toggle_tuner (const doc::ActionToggleTuner &action)
{
	_tuner_flag = ! _tuner_flag;
	if (! _tuner_flag)
	{
		_tuner_ptr   = 0;
		_tuner_pi_id = -1;
	}

	apply_settings ();

	if (_obs_ptr != 0)
	{
		_obs_ptr->set_tuner (_tuner_flag);
	}
}



void	Model::build_slot_info ()
{
	_slot_info.clear ();
	PluginPool &   pi_pool = _central.use_pi_pool ();

	for (const SlotPiId &spi : _pi_id_list)
	{
		ModelObserverInterface::SlotInfo info;
		for (size_t type = 0; type < spi._pi_id_arr.size (); ++type)
		{
			const int      pi_id = spi._pi_id_arr [type];
			if (pi_id >= 0)
			{
				const PluginPool::PluginDetails &   details =
					pi_pool.use_plugin (pi_id);
				info [type] = ModelObserverInterface::PluginInfoSPtr (
					new ModelObserverInterface::PluginInfo (
						*details._pi_uptr,
						*details._desc_ptr,
						details._param_arr
					)
				);
			}
		}

		_slot_info.push_back (info);
	}
}



void	Model::notify_slot_info ()
{
	assert (_obs_ptr != 0);
	assert (_slot_info.size () == _pi_id_list.size ());

	_obs_ptr->set_slot_info_for_current_preset (_slot_info);
}



// Returns -1 if not found
int	Model::find_slot_cur_preset (const doc::FxId &fx_id) const
{
	assert (fx_id._location_type >= 0);
	assert (fx_id._location_type < doc::FxId::LocType_NBR_ELT);

	int            found_pos = -1;

	const int      nbr_slots = _preset_cur._slot_list.size ();
	for (int pos = 0; pos < nbr_slots && found_pos < 0; ++pos)
	{
		if (! _preset_cur.is_slot_empty (pos))
		{
			const doc::Slot & slot = *(_preset_cur._slot_list [pos]);

			if (fx_id._location_type == doc::FxId::LocType_CATEGORY)
			{
				if (slot._pi_model == fx_id._label_or_model)
				{
					found_pos = pos;
				}
			}
			else
			{
				if (! slot._label.empty () && fx_id._label_or_model == slot._label)
				{
					found_pos = pos;
				}
			}
		}
	}

	return found_pos;
}



// Returns -1 in slot_index if not found
void	Model::find_slot_type_cur_preset (int &slot_index, PiType &type, int pi_id) const
{
	assert (pi_id >= 0);
	assert (pi_id != _dummy_mix_id);

	slot_index = -1;
	const int      nbr_slots = _preset_cur._slot_list.size ();
	assert (nbr_slots == int (_pi_id_list.size ()));
	for (int pos = 0; pos < nbr_slots && slot_index < 0; ++pos)
	{
		const SlotPiId &  spi = _pi_id_list [pos];
		for (int tt = 0; tt < PiType_NBR_ELT && slot_index < 0; ++tt)
		{
			if (spi._pi_id_arr [tt] == pi_id)
			{
				slot_index = pos;
				type       = PiType (tt);
			}
		}
	}
}



bool	Model::update_parameter (doc::Preset &preset, int slot_index, PiType type, int index, float val)
{
	bool           ok_flag = true;

	if (preset.is_slot_empty (slot_index))
	{
		ok_flag = false;
		assert (false);
	}
	else
	{
		doc::Slot &    slot = *(preset._slot_list [slot_index]);
		doc::PluginSettings &  settings =
				(type == PiType_MIX)
			? slot._settings_mixer
			: slot._settings_all [slot._pi_model];
		if (index >= int (settings._param_list.size ()))
		{
			ok_flag = false;
			assert (false);
		}
		else
		{
			settings._param_list [index] = val;
		}
	}

	return ok_flag;
}



void	Model::fill_pi_init_data (int slot_index, ModelObserverInterface::PluginInitData &pi_data)
{
	assert (! _preset_cur.is_slot_empty (slot_index));

	const doc::Slot & slot = *(_preset_cur._slot_list [slot_index]);
	pi_data._model = slot._pi_model;

	const int      pi_id = _pi_id_list [slot_index]._pi_id_arr [PiType_MAIN];
	assert (pi_id >= 0);

	const PluginPool::PluginDetails &   details =
		_central.use_pi_pool ().use_plugin (pi_id);
	pi_data._nbr_io_arr [piapi::PluginInterface::Dir_IN ] = 1;
	pi_data._nbr_io_arr [piapi::PluginInterface::Dir_OUT] = 1;
	details._desc_ptr->get_nbr_io (
		pi_data._nbr_io_arr [piapi::PluginInterface::Dir_IN ],
		pi_data._nbr_io_arr [piapi::PluginInterface::Dir_OUT]
	);
	for (int categ = 0; categ < piapi::ParamCateg_NBR_ELT; ++categ)
	{
		pi_data._nbr_param_arr [categ] =
			details._desc_ptr->get_nbr_param (piapi::ParamCateg (categ));
	}
	pi_data._prefer_stereo_flag = details._desc_ptr->prefer_stereo ();
}



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
