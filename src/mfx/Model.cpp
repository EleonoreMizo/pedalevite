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

#include "mfx/pi/DryWet.h"
#include "mfx/pi/Tuner.h"
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
{ {
	2, 3, 4, 5, 6, 7, 8, 9,
	14, 15, 16, 17
} };



Model::Model (ui::UserInputInterface::MsgQueue &queue_input_to_cmd, ui::UserInputInterface::MsgQueue &queue_input_to_audio, ui::UserInputInterface &input_device)
:	_central (queue_input_to_audio, input_device)
,	_bank ()
,	_preset_index (0)
,	_layout_cur ()
,	_layout_base ()
,	_layout_bank ()
,	_preset_cur ()
,	_pi_id_list ()
,	_pedal_state_arr ()
,	_tuner_flag (false)
,	_tuner_ptr (0)
,	_input_device (input_device)
,	_queue_input_to_cmd (queue_input_to_cmd)
,	_slot_info ()
{
	// Nothing
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



// obs_ptr can be 0.
void	Model::set_observer (ModelObserverInterface *obs_ptr)
{
	_obs_ptr = obs_ptr;
}



void	Model::process_messages ()
{
	_central.process_queue_audio_to_cmd ();
	process_msg_ui ();

	/*** To do: check hold state for pedals ***/

	if (_obs_ptr != 0 && _tuner_flag && _tuner_ptr)
	{
		const float    freq = _tuner_ptr->get_freq ();
		_obs_ptr->set_tuner_freq (freq);
	}
}



void	Model::load_pedalboard_layout (const doc::PedalboardLayout &layout)
{
	_layout_base = layout;

	update_layout_bank ();
}



void	Model::load_bank (const doc::Bank &bank, int preset)
{
	assert (preset >= 0);
	assert (preset < int (_bank._preset_arr.size ()));

	_bank         = bank;
	_preset_index = preset;

	update_layout_bank ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Model::SlotPiId::SlotPiId ()
:	_pi_id_arr ({{ -1, -1 }})
{
	// Nothing
}



void	Model::update_layout_bank ()
{
	_layout_bank = _layout_base;
	_layout_bank.merge_layout (_bank._layout);

	_layout_cur  = _layout_bank;
	_layout_cur.merge_layout (_bank._preset_arr [_preset_index]._layout);

	apply_settings ();
}



void	Model::preinstantiate_all_plugins_from_bank ()
{
	// Counts all the plug-ins used in the bank

	// [model] = count
	std::map <pi::PluginModel, int>  pi_cnt_bank;

	for (size_t preset_index = 0
	;	preset_index < _bank._preset_arr.size ()
	;	++preset_index)
	{
		// Count for this preset
		const doc::Preset &  preset = _bank._preset_arr [preset_index];
		std::map <pi::PluginModel, int>  pi_cnt_preset;
		for (const auto &slot_sptr : preset._slot_list)
		{
			if (slot_sptr.get () != 0)
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



void	Model::apply_settings ()
{
	_central.clear ();
	_pi_id_list.clear ();
	_slot_info.clear ();

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

	build_slot_info ();

	if (_obs_ptr != 0)
	{
		notify_slot_info ();
	}
}



void	Model::apply_settings_normal ()
{
	// Don't delete _preset_cur, we need it to check the differences
	// with the new preset

	const doc::Preset &  preset = _bank._preset_arr [_preset_index];
	_preset_cur._name = preset._name;

	const int      nbr_slots = preset._slot_list.size ();
	for (int slot_index = 0; slot_index < nbr_slots; ++slot_index)
	{
		_central.insert_slot (slot_index);
		_pi_id_list.push_back (SlotPiId ());

		// Empty slot
		if (preset._slot_list [slot_index].get () == 0)
		{
			_preset_cur._slot_list [slot_index].reset ();
		}

		// Full slot
		else
		{
			const doc::Slot & slot     = *(preset._slot_list [slot_index]);

			// Create it if it doesn't exist
			if (_preset_cur._slot_list [slot_index].get () == 0)
			{
				_preset_cur._slot_list [slot_index] = doc::Preset::SlotSPtr (
					new doc::Slot
				);
			}
			doc::Slot &       slot_cur = *(_preset_cur._slot_list [slot_index]);

			slot_cur._label = slot._label;

			// First check if we need a mixer plug-in.
			// Updates the parameters
			check_mixer_plugin (slot_index);

			// Now the main plug-in
			const int      pi_id =
				_central.set_plugin (slot_index, slot._pi_model);
			_pi_id_list [slot_index]._pi_id_arr [PiType_MIX] = pi_id;

			// Settings
			slot_cur._settings_all = slot._settings_all;
			auto           it_s = slot_cur._settings_all.find (slot._pi_model);
			if (it_s == slot_cur._settings_all.end ())
			{
				assert (false);
			}
			else
			{
				_central.force_mono (slot_index, it_s->second._force_mono_flag);
				send_effect_settings (pi_id, it_s->second);
			}
		}
	}
}



void	Model::apply_settings_tuner ()
{
	_central.insert_slot (0);
	_pi_id_list.resize (1);
	_central.remove_mixer (0);

	const int      tuner_id = _central.set_plugin (0, pi::PluginModel_TUNER);
	_pi_id_list [0]._pi_id_arr [PiType_MAIN] = tuner_id;

	const PluginPool::PluginDetails &   details =
		_central.use_pi_pool ().use_plugin (tuner_id);
	_tuner_ptr = dynamic_cast <pi::Tuner *> (details._pi_uptr.get ());
	assert (_tuner_ptr != 0);
}



// Adds or removes the mixer plug-in, depending on the current
// settings.
// We need it if there is an automation or if it is set to something
// different of 100% wet at 0 dB.
// Does not commit anything
// The slot should exist both in bank and in the current preset.
void	Model::check_mixer_plugin (int slot_index)
{
	assert (_preset_cur._slot_list [slot_index].get () != 0);

	const doc::Preset &  preset = _bank._preset_arr [_preset_index];
	assert (preset._slot_list [slot_index].get () != 0);

	const bool     use_cur_flag = has_mixer_plugin (_preset_cur, slot_index);
	const bool     use_new_flag = has_mixer_plugin (preset     , slot_index);
	
	const doc::Slot & slot     = *(     preset._slot_list [slot_index]);
	doc::Slot &       slot_cur = *(_preset_cur._slot_list [slot_index]);

	// Instantiation
	if (! use_cur_flag && use_new_flag)
	{
		const int      pi_id = _central.set_mixer (slot_index);
		_pi_id_list [slot_index]._pi_id_arr [PiType_MIX] = pi_id;
	}

	// Removal
	else if (use_cur_flag && ! use_new_flag)
	{
		_central.remove_mixer (slot_index);
		slot_cur._settings_mixer = slot._settings_mixer;
		_pi_id_list [slot_index]._pi_id_arr [PiType_MIX] = -1;
	}

	// Setting update
	if (use_new_flag)
	{
		const int      pi_id = _pi_id_list [slot_index]._pi_id_arr [PiType_MIX];
		slot_cur._settings_mixer = slot._settings_mixer;
		send_effect_settings (pi_id, slot_cur._settings_mixer);
	}
}



bool	Model::has_mixer_plugin (const doc::Preset &preset, int slot_index)
{
	assert (slot_index >= 0);
	assert (slot_index < int (preset._slot_list.size ()));

	const doc::Slot & slot = *(preset._slot_list [slot_index]);

	const bool     use_flag = (
		   ! slot._settings_mixer._map_param_ctrl.empty ()
		|| slot._settings_mixer._param_list [pi::DryWet::Param_BYPASS] != 0
		|| slot._settings_mixer._param_list [pi::DryWet::Param_WET   ] != 1
		|| slot._settings_mixer._param_list [pi::DryWet::Param_GAIN  ] != pi::DryWet::_gain_neutral
	);

	return use_flag;
}



// Does not commit anything
void	Model::send_effect_settings (int pi_id, const doc::PluginSettings &settings)
{
	// Parameters
	PluginPool::PluginDetails &   details =
		_central.use_pi_pool ().use_plugin (pi_id);
	const int      nbr_param =
		details._pi_uptr->get_nbr_param (piapi::ParamCateg_GLOBAL);
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

		const doc::PedalActionCycle::ActionArray &   action_list =
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

	/*** To do ***/

}



void	Model::process_action_param (const doc::ActionParam &action)
{
	const int      slot_pos = find_slot_cur_preset (action._fx_id);
	if (slot_pos >= 0)
	{
		const int      pi_id =
			_pi_id_list [slot_pos]._pi_id_arr [action._fx_id._type];
		assert (pi_id >= 0);

		_central.set_param (pi_id, action._index, action._val);
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
	_preset_index = new_index;

	apply_settings ();
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
		_tuner_ptr = 0;
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
		if (_preset_cur._slot_list [pos].get () != 0)
		{
			const doc::Slot & slot = *(_preset_cur._slot_list [pos]);

			if (fx_id._location_type == doc::FxId::LocType_CATEGORY)
			{
				if (slot._pi_model == fx_id._categ)
				{
					found_pos = pos;
				}
			}
			else
			{
				if (! slot._label.empty () && fx_id._label == slot._label)
				{
					found_pos = pos;
				}
			}
		}
	}

	return found_pos;
}



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
