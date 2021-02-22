/*****************************************************************************

        Cnx_mfx_cmd_lat.cpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/cmd/lat/Cnx.h"

#include <cassert>



namespace mfx
{
namespace cmd
{
namespace lat
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Cnx::set_node (piapi::Dir dir, int node_index)
{
	assert (dir >= 0);
   assert (dir < piapi::Dir_NBR_ELT);
	assert (node_index >= 0);

	_end_arr [dir] = node_index;
}



int	Cnx::get_node (piapi::Dir dir) const
{
	assert (dir >= 0);
   assert (dir < piapi::Dir_NBR_ELT);

   const int      node_index = _end_arr [dir];
	assert (node_index >= 0);

	return node_index;
}



void	Cnx::set_comp_delay (int dly_spl)
{
	assert (dly_spl >= 0);

	_comp_delay = dly_spl;
}



int	Cnx::get_comp_delay () const
{
	return _comp_delay;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace lat
}  // namespace cmd
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
