/*****************************************************************************

        Plugin.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_cmd_Plugin_HEADER_INCLUDED)
#define mfx_cmd_Plugin_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/doc/CtrlLinkSet.h"

#include <map>
#include <memory>
#include <vector>



namespace mfx
{
namespace cmd
{



class Plugin
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef std::shared_ptr <doc::CtrlLinkSet> CtrlLinkSetSPtr;
	typedef std::map <int, CtrlLinkSetSPtr> MapParamCtrl;

	               Plugin ()                        = default;
	               Plugin (const Plugin &other)     = default;
	virtual        ~Plugin ()                       = default;

	Plugin &       operator = (const Plugin &other) = default;

	int            _pi_id = -1;  // -1 = not set
	std::string    _model;
	MapParamCtrl   _ctrl_map;
	std::vector <float>          // If there are settings to apply during the commit. Automatically removed at the end of the transaction.
	               _param_list;
	std::vector <int>            // Port index for each signal pin. Vector size can be less than the number of pins.
	               _sig_port_list;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Plugin &other) const = delete;
	bool           operator != (const Plugin &other) const = delete;

}; // class Plugin



}  // namespace cmd
}  // namespace mfx



//#include "mfx/cmd/Plugin.hpp"



#endif   // mfx_cmd_Plugin_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
