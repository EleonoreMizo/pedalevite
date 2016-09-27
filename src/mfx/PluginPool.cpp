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

#include "fstb/Err.h"
#include "mfx/pi/export.h"
#include "mfx/piapi/FactoryInterface.h"
#include "mfx/piapi/PluginDescInterface.h"
#include "mfx/PluginPool.h"

#include <stdexcept>

#include <cassert>



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



PluginPool::PluginPool ()
:	_fact_arr ()
,	_map_model_to_factory ()
,	_pi_arr ()
,	_nbr_plugins (0)
{
	const int      ret_val = ::enum_factories (_fact_arr);
	if (ret_val != fstb::Err_OK)
	{
		assert (false);
		throw std::runtime_error (
			"Cannot enumerate the plug-in factories"
		);
	}

	for (auto &sptr : _fact_arr)
	{
		if (sptr.get () == 0)
		{
			throw std::runtime_error (
				"Pointer to the factory is null"
			);
		}
		const piapi::PluginDescInterface &  desc = sptr->describe ();
		const std::string  model_id = desc.get_unique_id ();
		if (_map_model_to_factory.find (model_id) != _map_model_to_factory.end ())
		{
			assert (false);
			throw std::runtime_error (
				"Plug-in model found twice in the factory list"
			);
		}
		_map_model_to_factory [model_id] = sptr;
	}
}



int	PluginPool::get_room () const
{
	return (Cst::_max_nbr_plugins - _nbr_plugins);
}



// Please ensure there is room first.
int	PluginPool::create (std::string model_id)
{
	int            found_pos = -1;

	// Finds the factory
	auto           it = _map_model_to_factory.find (model_id);
	if (it != _map_model_to_factory.end ())
	{
		piapi::FactoryInterface & fact = *(it->second);
		const piapi::PluginDescInterface &  desc = fact.describe ();

		// Creates the plug-in
		PluginUPtr     pi_uptr (fact.create ());
		if (pi_uptr.get () == 0)
		{
			assert (false);
		}
		else
		{
			found_pos = add (pi_uptr, desc);
		}
	}

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



std::vector <int>	PluginPool::list_plugins (SharedRscState state) const
{
	assert (state >= 0);
	assert (state < SharedRscState_NBR_ELT);

	std::vector <int> id_arr;

	for (int index = 0; index < int (_pi_arr.size ()); ++index)
	{
		const PluginSlot &   slot = _pi_arr [index];
		if (slot._state == state)
		{
			id_arr.push_back (index);
		}
	}

	return id_arr;
}



std::vector <std::string>	PluginPool::list_models () const
{
	std::vector <std::string>   name_list;

	for (const auto &p : _map_model_to_factory)
	{
		name_list.push_back (p.first);
	}

	return name_list;
}



const piapi::PluginDescInterface &	PluginPool::get_model_desc (std::string model_id) const
{
	auto           it = _map_model_to_factory.find (model_id);
	if (it == _map_model_to_factory.end ())
	{
		assert (false);
	}

	return it->second->describe ();
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



SharedRscState	PluginPool::get_state (int index)
{
	assert (index >= 0);
	assert (index < Cst::_max_nbr_plugins);

	const PluginSlot &   slot = _pi_arr [index];

	return slot._state;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



PluginPool::PluginSlot::~PluginSlot ()
{
	assert (_state != SharedRscState_INUSE);
}



int	PluginPool::add (PluginUPtr &pi_uptr, const piapi::PluginDescInterface &desc)
{
	assert (get_room () > 0);
	assert (pi_uptr.get () != 0);

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
				desc.get_nbr_param (piapi::ParamCateg_GLOBAL);
			slot._details._param_arr.resize (nbr_param);
			slot._details._param_update.set_nbr_elt (nbr_param);
			slot._details._param_update_from_audio.clear ();
			slot._details._param_update_from_audio.resize (nbr_param, false);

			for (int param_index = 0; param_index < nbr_param; ++param_index)
			{
				slot._details._param_arr [param_index] = float (
					pi_uptr->get_param_val (
						piapi::ParamCateg_GLOBAL,
						param_index,
						0
					)
				);
			}

			slot._details._pi_uptr.swap (pi_uptr);
			slot._details._desc_ptr = &desc;

			found_pos = index;
			++ _nbr_plugins;
		}
	}

	assert (pi_uptr.get () == 0);

	return found_pos;
}



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
