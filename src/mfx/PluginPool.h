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

#include "fstb/BitFieldSparse.h"
#include "mfx/piapi/PluginInterface.h"
#include "mfx/Cst.h"
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

	typedef std::unique_ptr <piapi::PluginInterface> PluginUPtr;

	class PluginDetails
	{
	public:
		PluginUPtr     _pi_uptr;
		const piapi::PluginDescInterface *
		               _desc_ptr = 0;

		// Must be set by the client after each reset()
		int            _latency  = 0;

		// Only for ParamCateg_GLOBAL. Read-only for non-audio threads.
		std::vector <float>
		               _param_arr;

		// Final value of modulated parameters. Negative if not available.
		// Only for ParamCateg_GLOBAL. Read-only for non-audio threads.
		// Only indicative, there is no guarantee of validity.
		std::vector <float>
		               _param_mod_arr;

		// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
		// Audio thread only

		// Same remark. Managed by the audio thread only.
		fstb::BitFieldSparse
		               _param_update;

		// Indicates that the latest change comes from the audio thread.
		// Same remark. Audio thread only
		std::vector <bool>
		               _param_update_from_audio;

		// Reset flags. Audio thread only
		bool           _rst_steady_flag = false;
		bool           _rst_full_flag   = false;
	};

	               PluginPool ();
	virtual        ~PluginPool () = default;

	// Command thread
	int            get_room () const;
	int            create (std::string model_id);
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
		               ~PluginSlot ();
		PluginDetails  _details;
		SharedRscState _state = SharedRscState_FREE;
	};

	typedef std::array <PluginSlot, Cst::_max_nbr_plugins> PluginArray;

	typedef std::vector <std::shared_ptr <piapi::FactoryInterface> > FactoryList;
	typedef std::map <std::string, std::shared_ptr <piapi::FactoryInterface> > MapIdToFactory;

	int            add (PluginUPtr &pi_uptr, const piapi::PluginDescInterface &desc);

	FactoryList    _fact_arr;
	MapIdToFactory _map_model_to_factory;
	PluginArray    _pi_arr;
	int            _nbr_plugins = 0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               PluginPool (const PluginPool &other)        = delete;
	PluginPool &   operator = (const PluginPool &other)        = delete;
	bool           operator == (const PluginPool &other) const = delete;
	bool           operator != (const PluginPool &other) const = delete;

}; // class PluginPool



}  // namespace mfx



//#include "mfx/PluginPool.hpp"



#endif   // mfx_PluginPool_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
