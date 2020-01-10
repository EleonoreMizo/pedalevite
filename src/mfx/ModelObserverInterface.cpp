/*****************************************************************************

        ModelObserverInterface.cpp
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

#include "mfx/Cst.h"
#include "mfx/PedalLoc.h"
#include "mfx/ModelObserverInterface.h"

#include <cassert>



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ModelObserverInterface::PluginInfo::PluginInfo (const piapi::PluginInterface &pi, const piapi::PluginDescInterface &desc, const std::vector <float> &param_arr)
:	_pi (pi)
,	_desc (desc)
,	_param_arr (param_arr)
{
	// Nothing
}



void	ModelObserverInterface::set_tempo (double bpm)
{
	assert (bpm > 0);
	
	do_set_tempo (bpm);
}



void	ModelObserverInterface::set_edit_mode (bool edit_flag)
{
	do_set_edit_mode (edit_flag);
}



void	ModelObserverInterface::set_save_mode (doc::Setup::SaveMode mode)
{
	assert (mode >= 0);
	assert (mode < doc::Setup::SaveMode_NBR_ELT);

	do_set_save_mode (mode);
}



void	ModelObserverInterface::set_setup_name (std::string name)
{
	do_set_setup_name (name);
}



// Reference lifetime is the call. Please make a copy.
void	ModelObserverInterface::set_pedalboard_layout (const doc::PedalboardLayout &layout)
{
	do_set_pedalboard_layout (layout);
}



// Reference lifetime is the call. Please make a copy.
void	ModelObserverInterface::set_pedal (const PedalLoc &loc, const doc::PedalActionGroup &content)
{
	assert (loc._type >= 0);
	assert (loc._type < PedalLoc::Type_NBR_ELT);
	assert (loc._pedal_index >= 0);
	assert (loc._pedal_index < Cst::_nbr_pedals);

	do_set_pedal (loc, content);
}



// Reference lifetime is the call. Please make a copy.
void	ModelObserverInterface::set_bank (int index, const doc::Bank &bank)
{
	assert (index >= 0);
	assert (index < Cst::_nbr_banks);

	do_set_bank (index, bank);
}



void	ModelObserverInterface::select_bank (int index)
{
	assert (index >= 0);
	assert (index < Cst::_nbr_banks);

	do_select_bank (index);
}



void	ModelObserverInterface::set_bank_name (std::string name)
{
	do_set_bank_name (name);
}



void	ModelObserverInterface::set_preset_name (std::string name)
{
	do_set_preset_name (name);
}



void	ModelObserverInterface::set_preset (int bank_index, int preset_index, const doc::Preset &preset)
{
	assert (bank_index >= 0);
	assert (bank_index < Cst::_nbr_banks);
	assert (preset_index >= 0);
	assert (preset_index < Cst::_nbr_presets_per_bank);

	do_set_preset (bank_index, preset_index, preset);
}



// set_slot_info_for_current_preset to be called later
void	ModelObserverInterface::activate_preset (int preset)
{
	assert (preset >= 0);
	assert (preset < Cst::_nbr_presets_per_bank);

	do_activate_preset (preset);
}



// bank_index < 0: use the current bank
void	ModelObserverInterface::store_preset (int preset_index, int bank_index)
{
	assert (preset_index >= 0);
	assert (preset_index < Cst::_nbr_presets_per_bank);
	assert (bank_index < Cst::_nbr_banks);

	do_store_preset (preset_index, bank_index);
}



void	ModelObserverInterface::set_prog_switch_mode (doc::ProgSwitchMode mode)
{
	assert (int (mode) >= 0);
	assert (mode < doc::ProgSwitchMode::NBR_ELT);

	do_set_prog_switch_mode (mode);
}



void	ModelObserverInterface::set_chn_mode (ChnMode mode)
{
	assert (mode >= 0);
	assert (mode < ChnMode_NBR_ELT);

	do_set_chn_mode (mode);
}



void	ModelObserverInterface::set_master_vol (float vol)
{
	assert (vol > 0);

	do_set_master_vol (vol);
}



void	ModelObserverInterface::set_tuner (bool active_flag)
{
	do_set_tuner (active_flag);
}



void	ModelObserverInterface::set_tuner_freq (float freq)
{
	do_set_tuner_freq (freq);
}



// Reference lifetime is the call. Please make a copy.
void	ModelObserverInterface::set_slot_info_for_current_preset (const SlotInfoMap &info_map)
{
	do_set_slot_info_for_current_preset (info_map);
}



void	ModelObserverInterface::set_param (int slot_id, int index, float val, PiType type)
{
	assert (slot_id >= 0);
	assert (index >= 0);
	assert (val >= 0);
	assert (val <= 1);
	assert (type >= 0);
	assert (type < PiType_NBR_ELT);

	do_set_param (slot_id, index, val, type);
}



// set_param has been called before with the real parameter value
void	ModelObserverInterface::set_param_beats (int slot_id, int index, float beats)
{
	assert (slot_id >= 0);
	assert (index >= 0);
	assert (beats >= 0);

	do_set_param_beats (slot_id, index, beats);
}



// set_slot_info_for_current_preset to be called later
void	ModelObserverInterface::add_slot (int slot_id)
{
	assert (slot_id >= 0);

	do_add_slot (slot_id);
}



// remove_plugin et al. to be called before
void	ModelObserverInterface::remove_slot (int slot_id)
{
	assert (slot_id >= 0);

	do_remove_slot (slot_id);
}



void	ModelObserverInterface::set_routing (const doc::Routing &routing)
{
	do_set_routing (routing);
}



void	ModelObserverInterface::set_slot_label (int slot_id, std::string name)
{
	assert (slot_id >= 0);

	do_set_slot_label (slot_id, name);
}



// set_slot_info_for_current_preset to be called later
void	ModelObserverInterface::set_plugin (int slot_id, const PluginInitData &pi_data)
{
	assert (slot_id >= 0);

	do_set_plugin (slot_id, pi_data);
}



// set_slot_info_for_current_preset to be called later
void	ModelObserverInterface::remove_plugin (int slot_id)
{
	assert (slot_id >= 0);

	do_remove_plugin (slot_id);
}



void	ModelObserverInterface::set_plugin_mono (int slot_id, bool mono_flag)
{
	assert (slot_id >= 0);

	do_set_plugin_mono (slot_id, mono_flag);
}



void	ModelObserverInterface::set_plugin_reset (int slot_id, bool reset_flag)
{
	assert (slot_id >= 0);

	do_set_plugin_reset (slot_id, reset_flag);
}



void	ModelObserverInterface::set_param_pres (int slot_id, PiType type, int index, const doc::ParamPresentation *pres_ptr)
{
	assert (slot_id >= 0);
	assert (type >= 0);
	assert (type < PiType_NBR_ELT);
	assert (index >= 0);

	do_set_param_pres (slot_id, type, index, pres_ptr);
}



void	ModelObserverInterface::set_param_ctrl (int slot_id, PiType type, int index, const doc::CtrlLinkSet &cls)
{
	assert (slot_id >= 0);
	assert (type >= 0);
	assert (type < PiType_NBR_ELT);
	assert (index >= 0);

	do_set_param_ctrl (slot_id, type, index, cls);
}



// rotenc_index < 0: removes the override
void	ModelObserverInterface::override_param_ctrl (int slot_id, PiType type, int index, int rotenc_index)
{
	assert (slot_id >= 0);
	assert (type >= 0);
	assert (type < PiType_NBR_ELT);
	assert (index >= 0);

	do_override_param_ctrl (slot_id, type, index, rotenc_index);
}



void	ModelObserverInterface::set_signal_port (int port_id, const doc::SignalPort &port)
{
	assert (port_id >= 0);
	assert (port._sig_index >= 0);
	assert (port._slot_id >= 0);

	do_set_signal_port (port_id, port);
}



void	ModelObserverInterface::clear_signal_port (int port_id)
{
	assert (port_id >= 0);

	do_clear_signal_port (port_id);
}



void	ModelObserverInterface::add_settings (std::string model, int index, std::string name, const doc::PluginSettings &s_main, const doc::PluginSettings &s_mix)
{
	assert (! model.empty ());
	assert (index >= 0);

	do_add_settings (model, index, name, s_main, s_mix);
}



void	ModelObserverInterface::remove_settings (std::string model, int index)
{
	assert (! model.empty ());
	assert (index >= 0);

	do_remove_settings (model, index);
}



void	ModelObserverInterface::clear_all_settings ()
{
	do_clear_all_settings ();
}



void	ModelObserverInterface::set_click (bool click_flag)
{
	do_set_click (click_flag);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
