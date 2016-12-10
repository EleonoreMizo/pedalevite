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
#include "mfx/ChnMode.h"
#include "mfx/ModelObserverInterface.h"

#include <map>



namespace mfx
{

namespace doc
{
	class ActionBank;
	class ActionParam;
	class ActionPreset;
	class ActionToggleFx;
	class ActionToggleTuner;
	class ActionTempo;
	class FxId;
}

class FileIOInterface;
class MeterResultSet;
class PedalLoc;

namespace pi
{
namespace tuner
{
	class Tuner;
}
}



class Model
:	public cmd::CentralCbInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       Model (ui::UserInputInterface::MsgQueue &queue_input_to_cmd, ui::UserInputInterface::MsgQueue &queue_input_to_audio, ui::UserInputInterface &input_device, FileIOInterface &file_io);
	virtual        ~Model ();

	// Audio
	void           set_process_info (double sample_freq, int max_block_size);
	void           process_block (float * const * dst_arr, const float * const * src_arr, int nbr_spl);
	MeterResultSet &
	               use_meters ();

	// Regular commands
	void           set_observer (ModelObserverInterface *obs_ptr);
	const piapi::PluginState &
	               use_default_settings (std::string model) const;

	void           process_messages (); // Call this regularly

	int            save_to_disk ();
	int            load_from_disk ();

	void           set_edit_mode (bool edit_flag);
	void           set_save_mode (doc::Setup::SaveMode mode);
	void           set_pedalboard_layout (const doc::PedalboardLayout &layout);
	void           set_pedal (const PedalLoc &loc, const doc::PedalActionGroup &content);
	void           set_setup_name (std::string name);
	void           set_bank (int index, const doc::Bank &bank);
	void           select_bank (int index);
	void           set_bank_name (std::string name);
	void           set_preset_name (std::string name);
	void           activate_preset (int preset_index);
	void           store_preset (int preset_index, int bank_index);
	void           set_chn_mode (ChnMode mode);
	void           set_master_vol (double vol);
	void           set_tuner (bool tuner_flag);
	int            add_slot ();
	void           remove_slot (int slot_id);
	void           insert_slot_in_chain (int index, int slot_id);
	void           erase_slot_from_chain (int index);
	void           set_slot_label (int slot_id, std::string name);
	void           set_plugin (int slot_id, std::string model);
	void           remove_plugin (int slot_id);
	void           set_plugin_mono (int slot_id, bool mono_flag);
	void           set_param (int slot_id, PiType type, int index, float val);
	void           set_param_beats (int slot_id, int index, float beats);
	void           set_param_ctrl (int slot_id, PiType type, int index, const doc::CtrlLinkSet &cls);
	void           remove_ctrl_src (const ControlSource &src);
	void           override_param_ctrl (int slot_id, PiType type, int index, int rotenc_index);
	void           reset_all_overridden_param_ctrl ();
	void           reset_all_overridden_param_ctrl (int slot_id);
	void           set_signal_port (int port_id, const doc::SignalPort &port);
	void           clear_signal_port (int port_id);
	void           clear_all_signal_ports_for_slot (int slot_id);

	std::vector <std::string>
	               list_plugin_models () const;
	const piapi::PluginDescInterface &
	               get_model_desc (std::string model_id) const;
	std::chrono::microseconds
	               get_cur_date () const;

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
	typedef std::map <int, SlotPiId> PiIdMap; // [slot_id] -> SlotPiId

	class PedalState
	{
	public:
		               PedalState ();
		bool           _press_flag;
		bool           _hold_flag;
		bool           _reset_cycle_on_preset_flag;
		std::chrono::microseconds
		               _press_ts;
		int            _cycle_pos;       // Counts only "PRESS" events
	};
	typedef std::array <PedalState, Cst::_nbr_pedals> PedalStateArray;

	class OverrideLoc
	{
	public:
		bool           operator < (const OverrideLoc &rhs) const;
		int            _pi_id;           // >= 0
		int            _index;           // Parameter index, >= 0
	};

	void           update_layout ();
	void           preinstantiate_all_plugins_from_bank ();
	void           apply_settings ();
	void           apply_settings_normal ();
	void           apply_settings_tuner ();
	void           check_mixer_plugin (int slot_id, int slot_index_central, int chain_flag);
	bool           has_mixer_plugin (const doc::Preset &preset, int slot_id);
	void           send_effect_settings (int pi_id, int slot_id, PiType type, const doc::PluginSettings &settings);
	void           process_msg_ui ();
	int            find_pedal (int switch_index) const;
	void           process_pedal (int pedal_index, bool set_flag, std::chrono::microseconds date);
	void           process_pedal_event (int pedal_index, doc::ActionTrigger trigger);
	void           process_action (const doc::PedalActionSingleInterface &action, std::chrono::microseconds ts);
	void           process_action_bank (const doc::ActionBank &action);
	void           process_action_param (const doc::ActionParam &action);
	void           process_action_preset (const doc::ActionPreset &action);
	void           process_action_toggle_fx (const doc::ActionToggleFx &action);
	void           process_action_toggle_tuner (const doc::ActionToggleTuner &action);
	void           process_action_tempo (const doc::ActionTempo &action, std::chrono::microseconds ts);
	void           build_slot_info ();
	void           notify_slot_info ();
	int            find_slot_cur_preset (const doc::FxId &fx_id) const;
	void           find_slot_type_cur_preset (int &slot_id, PiType &type, int pi_id) const;
	bool           update_parameter (doc::Preset &preset, int slot_id, PiType type, int index, float val);
	bool           update_parameter (doc::Preset &preset, doc::Preset::SlotMap::iterator it_slot, PiType type, int index, float val);
	void           fill_pi_init_data (int slot_id, ModelObserverInterface::PluginInitData &pi_data);
	void           update_all_beat_parameters ();
	void           update_all_overriden_param_ctrl ();
	void           update_param_ctrl (const OverrideLoc &loc);
	void           set_param_ctrl_with_override (const doc::CtrlLinkSet &cls, int pi_id, int slot_id, PiType type, int index);
	void           set_param_ctrl_internal (const doc::CtrlLinkSet &cls, int pi_id, int slot_id, PiType type, int index);
	void           add_default_ctrl (int selected_slot_id = -1);
	void           clear_signal_port (int port_id, bool req_exist_flag);

	cmd::Central   _central;

	// Global data
	doc::Setup     _setup;
	int            _bank_index;
	int            _preset_index;

	// Current and cached settings
	doc::Preset    _preset_cur;         // Current preset and settings, as known by cmd::Central (but does not feature the parameter override). Layout is without inherited layouts.
	doc::PedalboardLayout               // Final layout
	               _layout_cur;

	PiIdMap        _pi_id_map;         // Not affected by the tuner
	PedalStateArray
	               _pedal_state_arr;
	std::chrono::microseconds
	               _hold_time;          // Pedal minimum hold time. Microseconds.
	bool           _edit_flag;          // Changes must be mirrored to _setup
	bool           _edit_preset_flag;   // _preset_cur corresonds to _bank_index/_preset_index
	bool           _tuner_flag;
	int            _tuner_pi_id;
	pi::tuner::Tuner *
	               _tuner_ptr;          // Can be 0.

	FileIOInterface &
	               _file_io;
	ui::UserInputInterface &
	               _input_device;
	ui::UserInputInterface::MsgQueue &
	               _queue_input_to_cmd;
	ModelObserverInterface *            // Can be 0.
	               _obs_ptr;
	const int      _dummy_mix_id;
	std::chrono::microseconds
	               _tempo_last_ts;      // Timestamp of the last tempo pedal action
	double         _tempo;              // Actual tempo
	int            _latest_slot_id;     // >= 0

	ModelObserverInterface::SlotInfoMap // Cached. Not affected by the tuner
	               _slot_info;

	std::map <OverrideLoc, int>         // Parameter -> rot enc
	               _override_map;



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
