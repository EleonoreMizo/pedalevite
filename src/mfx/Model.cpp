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
#include "mfx/cmd/Cnx.h"
#include "mfx/doc/ActionBank.h"
#include "mfx/doc/ActionClick.h"
#include "mfx/doc/ActionParam.h"
#include "mfx/doc/ActionProg.h"
#include "mfx/doc/ActionSettings.h"
#include "mfx/doc/ActionTempo.h"
#include "mfx/doc/ActionTempoSet.h"
#include "mfx/doc/ActionToggleFx.h"
#include "mfx/doc/ActionToggleTuner.h"
#include "mfx/doc/SerRText.h"
#include "mfx/doc/SerWText.h"
#include "mfx/pi/dwm/DryWetDesc.h"
#include "mfx/pi/dwm/Param.h"
#include "mfx/pi/tuner/Tuner.h"
#include "mfx/FileIOInterface.h"
#include "mfx/Model.h"
#include "mfx/ModelMsgCmdConfLdSv.h"
#include "mfx/ModelObserverInterface.h"
#include "mfx/PedalLoc.h"
#include "mfx/ToolsParam.h"
#include "mfx/ToolsRouting.h"

#include <algorithm>
#include <set>
#include <tuple>

#include <cassert>



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



const std::array <int, Cst::_nbr_pedals>	Model::_pedal_to_switch_map =
{{
	8, 9, 14, 15, 16, 17,
	2, 3, 4, 5, 6, 7
}};



Model::Model (ui::UserInputInterface::MsgQueue &queue_input_to_cmd, ui::UserInputInterface::MsgQueue &queue_input_to_audio, ui::UserInputInterface &input_device, FileIOInterface &file_io)
:	_central (queue_input_to_audio, input_device)
,	_sample_freq (0)
,	_setup ()
,	_bank_index (0)
,	_prog_index (0)
,	_prog_cur ()
,	_layout_cur ()
,	_pi_id_map ()
,	_pedal_state_arr ()
,	_hold_time (std::chrono::seconds (1))
,	_edit_flag (false)
,	_edit_prog_flag (false)
,	_tuner_flag (false)
,	_tuner_pi_id (-1)
,	_tuner_ptr (nullptr)
,	_click_flag (false)
,	_click_slot ()
,	_file_io (file_io)
,	_input_device (input_device)
,	_queue_input_to_cmd (queue_input_to_cmd)
,	_obs_ptr (nullptr)
,	_async_cmd ()
,	_dummy_mix_id (_central.get_dummy_mix_id ())
,	_tempo_last_ts (_central.get_cur_date () - Cst::_tempo_detection_max * 2)
,	_tempo (Cst::_tempo_ref)
,	_latest_slot_id (0)
,	_slot_info ()
,	_override_map ()
,	_param_update_map ()
{
	_click_slot._pi_model = "\?click";
	_central.set_callback (this);
	_async_cmd.use_pool ().expand_to (64);
}



Model::~Model ()
{
	if (is_d2d_recording ())
	{
		stop_d2d_rec ();
	}

	// Flush the queue
	ui::UserInputInterface::MsgCell * cell_ptr = nullptr;
	do
	{
		cell_ptr = _queue_input_to_cmd.dequeue ();
		if (cell_ptr != nullptr)
		{
			_input_device.return_cell (*cell_ptr);
		}
	}
	while (cell_ptr != nullptr);
}



Model::CmdAsyncMgr &	Model::use_async_cmd ()
{
	return _async_cmd;
}



void	Model::set_process_info (double sample_freq, int max_block_size)
{
	_central.set_process_info (sample_freq, max_block_size);
	_sample_freq = sample_freq;
}



double	Model::get_sample_freq () const noexcept
{
	assert (_sample_freq > 0);

	return _sample_freq;
}



void	Model::process_block (float * const * dst_arr, const float * const * src_arr, int nbr_spl) noexcept
{
	_central.process_block (dst_arr, src_arr, nbr_spl);
}



MeterResultSet &	Model::use_meters () noexcept
{
	return _central.use_meters ();
}



float	Model::get_audio_period_ratio () const noexcept
{
	return _central.get_audio_period_ratio ();
}



void	Model::create_plugin_lists ()
{
	_central.create_plugin_lists ();
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



bool	Model::process_messages ()
{
	bool           proc_flag = false;

	proc_flag |= _central.process_queue_audio_to_cmd ();
	proc_flag |= process_msg_ui ();

	// Checks hold state for pedals
	const std::chrono::microseconds  date_us (_central.get_cur_date ());
	for (int ped_cnt = 0; ped_cnt < int (_pedal_state_arr.size ()); ++ped_cnt)
	{
		PedalState &   state = _pedal_state_arr [ped_cnt];
		if (   state._press_flag && ! state._hold_flag
		    && date_us >= state._press_ts + _hold_time)
		{
			state._hold_flag =
				process_pedal_event (ped_cnt, doc::ActionTrigger_HOLD);
		}
	}

	// Frequency detected by the tuner
	if (_obs_ptr != nullptr && _tuner_flag && _tuner_ptr != nullptr)
	{
		const float    freq = _tuner_ptr->get_freq ();
		_obs_ptr->set_tuner_freq (freq);
	}

	// Asynchronious commands
	proc_flag |= process_async_cmd ();

	return proc_flag;
}



int	Model::save_to_disk ()
{
	const std::string pathname = Cst::_config_dir + "/" + Cst::_config_current;

	return save_to_disk (pathname);
}



int	Model::save_to_disk (std::string pathname)
{
	assert (! pathname.empty ());

	int            ret_val = 0;

	doc::SerWText  ser_w;
	ser_w.clear ();
	_setup.ser_write (ser_w);
	ret_val = ser_w.terminate ();
	if (ret_val == 0)
	{
		const std::string content = ser_w.use_content ();
		ret_val = _file_io.write_txt_file (pathname, content);
	}

	return ret_val;
}



int	Model::load_from_disk ()
{
	const std::string pathname = Cst::_config_dir + "/" + Cst::_config_current;

	return load_from_disk (pathname);
}



int	Model::load_from_disk (std::string pathname)
{
	assert (! pathname.empty ());

	std::string    content;
	int            ret_val = _file_io.read_txt_file (pathname, content);

	std::unique_ptr <doc::Setup> sss_uptr;
	if (ret_val == 0)
	{
		doc::SerRText  ser_r;
		ser_r.start (content);
#if __cplusplus >= 201402
		sss_uptr = std::make_unique <doc::Setup> ();
#else // __cplusplus
		sss_uptr = std::unique_ptr <doc::Setup> (new doc::Setup);
#endif // __cplusplus
		sss_uptr->ser_read (ser_r);
		ret_val = ser_r.terminate ();
	}

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
		clear_all_settings ();
		for (const auto &node_cat : sss_uptr->_map_plugin_settings)
		{
			const std::string &  model = node_cat.first;
			const doc::CatalogPluginSettings &  cat = node_cat.second;
			const int      nbr_elt = int (cat._cell_arr.size ());
			for (int index = 0; index < nbr_elt; ++index)
			{
				if (cat._cell_arr [index].get () != nullptr)
				{
					const doc::CatalogPluginSettings::Cell &  cell =
						*(cat._cell_arr [index]);
					add_settings (
						model, index, cell._name, cell._main, cell._mixer
					);
				}
			}
		}
		set_chn_mode (sss_uptr->_chn_mode);
		set_master_vol (sss_uptr->_master_vol);

		select_bank (0);
		activate_prog (0);
	}

	return ret_val;
}



void	Model::set_setup_name (std::string name)
{
	_setup._name = name;

	if (_obs_ptr != nullptr)
	{
		_obs_ptr->set_setup_name (name);
	}
}



void	Model::set_edit_mode (bool edit_flag)
{
	if (edit_flag != _edit_flag)
	{
		if (! edit_flag && _edit_flag && _edit_prog_flag)
		{
			store_prog (_prog_index, -1);
		}

		_edit_flag = edit_flag;

		if (_obs_ptr != nullptr)
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

	if (_obs_ptr != nullptr)
	{
		_obs_ptr->set_save_mode (mode);
	}
}



void	Model::set_pedalboard_layout (const doc::PedalboardLayout &layout)
{
	_setup._layout = layout;

	update_layout ();

	if (_obs_ptr != nullptr)
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
		update_layout ();
		if (_obs_ptr != nullptr)
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

	if (_obs_ptr != nullptr)
	{
		_obs_ptr->set_bank (index, bank);
	}

	preinstantiate_all_plugins_from_bank ();

	if (_edit_flag && index == _bank_index)
	{
		_edit_prog_flag = false;
		activate_prog (_prog_index);
	}
}



// In edit mode, current program is automatically activated upon bank change.
void	Model::select_bank (int index)
{
	assert (index >= 0);
	assert (index < Cst::_nbr_banks);

	if (index != _bank_index)
	{
		if (_edit_flag && _edit_prog_flag)
		{
			store_prog (_prog_index, -1);
			_edit_prog_flag = false;
		}

		_bank_index = index;

		preinstantiate_all_plugins_from_bank ();

		if (_obs_ptr != nullptr)
		{
			_obs_ptr->select_bank (index);
		}

		if (_edit_flag)
		{
			activate_prog (_prog_index);
		}
	}
}



void	Model::set_bank_name (std::string name)
{
	_setup._bank_arr [_bank_index]._name = name;
	if (_obs_ptr != nullptr)
	{
		_obs_ptr->set_bank_name (name);
	}
}



void	Model::set_prog_name (std::string name)
{
	_prog_cur._name = name;
	if (_obs_ptr != nullptr)
	{
		_obs_ptr->set_prog_name (name);
	}
}



void	Model::set_prog (int bank_index, int prog_index, const doc::Program &prog)
{
	assert (bank_index >= 0);
	assert (bank_index < Cst::_nbr_banks);
	assert (prog_index >= 0);
	assert (prog_index < Cst::_nbr_prog_per_bank);

	_setup._bank_arr [prog_index]._prog_arr [prog_index] = prog;

	if (_obs_ptr != nullptr)
	{
		_obs_ptr->set_prog (bank_index, prog_index, prog);
	}

	if (   _edit_flag
	    && _edit_prog_flag
	    && bank_index == _bank_index
	    && prog_index == _prog_index)
	{
		activate_prog (_prog_index);
	}
}



void	Model::activate_prog (int prog_index)
{
	assert (prog_index >= 0);
	assert (prog_index < Cst::_nbr_prog_per_bank);

	reset_all_overridden_param_ctrl ();

	if (_edit_flag && _edit_prog_flag && prog_index != _prog_index)
	{
		store_prog (_prog_index, -1);
	}

	_prog_index = prog_index;
	_prog_cur   = _setup._bank_arr [_bank_index]._prog_arr [_prog_index];

	_edit_prog_flag = true;

	update_layout ();

	for (int ped_cnt = 0; ped_cnt < Cst::_nbr_pedals; ++ped_cnt)
	{
		const doc::PedalActionGroup & pg = _layout_cur._pedal_arr [ped_cnt];
		for (auto &act : pg._action_arr)
		{
			if (! act.is_empty_default () && act._reset_on_pc_flag)
			{
				_pedal_state_arr [ped_cnt]._cycle_pos = 0;
				break;
			}
		}
	}

	if (_obs_ptr != nullptr)
	{
		_obs_ptr->activate_prog (prog_index);
	}
}



// bank_index < 0: use the current bank
void	Model::store_prog (int prog_index, int bank_index)
{
	assert (prog_index >= 0);
	assert (prog_index < Cst::_nbr_prog_per_bank);
	assert (bank_index < Cst::_nbr_banks);

	const int         bank_index2 = (bank_index < 0) ? _bank_index : bank_index;

	_setup._bank_arr [bank_index2]._prog_arr [prog_index] = _prog_cur;

	if (_obs_ptr != nullptr)
	{
		_obs_ptr->store_prog (prog_index, bank_index);
	}
}



void	Model::set_prog_switch_mode (doc::ProgSwitchMode mode)
{
	assert (int (mode) >= 0);
	assert (mode < doc::ProgSwitchMode::NBR_ELT);

	_prog_cur._prog_switch_mode = mode;
	if (_obs_ptr != nullptr)
	{
		_obs_ptr->set_prog_switch_mode (mode);
	}
}



void	Model::set_chn_mode (ChnMode mode)
{
	assert (mode >= 0);
	assert (mode < ChnMode_NBR_ELT);

	_setup._chn_mode = mode;
	_central.set_chn_mode (mode);
	_central.commit ();

	if (_obs_ptr != nullptr)
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

	if (_obs_ptr != nullptr)
	{
		_obs_ptr->set_master_vol (float (vol));
	}
}



void	Model::set_tuner (bool tuner_flag)
{
	_tuner_flag = tuner_flag;
	if (! _tuner_flag)
	{
		_tuner_ptr   = nullptr;
		_tuner_pi_id = -1;
	}

	apply_settings ();

	if (_obs_ptr != nullptr)
	{
		_obs_ptr->set_tuner (_tuner_flag);
	}
}



void	Model::set_click (bool click_flag)
{
	_click_flag = click_flag;

	apply_settings ();

	if (_obs_ptr != nullptr)
	{
		_obs_ptr->set_click (_click_flag);
	}
}



void	Model::set_tempo (double bpm)
{
	assert (bpm >= Cst::_tempo_min);
	assert (bpm <= Cst::_tempo_max);

	_tempo = bpm;
	_central.set_tempo (float (bpm));

	if (_obs_ptr != nullptr)
	{
		_obs_ptr->set_tempo (bpm);
	}

	if (! _tuner_flag)
	{
		update_all_beat_parameters ();
	}
}



int	Model::add_slot ()
{
	const int      slot_id = _prog_cur.gen_slot_id ();
	_prog_cur._slot_map.insert (std::make_pair (
		slot_id,
		doc::Program::SlotSPtr ()
	));

	update_layout ();

	if (_obs_ptr != nullptr)
	{
		_obs_ptr->add_slot (slot_id);
	}

	return slot_id;
}



// Automatically removes the slot from the routing and kills the plug-in
void	Model::remove_slot (int slot_id)
{
	assert (slot_id >= 0);
	assert (_prog_cur._slot_map.find (slot_id) != _prog_cur._slot_map.end ());

	// Removes the slot from the routing
	doc::Routing   routing     = _prog_cur.use_routing (); // Makes a copy
	bool           change_flag = false;

	auto           it     = routing._cnx_audio_set.begin ();
	const auto     it_end = routing._cnx_audio_set.end ();
	while (it != it_end)
	{
		if (it->has_slot_id (slot_id))
		{
			it = routing._cnx_audio_set.erase (it);
			change_flag = true;
		}
		else
		{
			++it;
		}
	}

	if (change_flag)
	{
		set_routing (routing);
	}

	// Kills the plug-in
	remove_plugin (slot_id);

	// Finally removes the slot
	auto           it_slot = _prog_cur._slot_map.find (slot_id);
	assert (it_slot != _prog_cur._slot_map.end ());
	_prog_cur._slot_map.erase (it_slot);

	update_layout ();

	if (_obs_ptr != nullptr)
	{
		_obs_ptr->remove_slot (slot_id);
	}
}



void	Model::set_routing (const doc::Routing &routing)
{
	_prog_cur.set_routing (routing);

	update_layout ();

	if (_obs_ptr != nullptr)
	{
		_obs_ptr->set_routing (routing);
	}
}



void	Model::set_slot_label (int slot_id, std::string name)
{
	assert (slot_id >= 0);

	auto           it_slot = _prog_cur._slot_map.find (slot_id);
	assert (it_slot != _prog_cur._slot_map.end ());

	doc::Program::SlotSPtr &   slot_sptr = it_slot->second;
	if (slot_sptr.get () == nullptr)
	{
		slot_sptr = std::make_shared <doc::Slot> ();
	}
	slot_sptr->_label = name;

	if (_obs_ptr != nullptr)
	{
		_obs_ptr->set_slot_label (slot_id, name);
	}
}



void	Model::set_plugin (int slot_id, std::string model)
{
	assert (slot_id >= 0);
	assert (! model.empty ());

	clear_all_signal_ports_for_slot (slot_id);
	reset_all_overridden_param_ctrl (slot_id);

	auto           it_slot = _prog_cur._slot_map.find (slot_id);
	assert (it_slot != _prog_cur._slot_map.end ());

	doc::Program::SlotSPtr &   slot_sptr = it_slot->second;
	if (slot_sptr.get () == nullptr)
	{
		slot_sptr = std::make_shared <doc::Slot> ();
	}
	slot_sptr->_pi_model = model;

	apply_settings ();

	if (_obs_ptr != nullptr)
	{
		ModelObserverInterface::PluginInitData pi_data;
		fill_pi_init_data (slot_id, pi_data);

		_obs_ptr->set_plugin (slot_id, pi_data);

		// Parameter values
		const int      nbr_param =
			pi_data._nbr_param_arr [piapi::ParamCateg_GLOBAL];
		for (int p = 0; p < nbr_param; ++p)
		{
			const float    val = slot_sptr->_settings_all [model]._param_list [p];
			_obs_ptr->set_param (slot_id, p, val, PiType_MAIN);
		}
	}

	add_default_ctrl (slot_id);
}



void	Model::remove_plugin (int slot_id)
{
	assert (slot_id >= 0);

	clear_all_signal_ports_for_slot (slot_id);
	reset_all_overridden_param_ctrl (slot_id);

	auto           it_slot = _prog_cur._slot_map.find (slot_id);
	assert (it_slot != _prog_cur._slot_map.end ());

	const doc::Program::SlotSPtr & slot_sptr = it_slot->second;
	if (slot_sptr.get () != nullptr)
	{
		slot_sptr->_pi_model.clear ();
	}

	apply_settings ();

	if (_obs_ptr != nullptr)
	{
		_obs_ptr->remove_plugin (slot_id);
	}
}



void	Model::set_plugin_mono (int slot_id, bool mono_flag)
{
	assert (slot_id >= 0);

	doc::Slot &    slot = _prog_cur.use_slot (slot_id);
	doc::PluginSettings &   settings = slot.use_settings (PiType_MAIN);

	settings._force_mono_flag = mono_flag;

	apply_settings ();

	if (_obs_ptr != nullptr)
	{
		_obs_ptr->set_plugin_mono (slot_id, mono_flag);
	}
}


void	Model::set_plugin_reset (int slot_id, bool reset_flag)
{
	assert (slot_id >= 0);

	doc::Slot &    slot = _prog_cur.use_slot (slot_id);
	doc::PluginSettings &   settings = slot.use_settings (PiType_MAIN);

	settings._force_reset_flag = reset_flag;

	apply_settings ();

	if (_obs_ptr != nullptr)
	{
		_obs_ptr->set_plugin_reset (slot_id, reset_flag);
	}
}



// pres_ptr == 0 to remove the presentation
void	Model::set_param_pres (int slot_id, PiType type, int index, const doc::ParamPresentation *pres_ptr)
{
	assert (slot_id >= 0);
	assert (type >= 0);
	assert (type <= PiType_NBR_ELT);
	assert (index >= 0);

	set_param_pres_pre_commit (slot_id, type, index, pres_ptr);

	apply_settings ();

	if (_obs_ptr != nullptr)
	{
		_obs_ptr->set_param_pres (slot_id, type, index, pres_ptr);
	}
}


void	Model::set_param (int slot_id, PiType type, int index, float val)
{
	assert (! _prog_cur.is_slot_empty (slot_id));
	assert (type >= 0);
	assert (type < PiType_NBR_ELT);
	assert (index >= 0);
	assert (val >= 0);
	assert (val <= 1);

	const auto     it_id_map = _pi_id_map.find (slot_id);
	assert (it_id_map != _pi_id_map.end ());
	const int      pi_id     = it_id_map->second._pi_id_arr [type];
	assert (pi_id >= 0);

	set_param_pre_commit (slot_id, pi_id, type, index, val);

	// Add the mixer plug-in if necessary but don't remove it if not.
	// This avoids clicks when switching back and forth between configurations
	// with and without mixer. The removal will be effective at the next call
	// to apply_settings.
	if (pi_id == _dummy_mix_id && has_mixer_plugin (_prog_cur, slot_id))
	{
		apply_settings (); // pi_id not valid after this
	}

	if (_obs_ptr != nullptr)
	{
		_obs_ptr->set_param (slot_id, index, val, type);
	}
}



void	Model::set_param_beats (int slot_id, int index, float beats)
{
	assert (index >= 0);
	assert (beats >= 0);

	doc::Slot &    slot = _prog_cur.use_slot (slot_id);
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

	if (_obs_ptr != nullptr)
	{
		_obs_ptr->set_param_beats (slot_id, index, beats);
	}
}



void	Model::set_param_ctrl (int slot_id, PiType type, int index, const doc::CtrlLinkSet &cls)
{
	assert (type >= 0);
	assert (type < PiType_NBR_ELT);
	assert (index >= 0);

	set_prog_ctrl (_prog_cur, slot_id, type, index, cls);

	auto           it_id_map = _pi_id_map.find (slot_id);
	assert (it_id_map != _pi_id_map.end ());
	const int      pi_id = it_id_map->second._pi_id_arr [type];
	assert (pi_id >= 0);

	if (pi_id == _dummy_mix_id)
	{
		if (! cls.is_empty ())
		{
			apply_settings ();
		}
	}
	else if (! has_mixer_plugin (_prog_cur, slot_id))
	{
		apply_settings ();
	}
	else if (! _tuner_flag)
	{
		set_param_ctrl_with_override (cls, pi_id, slot_id, type, index);
		_central.commit ();
	}

	if (_obs_ptr != nullptr)
	{
		_obs_ptr->set_param_ctrl (slot_id, type, index, cls);
	}
}



// Removes all ControlSource occurrences from the current program.
// Does not handle the memory of the previous plugins
void	Model::remove_ctrl_src (const ControlSource &src)
{
	for (auto it_slot = _prog_cur._slot_map.begin ()
	;	it_slot != _prog_cur._slot_map.end ()
	;	++ it_slot)
	{
		if (! _prog_cur.is_slot_empty (it_slot))
		{
			const doc::Slot & slot = *(it_slot->second);

			for (int type_cnt = 0; type_cnt < PiType_NBR_ELT; ++type_cnt)
			{
				const PiType   type = static_cast <PiType> (type_cnt);
				const doc::PluginSettings *   settings_ptr =
					slot.test_and_get_settings (type);
				if (settings_ptr != nullptr)
				{
					auto           it_cls = settings_ptr->_map_param_ctrl.begin ();
					while (it_cls != settings_ptr->_map_param_ctrl.end ())
					{
						auto           it_cls_next = it_cls;
						++ it_cls_next;

						// Check if there is something to modify here
						bool           mod_flag = false;
						const doc::CtrlLinkSet &   cls_tst =
							it_cls->second;
						if (   cls_tst._bind_sptr.get () != nullptr
						    && cls_tst._bind_sptr->_source == src)
						{
							mod_flag = true;
						}
						else
						{
							for (const auto &link_sptr : cls_tst._mod_arr)
							{
								assert (link_sptr.get () != nullptr);
								if (link_sptr->_source == src)
								{
									mod_flag = true;
									break;
								}
							}
						}

						// Does the modification if required
						if (mod_flag)
						{
							doc::CtrlLinkSet  cls = it_cls->second;
							if (   cls._bind_sptr.get () != nullptr
								 && cls._bind_sptr->_source == src)
							{
								cls._bind_sptr.reset ();
							}
							size_t            pos = 0;
							while (pos < cls._mod_arr.size ())
							{
								if (cls._mod_arr [pos]->_source == src)
								{
									cls._mod_arr.erase (cls._mod_arr.begin () + pos);
								}
								else
								{
									++ pos;
								}
							}

							const int      slot_id = it_slot->first;
							const int      index   = it_cls->first;
							set_param_ctrl (slot_id, type, index, cls);
						}

						it_cls = it_cls_next;
					} // Loop over ControlLinkSet-s
				} // if settings
			} // Loop over types
		} // if slot not empty
	} // Loop over slots
}



// rotenc_index < 0: previous override is disabled, if existing.
// One should not assign a valid override if
// _setup._auto_assign_rotenc_flag is set.
void	Model::override_param_ctrl (int slot_id, PiType type, int index, int rotenc_index)
{
	assert (! _prog_cur.is_slot_empty (slot_id));
	assert (type >= 0);
	assert (type < PiType_NBR_ELT);
	assert (index >= 0);
	assert (rotenc_index < 0 || _setup._auto_assign_rotenc_flag);

	if (override_param_ctrl_no_commit (slot_id, type, index, rotenc_index))
	{
		_central.commit ();
	}

	if (_obs_ptr != nullptr)
	{
		_obs_ptr->override_param_ctrl (slot_id, type, index, rotenc_index);
	}
}



void	Model::reset_all_overridden_param_ctrl ()
{
	reset_and_override_param_ctrl_multi (std::vector <RotEncOverride> ());
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
				auto           it = _override_map.begin ();
				while (it != _override_map.end ())
				{
					it = std::find_if (
						it,
						_override_map.end (),
						[pi_id] (const std::pair <OverrideLoc, int> &x)
						{
							return (x.first._pi_id == pi_id);
						}
					);
					if (it != _override_map.end ())
					{
						it = _override_map.erase (it);
					}
				}
			}
		}
	}
}



// One should not assign a valid override if
// _setup._auto_assign_rotenc_flag is set.
void	Model::reset_and_override_param_ctrl_multi (const std::vector <RotEncOverride> &ovr_arr)
{
	std::vector <RotEncOverride>  ovr_notify_arr;
	ovr_notify_arr.reserve (_override_map.size () + ovr_arr.size ());

	bool           req_commit_flag = false;

	// Deletes all previous overrides
	while (! _override_map.empty ())
	{
		const OverrideLoc &  loc     = _override_map.begin ()->first;
		int                  slot_id = -1;
		PiType               type    = PiType_INVALID;
		find_slot_type_cur_prog (slot_id, type, loc._pi_id);
		if (slot_id >= 0)
		{
			RotEncOverride reo;
			reo._slot_id      = slot_id;
			reo._pi_type      = type;
			reo._index        = loc._index;
			reo._rotenc_index = -1;
			ovr_notify_arr.push_back (reo);
			req_commit_flag  |= override_param_ctrl_no_commit (
				slot_id, type, loc._index, -1
			); // loc becomes invalid after this call
		}
		else
		{
#if 0
			_override_map.erase (_override_map.end ());
#else
			assert (false);
#endif
		}
	}

	// Adds the new overrides
	for (const RotEncOverride &ovr : ovr_arr)
	{
		req_commit_flag |= override_param_ctrl_no_commit (
			ovr._slot_id, ovr._pi_type, ovr._index, ovr._rotenc_index
		);
		ovr_notify_arr.push_back (ovr);
	}

	// Commits if something has changed
	if (req_commit_flag)
	{
		_central.commit ();
	}

	// Notifications
	if (_obs_ptr != nullptr)
	{
		for (const RotEncOverride &ovr : ovr_notify_arr)
		{
			_obs_ptr->override_param_ctrl (
				ovr._slot_id, ovr._pi_type, ovr._index, ovr._rotenc_index
			);
		}
	}
}



void	Model::set_signal_port (int port_id, const doc::SignalPort &port)
{
	assert (_prog_cur._slot_map.find (port._slot_id) != _prog_cur._slot_map.end ());

	// Not strictly necessary, but helps to ensure global consistency.
	clear_signal_port (port_id, false);

	_prog_cur._port_map [port_id] = port;

	apply_settings ();

	if (_obs_ptr != nullptr)
	{
		_obs_ptr->set_signal_port (port_id, port);
	}
}



void	Model::clear_signal_port (int port_id)
{
	clear_signal_port (port_id, true);
}



void	Model::clear_all_signal_ports_for_slot (int slot_id)
{
	auto           it_port = _prog_cur._port_map.begin ();
	while (it_port != _prog_cur._port_map.end ())
	{
		// The element pointed by the iterator might be subject to deletion,
		// so we step first before doing anything.
		auto           it_port_tst = it_port;
		++ it_port;

		if (it_port_tst->second._slot_id == slot_id)
		{
			clear_signal_port (it_port_tst->first);
		}
	}
}



void	Model::add_settings (std::string model, int index, std::string name, const doc::PluginSettings &s_main, const doc::PluginSettings &s_mix)
{
	assert (! model.empty ());
	assert (index >= 0);

	doc::CatalogPluginSettings &  cat = _setup._map_plugin_settings [model];
	doc::CatalogPluginSettings::Cell cell;
	cell._name  = name;
	cell._main  = s_main;
	cell._mixer = s_mix;
	cat.add_settings (index, cell);

	if (_obs_ptr != nullptr)
	{
		_obs_ptr->add_settings (model, index, name, s_main, s_mix);
	}
}



void	Model::remove_settings (std::string model, int index)
{
	assert (! model.empty ());
	assert (index >= 0);

	auto           it_cat = _setup._map_plugin_settings.find (model);
	assert (it_cat != _setup._map_plugin_settings.end ());

	doc::CatalogPluginSettings &  cat = it_cat->second;
	cat.remove_settings (index);

	if (_obs_ptr != nullptr)
	{
		_obs_ptr->remove_settings (model, index);
	}
}



void	Model::clear_all_settings ()
{
	_setup._map_plugin_settings.clear ();

	if (_obs_ptr != nullptr)
	{
		_obs_ptr->clear_all_settings ();
	}
}



void	Model::load_plugin_settings (int slot_id, const doc::PluginSettings &s_main, const doc::PluginSettings &s_mix)
{
	assert (slot_id >= 0);

	load_plugin_settings (slot_id, PiType_MAIN, s_main);
	load_plugin_settings (slot_id, PiType_MIX , s_mix );
}



void	Model::load_plugin_settings (int slot_id, PiType type, const doc::PluginSettings &settings)
{
	assert (slot_id >= 0);
	assert (type >= 0);
	assert (type < PiType_NBR_ELT);

	// Parameters
	const int      nbr_param = int (settings._param_list.size ());
	for (int index = 0; index < nbr_param; ++index)
	{
		const float    val = settings._param_list [index];
		set_param (slot_id, type, index, val);
	}

	// Controllers
	for (auto &node_ctrl : settings._map_param_ctrl)
	{
		const int                  index = node_ctrl.first;
		const doc::CtrlLinkSet &   cls   = node_ctrl.second;
		set_param_pres (slot_id, type, index, nullptr);
		set_param_ctrl (slot_id, type, index, cls);
	}

	// Presentation
	for (auto &node_pres : settings._map_param_pres)
	{
		const int                        index = node_pres.first;
		const doc::ParamPresentation &   pres = node_pres.second;
		set_param_pres (slot_id, type, index, &pres);
	}

	// Flags
	if (type == PiType_MAIN)
	{
		set_plugin_mono (slot_id, settings._force_mono_flag);
		set_plugin_reset (slot_id, settings._force_reset_flag);
	}
}



void	Model::enable_auto_rotenc_override (bool ovr_flag)
{
	_setup._auto_assign_rotenc_flag = ovr_flag;

	if (_obs_ptr != nullptr)
	{
		_obs_ptr->enable_auto_rotenc_override (ovr_flag);
	}
}



std::vector <std::string>	Model::list_plugin_models () const
{
	return _central.use_pi_pool ().list_models ();
}



const std::vector <std::string> &	Model::use_aud_pi_list () const
{
	return _central.use_aud_pi_list ();
}



const std::vector <std::string> &	Model::use_sig_pi_list () const
{
	return _central.use_sig_pi_list ();
}



const piapi::PluginDescInterface &	Model::get_model_desc (std::string model_id) const
{
	return _central.use_pi_pool ().get_model_desc (model_id);
}



std::chrono::microseconds	Model::get_cur_date () const
{
	return _central.get_cur_date ();
}



// Gets the final parameter value, after modulation.
float	Model::get_param_val_mod (int slot_id, PiType type, int index) const
{
	assert (! _prog_cur.is_slot_empty (slot_id));
	assert (type >= 0);
	assert (type < PiType_NBR_ELT);
	assert (index >= 0);

	const PluginDetails & details = use_plugin_details (slot_id, type);
	float          val = details._param_mod_arr [index];
	if (val < 0)
	{
		val = details._param_arr [index];
	}

	return val;
}



#if defined (mfx_PluginDetails_USE_TIMINGS)

Model::CpuMeter	Model::get_plugin_cpu_meter (int slot_id) const
{
	assert (! _prog_cur.is_slot_empty (slot_id));

	const PluginDetails & details = use_plugin_details (slot_id, PiType_MAIN);

	return { details._dsp_use._rms, details._dsp_use._peak };
}

#endif // mfx_PluginDetails_USE_TIMINGS



int	Model::start_d2d_rec (const char pathname_0 [], size_t max_len)
{
	return _central.start_d2d_rec (pathname_0, max_len);
}



int	Model::stop_d2d_rec ()
{
	return _central.stop_d2d_rec ();
}



bool	Model::is_d2d_recording () const
{
	return _central.is_d2d_recording ();
}



int64_t	Model::get_d2d_size_frames () const
{
	return _central.get_d2d_size_frames ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Model::do_process_msg_audio_to_cmd (const WaMsg &msg)
{
	if (msg._type == WaMsg::Type_PARAM)
	{
		const int      pi_id = msg._content._param._plugin_id;
		const int      index = msg._content._param._index;
		const float    val   = msg._content._param._val;

		int            slot_id = -1;
		PiType         type    = PiType_INVALID;
		find_slot_type_cur_prog (slot_id, type, pi_id);
		if (slot_id >= 0)
		{
			auto           it_slot = _prog_cur._slot_map.find (slot_id);
			assert (it_slot != _prog_cur._slot_map.end ());

			const bool     ok_flag =
				set_prog_param (_prog_cur, it_slot, type, index, val);

			if (ok_flag)
			{
				if (_obs_ptr != nullptr)
				{
					_obs_ptr->set_param (slot_id, index, val, type);
				}

				// Checks if the parameter is tempo-controlled
				if (type == PiType_MAIN)
				{
					doc::Slot &    slot = *(it_slot->second);
					doc::PluginSettings &   settings = slot.use_settings (PiType_MAIN);
					assert (index < int (settings._param_list.size ()));
					doc::ParamPresentation *   pres_ptr =
						settings.use_pres_if_tempo_ctrl (index);
					if (pres_ptr != nullptr)
					{
						const piapi::PluginDescInterface &	pi_desc =
							get_model_desc (slot._pi_model);
						const piapi::ParamDescInterface &   param_desc =
							pi_desc.get_param_info (piapi::ParamCateg_GLOBAL, index);

						const float    val_beats = float (
							ToolsParam::conv_nrm_to_beats (val, param_desc, _tempo)
						);
						pres_ptr->_ref_beats = val_beats;

						if (_obs_ptr != nullptr)
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



Model::PedalState::PedalState ()
:	_press_flag (false)
,	_hold_flag (false)
,	_press_ts (INT64_MIN)
,	_cycle_pos (0)
{
	// Nothing
}



bool	Model::OverrideLoc::operator < (const OverrideLoc &rhs) const
{
	return (
		  std::tie (    _pi_id,     _index)
		< std::tie (rhs._pi_id, rhs._index)
	);
}



void	Model::update_layout ()
{
	_layout_cur = _setup._layout;
	_layout_cur.merge_layout (_setup._bank_arr [_bank_index]._layout);
	_layout_cur.merge_layout (_prog_cur._layout);

	apply_settings ();
}



void	Model::preinstantiate_all_plugins_from_bank ()
{
	const doc::Bank & bank = _setup._bank_arr [_bank_index];

	// Counts all the plug-ins used in the bank

	// [model] = count, settings
	typedef std::pair <int, piapi::PluginState> CountState;
	std::map <std::string, CountState>   pi_cnt_bank;
	int            max_nbr_slots = 0;

	for (size_t prog_index = 0
	;	prog_index < bank._prog_arr.size ()
	;	++prog_index)
	{
		// Count for this program
		const doc::Program & prog = bank._prog_arr [prog_index];

		// Slots (potential Dry/Wet/Mix plug-ins)
		const int      nbr_slots = int (prog._slot_map.size ());
		max_nbr_slots = std::max (max_nbr_slots, nbr_slots);

		// Standard plug-ins
		std::map <std::string, CountState>  pi_cnt_prog;
		for (const auto &node : prog._slot_map)
		{
			const auto &   slot_sptr = node.second;
			if (slot_sptr.get () != nullptr && ! slot_sptr->is_empty ())
			{
				const doc::Slot & slot = *slot_sptr;
				auto           it = pi_cnt_prog.find (slot._pi_model);
				if (it != pi_cnt_prog.end ())
				{
					++ it->second.first;
				}
				else
				{
					std::pair <std::string, CountState>   p;
					p.first = slot._pi_model;
					CountState &    cs = p.second;
					cs.first = 1;
					piapi::PluginState & state = cs.second;
					const doc::PluginSettings &   settings =
						slot.use_settings (PiType_MAIN);
					for (auto val : settings._param_list)
					{
						state._param_list.push_back (val);
					}
					pi_cnt_prog.insert (std::make_pair (slot._pi_model, cs));
				}
			}
		}

		// Merge with bank results
		for (const auto &elt : pi_cnt_prog)
		{
			auto           it = pi_cnt_bank.find (elt.first);
			if (it == pi_cnt_bank.end ())
			{
				pi_cnt_bank.insert (elt);
			}
			else
			{
				it->second.first = std::max (it->second.first, elt.second.first);
			}
		}
	}

	_central.preinstantiate_plugins (Cst::_plugin_dwm, max_nbr_slots, nullptr);

	// Instantiate all the plug-ins
	for (const auto & node : pi_cnt_bank)
	{
		const std::string &  model_id = node.first;
		const CountState &   cs       = node.second;
		const int      nbr_instances  = cs.first;

		_central.preinstantiate_plugins (model_id, nbr_instances, &cs.second);
	}
}



// Transmit to _central a program built from scratch,
// based on _prog_cur and _layout_cur
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

		if (_obs_ptr != nullptr)
		{
			notify_slot_info ();
		}
	}
}



void	Model::apply_settings_normal ()
{
	_pi_id_map.clear ();
	_slot_info.clear ();
	SlotIdToPosMap slot_pos_map;

	_central.set_prog_switch_mode (_prog_cur._prog_switch_mode);

	// Audio graph last, signal plug-ins before
	std::vector <int> onl;
	const int      audio_pos = ToolsRouting::build_ordered_node_list (
		onl, false, _prog_cur, use_aud_pi_list ()
	);
	const int      nbr_slots = int (onl.size ());

	// Adds all plug-ins
	int            slot_index_central = 0;
	for (int slot_index = 0; slot_index < nbr_slots; ++slot_index)
	{
		const bool     audio_flag = (slot_index >= audio_pos);
		const int      slot_id    = onl [slot_index];

		// Fills the map between slot_ids and slot positions in Central
		slot_pos_map [slot_id] = slot_index_central;

		auto           it_id_map  =
			_pi_id_map.insert (std::make_pair (slot_id, SlotPiId ())).first;
		auto           it_slot    = _prog_cur._slot_map.find (slot_id);
		assert (it_slot != _prog_cur._slot_map.end ());

		// Adds a slot in central
		_central.insert_slot (slot_index_central);

		// Empty slot
		if (_prog_cur.is_slot_empty (it_slot))
		{
			_central.remove_mixer (slot_index_central);
		}

		// Full slot
		else
		{
			doc::Slot &	   slot = *(it_slot->second);

			const piapi::PluginDescInterface &	desc =
				get_model_desc (slot._pi_model);
			int            nbr_i = 1;
			int            nbr_o = 1;
			int            nbr_s = 0;
			desc.get_nbr_io (nbr_i, nbr_o, nbr_s);
			const bool     gen_audio_flag = (audio_flag && nbr_o > 0);

			// Check first if we need a mixer plug-in.
			// Updates the parameters
			check_mixer_plugin (slot_id, slot_index_central, audio_flag);

			// Now the main plug-in
			const int      pi_id = insert_plugin_main (
				slot, slot_id, it_id_map, slot_index_central, gen_audio_flag
			);

			// Registers signal ports
			for (int sig_index = 0; sig_index < nbr_s; ++sig_index)
			{
				int            port_id = -1;
				const auto     it_sig = std::find_if (
					_prog_cur._port_map.begin (),
					_prog_cur._port_map.end (),
					[slot_id, sig_index] (const doc::Program::PortMap::value_type &node)
					{
						return (   node.second._slot_id   == slot_id
						        && node.second._sig_index == sig_index);
					}
				);
				if (it_sig != _prog_cur._port_map.end ())
				{
					port_id = it_sig->first;
				}
				_central.set_sig_source (pi_id, sig_index, port_id);
			}
		}

		++ slot_index_central;
	}

	// Makes sure there is no duplicate slot_id
	assert (int (slot_pos_map.size ()) == nbr_slots);

	// Then the click
	int            pos_click = -1;
	if (_click_flag)
	{
		pos_click = slot_index_central;

		_central.insert_slot (slot_index_central);
		_central.remove_mixer (slot_index_central);

		insert_plugin_main (
			_click_slot, -1, _pi_id_map.end (), slot_index_central, true
		);

		++ slot_index_central;
	}

	assert (_prog_cur._slot_map.size () == _pi_id_map.size ());

	// Now, the routing
	apply_routing (slot_pos_map);

	// Routing for the click
	if (_click_flag)
	{
		cmd::Cnx       cnx;
		cnx._src._slot_type = cmd::CnxEnd::SlotType_NORMAL;
		cnx._src._slot_pos  = pos_click;
		cnx._src._pin       = 0;
		cnx._dst._slot_type = cmd::CnxEnd::SlotType_IO;
		cnx._dst._slot_pos  = 0;
		cnx._dst._pin       = 0;
		_central.add_cnx (cnx);
	}
}



void	Model::apply_settings_tuner ()
{
	assert (_tuner_flag);

	_central.insert_slot (0);
	_central.remove_mixer (0);

	_tuner_pi_id = _central.set_plugin (0, Cst::_plugin_tuner, false, true);

	const PluginDetails &   details =
		_central.use_pi_pool ().use_plugin (_tuner_pi_id);
	_tuner_ptr = dynamic_cast <pi::tuner::Tuner *> (details._pi_uptr.get ());
	assert (_tuner_ptr != nullptr);

	// Routing
	_central.clear_routing ();
	cmd::Cnx       cnx;
	cnx._src._slot_type = cmd::CnxEnd::SlotType_IO;
	cnx._src._slot_pos  = 0;
	cnx._src._pin       = 0;
	cnx._dst._slot_type = cmd::CnxEnd::SlotType_NORMAL;
	cnx._dst._slot_pos  = 0;
	cnx._dst._pin       = 0;
	_central.add_cnx (cnx);
}



void	Model::apply_routing (const SlotIdToPosMap &pos_map)
{
	_central.clear_routing ();

	for (const doc::Cnx &cnx_d : _prog_cur.use_routing ()._cnx_audio_set)
	{
		const cmd::Cnx cnx_c { convert_connection (cnx_d, pos_map) };
		_central.add_cnx (cnx_c);
	}
}



// slot_id can be negative if the slot isn't part of _prog_cur,
// same for it_id_map that should be _pi_id_map.end () in this case.
int	Model::insert_plugin_main (doc::Slot &slot, int slot_id, PiIdMap::iterator it_id_map, int slot_index_central, bool gen_audio_flag)
{
	assert (   (slot_id >= 0 && it_id_map != _pi_id_map.end ())
	        || (slot_id <  0 && it_id_map == _pi_id_map.end ()));

	auto           it_s  = slot._settings_all.find (slot._pi_model);
	int            pi_id = -1;
	if (it_s == slot._settings_all.end ())
	{
		// Probably the first creation on this slot.
		// Creates the plug-ins and collects the parameter list
		pi_id = _central.set_plugin (
			slot_index_central,
			slot._pi_model,
			false,
			gen_audio_flag
		);
		doc::PluginSettings &	settings =
			slot._settings_all [slot._pi_model];
		settings._param_list =
			_central.use_pi_pool ().use_plugin (pi_id)._param_arr;
		if (it_id_map != _pi_id_map.end ())
		{
			it_id_map->second._pi_id_arr [PiType_MAIN] = pi_id;
		}
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
			it_s->second._force_reset_flag,
			gen_audio_flag
		);
		if (it_id_map != _pi_id_map.end ())
		{
			it_id_map->second._pi_id_arr [PiType_MAIN] = pi_id;
		}
		send_effect_settings (pi_id, slot_id, PiType_MAIN, it_s->second);
		/*** To do: send some "quick clean" event to the plug-in if _force_reset_flag ***/
	}

	return (pi_id);
}



// Adds or removes the mixer plug-in, depending on the current
// settings.
// We need it if there is an automation or if it is set to something
// different of 100% wet at 0 dB.
// Does not commit anything
// The slot should exist in the current program.
void	Model::check_mixer_plugin (int slot_id, int slot_index_central, int chain_flag)
{
	auto           it_slot = _prog_cur._slot_map.find (slot_id);
	assert (it_slot != _prog_cur._slot_map.end ());
	assert (it_slot->second.get () != nullptr);

	const bool        use_flag =
		chain_flag && has_mixer_plugin (_prog_cur, slot_id);
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
bool	Model::has_mixer_plugin (const doc::Program &prog, int slot_id)
{
	const doc::Slot & slot = prog.use_slot (slot_id);
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
						const doc::ActionParam &   ac =
							dynamic_cast <const doc::ActionParam &> (action);
						if (ac._fx_id._type == PiType_MIX)
						{
							use_flag = slot.is_referenced_by (ac._fx_id);
						}
					}
					else if (action.get_type () == doc::ActionType_TOGGLE_FX)
					{
						const doc::ActionToggleFx &   ac =
							dynamic_cast <const doc::ActionToggleFx &> (action);
						use_flag = slot.is_referenced_by (ac._fx_id);
					}
				}
			}
		}
	}

	return use_flag;
}



// Does not commit anything
// If slot_id < 0, does not setup modulations and automations
void	Model::send_effect_settings (int pi_id, int slot_id, PiType type, const doc::PluginSettings &settings)
{
	// Parameters
	const PluginDetails &   details = _central.use_pi_pool ().use_plugin (pi_id);
	int            nbr_param =
		details._desc_ptr->get_nbr_param (piapi::ParamCateg_GLOBAL);
	const int      nbr_param_check = int (settings._param_list.size ());
	assert (nbr_param == nbr_param_check); // Most likely a plug-in version mismatch
	nbr_param = std::min (nbr_param, nbr_param_check); // Just to make sure

	std::vector <float>  param_list (
		settings._param_list.begin (),
		settings._param_list.begin () + nbr_param
	);
	_central.set_pi_state (pi_id, param_list);

	// Modulations and automations
	if (slot_id >= 0)
	{
		for (const auto &x : settings._map_param_ctrl)
		{
			const int   p_index = x.first;
			set_param_ctrl_with_override (x.second, pi_id, slot_id, type, p_index);
		}
	}
}



bool	Model::process_msg_ui ()
{
	bool           proc_flag = false;

	ui::UserInputInterface::MsgCell * cell_ptr = nullptr;
	do
	{
		cell_ptr = _queue_input_to_cmd.dequeue ();
		if (cell_ptr != nullptr)
		{
			proc_flag = true;

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
					const float    val = cell_ptr->_val.get_val ();
					const std::chrono::microseconds  date (cell_ptr->_val.get_date ());
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
	while (cell_ptr != nullptr);

	return proc_flag;
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



void	Model::process_pedal (int pedal_index, bool set_flag, std::chrono::microseconds date)
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
		const bool     proc_flag = ! state._hold_flag;
		state._hold_flag  = false;
		state._press_flag = false;
		state._press_ts   = date;
		if (proc_flag)
		{
			process_pedal_event (pedal_index, doc::ActionTrigger_RELEASE);
		}
	}
}



// Commits modifications
// Returns true if at least one action has been executed (and false if the
// event is empty)
bool	Model::process_pedal_event (int pedal_index, doc::ActionTrigger trigger)
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
			assert (action_sptr.get () != nullptr);
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

	commit_cumulated_changes ();

	return (nbr_actions > 0);
}



void	Model::process_action (const doc::PedalActionSingleInterface &action, std::chrono::microseconds ts)
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

	case doc::ActionType_PROG:
		process_action_prog (dynamic_cast <const doc::ActionProg &> (action));
		break;

	case doc::ActionType_TOGGLE_FX:
		process_action_toggle_fx (dynamic_cast <const doc::ActionToggleFx &> (action));
		break;

	case doc::ActionType_TOGGLE_TUNER:
		process_action_toggle_tuner (dynamic_cast <const doc::ActionToggleTuner &> (action));
		break;

	case doc::ActionType_TEMPO:
		process_action_tempo_tap (dynamic_cast <const doc::ActionTempo &> (action), ts);
		break;

	case doc::ActionType_SETTINGS:
		process_action_settings (dynamic_cast <const doc::ActionSettings &> (action));
		break;

	case doc::ActionType_TEMPO_SET:
		process_action_tempo_set (dynamic_cast <const doc::ActionTempoSet &> (action));
		break;

	case doc::ActionType_CLICK:
		process_action_click (dynamic_cast <const doc::ActionClick &> (action));
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
	std::array <int, Cst::_max_named_targets> result_arr;
	int            nbr_results = 0;
	find_slot_cur_prog (result_arr, nbr_results, action._fx_id);

	for (int res_cnt = 0; res_cnt < nbr_results; ++res_cnt)
	{
		const int      slot_id   = result_arr [res_cnt];
		const auto     it_id_map = _pi_id_map.find (slot_id);
		assert (it_id_map != _pi_id_map.end ());
		const int      pi_id     =
			it_id_map->second._pi_id_arr [action._fx_id._type];
		assert (pi_id >= 0);

		if (pi_id != _dummy_mix_id)
		{
			// At this point we are not sure about the type of a
			// named plugin, so we have to check the parameter range.
			PluginPool &   pi_pool   = _central.use_pi_pool ();
			const PluginDetails & details = pi_pool.use_plugin (pi_id);
			const int      nbr_param =
				details._desc_ptr->get_nbr_param (piapi::ParamCateg_GLOBAL);
			if (action._index < nbr_param)
			{
				const bool     ok_flag = set_param_pre_commit (
					slot_id,
					pi_id,
					action._fx_id._type,
					action._index,
					action._val
				);
				if (ok_flag)
				{
					push_set_param (
						slot_id,
						action._fx_id._type,
						action._index,
						action._val,
						false,
						0
					);
				}
			}
		}
	}
}



void	Model::process_action_prog (const doc::ActionProg &action)
{
	int            new_index = action._val;
	if (action._relative_flag)
	{
		new_index += _prog_index;
		new_index += Cst::_nbr_prog_per_bank;
		assert (new_index >= 0);
		new_index %= Cst::_nbr_prog_per_bank;
	}

	activate_prog (new_index);
}



void	Model::process_action_toggle_fx (const doc::ActionToggleFx &action)
{
	std::array <int, Cst::_max_named_targets> result_arr;
	int            nbr_results = 0;
	find_slot_cur_prog (result_arr, nbr_results, action._fx_id);

	float          bypass_val = 0;
	for (int res_cnt = 0; res_cnt < nbr_results; ++res_cnt)
	{
		const int      slot_id = result_arr [res_cnt];

		// Retrieves the state of the plug-in's bypass and flips it
		if (res_cnt == 0)
		{
			const doc::Slot &    slot = _prog_cur.use_slot (slot_id);
			const doc::PluginSettings &   settings =
				slot.use_settings (PiType_MIX);
			if (settings._param_list [pi::dwm::Param_BYPASS] < 0.5f)
			{
				bypass_val = 1;
			}
		}

		// Sets the new bypass value
		const auto     it_id_map = _pi_id_map.find (slot_id);
		assert (it_id_map != _pi_id_map.end ());
		const int      pi_id     =
			it_id_map->second._pi_id_arr [PiType_MIX];
		assert (pi_id >= 0);

		if (pi_id != _dummy_mix_id)
		{
			const bool     ok_flag = set_param_pre_commit (
				slot_id,
				pi_id,
				PiType_MIX,
				pi::dwm::Param_BYPASS,
				bypass_val
			);
			if (ok_flag)
			{
				push_set_param (
					slot_id,
					PiType_MIX,
					pi::dwm::Param_BYPASS,
					bypass_val,
					false,
					0
				);
			}
		}
	}
}



void	Model::process_action_toggle_tuner (const doc::ActionToggleTuner &action)
{
	fstb::unused (action);

	set_tuner (! _tuner_flag);
}



void	Model::process_action_tempo_tap (const doc::ActionTempo &action, std::chrono::microseconds ts)
{
	fstb::unused (action);

	const std::chrono::microseconds  dist = ts - _tempo_last_ts;
	if (   dist <= Cst::_tempo_detection_max
	    && dist >= Cst::_tempo_detection_min)
	{
		const double   tempo = (60.0 * 1'000'000) / double (dist.count ());

		process_action_tempo (tempo);
	}

	_tempo_last_ts = ts;
}



void	Model::process_action_settings (const doc::ActionSettings &action)
{
	assert (! action._relative_flag); /*** To do ***/

	std::array <int, Cst::_max_named_targets> result_arr;
	int            nbr_results = 0;
	find_slot_cur_prog (result_arr, nbr_results, action._fx_id);

	for (int res_cnt = 0; res_cnt < nbr_results; ++res_cnt)
	{
		const int      slot_id = result_arr [res_cnt];

		// Finds the program
		auto           it_slot = _prog_cur._slot_map.find (slot_id);
		assert (it_slot != _prog_cur._slot_map.end ());
		const std::string &  pi_model = it_slot->second->_pi_model;

		const auto     it_cat = _setup._map_plugin_settings.find (pi_model);
		if (it_cat != _setup._map_plugin_settings.end ())
		{
			if (it_cat->second.is_preset_existing (action._val))
			{
				const doc::CatalogPluginSettings::Cell & cell =
					*(it_cat->second._cell_arr [action._val]);

				apply_plugin_settings (
					slot_id, PiType_MAIN, cell._main , true, false
				);
				apply_plugin_settings (
					slot_id, PiType_MIX , cell._mixer, true, false
				);
			}
		}
	}
}



void	Model::process_action_tempo_set (const doc::ActionTempoSet &action)
{
	process_action_tempo (action._tempo_bpm);
}



void	Model::process_action_click (const doc::ActionClick &action)
{
	switch (action._mode)
	{
	case doc::ActionClick::Mode_OFF:
		set_click (false);
		break;
	case doc::ActionClick::Mode_ON:
		set_click (true);
		break;
	case doc::ActionClick::Mode_TOGGLE:
		set_click (! _click_flag);
		break;
	default:
		assert (false);
		break;
	}
}



void	Model::process_action_tempo (double tempo)
{
	assert (tempo > 0);

	// Fits tempo into the accepted range
	while (tempo > Cst::_tempo_max)
	{
		tempo *= 0.5;
	}
	while (tempo < Cst::_tempo_min)
	{
		tempo *= 2;
	}

	set_tempo (tempo);
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
				const PluginDetails &   details = pi_pool.use_plugin (pi_id);
				info [type] =
					std::make_shared <ModelObserverInterface::PluginInfo> (
						*details._pi_uptr,
						*details._desc_ptr,
						details._param_arr
					);
			}
		}

		_slot_info [node.first] = info;
	}
}



void	Model::notify_slot_info ()
{
	assert (_obs_ptr != nullptr);
	assert (_slot_info.size () == _pi_id_map.size ());

	_obs_ptr->set_slot_info_for_current_prog (_slot_info);
}



// result_arr is filled with found slot_id.
// nbr_results is the number of slot_id found, limited by the array size.
// Returns false if not all slots were scanned (limit reached before).
bool	Model::find_slot_cur_prog (std::array <int, Cst::_max_named_targets> &result_arr, int &nbr_results, const doc::FxId &fx_id) const
{
	assert (fx_id._location_type >= 0);
	assert (fx_id._location_type < doc::FxId::LocType_NBR_ELT);

	nbr_results = 0;
	auto           it = _prog_cur._slot_map.begin ();
	while (   it != _prog_cur._slot_map.end ()
	       && nbr_results < Cst::_max_named_targets)
	{
		if (! _prog_cur.is_slot_empty (it))
		{
			const doc::Slot & slot = *(it->second);

			if (fx_id._location_type == doc::FxId::LocType_CATEGORY)
			{
				if (slot._pi_model == fx_id._label_or_model)
				{
					result_arr [nbr_results] = it->first;
					++ nbr_results;
				}
			}
			else
			{
				if (! slot._label.empty () && fx_id._label_or_model == slot._label)
				{
					result_arr [nbr_results] = it->first;
					++ nbr_results;
				}
			}
		}

		++ it;
	}

	return (   nbr_results < Cst::_max_named_targets
	        || it == _prog_cur._slot_map.end ());
}



// Returns -1 in slot_id if not found
void	Model::find_slot_type_cur_prog (int &slot_id, PiType &type, int pi_id) const
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



const PluginDetails &	Model::use_plugin_details (int slot_id, PiType type) const
{
	assert (! _prog_cur.is_slot_empty (slot_id));
	assert (type >= 0);
	assert (type < PiType_NBR_ELT);

	const auto     it_id_map = _pi_id_map.find (slot_id);
	assert (it_id_map != _pi_id_map.end ());
	const int      pi_id     = it_id_map->second._pi_id_arr [type];
	assert (pi_id >= 0);

	const PluginDetails & details = _central.use_pi_pool ().use_plugin (pi_id);

	return details;
}



bool	Model::set_param_pre_commit (int slot_id, int pi_id, PiType type, int index, float val)
{
	assert (! _prog_cur.is_slot_empty (slot_id));
	assert (pi_id >= 0);
	assert (type >= 0);
	assert (type < PiType_NBR_ELT);
	assert (index >= 0);
	assert (val >= 0);
	assert (val <= 1);

	if (! _tuner_flag && pi_id != _dummy_mix_id)
	{
		_central.set_param (pi_id, index, val);
	}

	return set_prog_param (_prog_cur, slot_id, type, index, val);
}



bool	Model::set_param_beats_pre_commit (int slot_id, int pi_id, int index, float val_beats, doc::ParamPresentation &pres, const piapi::PluginDescInterface &pi_desc, float &val_nrm)
{
	assert (! _prog_cur.is_slot_empty (slot_id));
	assert (pi_id >= 0);
	assert (index >= 0);
	assert (val_beats > 0);
	assert (&pres == _prog_cur.use_slot (slot_id)
	                          .use_settings (PiType_MAIN)
	                          .use_pres_if_tempo_ctrl (index));
	assert (&pi_desc == &get_model_desc (_prog_cur.use_slot (slot_id)._pi_model));

	// Stores the beat value in the document
	pres._ref_beats = val_beats;

	// Converts the value from beats to the internal parameter unit
	// Clips and converts to a normalized value
	const piapi::ParamDescInterface &   param_desc =
		pi_desc.get_param_info (piapi::ParamCateg_GLOBAL, index);
	val_nrm = float (ToolsParam::conv_beats_to_nrm (
		val_beats, param_desc, _tempo
	));

	return set_param_pre_commit (slot_id, pi_id, PiType_MAIN, index, val_nrm);
}



// Requires a full apply_settings() actually
bool	Model::set_param_pres_pre_commit (int slot_id, PiType type, int index, const doc::ParamPresentation *pres_ptr)
{
	assert (slot_id >= 0);
	assert (type >= 0);
	assert (type <= PiType_NBR_ELT);
	assert (index >= 0);

	bool           ok_flag = true;

	doc::Slot &    slot = _prog_cur.use_slot (slot_id);
	doc::PluginSettings &   settings = slot.use_settings (type);

	if (index >= int (settings._param_list.size ()))
	{
		ok_flag = false;
		assert (false);
	}
	else
	{
		if (pres_ptr == nullptr)
		{
			auto           pres_it = settings._map_param_pres.find (index);
			if (pres_it != settings._map_param_pres.end ())
			{
				settings._map_param_pres.erase (pres_it);
			}
		}
		else
		{
			settings._map_param_pres [index] = *pres_ptr;
		}
	}

	return ok_flag;
}



void	Model::push_set_param (int slot_id, PiType type, int index, float val, bool beat_flag, float val_beats)
{
	assert (! _prog_cur.is_slot_empty (slot_id));
	assert (type >= 0);
	assert (type < PiType_NBR_ELT);
	assert (index >= 0);
	assert (val >= 0);
	assert (val <= 1);
	assert (! beat_flag || val_beats > 0);

	ParamUpdate       update;
	update._update    = ParamUpdate::Update_VAL;
	update._type      = type;
	update._index     = index;
	update._val       = val;
	update._beat_flag = beat_flag;
	update._val_beats = val_beats;
	_param_update_map.insert (std::make_pair (slot_id, update));
}



void	Model::push_set_param_ctrl (int slot_id, PiType type, int index, const doc::CtrlLinkSet &cls)
{
	assert (! _prog_cur.is_slot_empty (slot_id));
	assert (type >= 0);
	assert (type < PiType_NBR_ELT);
	assert (index >= 0);

	ParamUpdate       update;
	update._update = ParamUpdate::Update_CTRL;
	update._type   = type;
	update._index  = index;
	auto              it =
		_param_update_map.insert (std::make_pair (slot_id, update));
	it->second._cls = cls;
}



void	Model::push_set_param_pres (int slot_id, PiType type, int index, const doc::ParamPresentation *pres_ptr)
{
	assert (! _prog_cur.is_slot_empty (slot_id));
	assert (type >= 0);
	assert (type < PiType_NBR_ELT);
	assert (index >= 0);

	ParamUpdate       update;
	update._update    = ParamUpdate::Update_PRES;
	update._type      = type;
	update._index     = index;
	update._pres_flag = (pres_ptr != nullptr);
	auto              it =
		_param_update_map.insert (std::make_pair (slot_id, update));
	if (pres_ptr != nullptr)
	{
		it->second._pres = *pres_ptr;
	}
}



bool	Model::set_prog_param (doc::Program &prog, int slot_id, PiType type, int index, float val)
{
	auto           it_slot = prog._slot_map.find (slot_id);

	return set_prog_param (prog, it_slot, type, index, val);
}



bool	Model::set_prog_param (doc::Program &prog, doc::Program::SlotMap::iterator it_slot, PiType type, int index, float val)
{
	bool           ok_flag = true;
	if (prog.is_slot_empty (it_slot))
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



void	Model::set_prog_ctrl (doc::Program &prog, int slot_id, PiType type, int index, const doc::CtrlLinkSet &cls)
{
	doc::Slot &    slot = prog.use_slot (slot_id);
	doc::PluginSettings &   settings = slot.use_settings (type);
	assert (index < int (settings._param_list.size ()));

	const bool     cls_empty_flag = cls.is_empty ();

	if (cls_empty_flag)
	{
		auto           it_cls = settings._map_param_ctrl.find (index);
		if (it_cls != settings._map_param_ctrl.end ())
		{
			settings._map_param_ctrl.erase (it_cls);
		}
	}
	else
	{
		settings._map_param_ctrl [index] = cls;
	}
}



void	Model::commit_cumulated_changes ()
{
	bool           apply_settings_flag = false;

	// Collects information
	PiIdMap::const_iterator it_id_map = _pi_id_map.end ();
	int            slot_id   = -1;
	int            pi_id_mix = -1;
	for (const auto &node : _param_update_map)
	{
		if (node.first != slot_id)
		{
			slot_id    = node.first;
			it_id_map  = _pi_id_map.find (slot_id);
			assert (it_id_map != _pi_id_map.end ());
			pi_id_mix  = it_id_map->second._pi_id_arr [PiType_MIX];
			assert (pi_id_mix >= 0);

			// Condition for any change
			if (   pi_id_mix == _dummy_mix_id
			    && has_mixer_plugin (_prog_cur, slot_id))
			{
				apply_settings_flag = true;
			}
		}

		// Presentation changes always require apply_settings()
		if (node.second._update == ParamUpdate::Update_PRES)
		{
			apply_settings_flag = true;
		}

		// No need to stay more
		if (apply_settings_flag)
		{
			break;
		}
	}

	// Commit things
	if (apply_settings_flag)
	{
		apply_settings ();
	}
	else
	{
		_central.commit ();
	}

	// Propagates to the views
	if (_obs_ptr != nullptr)
	{
		for (const auto &node : _param_update_map)
		{
			slot_id = node.first;
			const ParamUpdate &  upd = node.second;
			switch (upd._update)
			{
			case ParamUpdate::Update_VAL:
				_obs_ptr->set_param (slot_id, upd._index, upd._val, upd._type);
				if (upd._beat_flag)
				{
					_obs_ptr->set_param_beats (slot_id, upd._index, upd._val_beats);
				}
				break;

			case ParamUpdate::Update_CTRL:
				_obs_ptr->set_param_ctrl (
					slot_id, upd._type, upd._index, upd._cls
				);
				break;

			case ParamUpdate::Update_PRES:
				_obs_ptr->set_param_pres (
					slot_id, upd._type, upd._index,
					(upd._pres_flag) ? &upd._pres : nullptr
				);
				break;

			default:
				assert (false);
				break;
			}
		}
	}

	_param_update_map.clear ();
}



void	Model::fill_pi_init_data (int slot_id, ModelObserverInterface::PluginInitData &pi_data)
{
	const doc::Slot & slot = _prog_cur.use_slot (slot_id);
	pi_data._model = slot._pi_model;

	const int      pi_id = _pi_id_map [slot_id]._pi_id_arr [PiType_MAIN];
	assert (pi_id >= 0);

	const PluginDetails &   details = _central.use_pi_pool ().use_plugin (pi_id);
	pi_data._nbr_io_arr [Dir_IN ] = 1;
	pi_data._nbr_io_arr [Dir_OUT] = 1;
	pi_data._nbr_sig              = 0;
	details._desc_ptr->get_nbr_io (
		pi_data._nbr_io_arr [Dir_IN ],
		pi_data._nbr_io_arr [Dir_OUT],
		pi_data._nbr_sig
	);
	for (int categ = 0; categ < piapi::ParamCateg_NBR_ELT; ++categ)
	{
		pi_data._nbr_param_arr [categ] =
			details._desc_ptr->get_nbr_param (piapi::ParamCateg (categ));
	}
}



// Does not commit, returns true if needed
bool	Model::update_all_beat_parameters ()
{
	assert (! _tuner_flag);

	bool           need_commit_flag = false;

	for (auto it_slot = _prog_cur._slot_map.begin ()
	;	it_slot != _prog_cur._slot_map.end ()
	;	++ it_slot)
	{
		if (! _prog_cur.is_slot_empty (it_slot))
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

	return need_commit_flag;
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



// Requires commit if true is returned
bool	Model::override_param_ctrl_no_commit (int slot_id, PiType type, int index, int rotenc_index)
{
	assert (! _prog_cur.is_slot_empty (slot_id));
	assert (type >= 0);
	assert (type < PiType_NBR_ELT);
	assert (index >= 0);
	assert (rotenc_index < 0 || _setup._auto_assign_rotenc_flag);

	bool           req_commit_flag = false;

	auto           it_id_map = _pi_id_map.find (slot_id);
	assert (it_id_map != _pi_id_map.end ());
	const int      pi_id     = it_id_map->second._pi_id_arr [type];
	assert (pi_id >= 0);

	const OverrideLoc loc { pi_id, index };

	if (rotenc_index < 0)
	{
		auto           it = _override_map.find (loc);
		if (it != _override_map.end ())
		{
			_override_map.erase (it);
			update_param_ctrl (loc);
			req_commit_flag = true;
		}
	}
	else
	{
		_override_map [loc] = rotenc_index;
		update_param_ctrl (loc);
		req_commit_flag = true;
	}

	return req_commit_flag;
}



// Requires commit
void	Model::update_param_ctrl (const OverrideLoc &loc)
{
	const int      pi_id      = loc._pi_id;
	const int      index      = loc._index;
	int            slot_id    = -1;
	PiType         type       = PiType_INVALID;
	find_slot_type_cur_prog (slot_id, type, pi_id);

	if (slot_id >= 0)
	{
		auto           it_slot = _prog_cur._slot_map.find (slot_id);
		assert (it_slot != _prog_cur._slot_map.end ());
		if (! _prog_cur.is_slot_empty (it_slot))
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
	assert (slot_id >= 0);

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
		if (clso._bind_sptr.get () == nullptr)
		{
			const int      rotenc_index = it->second;

			// Uses the default values
			auto           link_sptr (std::make_shared <doc::CtrlLink> ());
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

	doc::Slot &    slot = _prog_cur.use_slot (slot_id);
	doc::PluginSettings &   settings = slot.use_settings (type);
	assert (index < int (settings._param_list.size ()));

	const doc::CtrlLinkSet *   cls_ptr = &cls;
	doc::CtrlLinkSet           cls_mod;
	if (type == PiType_MAIN)
	{
		const doc::ParamPresentation *   pres_ptr =
			settings.use_pres_if_tempo_ctrl (index);
		if (pres_ptr != nullptr)
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

	if (! _tuner_flag)
	{
		_central.set_mod (pi_id, index, *cls_ptr);
	}
}



// Configures the default controllers, if any
// selected_slot_id adds to only one slot, -1: all slots
void	Model::add_default_ctrl (int selected_slot_id)
{
	// First, lists the slots in the processing order
	std::vector <int> slot_id_list;
	ToolsRouting::build_ordered_node_list (
		slot_id_list, true, _prog_cur, use_aud_pi_list ()
	);

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
		const auto     it_slot = _prog_cur._slot_map.find (slot_id);
		assert (it_slot != _prog_cur._slot_map.end ());
		if (! _prog_cur.is_slot_empty (it_slot))
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
								ok_flag =
									(it_ctrl->second._bind_sptr.get () == nullptr);
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
					if (cls._bind_sptr.get () != nullptr)
					{
						if (cls._bind_sptr->_source._type == ControllerType_POT)
						{
							used_pot_set.insert (cls._bind_sptr->_source._index);
						}
					}
					for (const auto &mod_sptr : cls._mod_arr)
					{
						assert (mod_sptr.get () != nullptr);
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
			const doc::Slot & slot = _prog_cur.use_slot (slot_id);
			const doc::PluginSettings &   settings =
				slot.use_settings (PiType_MAIN);
			const auto     it_ctrl =
				settings._map_param_ctrl.find (index);
			if (it_ctrl != settings._map_param_ctrl.end ())
			{
				assert (it_ctrl->second._bind_sptr.get () == nullptr);
				cls = it_ctrl->second;
			}

			// Inserts the controller
			cls._bind_sptr = std::make_shared <doc::CtrlLink> ();
			cls._bind_sptr->_source._type  = ControllerType_POT;
			cls._bind_sptr->_source._index = pot_index;

			set_param_ctrl (slot_id, PiType_MAIN, index, cls);

			// Marks the pot as used
			used_pot_set.insert (pot_index);
		}
	}
}



void	Model::clear_signal_port (int port_id, bool req_exist_flag)
{
	const auto     it_port = _prog_cur._port_map.find (port_id);
	if (it_port == _prog_cur._port_map.end ())
	{
		if (req_exist_flag)
		{
			assert (false);
		}
	}
	else
	{
		// Removes any controller listening to this port.
		// Not strictly necessary, but helps to ensure global consistency.
		ControlSource  src;
		src._index = port_id;
		src._type  = ControllerType_FX_SIG;
		remove_ctrl_src (src);

		_prog_cur._port_map.erase (it_port);

		apply_settings ();

		if (_obs_ptr != nullptr)
		{
			_obs_ptr->clear_signal_port (port_id);
		}
	}
}



// Slot must exist and contain a plug-in of the same type as the settings.
// Does not copy the flags
// Requires commit
void		Model::apply_plugin_settings (int slot_id, PiType type, const doc::PluginSettings &settings, bool ctrl_flag, bool pres_flag)
{
	doc::Slot &    slot = _prog_cur.use_slot (slot_id);

	const int      nbr_param = int (settings._param_list.size ());
	const doc::CtrlLinkSet cls_empty {};
	auto           it_ctrl   = settings._map_param_ctrl.begin ();
	auto           it_pres   = settings._map_param_pres.begin ();
	const auto     it_id_map = _pi_id_map.find (slot_id);
	assert (it_id_map != _pi_id_map.end ());
	const int      pi_id     = it_id_map->second._pi_id_arr [type];
	assert (pi_id >= 0);
	doc::PluginSettings &   settings_new = slot.use_settings (type);
	const piapi::PluginDescInterface &	pi_desc =
		get_model_desc (slot._pi_model);
	for (int index = 0; index < nbr_param; ++index)
	{
		bool           param_set_flag = false;
		bool           ctrl_set_flag  = false;
		bool           pres_set_flag  = false;

		// Presentation
		if (pres_flag && it_pres != settings._map_param_pres.end ())
		{
			if (index >= it_pres->first)
			{
				if (index == it_pres->first)
				{
					const doc::ParamPresentation & pres = it_pres->second;
					set_param_pres_pre_commit (slot_id, type, index, &pres);
					push_set_param_pres (slot_id, type, index, &pres);
					pres_set_flag = true;

					if (type == PiType_MAIN && pres._ref_beats >= 0)
					{
						doc::ParamPresentation *   pres_new_ptr =
							settings_new.use_pres_if_tempo_ctrl (index);
						assert (pres_new_ptr != nullptr);
						float          val_nrm = 0;
						set_param_beats_pre_commit (
							slot_id,
							pi_id,
							index,
							pres._ref_beats,
							*pres_new_ptr,
							pi_desc,
							val_nrm
						);
						push_set_param (
							slot_id, type, index, val_nrm, true, pres._ref_beats
						);
						param_set_flag = true;
					}
				}
				++ it_pres;
			}
		}

		// Controllers
		if (ctrl_flag && it_ctrl != settings._map_param_ctrl.end ())
		{
			if (index >= it_ctrl->first)
			{
				if (index == it_ctrl->first)
				{
					set_prog_ctrl (
						_prog_cur, slot_id, type, index, it_ctrl->second
					);
					push_set_param_ctrl (slot_id, type, index, it_ctrl->second);
					ctrl_set_flag = true;
				}
				++ it_ctrl;
			}
		}

		// Default action
		if (! param_set_flag)
		{
			const float       val = settings._param_list [index];
			set_param_pre_commit (slot_id, pi_id, type, index, val);
			push_set_param (slot_id, type, index, val, false, 0);
		}
		if (ctrl_flag && ! ctrl_set_flag)
		{
			set_prog_ctrl (_prog_cur, slot_id, type, index, cls_empty);
			push_set_param_ctrl (slot_id, type, index, cls_empty);
		}
		if (pres_flag && ! pres_set_flag)
		{
			set_param_pres_pre_commit (slot_id, type, index, nullptr);
			push_set_param_pres (slot_id, type, index, nullptr);
		}
	}
}



bool	Model::process_async_cmd ()
{
	bool           proc_flag = false;

	CmdAsyncMgr::CellType * cell_ptr = nullptr;
	do
	{
		cell_ptr = _async_cmd.dequeue ();
		if (cell_ptr != nullptr)
		{
			proc_flag = true;
			cell_ptr->_val._content._msg_sptr->process (*this);
			cell_ptr->_val.ret ();
		}
	}
	while (cell_ptr != nullptr);

	return proc_flag;
}



cmd::Cnx	Model::convert_connection (const doc::Cnx &cnx_doc, const SlotIdToPosMap &pos_map)
{
	assert (cnx_doc.is_valid ());

	cmd::Cnx       cnx;

	convert_cnx_end (cnx._src, cnx_doc.use_src (), pos_map);
	convert_cnx_end (cnx._dst, cnx_doc.use_dst (), pos_map);

	return cnx;
}



void	Model::convert_cnx_end (cmd::CnxEnd &ce_c, const doc::CnxEnd &ce_d, const SlotIdToPosMap &pos_map)
{
	// Audio I/O
	if (ce_d.get_type () == doc::CnxEnd::Type_IO)
	{
		ce_c._slot_type = cmd::CnxEnd::SlotType_IO;
		ce_c._slot_pos  = 0;
		ce_c._pin       = ce_d.get_pin ();
	}

	// Send / Return
	else if (ce_d.get_type () == doc::CnxEnd::Type_RS)
	{
		ce_c._slot_type = cmd::CnxEnd::SlotType_RS;
		ce_c._slot_pos  = 0;
		ce_c._pin       = ce_d.get_pin ();
	}

	// Standard node
	else
	{
		assert (ce_d.get_type () == doc::CnxEnd::Type_NORMAL);
		const int      slot_id = ce_d.get_slot_id ();
		const SlotIdToPosMap::const_iterator it = pos_map.find (slot_id);
		assert (it != pos_map.end ());
		const int      slot_pos = it->second;
		assert (slot_pos >= 0);

		ce_c._slot_type = cmd::CnxEnd::SlotType_NORMAL;
		ce_c._slot_pos  = slot_pos;
		ce_c._pin       = ce_d.get_pin ();
	}
}



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
