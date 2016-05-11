/*****************************************************************************

        PluginPool.cpp
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

#include "mfx/PluginPool.h"

#include <cassert>



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	PluginPool::get_room () const
{
	return (Cst::_max_nbr_plugins - _nbr_plugins);
}



// Please ensure there is room first.
// Plug-in must be initialized.
int	PluginPool::add (PluginUPtr &pi_uptr)
{
	assert (get_room () > 0);
	assert (pi_uptr.get () != 0);
	assert (pi_uptr->get_state () >= piapi::PluginInterface::State_INITIALISED);

	int            found_pos = -1;
	for (int index = 0; index < Cst::_max_nbr_plugins && found_pos < 0; ++index)
	{
		PluginSlot &      slot = _pi_arr [index];
		if (slot._state == SharedRscState_FREE)
		{
			assert (slot._details._pi_uptr.get () == 0);
			slot._state = SharedRscState_INUSE;

			// Parameters
			const int      nbr_param =
				pi_uptr->get_nbr_param (piapi::ParamCateg_GLOBAL);
			slot._details._param_arr.resize (nbr_param);
			slot._details._param_update.set_nbr_elt (nbr_param);
			slot._details._param_update_from_audio.clear ();
			slot._details._param_update_from_audio.resize (nbr_param, false);

			for (int param_index = 0; param_index < nbr_param; ++param_index)
			{
				slot._details._param_arr [param_index] = pi_uptr->get_param_val (
					piapi::ParamCateg_GLOBAL,
					param_index,
					0
				);
			}

			slot._details._pi_uptr.swap (pi_uptr);

			found_pos = index;
			++ _nbr_plugins;
		}
	}

	assert (pi_uptr.get () == 0);

	return found_pos;
}



void	PluginPool::schedule_for_release (int index)
{
	assert (index >= 0);
	assert (index < Cst::_max_nbr_plugins);

	PluginSlot &      slot = _pi_arr [index];
	assert (slot._state == SharedRscState_INUSE);

	slot._state = SharedRscState_RECYCLING;
}



void	PluginPool::release (int index)
{
	assert (index >= 0);
	assert (index < Cst::_max_nbr_plugins);

	PluginSlot &      slot = _pi_arr [index];
	assert (slot._state == SharedRscState_RECYCLING);

	if (slot._state != SharedRscState_FREE)
	{
		slot._details._pi_uptr.reset ();
		slot._details._param_arr.clear ();
		slot._state = SharedRscState_FREE;
		-- _nbr_plugins;
	}
}



PluginPool::PluginDetails &	PluginPool::use_plugin (int index)
{
	assert (index >= 0);
	assert (index < Cst::_max_nbr_plugins);

	PluginSlot &   slot = _pi_arr [index];
	assert (slot._state != SharedRscState_FREE);
	assert (slot._details._pi_uptr.get () != 0);

	return slot._details;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



PluginPool::PluginSlot::~PluginSlot ()
{
	assert (_state != SharedRscState_INUSE);
}



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
