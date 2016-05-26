/*****************************************************************************

        Model.h
        Author: Laurent de Soras, 2016

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
#include "mfx/doc/Bank.h"
#include "mfx/doc/PedalboardLayout.h"
#include "mfx/ui/UserInputInterface.h"



namespace mfx
{

namespace doc
{
	class ActionBank;
	class ActionParam;
	class ActionPreset;
	class ActionToggleFx;
	class ActionToggleTuner;
}



class Model
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       Model (ui::UserInputInterface::MsgQueue &queue_input_to_cmd, ui::UserInputInterface::MsgQueue &queue_input_to_audio, ui::UserInputInterface &input_device);
	virtual        ~Model ();

	// From the command thread
	void           process_messages (); // Call this regularly
	void           load_pedalboard_layout (const doc::PedalboardLayout &layout);
	void           load_bank (const doc::Bank &bank, int preset);

	static const std::array <int, Cst::_nbr_pedals> // [Pedal number] = Input switch index
	               _pedal_to_switch_map;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



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

	void           update_layout_bank ();
	void           preinstantiate_all_plugins_from_bank ();
	void           apply_settings ();
	void           check_mixer_plugin (int slot_index);
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

	cmd::Central   _central;
	doc::Bank      _bank;
	int            _preset_index;
	doc::PedalboardLayout
	               _layout_cur;
	doc::PedalboardLayout               // Base layout, without inherited pedals
	               _layout_base;
	doc::PedalboardLayout               // Cached: base layout + current bank.
	               _layout_bank;

	doc::Preset    _preset_cur;         // Current preset and settings, as known by cmd::Central. Must be synchronized with _bank._preset_arr [_preset_index].
	PiIdList       _pi_id_list;
	PedalStateArray
	               _pedal_state_arr;

	ui::UserInputInterface &
	               _input_device;
	ui::UserInputInterface::MsgQueue &
	               _queue_input_to_cmd;



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
