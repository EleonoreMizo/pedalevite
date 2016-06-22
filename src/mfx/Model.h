/*****************************************************************************

        Model.h
        Author: Laurent de Soras, 2016

All calls must be synchronized.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_Model_HEADER_INCLUDED)
#define mfx_Model_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/cmd/Central.h"
#include "mfx/cmd/CentralCbInterface.h"
#include "mfx/doc/Bank.h"
#include "mfx/doc/PedalboardLayout.h"
#include "mfx/doc/Setup.h"
#include "mfx/ui/UserInputInterface.h"
#include "mfx/ModelObserverInterface.h"



namespace mfx
{

namespace doc
{
	class ActionBank;
	class ActionParam;
	class ActionPreset;
	class ActionToggleFx;
	class ActionToggleTuner;
	class FxId;
}

namespace pi
{
	class Tuner;
}



class Model
:	public cmd::CentralCbInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       Model (ui::UserInputInterface::MsgQueue &queue_input_to_cmd, ui::UserInputInterface::MsgQueue &queue_input_to_audio, ui::UserInputInterface &input_device);
	virtual        ~Model ();

	// Audio
	void           set_process_info (double sample_freq, int max_block_size);
	void           process_block (float * const * dst_arr, const float * const * src_arr, int nbr_spl);
	bool           check_signal_clipping ();

	// Regular commands
	void           set_observer (ModelObserverInterface *obs_ptr);
	const piapi::PluginState &
	               use_default_settings (pi::PluginModel model) const;

	void           process_messages (); // Call this regularly

	void           set_edit_mode (bool edit_flag);
	void           set_pedalboard_layout (const doc::PedalboardLayout &layout);
	void           set_bank (int index, const doc::Bank &bank);
	void           select_bank (int index);
	void           set_bank_name (std::string name);
	void           set_preset_name (std::string name);
	void           activate_preset (int index);
	void           store_preset (int index);
	void           set_nbr_slots (int nbr_slots);
	void           insert_slot (int slot_index);
	void           erase_slot (int slot_index);
	void           set_plugin (int slot_index, pi::PluginModel type);
	void           remove_plugin (int slot_index);
	void           set_param (int slot_index, PiType type, int index, float val);
	void           set_param_ctrl (int slot_index, PiType type, int index, const doc::CtrlLinkSet &cls);

	static const std::array <int, Cst::_nbr_pedals> // [Pedal number] = Input switch index
	               _pedal_to_switch_map;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::cmd::CentralCbInterface
	virtual void   do_process_msg_audio_to_cmd (const Msg &msg);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	class SlotPiId
	{
	public:
		               SlotPiId ();
		std::array <int, PiType_NBR_ELT>
		               _pi_id_arr;
	};
	typedef std::vector <SlotPiId> PiIdList;

	class PedalState
	{
	public:
		bool           _press_flag = false;
		bool           _hold_flag  = false;
		int64_t        _press_ts   = INT64_MIN;
		int            _cycle_pos  = 0; // Counts only "PRESS" events
	};
	typedef std::array <PedalState, Cst::_nbr_pedals> PedalStateArray;

	void           update_layout ();
	void           preinstantiate_all_plugins_from_bank ();
	void           apply_settings ();
	void           apply_settings_normal ();
	void           apply_settings_tuner ();
	void           check_mixer_plugin (int slot_index, int slot_index_central);
	bool           has_mixer_plugin (const doc::Preset &preset, int slot_index);
	void           send_effect_settings (int pi_id, const doc::PluginSettings &settings);
	void           process_msg_ui ();
	int            find_pedal (int switch_index) const;
	void           process_pedal (int pedal_index, bool set_flag, int64_t date);
	void           process_pedal_event (int pedal_index, doc::ActionTrigger trigger);
	void           process_action (const doc::PedalActionSingleInterface &action);
	void           process_action_bank (const doc::ActionBank &action);
	void           process_action_param (const doc::ActionParam &action);
	void           process_action_preset (const doc::ActionPreset &action);
	void           process_action_toggle_fx (const doc::ActionToggleFx &action);
	void           process_action_toggle_tuner (const doc::ActionToggleTuner &action);
	void           build_slot_info ();
	void           notify_slot_info ();
	int            find_slot_cur_preset (const doc::FxId &fx_id) const;
	void           find_slot_type_cur_preset (int &slot_index, PiType &type, int pi_id) const;
	bool           update_parameter (doc::Preset &preset, int slot_index, PiType type, int index, float val);
	void           fill_pi_init_data (int slot_index, ModelObserverInterface::PluginInitData &pi_data);

	cmd::Central   _central;

	// Global data
	doc::Setup     _setup;
	int            _bank_index;
	int            _preset_index;

	// Current and cached settings
	doc::Preset    _preset_cur;         // Current preset and settings, as known by cmd::Central. Layout is without inherited layouts
	doc::PedalboardLayout               // Final layout
	               _layout_cur;

	PiIdList       _pi_id_list;         // Not affected by the tuner
	PedalStateArray
	               _pedal_state_arr;
	int64_t        _hold_time;          // Pedal minimum hold time. Microseconds.
	bool           _edit_flag;          // Changes must be mirrored to _setup
	bool           _edit_preset_flag;   // _preset_cur corresonds to _bank_index/_preset_index
	bool           _tuner_flag;
	int            _tuner_pi_id;
	pi::Tuner *    _tuner_ptr;          // Can be 0.

	ui::UserInputInterface &
	               _input_device;
	ui::UserInputInterface::MsgQueue &
	               _queue_input_to_cmd;
	ModelObserverInterface *            // Can be 0.
	               _obs_ptr;
	const int      _dummy_mix_id;

	ModelObserverInterface::SlotInfoList   // Cached. Not affected by the tuner
	               _slot_info;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Model ()                               = delete;
	               Model (const Model &other)             = delete;
	Model &        operator = (const Model &other)        = delete;
	bool           operator == (const Model &other) const = delete;
	bool           operator != (const Model &other) const = delete;

}; // class Model



}  // namespace mfx



//#include "mfx/Model.hpp"



#endif   // mfx_Model_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
