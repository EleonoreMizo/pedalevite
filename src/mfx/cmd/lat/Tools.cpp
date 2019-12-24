/*****************************************************************************

        Tools.cpp
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
#include "mfx/cmd/lat/GraphInterface.h"
#include "mfx/cmd/lat/Node.h"
#include "mfx/cmd/lat/Tools.h"

#include <array>
#include <vector>

#include <cassert>



namespace mfx
{
namespace cmd
{
namespace lat
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	Tools::get_next_node (const GraphInterface &graph, int node_index, piapi::Dir dir, int ref_index)
{
	assert (node_index >= 0);
	assert (node_index < graph.get_nbr_nodes ());
	assert (dir >= 0);
	assert (dir < piapi::Dir_NBR_ELT);
	assert (ref_index >= 0);

	const Node &   node        = graph.use_node (node_index);

	const int      cnx_index   = node.get_cnx (dir, ref_index);
	const Cnx &    cnx         = graph.use_cnx (cnx_index);

	const piapi::Dir   inv_dir = invert_dir (dir);
	const int      next_index  = cnx.get_node (inv_dir);

	return next_index;
}



piapi::Dir	Tools::invert_dir (piapi::Dir dir)
{
	assert (dir >= 0);
	assert (dir < piapi::Dir_NBR_ELT);

	const int      inv_dir = (piapi::Dir_NBR_ELT - 1) - dir;

	return static_cast <piapi::Dir> (inv_dir);
}



void	Tools::reflect_cnx_on_nodes (GraphInterface &graph)
{
	typedef std::array <int, piapi::Dir_NBR_ELT>	NbrCnxPerNode;
	typedef std::vector <NbrCnxPerNode>          NbrCnxArr;

	const int        nbr_cnx   = graph.get_nbr_cnx ();
	const int        nbr_nodes = graph.get_nbr_nodes ();

	// Initialises connexion count arrays
	NbrCnxArr        nbr_cnx_arr (nbr_nodes, {{ 0, 0 }});

	// Counts connexions per node
	for (int cnx_cnt = 0; cnx_cnt < nbr_cnx; ++cnx_cnt)
	{
		const Cnx &    cnx = graph.use_cnx (cnx_cnt);

		const int      node_src_index = cnx.get_node (piapi::Dir_OUT);
		const int      node_dst_index = cnx.get_node (piapi::Dir_IN );

		++ nbr_cnx_arr [node_dst_index] [piapi::Dir_IN ];
		++ nbr_cnx_arr [node_src_index] [piapi::Dir_OUT];
	}

	// Reserves memory for each node
	for (int node_cnt = 0; node_cnt < nbr_nodes; ++node_cnt)
	{
		Node &         node    = graph.use_node (node_cnt);
      const int      nbr_in  = nbr_cnx_arr [node_cnt] [piapi::Dir_IN ];
      const int      nbr_out = nbr_cnx_arr [node_cnt] [piapi::Dir_OUT];
		node.set_nbr_cnx (piapi::Dir_IN , nbr_in );
		node.set_nbr_cnx (piapi::Dir_OUT, nbr_out);
	}

	// Sets connexions on the node objects
	for (int cnx_cnt = 0; cnx_cnt < nbr_cnx; ++cnx_cnt)
	{
		const Cnx &    cnx            = graph.use_cnx (cnx_cnt);
		const int      node_src_index = cnx.get_node (piapi::Dir_OUT);
		const int      node_dst_index = cnx.get_node (piapi::Dir_IN );

		Node &         node_dst = graph.use_node (node_dst_index);
		Node &         node_src = graph.use_node (node_src_index);

		int &          dst_pos = nbr_cnx_arr [node_dst_index] [piapi::Dir_IN ];
		int &          src_pos = nbr_cnx_arr [node_src_index] [piapi::Dir_OUT];
		-- dst_pos;
		-- src_pos;

		node_dst.set_cnx (piapi::Dir_IN , dst_pos, cnx_cnt);
		node_src.set_cnx (piapi::Dir_OUT, src_pos, cnx_cnt);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace lat
}  // namespace cmd
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
