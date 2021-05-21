/*****************************************************************************

        PluginPoolHostInterface.h
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_PluginPoolHostInterface_HEADER_INCLUDED)
#define mfx_PluginPoolHostInterface_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/piapi/HostInterface.h"



namespace mfx
{



class PluginPoolHostInterface
:	public piapi::HostInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               PluginPoolHostInterface () = default;

	void           set_plugin_id (int pi_id) noexcept;
	int            get_plugin_id () const noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual void   do_set_plugin_id (int pi_id) noexcept = 0;
	virtual int    do_get_plugin_id () const noexcept = 0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               PluginPoolHostInterface (const PluginPoolHostInterface &other) = delete;
	               PluginPoolHostInterface (PluginPoolHostInterface &&other)      = delete;
	PluginPoolHostInterface &
	               operator = (const PluginPoolHostInterface &other) = delete;
	PluginPoolHostInterface &
	               operator = (PluginPoolHostInterface &&other)      = delete;

}; // class PluginPoolHostInterface



}  // namespace mfx



//#include "mfx/PluginPoolHostInterface.hpp"



#endif   // mfx_PluginPoolHostInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
