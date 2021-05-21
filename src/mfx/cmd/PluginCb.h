/*****************************************************************************

        PluginCb.h
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_cmd_PluginCb_HEADER_INCLUDED)
#define mfx_cmd_PluginCb_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/PluginPoolHostInterface.h"



namespace mfx
{
namespace cmd
{



class Central;

class PluginCb
:	public PluginPoolHostInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       PluginCb (Central &central) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// PluginPoolHostInterface
	void           do_set_plugin_id (int pi_id) noexcept override;
	int            do_get_plugin_id () const noexcept override;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	Central &      _central;
	int            _pi_id = -1;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               PluginCb ()                               = delete;
	               PluginCb (const PluginCb &other)          = delete;
	               PluginCb (PluginCb &&other)               = delete;
	PluginCb &     operator = (const PluginCb &other)        = delete;
	PluginCb &     operator = (PluginCb &&other)             = delete;
	bool           operator == (const PluginCb &other) const = delete;
	bool           operator != (const PluginCb &other) const = delete;

}; // class PluginCb



}  // namespace cmd
}  // namespace mfx



//#include "mfx/cmd/PluginCb.hpp"



#endif   // mfx_cmd_PluginCb_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
