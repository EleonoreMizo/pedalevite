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

#include "fstb/fnc.h"
#include "mfx/pi/dwm/DryWetDesc.h"
#include "mfx/pi/dwm/Param.h"
#include "mfx/pi/tuner/Tuner.h"
#include "mfx/doc/ActionBank.h"
#include "mfx/doc/ActionParam.h"
#include "mfx/doc/ActionPreset.h"
#include "mfx/doc/ActionTempo.h"
#include "mfx/doc/ActionToggleFx.h"
#include "mfx/doc/ActionToggleTuner.h"
#include "mfx/doc/SerRText.h"
#include "mfx/doc/SerWText.h"
#include "mfx/FileIOInterface.h"
#include "mfx/Model.h"
#include "mfx/ModelObserverInterface.h"
#include "mfx/PedalLoc.h"
#include "mfx/ToolsParam.h"

#include <algorithm>
#include <set>

#include <cassert>



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



const std::array <int, Cst::_nbr_pedals>	Model::_pedal_to_switch_map =
{{
	2, 3, 4, 5, 6, 7, 8, 9,
	14, 15, 16, 17
}};



Model::Model (ui::UserInputInterface::MsgQueue &queue_input_to_cmd, ui::UserInputInterface::MsgQueue &queue_input_to_audio, ui::UserInputInterface &input_device, FileIOInterface &file_io)
:	_central (queue_input_to_audio, input_device)
,	_setup ()
,	_bank_index (0)
,	_preset_index (0)
,	_preset_cur ()
,	_layout_cur ()
,	_pi_id_map ()
,	_pedal_state_arr ()
,	_hold_time (2 * 1000*1000) // 2 s
,	_edit_flag (false)
,	_edit_preset_flag (false)
,	_tuner_flag (false)
,	_tuner_pi_id (-1)
,	_tuner_ptr (0)
,	_file_io (file_io)
,	_input_device (input_device)
,	_queue_input_to_cmd (queue_input_to_cmd)
,	_obs_ptr (0)
,	_dummy_mix_id (_central.get_dummy_mix_id ())
,	_tempo_last_ts (_central.get_cur_date () - Cst::_tempo_detection_max * 2)
,	_tempo (Cst::_tempo_ref)
,	_latest_slot_id (0)
,	_slot_info ()
,	_override_map ()
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



MeterResultSet &	Model::use_meters ()
{
	return _central.use_meters ();
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



int	Model::save_to_disk ()
{
	int            ret_val = 0;

	doc::SerWText  ser_w;
	ser_w.clear ();
	_setup.ser_write (ser_w);
	ret_val = ser_w.terminate ();
	if (ret_val == 0)
	{
		const std::string content = ser_w.use_content ();
		std::string    pathname = Cst::_config_dir + "/" + Cst::_config_current;
		ret_val = _file_io.write_txt_file (pathname, content);
	}

	return ret_val;
}



int	Model::load_from_disk ()
{
	int            ret_val = 0;

	std::string    content;

	std::string    pathname = Cst::_config_dir + "/" + Cst::_config_current;
	ret_val = _file_io.read_txt_file (pathname, content);

	if (ret_val == 0)
	{
		doc::SerRText  ser_r;
		ser_r.start (content);
		std::unique_ptr <doc::Setup> sss_uptr (new doc::Setup);
		sss_uptr->ser_read (ser_r);
		ret_val = ser_r.terminate ();
		if (ret_val == 0)
		{
			const int      nbr_banks = int (sss_uptr->_bank_arr.size ());
			assert (nbr_banks == Cst::_nbr_banks);

			set_setup_name (sss_uptr->_name);
			set_pedalboard_layout (sss_uptr->_layout);
			set_save_mode (sss_uptr->_save_mode);
			for (int bank_cnt = 0; bank_cnt < nbr_banks; ++bank_cnt)
			{
				set_bank (bank_cnt, sss_uptr->_bank_arr [bank_cnt]);
			}
			set_chn_mode (sss_uptr->_chn_mode);
			set_master_vol (sss_uptr->_master_vol);

			select_bank (0);
			activate_preset (0);
		}
	}

	return ret_val;
}



void	Model::set_setup_name (std::string name)
{
	_setup._name = name;

	if (_obs_ptr != 0)
	{
		_obs_ptr->set_setup_name (name);
	}
}



void	Model::set_edit_mode (bool edit_flag)
{
	if (edit_flag != _edit_flag)
	{
		if (! edit_flag && _edit_flag && _edit_preset_flag)
		{
			store_preset (_preset_index, -1);
		}

		_edit_flag = edit_flag;

		if (_obs_ptr != 0)
		{
			_obs_ptr->set_edit_mode (edit_flag);
		}
	}
}



void	Model::set_save_mode (doc::Setup::SaveMode mode)
{
	assert (mode >= 0);
	assert (mode < doc::Setup::SaveMode_NBR_ELT);

	_setup._save_mode = mode;

	if (_obs_ptr != 0)
	{
		_obs_ptr->set_save_mode (mode);
	}
}



void	Model::set_pedalboard_layout (const doc::PedalboardLayout &layout)
{
	_setup._layout = layout;

	update_layout ();

	if (_obs_ptr != 0)
	{
		_obs_ptr->set_pedalboard_layout (layout);
	}
}



void	Model::set_pedal (const PedalLoc &loc, const doc::PedalActionGroup &content)
{
	assert (loc._type >= 0);
	assert (loc._type < PedalLoc::Type_NBR_ELT);
	assert (loc._pedal_index >= 0);
	assert (loc._pedal_index < Cst::_nbr_pedals);

	doc::PedalboardLayout * layout_ptr = 0;

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

	case PedalLoc::Type_PRESET:
		assert (loc._bank_index >= 0);
		assert (loc._bank_index < Cst::_nbr_banks);
		assert (loc._preset_index >= 0);
		assert (loc._preset_index < Cst::_nbr_presets_per_bank);
		layout_ptr = &_setup._bank_arr [loc._bank_index]._preset_arr [loc._preset_index]._layout;
		break;

	case PedalLoc::Type_PRESET_CUR:
		layout_ptr = &_preset_cur._layout;
		break;
		
	default:
		assert (false);
		break;
	}

	if (layout_ptr != 0)
	{
		layout_ptr->_pedal_arr [loc._pedal_index] = content;
		update_layout ();
		if (_obs_ptr != 0)
		{
			_obs_ptr->set_pedal (loc, content);
		}
	}
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
			store_preset (_preset_index, -1);
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



void	Model::activate_preset (int preset_index)
{
	assert (preset_index >= 0);
	assert (preset_index < Cst::_nbr_presets_per_bank);

	reset_all_overridden_param_ctrl ();

	if (_edit_flag && _edit_preset_flag && preset_index != _preset_index)
	{
		store_preset (_preset_index, -1);
	}

	_preset_index = preset_index;
	_preset_cur   = _setup._bank_arr [_bank_index]._preset_arr [_preset_index];

	_edit_preset_flag = true;

	update_layout ();

	if (_obs_ptr != 0)
	{
		_obs_ptr->activate_preset (preset_index);
	}

	add_default_ctrl (-1);
}



// bank_index < 0: use the current bank
void	Model::store_preset (int preset_index, int bank_index)
{
	assert (preset_index >= 0);
	assert (preset_index < Cst::_nbr_presets_per_bank);
	assert (bank_index < Cst::_nbr_banks);

	const int         bank_index2 = (bank_index < 0) ? _bank_index : bank_index;

	_setup._bank_arr [bank_index2]._preset_arr [preset_index] = _preset_cur;

	if (_obs_ptr != 0)
	{
		_obs_ptr->store_preset (preset_index, bank_index);
	}
}



void	Model::set_chn_mode (ChnMode mode)
{
	assert (mode >= 0);
	assert (mode < ChnMode_NBR_ELT);

	_setup._chn_mode = mode;
	_central.set_chn_mode (mode);
	_central.commit ();

	if (_obs_ptr != 0)
	{
		_obs_ptr->set_chn_mode (mode);
	}
}



void	Model::set_master_vol (double vol)
{
	assert (vol > 0);

	_setup._master_vol = vol;
	_central.set_master_vol (vol);
	_central.commit ();

	if (_obs_ptr != 0)
	{
		_obs_ptr->set_master_vol (float (vol));
	}
}



int	Model::insert_slot (int slot_index)
{
	assert (slot_index >= 0);
	assert (slot_index <= int (_preset_cur._routing._chain.size ()));

	const int      slot_id = _preset_cur.gen_slot_id ();
	_preset_cur._slot_map.insert (std::make_pair (
		slot_id,
		doc::Preset::SlotSPtr ()
	));
	_preset_cur._routing._chain.insert (
		_preset_cur._routing._chain.begin () + slot_index,
		slot_id
	);

	update_layout ();

	if (_obs_ptr != 0)
	{
		_obs_ptr->insert_slot (slot_index, slot_id);
	}

	return slot_id;
}



void	Model::erase_slot (int slot_index)
{
	assert (slot_index >= 0);
	assert (slot_index < int (_preset_cur._routing._chain.size ()));

	const int      slot_id = _preset_cur._routing._chain [slot_index];

	remove_plugin (slot_id);

	_preset_cur._routing._chain.erase (
		_preset_cur._routing._chain.begin () + slot_index
	);
	auto           it_slot = _preset_cur._slot_map.find (slot_id);
	assert (it_slot != _preset_cur._slot_map.end ());
	_preset_cur._slot_map.erase (it_slot);

	update_layout ();

	if (_obs_ptr != 0)
	{
		_obs_ptr->erase_slot (slot_index);
	}
}



void	Model::set_slot_label (int slot_id, std::string name)
{
	assert (slot_id >= 0);

	auto           it_slot = _preset_cur._slot_map.find (slot_id);
	assert (it_slot != _preset_cur._slot_map.end ());

	doc::Preset::SlotSPtr &	slot_sptr = it_slot->second;
	if (slot_sptr.get () == 0)
	{
		slot_sptr = doc::Preset::SlotSPtr (new doc::Slot);
	}
	slot_sptr->_label = name;

	if (_obs_ptr != 0)
	{
		_obs_ptr->set_slot_label (slot_id, name);
	}
}



void	Model::set_plugin (int slot_id, std::string model)
{
	assert (slot_id >= 0);
	assert (! model.empty ());

	reset_all_overridden_param_ctrl (slot_id);

	auto           it_slot = _preset_cur._slot_map.find (slot_id);
	assert (it_slot != _preset_cur._slot_map.end ());

	doc::Preset::SlotSPtr &	slot_sptr = it_slot->second;
	if (slot_sptr.get () == 0)
	{
		slot_sptr = doc::Preset::SlotSPtr (new doc::Slot);
	}
	slot_sptr->_pi_model = model;

	apply_settings ();

	if (_obs_ptr != 0)
	{
		ModelObserverInterface::PluginInitData pi_data;
		fill_pi_init_data (slot_id, pi_data);

		_obs_ptr->set_plugin (slot_id, pi_data);

		// Parameter values
		const int      nbr_param =
			pi_data._nbr_param_arr [piapi::ParamCateg_GLOBAL];
		const int      pi_id = _pi_id_map [slot_id]._pi_id_arr [PiType_MAIN];
		for (int p = 0; p < nbr_param; ++p)
		{
			const float    val = slot_sptr->_settings_all [model]._param_list [p];
			_obs_ptr->set_param (pi_id, p, val, slot_id, PiType_MAIN);
		}
	}

	add_default_ctrl (slot_id);
}



void	Model::remove_plugin (int slot_id)
{
	assert (slot_id >= 0);

	reset_all_overridden_param_ctrl (slot_id);

	auto           it_slot = _preset_cur._slot_map.find (slot_id);
	assert (it_slot != _preset_cur._slot_map.end ());

	doc::Preset::SlotSPtr &	slot_sptr = it_slot->second;
	if (slot_sptr.get () != 0)
	{
		slot_sptr->_pi_model.clear ();
	}

	apply_settings ();

	if (_obs_ptr != 0)
	{
		_obs_ptr->remove_plugin (slot_id);
	}
}



void	Model::set_plugin_mono (int slot_id, bool mono_flag)
{
	assert (slot_id >= 0);

	doc::Slot &    slot = _preset_cur.use_slot (slot_id);
	doc::PluginSettings &   settings = slot.use_settings (PiType_MAIN);

	settings._force_mono_flag = mono_flag;

	apply_settings ();

	if (_obs_ptr != 0)
	{
		_obs_ptr->set_plugin_mono (slot_id, mono_flag);
	}
}



void	Model::set_param (int slot_id, PiType type, int index, float val)
{
	assert (! _preset_cur.is_slot_empty (slot_id));
	assert (type >= 0);
	assert (type < PiType_NBR_ELT);
	assert (index >= 0);
	assert (val >= 0);
	assert (val <= 1);

	auto           it_id_map = _pi_id_map.find (slot_id);
	assert (it_id_map != _pi_id_map.end ());
	int            pi_id     = it_id_map->second._pi_id_arr [type];
	assert (pi_id >= 0);

	if (! _tuner_flag && pi_id != _dummy_mix_id)
	{
		_central.set_param (pi_id, index, val);
	}

	update_parameter (_preset_cur, slot_id, type, index, val);

	// Add the mixer plug-in if necessary but don't remove it if not.
	// This avoids clicks when switching back and forth between configurations
	// with and without mixer. The removal will be effective at the next call
	// to apply_settings.
	if (pi_id == _dummy_mix_id && has_mixer_plugin (_preset_cur, slot_id))
	{
		apply_settings ();

		// apply_settings() changed the list
		it_id_map = _pi_id_map.find (slot_id);
		pi_id     = it_id_map->second._pi_id_arr [type];
		assert (pi_id >= 0);
	}

	if (_obs_ptr != 0)
	{
		_obs_ptr->set_param (pi_id, index, val, slot_id, type);
	}
}



void	Model::set_param_beats (int slot_id, int index, float beats)
{
	assert (index >= 0);
	assert (beats >= 0);

	doc::Slot &    slot = _preset_cur.use_slot (slot_id);
	doc::PluginSettings &   settings = slot.use_settings (PiType_MAIN);
	assert (index < int (settings._param_list.size ()));

	// Stores the beat value in the document
	auto           it_pres = settings._map_param_pres.find (index);
	assert (it_pres != settings._map_param_pres.end ());
	it_pres->second._ref_beats = beats;

	const piapi::PluginDescInterface &	pi_desc =
		get_model_desc (slot._pi_model);
	const piapi::ParamDescInterface &   param_desc =
		pi_desc.get_param_info (piapi::ParamCateg_GLOBAL, index);

	// Converts the value from beats to the internal parameter unit
	// Clips and converts to a normalized value
	const double   val_nrm = ToolsParam::conv_beats_to_nrm (
		beats, param_desc, _tempo
	);

	// Sets the parameter
	set_param (slot_id, PiType_MAIN, index, float (val_nrm));

	if (_obs_ptr != 0)
	{
		_obs_ptr->set_param_beats (slot_id, index, beats);
	}
}



void	Model::set_param_ctrl (int slot_id, PiType type, int index, const doc::CtrlLinkSet &cls)
{
	assert (type >= 0);
	assert (type < PiType_NBR_ELT);
	assert (index >= 0);

	doc::Slot &    slot = _preset_cur.use_slot (slot_id);
	doc::PluginSettings &   settings = slot.use_settings (type);
	assert (index < int (settings._param_list.size ()));

	settings._map_param_ctrl [index] = cls;

	auto           it_id_map = _pi_id_map.find (slot_id);
	assert (it_id_map != _pi_id_map.end ());
	int            pi_id = it_id_map->second._pi_id_arr [type];
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
	else if (! has_mixer_plugin (_preset_cur, slot_id))
	{
		apply_settings ();
	}
	else if (! _tuner_flag)
	{
		set_param_ctrl_with_override (cls, pi_id, slot_id, type, index);
		_central.commit ();
	}

	if (_obs_ptr != 0)
	{
		// apply_settings() changed the list
		it_id_map = _pi_id_map.find (slot_id);
		pi_id     = it_id_map->second._pi_id_arr [type];

		_obs_ptr->set_param_ctrl (slot_id, type, index, cls);
	}
}



// rotenc_index < 0: previous override is disabled, if existing.
void	Model::override_param_ctrl (int slot_id, PiType type, int index, int rotenc_index)
{
	assert (! _preset_cur.is_slot_empty (slot_id));
	assert (type >= 0);
	assert (type < PiType_NBR_ELT);
	assert (index >= 0);

	auto           it_id_map = _pi_id_map.find (slot_id);
	assert (it_id_map != _pi_id_map.end ());
	int            pi_id     = it_id_map->second._pi_id_arr [type];
	assert (pi_id >= 0);

	const OverrideLoc loc { pi_id, index };

	if (rotenc_index < 0)
	{
		auto           it = _override_map.find (loc);
		if (it != _override_map.end ())
		{
			_override_map.erase (it);
			update_param_ctrl (loc);
			_central.commit ();
		}
	}
	else
	{
		_override_map [loc] = rotenc_index;
		update_param_ctrl (loc);
		_central.commit ();
	}

	if (_obs_ptr != 0)
	{
		_obs_ptr->override_param_ctrl (slot_id, type, index, rotenc_index);
	}
}



void	Model::reset_all_overridden_param_ctrl ()
{
	while (! _override_map.empty ())
	{
		const OverrideLoc &  loc = _override_map.begin ()->first;
		int            slot_id;
		PiType         type;
		find_slot_type_cur_preset (slot_id, type, loc._pi_id);
		if (slot_id >= 0)
		{
			override_param_ctrl (slot_id, type, loc._index, -1);
		}
	}
}



void	Model::reset_all_overridden_param_ctrl (int slot_id)
{
	assert (slot_id >= 0);

	auto           it_pi_map = _pi_id_map.find (slot_id);
	if (it_pi_map != _pi_id_map.end ())
	{
		for (int type_cnt = 0; type_cnt < PiType_NBR_ELT; ++type_cnt)
		{
			const int      pi_id = it_pi_map->second._pi_id_arr [type_cnt];

			if (pi_id >= 0)
			{
				bool           erase_flag = false;
				do
				{
					const auto     it = std::find_if (
						_override_map.begin (),
						_override_map.end (),
						[pi_id] (const std::pair <OverrideLoc, int> &x)
						{
							return (x.first._pi_id == pi_id);
						}
					);
					erase_flag = (it != _override_map.end ());
					if (erase_flag)
					{
						_override_map.erase (it);
					}
				}
				while (erase_flag);
			}
		}
	}
}



std::vector <std::string>	Model::list_plugin_models () const
{
	return _central.use_pi_pool ().list_models ();
}



const piapi::PluginDescInterface &	Model::get_model_desc (std::string model_id) const
{
	return _central.use_pi_pool ().get_model_desc (model_id);
}



int64_t	Model::get_cur_date () const
{
	return _central.get_cur_date ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Model::do_process_msg_audio_to_cmd (const Msg &msg)
{
	if (msg._type == Msg::Type_PARAM)
	{
		const int      pi_id = msg._content._param._plugin_id;
		const int      index = msg._content._param._index;
		const float    val   = msg._content._param._val;

		int            slot_id;
		PiType         type;
		find_slot_type_cur_preset (slot_id, type, pi_id);
		if (slot_id >= 0)
		{
			const bool     ok_flag =
				update_parameter (_preset_cur, slot_id, type, index, val);

			if (ok_flag)
			{
				if (_obs_ptr != 0)
				{
					_obs_ptr->set_param (pi_id, index, val, slot_id, type);
				}

				// Checks if the parameter is tempo-controlled
				if (type == PiType_MAIN)
				{
					auto           it_slot = _preset_cur._slot_map.find (slot_id);
					assert (it_slot != _preset_cur._slot_map.end ());

					doc::Slot &    slot = *(it_slot->second);
					doc::PluginSettings &   settings = slot.use_settings (PiType_MAIN);
					assert (index < int (settings._param_list.size ()));
					doc::ParamPresentation *   pres_ptr =
						settings.use_pres_if_tempo_ctrl (index);
					if (pres_ptr != 0)
					{
						const piapi::PluginDescInterface &	pi_desc =
							get_model_desc (slot._pi_model);
						const piapi::ParamDescInterface &   param_desc =
							pi_desc.get_param_info (piapi::ParamCateg_GLOBAL, index);

						const float    val_beats = float (
							ToolsParam::conv_nrm_to_beats (val, param_desc, _tempo)
						);
						pres_ptr->_ref_beats = val_beats;

						if (_obs_ptr != 0)
						{
							_obs_ptr->set_param_beats (slot_id, index, val_beats);
						}
					}
				}
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



bool	Model::OverrideLoc::operator < (const OverrideLoc &rhs) const
{
	if (_pi_id < rhs._pi_id)
	{
		return true;
	}
	else if (_pi_id == rhs._pi_id)
	{
		return (_index < rhs._index);
	}

	return false;
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
		for (const auto &node : preset._slot_map)
		{
			const auto &   slot_sptr = node.second;
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
	_pi_id_map.clear ();
	_slot_info.clear ();

	const int      nbr_slots = _preset_cur._routing._chain.size ();

	int            slot_index_central = 0;
	for (int slot_index = 0; slot_index < nbr_slots; ++slot_index)
	{
		const int      slot_id   = _preset_cur._routing._chain [slot_index];
		auto           it_id_map =
			_pi_id_map.insert (std::make_pair (slot_id, SlotPiId ())).first;
		auto           it_slot   = _preset_cur._slot_map.find (slot_id);
		assert (it_slot != _preset_cur._slot_map.end ());

		// Full slot
		if (! _preset_cur.is_slot_empty (it_slot))
		{
			_central.insert_slot (slot_index_central);

			doc::Slot &	   slot = *(it_slot->second);

			// Check first if we need a mixer plug-in.
			// Updates the parameters
			check_mixer_plugin (slot_id, slot_index_central);

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
				it_id_map->second._pi_id_arr [PiType_MAIN] = pi_id;
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
				it_id_map->second._pi_id_arr [PiType_MAIN] = pi_id;
				send_effect_settings (pi_id, slot_id, PiType_MAIN, it_s->second);
			}

			++ slot_index_central;
		}
	}

	assert (_preset_cur._slot_map.size () == _pi_id_map.size ());
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
void	Model::check_mixer_plugin (int slot_id, int slot_index_central)
{
	auto           it_slot = _preset_cur._slot_map.find (slot_id);
	assert (it_slot != _preset_cur._slot_map.end ());
	assert (it_slot->second.get () != 0);

	const bool        use_flag = has_mixer_plugin (_preset_cur, slot_id);
	const doc::Slot & slot     = *(it_slot->second);
	int &             id_ref   = _pi_id_map [slot_id]._pi_id_arr [PiType_MIX];

	// Instantiation and setting update
	if (use_flag)
	{
		if (id_ref < 0 || id_ref == _dummy_mix_id)
		{
			const int      pi_id = _central.set_mixer (slot_index_central);
			id_ref = pi_id;
			send_effect_settings (
				pi_id, slot_id, PiType_MIX, slot._settings_mixer
			);
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
bool	Model::has_mixer_plugin (const doc::Preset &preset, int slot_id)
{
	const doc::Slot & slot = preset.use_slot (slot_id);
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
void	Model::send_effect_settings (int pi_id, int slot_id, PiType type, const doc::PluginSettings &settings)
{
	// Parameters
	PluginPool::PluginDetails &   details =
		_central.use_pi_pool ().use_plugin (pi_id);
	int            nbr_param =
		details._desc_ptr->get_nbr_param (piapi::ParamCateg_GLOBAL);
	const int      nbr_param_check = int (settings._param_list.size ());
	assert (nbr_param == nbr_param_check); // Most likely a plug-in version mismatch
	nbr_param = std::min (nbr_param, nbr_param_check); // Just to make sure
	for (int p_index = 0; p_index < nbr_param; ++p_index)
	{
		const float    val = settings._param_list [p_index];
		_central.set_param (pi_id, p_index, val);
	}

	// Modulations and automations
	for (const auto &x : settings._map_param_ctrl)
	{
		const int   p_index = x.first;
		set_param_ctrl_with_override (x.second, pi_id, slot_id, type, p_index);
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
			process_action (action, state._press_ts);
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



void	Model::process_action (const doc::PedalActionSingleInterface &action, int64_t ts)
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

	case doc::ActionType_TEMPO:
		process_action_tempo (dynamic_cast <const doc::ActionTempo &> (action), ts);
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
	const int      slot_id = find_slot_cur_preset (action._fx_id);
	if (slot_id >= 0)
	{
		const auto     it_id_map = _pi_id_map.find (slot_id);
		assert (it_id_map != _pi_id_map.end ());
		const int      pi_id =
			it_id_map->second._pi_id_arr [action._fx_id._type];
		assert (pi_id >= 0);

		if (pi_id != _dummy_mix_id)
		{
			// At this point we are not sure about the type of the
			// named plugin, so we have to check the parameter range.
			PluginPool &   pi_pool   = _central.use_pi_pool ();
			PluginPool::PluginDetails & details = pi_pool.use_plugin (pi_id);
			const int      nbr_param =
				details._desc_ptr->get_nbr_param (piapi::ParamCateg_GLOBAL);
			if (action._index < nbr_param)
			{
				_central.set_param (pi_id, action._index, action._val);
			}
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



void	Model::process_action_tempo (const doc::ActionTempo &action, int64_t ts)
{
	const int64_t  dist = ts - _tempo_last_ts;
	if (   dist <= Cst::_tempo_detection_max
	    && dist >= Cst::_tempo_detection_min)
	{
		double      tempo = (60.0 * 1000*1000) / double (dist);

		// Fits tempo into the accepted range
		while (tempo > Cst::_tempo_max)
		{
			tempo *= 0.5;
		}
		while (tempo < Cst::_tempo_min)
		{
			tempo *= 2;
		}

		_tempo = tempo;
		_central.set_tempo (float (tempo));

		if (_obs_ptr != 0)
		{
			_obs_ptr->set_tempo (tempo);
		}

		if (! _tuner_flag)
		{
			update_all_beat_parameters ();
		}
	}

	_tempo_last_ts = ts;
}



void	Model::build_slot_info ()
{
	_slot_info.clear ();
	PluginPool &   pi_pool = _central.use_pi_pool ();

	for (const auto &node : _pi_id_map)
	{
		const SlotPiId &  spi = node.second;
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

		_slot_info [node.first] = info;
	}
}



void	Model::notify_slot_info ()
{
	assert (_obs_ptr != 0);
	assert (_slot_info.size () == _pi_id_map.size ());

	_obs_ptr->set_slot_info_for_current_preset (_slot_info);
}



// Returns a slot_id, -1 if not found
/*** To do: return a set instead of a single element ***/
int	Model::find_slot_cur_preset (const doc::FxId &fx_id) const
{
	assert (fx_id._location_type >= 0);
	assert (fx_id._location_type < doc::FxId::LocType_NBR_ELT);

	int            found_slot_id = -1;

	for (auto it = _preset_cur._slot_map.begin ()
	;	it != _preset_cur._slot_map.end () && found_slot_id < 0
	;	++ it)
	{
		if (! _preset_cur.is_slot_empty (it))
		{
			const doc::Slot & slot = *(it->second);

			if (fx_id._location_type == doc::FxId::LocType_CATEGORY)
			{
				if (slot._pi_model == fx_id._label_or_model)
				{
					found_slot_id = it->first;
				}
			}
			else
			{
				if (! slot._label.empty () && fx_id._label_or_model == slot._label)
				{
					found_slot_id = it->first;
				}
			}
		}
	}

	return found_slot_id;
}



// Returns -1 in slot_id if not found
void	Model::find_slot_type_cur_preset (int &slot_id, PiType &type, int pi_id) const
{
	assert (pi_id >= 0);
	assert (pi_id != _dummy_mix_id);

	slot_id = -1;
	for (auto it_pi_map = _pi_id_map.cbegin ()
	;	it_pi_map != _pi_id_map.cend () && slot_id < 0
	;	++ it_pi_map)
	{
		const SlotPiId &  spi = it_pi_map->second;
		for (int tt = 0; tt < PiType_NBR_ELT && slot_id < 0; ++tt)
		{
			if (spi._pi_id_arr [tt] == pi_id)
			{
				slot_id = it_pi_map->first;
				type    = PiType (tt);
			}
		}
	}
}



bool	Model::update_parameter (doc::Preset &preset, int slot_id, PiType type, int index, float val)
{
	bool           ok_flag = true;

	auto           it_slot = preset._slot_map.find (slot_id);
	if (preset.is_slot_empty (it_slot))
	{
		ok_flag = false;
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



void	Model::fill_pi_init_data (int slot_id, ModelObserverInterface::PluginInitData &pi_data)
{
	const doc::Slot & slot = _preset_cur.use_slot (slot_id);
	pi_data._model = slot._pi_model;

	const int      pi_id = _pi_id_map [slot_id]._pi_id_arr [PiType_MAIN];
	assert (pi_id >= 0);

	const PluginPool::PluginDetails &   details =
		_central.use_pi_pool ().use_plugin (pi_id);
	pi_data._nbr_io_arr [Dir_IN ] = 1;
	pi_data._nbr_io_arr [Dir_OUT] = 1;
	details._desc_ptr->get_nbr_io (
		pi_data._nbr_io_arr [Dir_IN ],
		pi_data._nbr_io_arr [Dir_OUT]
	);
	for (int categ = 0; categ < piapi::ParamCateg_NBR_ELT; ++categ)
	{
		pi_data._nbr_param_arr [categ] =
			details._desc_ptr->get_nbr_param (piapi::ParamCateg (categ));
	}
	pi_data._prefer_stereo_flag = details._desc_ptr->prefer_stereo ();
}



void	Model::update_all_beat_parameters ()
{
	assert (! _tuner_flag);

	bool           need_commit_flag = false;

	for (auto it_slot = _preset_cur._slot_map.begin ()
	;	it_slot != _preset_cur._slot_map.end ()
	;	++ it_slot)
	{
		if (! _preset_cur.is_slot_empty (it_slot))
		{
			const int      slot_id = it_slot->first;
			doc::Slot &    slot    = *(it_slot->second);
			doc::PluginSettings &   settings = slot.use_settings (PiType_MAIN);
			const int      pi_id   =
				_pi_id_map [slot_id]._pi_id_arr [PiType_MAIN];
			for (auto &p : settings._map_param_pres)
			{
				const float    beats = p.second._ref_beats;
				if (beats >= 0)
				{
					const int      index = p.first;
					set_param_beats (slot_id, index, beats);

					if (pi_id >= 0)
					{
						auto           it_cls = settings._map_param_ctrl.find (index);
						if (it_cls != settings._map_param_ctrl.end ())
						{
							set_param_ctrl_with_override (
								it_cls->second, pi_id, slot_id, PiType_MAIN, index
							);
							need_commit_flag = true;
						}
					}
				}
			}
		}
	}

	if (need_commit_flag)
	{
		_central.commit ();
	}
}



// Requires commit
void	Model::update_all_overriden_param_ctrl ()
{
	for (auto &node : _override_map)
	{
		const OverrideLoc &  loc = node.first;
		update_param_ctrl (loc);
	}
}



// Requires commit
void	Model::update_param_ctrl (const OverrideLoc &loc)
{
	const int      pi_id      = loc._pi_id;
	const int      index      = loc._index;
	int            slot_id;
	PiType         type;
	find_slot_type_cur_preset (slot_id, type, pi_id);

	if (slot_id >= 0)
	{
		auto           it_slot = _preset_cur._slot_map.find (slot_id);
		assert (it_slot != _preset_cur._slot_map.end ());
		if (! _preset_cur.is_slot_empty (it_slot))
		{
			doc::CtrlLinkSet  cls;

			doc::Slot &    slot = *(it_slot->second);
			doc::PluginSettings &   settings = slot.use_settings (type);
			const auto     it_cls = settings._map_param_ctrl.find (index);
			if (it_cls != settings._map_param_ctrl.end ())
			{
				cls = it_cls->second;
			}

			set_param_ctrl_with_override (cls, pi_id, slot_id, type, index);
		}
	}
}



// Requires commit
void	Model::set_param_ctrl_with_override (const doc::CtrlLinkSet &cls, int pi_id, int slot_id, PiType type, int index)
{
	const OverrideLoc loc { pi_id, index };
	auto           it = _override_map.find (loc);
	if (it == _override_map.end ())
	{
		set_param_ctrl_internal (cls, pi_id, slot_id, type, index);
	}
	else
	{
		doc::CtrlLinkSet  clso (cls);

		// Uses the override only if there is no direct control
		if (clso._bind_sptr.get () == 0)
		{
			const int      rotenc_index = it->second;

			// Uses the default values
			doc::CtrlLinkSet::LinkSPtr link_sptr (new doc::CtrlLink);
			link_sptr->_source._type  = ControllerType_ROTENC;
			link_sptr->_source._index = rotenc_index;

			clso._bind_sptr = link_sptr;
		}

		set_param_ctrl_internal (clso, pi_id, slot_id, type, index);
	}
}



// Requires commit
void	Model::set_param_ctrl_internal (const doc::CtrlLinkSet &cls, int pi_id, int slot_id, PiType type, int index)
{
	assert (pi_id >= 0);
	assert (type >= 0);
	assert (type < PiType_NBR_ELT);
	assert (index >= 0);

	doc::Slot &    slot = _preset_cur.use_slot (slot_id);
	doc::PluginSettings &   settings = slot.use_settings (type);
	assert (index < int (settings._param_list.size ()));

	const doc::CtrlLinkSet *   cls_ptr = &cls;
	doc::CtrlLinkSet           cls_mod;
	if (type == PiType_MAIN)
	{
		const doc::ParamPresentation *   pres_ptr =
			settings.use_pres_if_tempo_ctrl (index);
		if (pres_ptr != 0)
		{
			const piapi::PluginDescInterface &  desc_pi =
				get_model_desc (slot._pi_model);
			const piapi::ParamDescInterface &   desc    =
				desc_pi.get_param_info (mfx::piapi::ParamCateg_GLOBAL, index);

			cls_mod = cls;
			ToolsParam::add_beat_notch_list_if_linked (cls_mod, desc, _tempo);
			cls_ptr = &cls_mod;
		}
	}

	_central.set_mod (pi_id, index, *cls_ptr);
}



// Configures the default controllers, if any
// selected_slot_id adds to only one slot, -1: all slots
void	Model::add_default_ctrl (int selected_slot_id)
{
	// First, lists the slots in the processing order
	std::set <int> rem_slot_id;
	for (const auto &node : _preset_cur._slot_map)
	{
		rem_slot_id.insert (node.first);
	}
	std::vector <int> slot_id_list = _preset_cur._routing._chain; // The main chain
	for (int rem_id : slot_id_list)
	{
		const auto     it = rem_slot_id.find (rem_id);
		assert (it != rem_slot_id.end ());
		rem_slot_id.erase (it);
	}
	for (int rem_id : rem_slot_id) // Adds all the remaining slots
	{
		slot_id_list.push_back (rem_id);
	}

	std::set <int> used_pot_set;

	// Pair: slot_id, parameter
	std::vector <std::pair <int, int> > ctrl_param_arr;

	// Lists:
	// - Parameters requiering a default controller
	// - Used controllers
	// Checks only the analogue absolute controllers (potentiometers)
	const int      nbr_slots = int (slot_id_list.size ());
	for (int slot_pos = 0; slot_pos < nbr_slots; ++slot_pos)
	{
		const int      slot_id = slot_id_list [slot_pos];
		const auto     it_slot = _preset_cur._slot_map.find (slot_id);
		assert (it_slot != _preset_cur._slot_map.end ());
		if (! _preset_cur.is_slot_empty (it_slot))
		{
			const doc::Slot & slot = *(it_slot->second);
			for (int type = 0; type < PiType_NBR_ELT; ++type)
			{
				const doc::PluginSettings &   settings =
					slot.use_settings (static_cast <PiType> (type));

				// Check the default controllers only for the main plug-in,
				// we know that the mixer plug-in doesn't require this.
				if (   type == PiType_MAIN
				    && (   selected_slot_id < 0
				        || slot_id == selected_slot_id))
				{
					const piapi::PluginDescInterface & desc =
						get_model_desc (slot._pi_model);
					const int      nbr_param =
						desc.get_nbr_param (piapi::ParamCateg_GLOBAL);
					for (int index = 0; index < nbr_param; ++index)
					{
						const auto &   pdesc =
							desc.get_param_info (piapi::ParamCateg_GLOBAL, index);
						const int32_t  flags = pdesc.get_flags ();
						if ((flags & piapi::ParamDescInterface::Flags_AUTOLINK) != 0)
						{
							bool           ok_flag = true;
							const auto     it_ctrl =
								settings._map_param_ctrl.find (index);
							if (it_ctrl != settings._map_param_ctrl.end ())
							{
								ok_flag = (it_ctrl->second._bind_sptr.get () == 0);
							}

							if (ok_flag)
							{
								ctrl_param_arr.push_back (
									std::make_pair (slot_id, index)
								);
							}
						}
					}
				}

				// Lists the controllers
				for (const auto &node : settings._map_param_ctrl)
				{
					const doc::CtrlLinkSet &   cls = node.second;
					if (cls._bind_sptr.get () != 0)
					{
						if (cls._bind_sptr->_source._type == ControllerType_POT)
						{
							used_pot_set.insert (cls._bind_sptr->_source._index);
						}
					}
					for (const auto &mod_sptr : cls._mod_arr)
					{
						assert (mod_sptr.get () != 0);
						if (mod_sptr->_source._type == ControllerType_POT)
						{
							used_pot_set.insert (mod_sptr->_source._index);
						}
					}
				}
			}
		}
	}

	// Now associates the listed parameters to the free controllers
	for (const auto &p : ctrl_param_arr)
	{
		const int      slot_id = p.first;
		const int      index   = p.second;

		// Finds a free controller
		int            pot_index = 0;
		while (   used_pot_set.find (pot_index) != used_pot_set.end ()
		       && pot_index < Cst::_nbr_pot)
		{
			++ pot_index;
		}

		if (pot_index < Cst::_nbr_pot)
		{
			doc::CtrlLinkSet  cls;

			// Retrieves the existing controllers for this parameter
			const doc::Slot & slot = _preset_cur.use_slot (slot_id);
			const doc::PluginSettings &   settings =
				slot.use_settings (PiType_MAIN);
			const auto     it_ctrl =
				settings._map_param_ctrl.find (index);
			if (it_ctrl != settings._map_param_ctrl.end ())
			{
				assert (it_ctrl->second._bind_sptr.get () == 0);
				cls = it_ctrl->second;
			}

			// Inserts the controller
			cls._bind_sptr = doc::CtrlLinkSet::LinkSPtr (new doc::CtrlLink);
			cls._bind_sptr->_source._type  = ControllerType_POT;
			cls._bind_sptr->_source._index = pot_index;

			set_param_ctrl (slot_id, PiType_MAIN, index, cls);

			// Marks the pot as used
			used_pot_set.insert (pot_index);
		}
	}
}



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
