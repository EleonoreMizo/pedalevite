/*****************************************************************************

        ModelObserverInterface.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_ModelObserverInterface_HEADER_INCLUDED)
#define mfx_ModelObserverInterface_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/doc/Setup.h"
#include "mfx/piapi/PluginInterface.h"
#include "mfx/ChnMode.h"
#include "mfx/Dir.h"
#include "mfx/PiType.h"

#include <array>
#include <map>
#include <memory>
#include <vector>



namespace mfx
{

class PedalLoc;

namespace doc
{
	class PedalboardLayout;
	class Bank;
	class CtrlLinkSet;
}

namespace piapi
{
	class PluginDescInterface;
}


class ModelObserverInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	class PluginInfo
	{
	public:
		               PluginInfo (const piapi::PluginInterface &pi, const piapi::PluginDescInterface &desc, const std::vector <float> &param_arr);
		const piapi::PluginInterface &
		               _pi;
		const piapi::PluginDescInterface &
		               _desc;
		const std::vector <float> &
		               _param_arr;
	};
	typedef std::shared_ptr <PluginInfo> PluginInfoSPtr;
	typedef std::array <PluginInfoSPtr, PiType_NBR_ELT> SlotInfo;
	typedef std::map <int, SlotInfo> SlotInfoMap;

	class PluginInitData
	{
	public:
		std::string    _model;
		std::array <int, Dir_NBR_ELT>
		               _nbr_io_arr;
		int            _nbr_sig;       // Number of signal outputs
		std::array <int, piapi::ParamCateg_NBR_ELT>
		               _nbr_param_arr;
		bool           _prefer_stereo_flag;
	};

	virtual        ~ModelObserverInterface () = default;

	void           set_tempo (double bpm);
	void           set_edit_mode (bool edit_flag);
	void           set_save_mode (doc::Setup::SaveMode mode);
	void           set_setup_name (std::string name);
	void           set_pedalboard_layout (const doc::PedalboardLayout &layout);
	void           set_pedal (const PedalLoc &loc, const doc::PedalActionGroup &content);
	void           set_bank (int index, const doc::Bank &bank);
	void           select_bank (int index);
	void           set_bank_name (std::string name);
	void           set_preset_name (std::string name);
	void           activate_preset (int index);
	void           store_preset (int preset_index, int bank_index);
	void           set_chn_mode (ChnMode mode);
	void           set_master_vol (float vol);
	void           set_tuner (bool active_flag);
	void           set_tuner_freq (float freq);
	void           set_slot_info_for_current_preset (const SlotInfoMap &info_map);
	void           set_param (int pi_id, int index, float val, int slot_id, PiType type);
	void           set_param_beats (int slot_id, int index, float beats);
	void           add_slot (int slot_id);
	void           remove_slot (int slot_id);
	void           insert_slot_in_chain (int index, int slot_id);
	void           erase_slot_from_chain (int index);
	void           set_slot_label (int slot_id, std::string name);
	void           set_plugin (int slot_id, const PluginInitData &pi_data);
	void           remove_plugin (int slot_id);
	void           set_plugin_mono (int slot_id, bool mono_flag);
	void           set_plugin_reset (int slot_id, bool reset_flag);
	void           set_param_pres (int slot_id, PiType type, int index, const doc::ParamPresentation *pres_ptr);
	void           set_param_ctrl (int slot_id, PiType type, int index, const doc::CtrlLinkSet &cls);
	void           override_param_ctrl (int slot_id, PiType type, int index, int rotenc_index);
	void           set_signal_port (int port_id, const doc::SignalPort &port);
	void           clear_signal_port (int port_id);
	void           add_settings (std::string model, int index, std::string name, const doc::PluginSettings &s_main, const doc::PluginSettings &s_mix);
	void           remove_settings (std::string model, int index);
	void           clear_all_settings ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual void   do_set_tempo (double bpm) = 0;
	virtual void   do_set_edit_mode (bool edit_flag) = 0;
	virtual void   do_set_save_mode (doc::Setup::SaveMode mode) = 0;
	virtual void   do_set_setup_name (std::string name) = 0;
	virtual void   do_set_pedalboard_layout (const doc::PedalboardLayout &layout) = 0;
	virtual void   do_set_pedal (const PedalLoc &loc, const doc::PedalActionGroup &content) = 0;
	virtual void   do_set_bank (int index, const doc::Bank &bank) = 0;
	virtual void   do_select_bank (int index) = 0;
	virtual void   do_set_bank_name (std::string name) = 0;
	virtual void   do_set_preset_name (std::string name) = 0;
	virtual void   do_activate_preset (int index) = 0;
	virtual void   do_store_preset (int preset_index, int bank_index) = 0;
	virtual void   do_set_chn_mode (ChnMode mode) = 0;
	virtual void   do_set_master_vol (float vol) = 0;
	virtual void   do_set_tuner (bool active_flag) = 0;
	virtual void   do_set_tuner_freq (float freq) = 0;
	virtual void   do_set_slot_info_for_current_preset (const SlotInfoMap &info_map) = 0;
	virtual void   do_set_param (int pi_id, int index, float val, int slot_index, PiType type) = 0;
	virtual void   do_set_param_beats (int slot_index, int index, float beats) = 0;
	virtual void   do_add_slot (int slot_id) = 0;
	virtual void   do_remove_slot (int slot_id) = 0;
	virtual void   do_insert_slot_in_chain (int index, int slot_id) = 0;
	virtual void   do_erase_slot_from_chain (int index) = 0;
	virtual void   do_set_slot_label (int slot_id, std::string name) = 0;
	virtual void   do_set_plugin (int slot_id, const PluginInitData &pi_data) = 0;
	virtual void   do_remove_plugin (int slot_id) = 0;
	virtual void   do_set_plugin_mono (int slot_id, bool mono_flag) = 0;
	virtual void   do_set_plugin_reset (int slot_id, bool reset_flag) = 0;
	virtual void   do_set_param_pres (int slot_id, PiType type, int index, const doc::ParamPresentation *pres_ptr) = 0;
	virtual void   do_set_param_ctrl (int slot_index, PiType type, int index, const doc::CtrlLinkSet &cls) = 0;
	virtual void   do_override_param_ctrl (int slot_index, PiType type, int index, int rotenc_index) = 0;
	virtual void   do_set_signal_port (int port_id, const doc::SignalPort &port) = 0;
	virtual void   do_clear_signal_port (int port_id) = 0;
	virtual void   do_add_settings (std::string model, int index, std::string name, const doc::PluginSettings &s_main, const doc::PluginSettings &s_mix) = 0;
	virtual void   do_remove_settings (std::string model, int index) = 0;
	virtual void   do_clear_all_settings () = 0;



}; // class ModelObserverInterface



}  // namespace mfx



//#include "mfx/ModelObserverInterface.hpp"



#endif   // mfx_ModelObserverInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
