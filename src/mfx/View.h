/*****************************************************************************

        View.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_View_HEADER_INCLUDED)
#define mfx_View_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/doc/Setup.h"
#include "mfx/ToolsRouting.h"
#include "mfx/ModelObserverInterface.h"

#include <set>



namespace mfx
{

namespace doc
{
	class FxId;
}



class View
:	public ModelObserverInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	class OverrideLoc
	{
	public:
		bool           operator < (const OverrideLoc &rhs) const;
		int            _slot_id;
		PiType         _type;
		int            _index;
	};
	typedef std::map <OverrideLoc, int> OverrideMap;

	               View () = default;

	void           add_observer (ModelObserverInterface &obs);
	void           remove_observer (ModelObserverInterface &obs);
	void           set_pi_lists (const std::vector <std::string> &pi_aud_list, const std::vector <std::string> &pi_sig_list);

	double         get_tempo () const;
	bool           is_editing () const;
	bool           is_tuner_active () const;
	bool           is_click_active () const;
	float          get_tuner_freq () const;
	const doc::Setup &
	               use_setup () const;
	const doc::Preset &
	               use_preset_cur () const;
	const SlotInfoMap &
	               use_slot_info_map () const;
	int            get_bank_index () const;
	int            get_preset_index () const;
	const OverrideMap &
	               use_param_ctrl_override_map () const;

	std::set <std::string>
	               collect_labels (bool cur_preset_flag) const;

	const std::vector <std::string> &
	               use_pi_aud_list () const;
	const std::vector <std::string> &
	               use_pi_sig_list () const;
	const ToolsRouting::NodeMap &
	               use_graph () const;
	const std::vector <int> &
	               use_slot_list_aud () const;
	const std::vector <int> &
	               use_slot_list_sig () const;
	int            build_ordered_node_list (std::vector <int> &slot_id_list, bool audio_first_flag) const;

	static void    update_parameter (doc::Preset &preset, int slot_index, PiType type, int index, float val);
	static float   get_param_val (const doc::Preset &preset, int slot_id, PiType type, int index);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::ModelObserverInterface
	void           do_set_tempo (double bpm) override;
	void           do_set_edit_mode (bool edit_flag) override;
	void           do_set_save_mode (doc::Setup::SaveMode mode) override;
	void           do_set_setup_name (std::string name) override;
	void           do_set_pedalboard_layout (const doc::PedalboardLayout &layout) override;
	void           do_set_pedal (const PedalLoc &loc, const doc::PedalActionGroup &content) override;
	void           do_set_bank (int index, const doc::Bank &bank) override;
	void           do_select_bank (int index) override;
	void           do_set_bank_name (std::string name) override;
	void           do_set_preset_name (std::string name) override;
	void           do_set_preset (int bank_index, int preset_index, const doc::Preset &preset) override;
	void           do_activate_preset (int index) override;
	void           do_store_preset (int preset_index, int bank_index) override;
	void           do_set_prog_switch_mode (doc::ProgSwitchMode mode) override;
	void           do_set_chn_mode (ChnMode mode) override;
	void           do_set_master_vol (float vol) override;
	void           do_set_tuner (bool active_flag) override;
	void           do_set_tuner_freq (float freq) override;
	void           do_set_slot_info_for_current_preset (const SlotInfoMap &info_map) override;
	void           do_set_param (int slot_id, int index, float val, PiType type) override;
	void           do_set_param_beats (int slot_id, int index, float beats) override;
	void           do_add_slot (int slot_id) override;
	void           do_remove_slot (int slot_id) override;
	void           do_set_routing (const doc::Routing &routing) override;
	void           do_set_slot_label (int slot_id, std::string name) override;
	void           do_set_plugin (int slot_id, const PluginInitData &pi_data) override;
	void           do_remove_plugin (int slot_id) override;
	void           do_set_plugin_mono (int slot_id, bool mono_flag) override;
	void           do_set_plugin_reset (int slot_id, bool reset_flag) override;
	void           do_set_param_pres (int slot_id, PiType type, int index, const doc::ParamPresentation *pres_ptr) override;
	void           do_set_param_ctrl (int slot_id, PiType type, int index, const doc::CtrlLinkSet &cls) override;
	void           do_override_param_ctrl (int slot_id, PiType type, int index, int rotenc_index) override;
	void           do_set_signal_port (int port_id, const doc::SignalPort &port) override;
	void           do_clear_signal_port (int port_id) override;
	void           do_add_settings (std::string model, int index, std::string name, const doc::PluginSettings &s_main, const doc::PluginSettings &s_mix) override;
	void           do_remove_settings (std::string model, int index) override;
	void           do_clear_all_settings () override;
	void           do_set_click (bool click_flag) override;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::set <ModelObserverInterface *> ObsSet;

	void           collect_labels (std::set <std::string> &labels, const doc::Preset &preset) const;
	void           collect_labels (std::set <std::string> &labels, const doc::PedalboardLayout &layout) const;
	void           collect_labels (std::set <std::string> &labels, const doc::FxId &fx_id) const;
	void           update_graph_info ();

	ObsSet         _obs_set;

	// Cached data
	double         _tempo        = Cst::_tempo_ref;
	bool           _edit_flag    = false;
	bool           _click_flag   = false;
	bool           _tuner_flag   = false;
	float          _tuner_freq   = 0;
	int            _bank_index   = 0;
	int            _preset_index = 0;
	doc::Setup     _setup;
	doc::Preset    _preset_cur;
	SlotInfoMap    _slot_info_map;
	OverrideMap    _override_map;
	std::vector <std::string>
	               _pi_aud_list;
	std::vector <std::string>
	               _pi_sig_list;

	// Computed from the cached data
	std::set <std::string>
	               _pi_aud_set;
	ToolsRouting::NodeMap
	               _graph;
	std::vector <int>
	               _slot_list_aud;
	std::vector <int>
	               _slot_list_sig;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               View (const View &other)              = delete;
	View &         operator = (const View &other)        = delete;
	bool           operator == (const View &other) const = delete;
	bool           operator != (const View &other) const = delete;

}; // class View



}  // namespace mfx



//#include "mfx/View.hpp"



#endif   // mfx_View_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
