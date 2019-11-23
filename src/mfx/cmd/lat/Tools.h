/*****************************************************************************

        Tools.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_cmd_lat_Tools_HEADER_INCLUDED)
#define mfx_cmd_lat_Tools_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/piapi/Dir.h"



namespace mfx
{
namespace cmd
{
namespace lat
{



class GraphInterface;

class Tools
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static int     get_next_node (const GraphInterface &graph, int node_index, piapi::Dir dir, int ref_index);
	static piapi::Dir
	               invert_dir (piapi::Dir dir);
	static void		reflect_cnx_on_nodes (GraphInterface &graph);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Tools ()                               = delete;
	               Tools (const Tools &other)             = delete;
	virtual        ~Tools ()                              = delete;
	Tools &        operator = (const Tools &other)        = delete;
	bool           operator == (const Tools &other) const = delete;
	bool           operator != (const Tools &other) const = delete;

}; // class Tools



}  // namespace lat
}  // namespace cmd
}  // namespace mfx



//#include "mfx/cmd/lat/Tools.hpp"



#endif   // mfx_cmd_lat_Tools_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
