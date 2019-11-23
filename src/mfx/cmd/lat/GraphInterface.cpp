/*****************************************************************************

        GraphInterface.cpp
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

#include "mfx/cmd/lat/GraphInterface.h"

#include <cassert>



namespace mfx
{
namespace cmd
{
namespace lat
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	GraphInterface::get_nbr_nodes () const
{
	const int      nbr_nodes = do_get_nbr_nodes ();
	assert (nbr_nodes >= 0);

	return nbr_nodes;
}



Node &	GraphInterface::use_node (int index)
{
	assert (index >= 0);
	assert (index < get_nbr_nodes ());

	return do_use_node (index);
}



const Node &	GraphInterface::use_node (int index) const
{
	assert (index >= 0);
	assert (index < get_nbr_nodes ());

	return do_use_node (index);
}



int	GraphInterface::get_nbr_cnx () const
{
	const int      nbr_cnx = do_get_nbr_cnx ();
	assert (nbr_cnx >= 0);

	return nbr_cnx;
}



Cnx &	GraphInterface::use_cnx (int index)
{
	assert (index >= 0);
	assert (index < get_nbr_cnx ());

	return do_use_cnx (index);
}



const Cnx &	GraphInterface::use_cnx (int index) const
{
	assert (index >= 0);
	assert (index < get_nbr_cnx ());

	return do_use_cnx (index);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace lat
}  // namespace cmd
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
