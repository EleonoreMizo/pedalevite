/*****************************************************************************

        ContainerInterface.cpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/ContainerInterface.h"

#include <utility>

#include <cassert>



namespace mfx
{
namespace uitk
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	ContainerInterface::push_back (NodeSPtr node_sptr)
{
	assert (node_sptr.get () != 0);

	do_push_back (node_sptr);
}



void	ContainerInterface::set_node (int pos, NodeSPtr node_sptr)
{
	assert (pos >= 0);
	assert (pos < get_nbr_nodes ());
	assert (node_sptr.get () != 0);

	do_set_node (pos, node_sptr);
}



// Insert before.
void	ContainerInterface::insert (int pos, NodeSPtr node_sptr)
{
	assert (pos >= 0);
	assert (pos <= get_nbr_nodes ());
	assert (node_sptr.get () != 0);

	do_insert (pos, node_sptr);
}



void	ContainerInterface::erase (int pos)
{
	assert (pos >= 0);
	assert (pos < get_nbr_nodes ());

	do_erase (pos);
}



int	ContainerInterface::get_nbr_nodes () const
{
	const int      nbr_nodes = do_get_nbr_nodes ();
	assert (nbr_nodes >= 0);

	return nbr_nodes;
}



ContainerInterface::NodeSPtr	ContainerInterface::use_node (int pos)
{
	assert (pos >= 0);
	assert (pos < get_nbr_nodes ());

	NodeSPtr       node_sptr = do_use_node (pos);

	return std::move (node_sptr);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
