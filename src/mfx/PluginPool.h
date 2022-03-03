/*****************************************************************************

        PluginPool.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_PluginPool_HEADER_INCLUDED)
#define mfx_PluginPool_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/Cst.h"
#include "mfx/PluginDetails.h"
#include "mfx/SharedRscState.h"

#include <array>
#include <atomic>
#include <map>
#include <memory>
#include <string>
#include <vector>



namespace mfx
{



namespace piapi
{
	class FactoryInterface;
	class PluginDescInterface;
}

class PluginPool
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef PluginDetails::PluginUPtr PluginUPtr;
	typedef PluginDetails::HostUPtr HostUPtr;

	               PluginPool ();
	               ~PluginPool () = default;

	// Command thread
	int            get_room () const;
	int            create (std::string model_id, HostUPtr host_uptr);
	void           schedule_for_release (int index);
	void           release (int index);
	std::vector <int>
	               list_plugins (SharedRscState state = SharedRscState_INUSE) const;

	// Any thread
	std::vector <std::string>
	               list_models () const;
	const piapi::PluginDescInterface &
	               get_model_desc (std::string model_id) const;
	PluginDetails& use_plugin (int index);
	const PluginDetails &
	               use_plugin (int index) const;
	SharedRscState get_state (int index);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	class PluginSlot
	{
	public:
		               PluginSlot ()                        = default;
		               PluginSlot (const PluginSlot &other) = delete;
		               PluginSlot (PluginSlot &&other)      = delete;
		               ~PluginSlot ();
		PluginSlot &   operator = (const PluginSlot &other) = delete;
		PluginSlot &   operator = (PluginSlot &&other)      = delete;

		PluginDetails  _details;
		SharedRscState _state = SharedRscState_FREE;
	};

	typedef std::array <PluginSlot, Cst::_max_nbr_plugins> PluginArray;

	typedef std::unique_ptr <piapi::FactoryInterface> FactoryUPtr;
	typedef std::vector <FactoryUPtr> FactoryList;
	typedef std::map <std::string, piapi::FactoryInterface *> MapIdToFactory;

	int            find_free_slot () const noexcept;
	void           add (int pi_pos, PluginUPtr pi_uptr, HostUPtr host_uptr, const piapi::PluginDescInterface &desc);

	FactoryList    _fact_arr;
	MapIdToFactory _map_model_to_factory;
	PluginArray    _pi_arr;
	int            _nbr_plugins = 0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               PluginPool (const PluginPool &other)        = delete;
	               PluginPool (const PluginPool &&other)       = delete;
	PluginPool &   operator = (const PluginPool &other)        = delete;
	PluginPool &   operator = (PluginPool &&other)             = delete;
	bool           operator == (const PluginPool &other) const = delete;
	bool           operator != (PluginPool &other) const       = delete;

}; // class PluginPool



}  // namespace mfx



//#include "mfx/PluginPool.hpp"



#endif   // mfx_PluginPool_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
