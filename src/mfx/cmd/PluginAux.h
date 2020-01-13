/*****************************************************************************

        PluginAux.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_cmd_PluginAux_HEADER_INCLUDED)
#define mfx_cmd_PluginAux_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/cmd/Cnx.h"

#include <string>
#include <vector>



namespace mfx
{
namespace cmd
{



class PluginAux
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           clear ();

	int            _pi_id     = -1;  // Negative = not set
	std::string    _model;

	// Compensation delay for the DelayInterface plug-ins. Automatically removed
	// when setting up ProcessingContextNode (set to 0).
	int            _comp_delay = 0;

	// For delay plug-ins only. Indicates on which primary connection (in the
	// graph without auxiliary plugins) it is inserted.
	// An invalid connction indicates that the plug-in is scheduled for later
	// removal.
	Cnx            _cnx;
	int            _cnx_index = -1; // Index within Document::_cnx_list



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const PluginAux &other) const = delete;
	bool           operator != (const PluginAux &other) const = delete;

}; // class PluginAux



}  // namespace cmd
}  // namespace mfx



//#include "mfx/cmd/PluginAux.hpp"



#endif   // mfx_cmd_PluginAux_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
