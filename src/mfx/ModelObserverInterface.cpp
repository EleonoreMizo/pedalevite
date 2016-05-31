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
#include "mfx/ModelObserverInterface.h"

#include <cassert>



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ModelObserverInterface::PluginInfo::PluginInfo (const piapi::PluginInterface &pi, const std::vector <float> &param_arr)
:	_pi (pi)
,	_param_arr (param_arr)
{
	// Nothing
}



// set_slot_info_for_current_preset to be called later
void	ModelObserverInterface::set_bank (const doc::Bank &bank, int preset)
{
	assert (preset >= 0);
	assert (preset < Cst::_nbr_presets_per_bank);

	do_set_bank (bank, preset);
}



// set_slot_info_for_current_preset to be called later
void	ModelObserverInterface::set_cur_preset (int preset)
{
	assert (preset >= 0);
	assert (preset < Cst::_nbr_presets_per_bank);

	do_set_cur_preset (preset);
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
void	ModelObserverInterface::set_slot_info_for_current_preset (const SlotInfoList &info_list)
{
	do_set_slot_info_for_current_preset (info_list);
}



void	ModelObserverInterface::set_param (int pi_id, int index, float val, int preset, int slot_index, PiType type)
{
	assert (pi_id >= 0);
	assert (index >= 0);
	assert (val >= 0);
	assert (val <= 1);
	assert (preset >= 0);
	assert (preset < Cst::_nbr_presets_per_bank);
	assert (slot_index >= 0);
	assert (type >= 0);
	assert (type < PiType_NBR_ELT);

	do_set_param (pi_id, index, val, preset, slot_index, type);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
