/*****************************************************************************

        PluginSettings.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_doc_PluginSettings_HEADER_INCLUDED)
#define mfx_doc_PluginSettings_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/doc/CtrlLinkSet.h"

#include <map>
#include <vector>



namespace mfx
{
namespace doc
{



class PluginSettings
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef std::map <int, CtrlLinkSet> MapParamCtrl;

	               PluginSettings ()                            = default;
	               PluginSettings (const PluginSettings &other) = default;
	virtual        ~PluginSettings ()                           = default;
	PluginSettings &
	               operator = (const PluginSettings &other)     = default;

	bool           _force_mono_flag;
	std::vector <float>
	               _param_list;
	MapParamCtrl   _map_param_ctrl;     // Parameter index -> controller list



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const PluginSettings &other) const = delete;
	bool           operator != (const PluginSettings &other) const = delete;

}; // class PluginSettings



}  // namespace doc
}  // namespace mfx



//#include "mfx/doc/PluginSettings.hpp"



#endif   // mfx_doc_PluginSettings_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
