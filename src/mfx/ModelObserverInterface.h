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

	virtual        ~ModelObserverInterface () = default;

	void           set_edit_mode (bool edit_flag);
	void           set_pedalboard_layout (const doc::PedalboardLayout &layout);
	void           set_bank (int index, const doc::Bank &bank);
	void           select_bank (int index);
	void           activate_preset (int index);
	void           store_preset (int index);
	void           set_tuner (bool active_flag);
	void           set_tuner_freq (float freq);
	void           set_slot_info_for_current_preset (const SlotInfoList &info_list);
	void           set_param (int pi_id, int index, float val, int slot_index, PiType type);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual void   do_set_edit_mode (bool edit_flag) = 0;
	virtual void   do_set_pedalboard_layout (const doc::PedalboardLayout &layout) = 0;
	virtual void   do_set_bank (int index, const doc::Bank &bank) = 0;
	virtual void   do_select_bank (int index) = 0;
	virtual void   do_activate_preset (int index) = 0;
	virtual void   do_store_preset (int index) = 0;
	virtual void   do_set_tuner (bool active_flag) = 0;
	virtual void   do_set_tuner_freq (float freq) = 0;
	virtual void	do_set_slot_info_for_current_preset (const SlotInfoList &info_list) = 0;
	virtual void   do_set_param (int pi_id, int index, float val, int slot_index, PiType type) = 0;



}; // class ModelObserverInterface



}  // namespace mfx



//#include "mfx/ModelObserverInterface.hpp"



#endif   // mfx_ModelObserverInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
