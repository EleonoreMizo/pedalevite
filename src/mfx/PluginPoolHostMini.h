/*****************************************************************************

        PluginPoolHostMini.h
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_PluginPoolHostMini_HEADER_INCLUDED)
#define mfx_PluginPoolHostMini_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/PluginPoolHostInterface.h"



namespace mfx
{



class PluginPoolHostMini
:	public PluginPoolHostInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// PluginPoolHostInterface
	void           do_set_plugin_id (int pi_id) noexcept override { _pi_id = pi_id; }
	int            do_get_plugin_id () const noexcept override { return _pi_id; }



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	int            _pi_id = -1;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const PluginPoolHostMini &other) const = delete;
	bool           operator != (const PluginPoolHostMini &other) const = delete;

}; // class PluginPoolHostMini



}  // namespace mfx



//#include "mfx/PluginPoolHostMini.hpp"



#endif   // mfx_PluginPoolHostMini_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
