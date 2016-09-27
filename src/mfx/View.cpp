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

#include "mfx/PedalLoc.h"
#include "mfx/View.h"

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



float	View::get_tuner_freq () const
{
	return _tuner_freq;
}



const doc::Setup &	View::use_setup () const
{
	return _setup;
}



const doc::Preset &	View::use_preset_cur () const
{
	return _preset_cur;
}



const ModelObserverInterface::SlotInfoMap &	View::use_slot_info_map () const
{
	return _slot_info_map;
}



int	View::get_bank_index () const
{
	return _bank_index;
}



int	View::get_preset_index () const
{
	return _preset_index;
}



const View::OverrideMap &	View::use_param_ctrl_override_map () const
{
	return _override_map;
}



// Allows negative slot_index and returns -1 in this case
int	View::conv_slot_index_to_id (int slot_index) const
{
	assert (slot_index < int (_preset_cur._routing._chain.size ()));
	return
		  (slot_index < 0)
		? -1
		: _preset_cur._routing._chain [slot_index];
}



void	View::update_parameter (doc::Preset &preset, int slot_id, PiType type, int index, float val)
{
	auto           it_slot = preset._slot_map.find (slot_id);
	if (preset.is_slot_empty (it_slot))
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



float	View::get_param_val (const doc::Preset &preset, int slot_id, PiType type, int index)
{
	float          val = 0;

	auto           it_slot = preset._slot_map.find (slot_id);
	assert (it_slot != preset._slot_map.end ());
	if (preset.is_slot_empty (it_slot))
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



void	View::do_set_preset_name (std::string name)
{
	_preset_cur._name = name;
	mfx_View_PROPAGATE (set_preset_name (name));
}



void	View::do_activate_preset (int index)
{
	_preset_index = index;
	_preset_cur   = _setup._bank_arr [_bank_index]._preset_arr [_preset_index];
	_slot_info_map.clear ();
	mfx_View_PROPAGATE (activate_preset (index));
}



void	View::do_store_preset (int preset_index, int bank_index)
{
	const int      bank_index2 = (bank_index < 0) ? _bank_index : bank_index;
	_setup._bank_arr [bank_index2]._preset_arr [preset_index] = _preset_cur;
	mfx_View_PROPAGATE (store_preset (preset_index, bank_index));
}



void	View::do_set_chn_mode (ChnMode mode)
{
	_setup._chn_mode = mode;
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



void	View::do_set_slot_info_for_current_preset (const SlotInfoMap &info_map)
{
	_slot_info_map = info_map;
	mfx_View_PROPAGATE (set_slot_info_for_current_preset (info_map));
}



void	View::do_set_param (int pi_id, int index, float val, int slot_id, PiType type)
{
	update_parameter (_preset_cur, slot_id, type, index, val);
	mfx_View_PROPAGATE (set_param (pi_id, index, val, slot_id, type));
}



void	View::do_set_param_beats (int slot_id, int index, float beats)
{
	doc::Slot &    slot = _preset_cur.use_slot (slot_id);
	doc::PluginSettings &   settings = slot.use_settings (PiType_MAIN);
	assert (index < int (settings._param_list.size ()));

	// Stores the beat value in the document
	auto           it_pres = settings._map_param_pres.find (index);
	assert (it_pres != settings._map_param_pres.end ());
	it_pres->second._ref_beats = beats;

	mfx_View_PROPAGATE (set_param_beats (slot_id, index, beats));
}



void	View::do_insert_slot (int slot_index, int slot_id)
{
	_preset_cur._slot_map.insert (std::make_pair (
		slot_id,
		doc::Preset::SlotSPtr ()
	));
	_preset_cur._routing._chain.insert (
		_preset_cur._routing._chain.begin () + slot_index,
		slot_id
	);
	_slot_info_map.clear ();
	mfx_View_PROPAGATE (insert_slot (slot_index, slot_id));
}



void	View::do_erase_slot (int slot_index)
{
	assert (slot_index < int (_preset_cur._routing._chain.size ()));

	const int      slot_id = _preset_cur._routing._chain [slot_index];
	_preset_cur._routing._chain.erase (
		_preset_cur._routing._chain.begin () + slot_index
	);
	auto           it_slot = _preset_cur._slot_map.find (slot_id);
	assert (it_slot != _preset_cur._slot_map.end ());
	_preset_cur._slot_map.erase (it_slot);

	_slot_info_map.clear ();

	mfx_View_PROPAGATE (erase_slot (slot_index));
}



void	View::do_set_slot_label (int slot_id, std::string name)
{
	auto           it_slot = _preset_cur._slot_map.find (slot_id);
	assert (it_slot != _preset_cur._slot_map.end ());
	doc::Preset::SlotSPtr &	slot_sptr = it_slot->second;
	if (slot_sptr.get () == 0)
	{
		slot_sptr = doc::Preset::SlotSPtr (new doc::Slot);
	}
	slot_sptr->_label = name;

	mfx_View_PROPAGATE (set_slot_label (slot_id, name));
}



void	View::do_set_plugin (int slot_id, const PluginInitData &pi_data)
{
	auto           it_slot = _preset_cur._slot_map.find (slot_id);
	assert (it_slot != _preset_cur._slot_map.end ());
	doc::Preset::SlotSPtr &	slot_sptr = it_slot->second;
	if (slot_sptr.get () == 0)
	{
		slot_sptr = doc::Preset::SlotSPtr (new doc::Slot);
	}
	slot_sptr->_pi_model = pi_data._model;
	const int      nbr_param = pi_data._nbr_param_arr [piapi::ParamCateg_GLOBAL];
	doc::PluginSettings &   settings = slot_sptr->_settings_all [pi_data._model];
	settings._param_list.resize (nbr_param, 0);

	mfx_View_PROPAGATE (set_plugin (slot_id, pi_data));
}



void	View::do_remove_plugin (int slot_id)
{
	auto           it_slot = _preset_cur._slot_map.find (slot_id);
	assert (it_slot != _preset_cur._slot_map.end ());
	doc::Preset::SlotSPtr &	slot_sptr = it_slot->second;
	if (slot_sptr.get () != 0)
	{
		slot_sptr->_pi_model.clear ();
	}

	mfx_View_PROPAGATE (remove_plugin (slot_id));
}



void	View::do_set_plugin_mono (int slot_id, bool mono_flag)
{
	doc::Slot &    slot    = _preset_cur.use_slot (slot_id);
	doc::PluginSettings &   settings = slot.use_settings (PiType_MAIN);
	settings._force_mono_flag = mono_flag;

	mfx_View_PROPAGATE (set_plugin_mono (slot_id, mono_flag));
}



void	View::do_set_param_ctrl (int slot_id, PiType type, int index, const doc::CtrlLinkSet &cls)
{
	doc::Slot &    slot = _preset_cur.use_slot (slot_id);
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



#undef mfx_View_PROPAGATE



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	View::OverrideLoc::operator < (const OverrideLoc &rhs) const
{
	if (_slot_id < rhs._slot_id)
	{
		return true;
	}
	else if (_slot_id == rhs._slot_id)
	{
		if (_type < rhs._type)
		{
			return true;
		}
		else if (_type == rhs._type)
		{
			return (_index < rhs._index);
		}
	}

	return false;
}



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
