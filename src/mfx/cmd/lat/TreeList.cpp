/*****************************************************************************

        TreeList.cpp
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
#include "mfx/cmd/lat/Node.h"
#include "mfx/cmd/lat/Tools.h"
#include "mfx/cmd/lat/TreeList.h"

#include <cassert>



namespace mfx
{
namespace cmd
{
namespace lat
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	TreeList::init (GraphInterface &graph)
{
	const int      nbr_nodes = graph.get_nbr_nodes ();
	_info_list.clear ();             // Max nbr trees = nbr nodes
	_node_list.resize (nbr_nodes);
	_nbr_trees = 0;

	int            node_start            = 0;
	int            node_list_start_index = 0;
	while (node_start < nbr_nodes)
	{
		const Node &      node = graph.use_node (node_start);
		if (! node.is_tree_set ())
		{
			assert (_nbr_trees == int (_info_list.size ()));
			_info_list.emplace_back (node_list_start_index);
			TreeInfo &     tree_info = _info_list.back ();

			build_rec (graph, node_start, _nbr_trees);

			node_list_start_index += tree_info._nbr_nodes;
			assert (node_list_start_index <= nbr_nodes);

			++ _nbr_trees;
		}

		++ node_start;
	}

	_info_list.resize (_nbr_trees);
}



void	TreeList::restore ()
{
	_info_list.clear ();
	_node_list.clear ();
	_nbr_trees = -1;
}



int	TreeList::get_nbr_trees () const
{
	assert (_nbr_trees >= 0);

	return _nbr_trees;
}



int	TreeList::get_nbr_nodes (int tree) const
{
	assert (tree >= 0);
	assert (tree < get_nbr_trees ());

	const TreeInfo &  tree_info = _info_list [tree];
	const int      nbr_nodes    = tree_info._nbr_nodes;
	assert (nbr_nodes > 0);

	return nbr_nodes;
}



int	TreeList::get_node (int tree, int index) const
{
	assert (tree >= 0);
	assert (tree < get_nbr_trees ());
	assert (index >= 0);
	assert (index < get_nbr_nodes (tree));

	const TreeInfo &  tree_info = _info_list [tree];
	const int      pos          = tree_info._start_index + index;
	const int      node_index   = _node_list [pos];

	return node_index;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



TreeList::TreeInfo::TreeInfo (int start_index)
:	_nbr_nodes (0)
,	_start_index (start_index)
{
	assert (start_index >= 0);
}



void	TreeList::build_rec (GraphInterface &graph, int node_index, int cur_tree)
{
	assert (node_index >= 0);
   assert (node_index < graph.get_nbr_nodes ());
	assert (cur_tree >= 0);
	assert (cur_tree < int (_info_list.size ()));
	assert (cur_tree <= _nbr_trees);

   // Associates node to current tree
   Node &         node = graph.use_node (node_index);
   node.set_tree (cur_tree);

   TreeInfo &     tree_info = _info_list [cur_tree];

   // Puts current node index into the node list of this tree
   const int      node_list_index =
      tree_info._start_index + tree_info._nbr_nodes;
   _node_list [node_list_index] = node_index;

   ++ tree_info._nbr_nodes;

   // Explores adjacent nodes
   for (int dir_cnt = 0; dir_cnt < piapi::Dir_NBR_ELT; ++dir_cnt)
   {
      const piapi::Dir   dir = static_cast <piapi::Dir> (dir_cnt);
      const int      nbr_cnx = node.get_nbr_cnx (dir);
      for (int cnx_pos = 0; cnx_pos < nbr_cnx; ++cnx_pos)
      {
         const int   next_node_index = Tools::get_next_node (
            graph,
            node_index,
            dir,
            cnx_pos
         );
         const Node &   next_node = graph.use_node (next_node_index);
         if (! next_node.is_tree_set ())
         {
            build_rec (graph, next_node_index, cur_tree);
         }
      }
   }
}



}  // namespace lat
}  // namespace cmd
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
