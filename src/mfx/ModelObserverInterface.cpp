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



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
