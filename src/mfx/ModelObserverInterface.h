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

#include "mfx/piapi/PluginInterface.h"
#include "mfx/PiType.h"

#include <array>
#include <memory>
#include <vector>



namespace mfx
{

namespace doc
{
	class PedalboardLayout;
	class Bank;
	class CtrlLinkSet;
}



class ModelObserverInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	class PluginInfo
	{
	public:
		               PluginInfo (const piapi::PluginInterface &pi, const std::vector <float> &param_arr);
		const piapi::PluginInterface &
		               _pi;
		const std::vector <float> &
		               _param_arr;
	};
	typedef std::shared_ptr <PluginInfo> PluginInfoSPtr;
	typedef std::array <PluginInfoSPtr, PiType_NBR_ELT> SlotInfo;
	typedef std::vector <SlotInfo> SlotInfoList;

	class PluginInitData
	{
	public:
		std::string    _model;
		std::array <int, piapi::PluginInterface::Dir_NBR_ELT>
		               _nbr_io_arr;
		std::array <int, piapi::ParamCateg_NBR_ELT>
		               _nbr_param_arr;
		bool           _prefer_stereo_flag;
	};

	virtual        ~ModelObserverInterface () = default;

	void           set_edit_mode (bool edit_flag);
	void           set_pedalboard_layout (const doc::PedalboardLayout &layout);
	void           set_bank (int index, const doc::Bank &bank);
	void           select_bank (int index);
	void           set_bank_name (std::string name);
	void           set_preset_name (std::string name);
	void           activate_preset (int index);
	void           store_preset (int index);
	void           set_tuner (bool active_flag);
	void           set_tuner_freq (float freq);
	void           set_slot_info_for_current_preset (const SlotInfoList &info_list);
	void           set_param (int pi_id, int index, float val, int slot_index, PiType type);
	void           set_nbr_slots (int nbr_slots);
	void           insert_slot (int slot_index);
	void           erase_slot (int slot_index);
	void           set_plugin (int slot_index, const PluginInitData &pi_data);
	void           remove_plugin (int slot_index);
	void           set_param_ctrl (int slot_index, PiType type, int index, const doc::CtrlLinkSet &cls);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual void   do_set_edit_mode (bool edit_flag) = 0;
	virtual void   do_set_pedalboard_layout (const doc::PedalboardLayout &layout) = 0;
	virtual void   do_set_bank (int index, const doc::Bank &bank) = 0;
	virtual void   do_select_bank (int index) = 0;
	virtual void   do_set_bank_name (std::string name) = 0;
	virtual void   do_set_preset_name (std::string name) = 0;
	virtual void   do_activate_preset (int index) = 0;
	virtual void   do_store_preset (int index) = 0;
	virtual void   do_set_tuner (bool active_flag) = 0;
	virtual void   do_set_tuner_freq (float freq) = 0;
	virtual void   do_set_slot_info_for_current_preset (const SlotInfoList &info_list) = 0;
	virtual void   do_set_param (int pi_id, int index, float val, int slot_index, PiType type) = 0;
	virtual void   do_set_nbr_slots (int nbr_slots) = 0;
	virtual void   do_insert_slot (int slot_index) = 0;
	virtual void   do_erase_slot (int slot_index) = 0;
	virtual void   do_set_plugin (int slot_index, const PluginInitData &pi_data) = 0;
	virtual void   do_remove_plugin (int slot_index) = 0;
	virtual void   do_set_param_ctrl (int slot_index, PiType type, int index, const doc::CtrlLinkSet &cls) = 0;



}; // class ModelObserverInterface



}  // namespace mfx



//#include "mfx/ModelObserverInterface.hpp"



#endif   // mfx_ModelObserverInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
