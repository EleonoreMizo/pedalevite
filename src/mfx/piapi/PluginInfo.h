/*****************************************************************************

        PluginInfo.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_piapi_PluginInfo_HEADER_INCLUDED)
#define mfx_piapi_PluginInfo_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/piapi/ChnPref.h"

#include <string>
#include <vector>



namespace mfx
{
namespace piapi
{



class PluginInfo
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	// Must be defined.
	// Internal plug-ins (not shown to the user) are prefixed with a '\?'
	std::string    _unique_id;

	// Must be defined. Multi-label.
	std::string    _name;

	std::vector <std::string>
	               _tag_list;

	ChnPref        _chn_pref = ChnPref::NONE;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const PluginInfo &other) const = delete;
	bool           operator != (const PluginInfo &other) const = delete;

}; // class PluginInfo



}  // namespace piapi
}  // namespace mfx



//#include "mfx/piapi/PluginInfo.hpp"



#endif   // mfx_piapi_PluginInfo_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
