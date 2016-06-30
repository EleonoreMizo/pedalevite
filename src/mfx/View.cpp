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



const ModelObserverInterface::SlotInfoList &	View::use_slot_info_list () const
{
	return _slot_info_list;
}



int	View::get_bank_index () const
{
	return _bank_index;
}



int	View::get_preset_index () const
{
	return _preset_index;
}



void	View::update_parameter (doc::Preset &preset, int slot_index, PiType type, int index, float val)
{
	if (preset.is_slot_empty (slot_index))
	{
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
			assert (false);
		}
		else
		{
			settings._param_list [index] = val;
		}
	}
}



float	View::get_param_val (const doc::Preset &preset, int slot_index, PiType type, int index)
{
	float          val = 0;

	if (preset.is_slot_empty (slot_index))
	{
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



void	View::do_set_pedalboard_layout (const doc::PedalboardLayout &layout)
{
	_setup._layout = layout;
	mfx_View_PROPAGATE (set_pedalboard_layout (layout));
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
	_slot_info_list.clear ();
	mfx_View_PROPAGATE (activate_preset (index));
}



void	View::do_store_preset (int index)
{
	_setup._bank_arr [_bank_index]._preset_arr [index] = _preset_cur;
	mfx_View_PROPAGATE (store_preset (index));
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



void	View::do_set_slot_info_for_current_preset (const SlotInfoList &info_list)
{
	_slot_info_list = info_list;
	mfx_View_PROPAGATE (set_slot_info_for_current_preset (info_list));
}



void	View::do_set_param (int pi_id, int index, float val, int slot_index, PiType type)
{
	update_parameter (_preset_cur, slot_index, type, index, val);
	mfx_View_PROPAGATE (set_param (pi_id, index, val, slot_index, type));
}



void	View::do_set_param_beats (int slot_index, int index, float beats)
{
	doc::Slot &    slot = *(_preset_cur._slot_list [slot_index]);
	doc::PluginSettings &   settings = slot.use_settings (PiType_MAIN);
	assert (index < int (settings._param_list.size ()));

	// Stores the beat value in the document
	auto           it_pres = settings._map_param_pres.find (index);
	assert (it_pres != settings._map_param_pres.end ());
	it_pres->second._ref_beats = beats;

	mfx_View_PROPAGATE (set_param_beats (slot_index, index, beats));
}



void	View::do_set_nbr_slots (int nbr_slots)
{
	_preset_cur._slot_list.resize (nbr_slots);
	mfx_View_PROPAGATE (set_nbr_slots (nbr_slots));
}



void	View::do_insert_slot (int slot_index)
{
	_preset_cur._slot_list.insert (
		_preset_cur._slot_list.begin () + slot_index,
		doc::Preset::SlotSPtr ()
	);
	_slot_info_list.clear ();
	mfx_View_PROPAGATE (insert_slot (slot_index));
}



void	View::do_erase_slot (int slot_index)
{
	_preset_cur._slot_list.erase (
		_preset_cur._slot_list.begin () + slot_index
	);
	_slot_info_list.clear ();
	mfx_View_PROPAGATE (erase_slot (slot_index));
}



void	View::do_set_plugin (int slot_index, const PluginInitData &pi_data)
{
	doc::Preset::SlotSPtr &	slot_sptr = _preset_cur._slot_list [slot_index];
	if (slot_sptr.get () == 0)
	{
		slot_sptr = doc::Preset::SlotSPtr (new doc::Slot);
	}
	slot_sptr->_pi_model = pi_data._model;
	const int      nbr_param = pi_data._nbr_param_arr [piapi::ParamCateg_GLOBAL];
	doc::PluginSettings &   settings = slot_sptr->_settings_all [pi_data._model];
	settings._param_list.resize (nbr_param, 0);

	mfx_View_PROPAGATE (set_plugin (slot_index, pi_data));
}



void	View::do_remove_plugin (int slot_index)
{
	doc::Preset::SlotSPtr &	slot_sptr = _preset_cur._slot_list [slot_index];
	if (slot_sptr.get () != 0)
	{
		slot_sptr->_pi_model.clear ();
	}

	mfx_View_PROPAGATE (remove_plugin (slot_index));
}



void	View::do_set_param_ctrl (int slot_index, PiType type, int index, const doc::CtrlLinkSet &cls)
{
	doc::Slot &    slot = *(_preset_cur._slot_list [slot_index]);
	doc::PluginSettings &   settings = slot.use_settings (type);
	settings._map_param_ctrl [index] = cls;

	mfx_View_PROPAGATE (set_param_ctrl (slot_index, type, index, cls));
}



#undef mfx_View_PROPAGATE



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
