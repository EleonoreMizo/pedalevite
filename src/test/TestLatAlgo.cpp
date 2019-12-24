/*****************************************************************************

        TestLatAlgo.cpp
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

#include "mfx/cmd/lat/Algo.h"
#include "mfx/cmd/lat/TreeList.h"
#include "test/TestLatAlgo.h"

#include <algorithm>
#include <array>
#include <set>

#include <cassert>
#include <cstdint>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestLatAlgo::perform_test ()
{
	int            ret_val = 0;

	if (ret_val == 0)
	{
		ret_val = test_tree_only ();
	}
	if (ret_val == 0)
	{
		ret_val = test_full ();
	}

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestLatAlgo::test_tree_only ()
{
	int            ret_val = 0;

   // 2 trees: 0+2+4 and 1+3
   const CnxInfo  cnx_info_arr [5] =
   {
      { 0, 4 },
      { 2, 0 },
      { 4, 0 },

      { 3, 1 },

		{ -1, -1 }
   };

   mfx::cmd::lat::Algo     graph;
   build_graph (graph, cnx_info_arr, 0);

   mfx::cmd::lat::TreeList tree_list;
   tree_list.init (graph);

	display_tree_list (tree_list);

	return ret_val;
}



int	TestLatAlgo::test_full ()
{
	int				ret_val = 0;

   const CnxInfo	cnx_info_arr_list [] [16] =
   {
		{
			{ 1, 0 },
			{ 1, 3 },
			{ 0, 2 },
			{ 3, 2 },
			{ -1, -1 }
		},
		{
			{ 4, 3 },
			{ 1, 0 },
			{ 0, 3 },
			{ 2, 0 },
			{ 5, 4 },
			{ -1, -1 }
		},
		{
			{ 0, 1 },
			{ 1, 2 },
			{ 0, 3 },
			{ 3, 2 },
			{ 4, 3 },
			{ 3, 5 },
			{ -1, -1 }
		},
		{
			{ 0, 1 },
			{ 1, 2 },
			{ 0, 3 },
			{ 3, 2 },
			{ 4, 3 },
			{ 3, 9 },
			{ 4, 5 },
			{ 5, 9 },
			{ 6, 5 },
			{ 5, 8 },
			{ 6, 7 },
			{ 7, 8 },
			{ -1, -1 }
		}
   };
	const NodeInfo node_info_arr_list [] [16] =
	{
		{
			{ 2, mfx::cmd::lat::Node::Nature_NORMAL },
			{ 0, mfx::cmd::lat::Node::Nature_SOURCE },
			{ 0, mfx::cmd::lat::Node::Nature_SINK   },
			{ 1, mfx::cmd::lat::Node::Nature_NORMAL },
			{ -1, mfx::cmd::lat::Node::Nature_NORMAL }
		},
		{
			{ 1, mfx::cmd::lat::Node::Nature_NORMAL },
			{ 0, mfx::cmd::lat::Node::Nature_SOURCE },
			{ 0, mfx::cmd::lat::Node::Nature_NORMAL },
			{ 0, mfx::cmd::lat::Node::Nature_SINK   },
			{ 2, mfx::cmd::lat::Node::Nature_NORMAL },
			{ 0, mfx::cmd::lat::Node::Nature_NORMAL },
			{ -1, mfx::cmd::lat::Node::Nature_NORMAL }
		},
		{
			{ 0, mfx::cmd::lat::Node::Nature_NORMAL },
			{ 2, mfx::cmd::lat::Node::Nature_NORMAL },
			{ 0, mfx::cmd::lat::Node::Nature_NORMAL },
			{ 1, mfx::cmd::lat::Node::Nature_NORMAL },
			{ 0, mfx::cmd::lat::Node::Nature_NORMAL },
			{ 0, mfx::cmd::lat::Node::Nature_NORMAL },
			{ -1, mfx::cmd::lat::Node::Nature_NORMAL }
		},
		{
			{ 0, mfx::cmd::lat::Node::Nature_NORMAL },
			{ 1, mfx::cmd::lat::Node::Nature_NORMAL },
			{ 0, mfx::cmd::lat::Node::Nature_NORMAL },
			{ 3, mfx::cmd::lat::Node::Nature_NORMAL },
			{ 0, mfx::cmd::lat::Node::Nature_NORMAL },
			{ 2, mfx::cmd::lat::Node::Nature_NORMAL },
			{ 0, mfx::cmd::lat::Node::Nature_NORMAL },
			{ 1, mfx::cmd::lat::Node::Nature_NORMAL },
			{ 0, mfx::cmd::lat::Node::Nature_NORMAL },
			{ 0, mfx::cmd::lat::Node::Nature_NORMAL },
			{ -1, mfx::cmd::lat::Node::Nature_NORMAL }
		}
	};
	const int      nbr_tests =
		sizeof (cnx_info_arr_list) / sizeof (cnx_info_arr_list [0]);

	for (int test_cnt = 0; test_cnt < nbr_tests && ret_val == 0; ++test_cnt)
	{
		mfx::cmd::lat::Algo  graph;
	   build_graph (
			graph,
			cnx_info_arr_list [test_cnt],
			node_info_arr_list [test_cnt]
		);
		graph.run ();
		display_graph (graph);

		printf ("%d recursive calls.\n", graph.get_bm_nbr_rec_calls ());

		ret_val = check_graph (graph);
		if (ret_val != 0)
		{
			printf ("TestLatAlgo::test_full(): check failed.\n");
		}

		printf ("\n");
	}

	if (ret_val == 0)
	{
		mfx::cmd::lat::Algo  graph;
		build_graph_random (graph, 0, 0);
		graph.run ();
	}

	if (ret_val == 0)
	{
		for (int test_cnt = 0; test_cnt < 10 && ret_val == 0; ++test_cnt)
		{
			mfx::cmd::lat::Algo  graph;
			build_graph_random (graph, 100, 150);
			graph.run ();
			printf (
				"%d nodes, %d connexions: %d recursive calls.\n",
				graph.get_nbr_nodes (),
				graph.get_nbr_cnx (),
				graph.get_bm_nbr_rec_calls ()
			);

			ret_val = check_graph (graph);
			if (ret_val != 0)
			{
				printf ("TestLatAlgo::test_full(): check failed.\n");
			}
		}

		printf ("\n");
	}

	return ret_val;
}



void	TestLatAlgo::build_graph (mfx::cmd::lat::Algo &graph, const CnxInfo cnx_info_arr [], const NodeInfo node_info_arr [])
{
	assert (&graph != 0);
	assert (cnx_info_arr != 0);

	// Counts connexions
	int            nbr_cnx = 0;
	while (cnx_info_arr [nbr_cnx]._node_src != -1)
	{
		++ nbr_cnx;
	}

   // Counts nodes
	int            nbr_nodes = 0;
	if (node_info_arr != 0)
	{
		while (node_info_arr [nbr_nodes]._latency != -1)
		{
			++ nbr_nodes;
		}
	}
	else
	{
		nbr_nodes = 0;
		for (int cnx_cnt = 0; cnx_cnt < nbr_cnx; ++cnx_cnt)
		{
			const CnxInfo &   cnx_info = cnx_info_arr [cnx_cnt];
			nbr_nodes = std::max (nbr_nodes, cnx_info._node_src + 1);
			nbr_nodes = std::max (nbr_nodes, cnx_info._node_dst + 1);
		}
	}

   graph.set_nbr_elt (nbr_nodes, nbr_cnx);

   std::vector <std::array <int, mfx::piapi::Dir_NBR_ELT> >  nbr_cnx_arr (nbr_nodes);

   // Counts number of connexions per node,
   // Set connexions
   for (int pass = 0; pass < 2; ++pass)
   {
      for (int node_cnt = 0; node_cnt < nbr_nodes; ++node_cnt)
      {
         nbr_cnx_arr [node_cnt] [mfx::piapi::Dir_OUT] = 0;
         nbr_cnx_arr [node_cnt] [mfx::piapi::Dir_IN ] = 0;
      }

      for (int cnx_cnt = 0; cnx_cnt < nbr_cnx; ++cnx_cnt)
      {
			const CnxInfo &      cnx_info = cnx_info_arr [cnx_cnt];
         mfx::cmd::lat::Cnx & cnx      = graph.use_cnx (cnx_cnt);
         cnx.set_node (mfx::piapi::Dir_OUT, cnx_info._node_src);
         cnx.set_node (mfx::piapi::Dir_IN , cnx_info._node_dst);

         int &          in  = nbr_cnx_arr [cnx_info._node_dst] [mfx::piapi::Dir_IN ];
         int &          out = nbr_cnx_arr [cnx_info._node_src] [mfx::piapi::Dir_OUT];
         if (pass == 1)
         {
            mfx::cmd::lat::Node &   node_src = graph.use_node (cnx_info._node_src);
            node_src.set_cnx (mfx::piapi::Dir_OUT, out, cnx_cnt);

            mfx::cmd::lat::Node &   node_dst = graph.use_node (cnx_info._node_dst);
            node_dst.set_cnx (mfx::piapi::Dir_IN, in, cnx_cnt);
         }

         ++ in;
         ++ out;
      }

      if (pass == 0)
      {
         for (int node_cnt = 0; node_cnt < nbr_nodes; ++node_cnt)
         {
            mfx::cmd::lat::Node &   node = graph.use_node (node_cnt);
            node.set_nbr_cnx (
               mfx::piapi::Dir_IN ,
               nbr_cnx_arr [node_cnt] [mfx::piapi::Dir_IN ]
            );
            node.set_nbr_cnx (
               mfx::piapi::Dir_OUT,
               nbr_cnx_arr [node_cnt] [mfx::piapi::Dir_OUT]
            );
         }
      }
   }

	if (node_info_arr != 0)
	{
      for (int node_cnt = 0; node_cnt < nbr_nodes; ++node_cnt)
      {
         mfx::cmd::lat::Node &   node      = graph.use_node (node_cnt);
			const NodeInfo &        node_info = node_info_arr [node_cnt];

			node.set_latency (node_info._latency);
			node.set_nature (node_info._nature);
      }
	}
}



void	TestLatAlgo::build_graph_random (mfx::cmd::lat::Algo &graph, int nbr_nodes, int nbr_cnx)
{
	assert (&graph != 0);
	assert (nbr_nodes >= 0);
	assert (nbr_cnx >= 0);
	assert (nbr_nodes <= RAND_MAX);

	typedef std::set <int64_t> CnxSet;
	typedef std::array <int, mfx::piapi::Dir_NBR_ELT> NbrCnxPerNode;
	typedef std::vector <NbrCnxPerNode> NbrCnxArr;

	graph.set_nbr_elt (nbr_nodes, nbr_cnx);

	NbrCnxArr      nbr_cnx_arr (nbr_nodes);

	// Nodes
	std::uniform_int_distribution <int> distr_lat (0, 9);
	for (int node_cnt = 0; node_cnt < nbr_nodes; ++node_cnt)
	{
		mfx::cmd::lat::Node &   node = graph.use_node (node_cnt);
		const int      latency = distr_lat (_generator);
		node.set_latency (latency);

		nbr_cnx_arr [node_cnt] [mfx::piapi::Dir_IN ] = 0;
		nbr_cnx_arr [node_cnt] [mfx::piapi::Dir_OUT] = 0;
	}

	// Connexions
	CnxSet				cnx_set;
	std::uniform_int_distribution <int> distr_node (0, std::max (nbr_nodes - 1, 0));
	for (int cnx_cnt = 0; cnx_cnt < nbr_cnx; ++cnx_cnt)
	{
		mfx::cmd::lat::Cnx & cnx = graph.use_cnx (cnx_cnt);
		int            node_src_index;
		int            node_dst_index;
		bool           retry_flag;
		do
		{
			retry_flag = false;

			const int		node_1 = distr_node (_generator);
			const int		node_2 = distr_node (_generator);
			node_src_index = std::min (node_1, node_2);
			node_dst_index = std::max (node_1, node_2);
			const int64_t  code =
				  (static_cast <int64_t> (node_src_index) << 32)
				+ (static_cast <int64_t> (node_dst_index) <<  0);

			if (   node_src_index == node_dst_index
			    || cnx_set.find (code) != cnx_set.end ())
			{
				retry_flag = true;
			}
			else
			{
				cnx_set.insert (code);
			}
		}
		while (retry_flag);

		cnx.set_node (mfx::piapi::Dir_OUT, node_src_index );
		cnx.set_node (mfx::piapi::Dir_IN , node_dst_index);

		++ nbr_cnx_arr [node_dst_index] [mfx::piapi::Dir_IN ];
		++ nbr_cnx_arr [node_src_index] [mfx::piapi::Dir_OUT];
	}

	for (int node_cnt = 0; node_cnt < nbr_nodes; ++node_cnt)
	{
		mfx::cmd::lat::Node &   node = graph.use_node (node_cnt);
		node.set_nbr_cnx (mfx::piapi::Dir_IN , nbr_cnx_arr [node_cnt] [mfx::piapi::Dir_IN ]);
		node.set_nbr_cnx (mfx::piapi::Dir_OUT, nbr_cnx_arr [node_cnt] [mfx::piapi::Dir_OUT]);
	}

	for (int cnx_cnt = 0; cnx_cnt < nbr_cnx; ++cnx_cnt)
	{
		const mfx::cmd::lat::Cnx & cnx   = graph.use_cnx (cnx_cnt);
		const int      node_src_index    = cnx.get_node (mfx::piapi::Dir_OUT);
		const int      node_dst_index    = cnx.get_node (mfx::piapi::Dir_IN );

		mfx::cmd::lat::Node &   node_dst = graph.use_node (node_dst_index);
		mfx::cmd::lat::Node &   node_src = graph.use_node (node_src_index);

		int &				dst_pos = nbr_cnx_arr [node_dst_index] [mfx::piapi::Dir_IN ];
		int &				src_pos = nbr_cnx_arr [node_src_index] [mfx::piapi::Dir_OUT];
		-- dst_pos;
		-- src_pos;

		node_dst.set_cnx (mfx::piapi::Dir_IN , dst_pos, cnx_cnt);
		node_src.set_cnx (mfx::piapi::Dir_OUT, src_pos, cnx_cnt);
	}
}



void	TestLatAlgo::display_tree_list (const mfx::cmd::lat::TreeList &tree_list)
{
	assert (&tree_list != 0);

	const int      nbr_trees = tree_list.get_nbr_trees ();
	printf ("%d trees:\n", nbr_trees);

	for (int tree_cnt = 0; tree_cnt < nbr_trees; ++tree_cnt)
	{
		const int      nbr_nodes = tree_list.get_nbr_nodes (tree_cnt);
		for (int node_cnt = 0; node_cnt < nbr_nodes; ++node_cnt)
		{
			if (node_cnt > 0)
			{
				printf (", ");
			}
			const int      node_index = tree_list.get_node (tree_cnt, node_cnt);
			printf ("%d", node_index);
		}
		printf ("\n");
	}
	printf ("\n");
}



void	TestLatAlgo::display_graph (const mfx::cmd::lat::GraphInterface &graph)
{
	assert (&graph != 0);

	const int      nbr_nodes = graph.get_nbr_nodes ();
	const int      nbr_cnx   = graph.get_nbr_cnx ();

	printf ("%d nodes:\n", nbr_nodes);
	for (int node_cnt = 0; node_cnt < nbr_nodes; ++node_cnt)
	{
		const mfx::cmd::lat::Node &   node = graph.use_node (node_cnt);
		printf (
			"%02d: Tree = %01d, TS = %d, L = %d, Nat. = %01d\n",
			node_cnt,
			node.get_tree (),
			node.get_timestamp (),
			node.get_latency (),
			node.get_nature ()
		);
	}

	printf ("%d connexions:\n", nbr_cnx);
	for (int cnx_cnt = 0; cnx_cnt < nbr_cnx; ++cnx_cnt)
	{
		const mfx::cmd::lat::Cnx & cnx = graph.use_cnx (cnx_cnt);
		printf (
			"%02d: %02d -> %02d, D = %d\n",
			cnx_cnt,
			cnx.get_node (mfx::piapi::Dir_OUT),
			cnx.get_node (mfx::piapi::Dir_IN ),
			cnx.get_comp_delay ()
		);
	}
}



int	TestLatAlgo::check_graph (const mfx::cmd::lat::GraphInterface &graph)
{
	assert (&graph != 0);

	int            ret_val = 0;

	const int      nbr_nodes = graph.get_nbr_nodes ();

	for (int node_cnt = 0; node_cnt < nbr_nodes && ret_val == 0; ++node_cnt)
	{
		const mfx::cmd::lat::Node &   node = graph.use_node (node_cnt);
		if (! node.is_timestamp_set ())
		{
			ret_val = -1;
			assert (false);
		}

		else
		{
			std::vector <NodeChk>   nc_arr (nbr_nodes);
			ret_val = check_graph_rec (graph, node_cnt, 0, nc_arr);
		}
	}

	return ret_val;
}



int	TestLatAlgo::check_graph_rec (const mfx::cmd::lat::GraphInterface &graph, int node_index, int cur_ts, std::vector <NodeChk> &nc_arr)
{
	assert (&graph != 0);
	assert (node_index >= 0);
	assert (node_index < graph.get_nbr_nodes ());
	assert (&nc_arr != 0);

	int            ret_val = 0;

	NodeChk &      nc = nc_arr [node_index];
	if (nc._timestamp_flag)
	{
		if (cur_ts != nc._timestamp)
		{
			ret_val = -1;
			assert (false);
		}
	}
	else
	{
		nc._timestamp      = cur_ts;
		nc._timestamp_flag = true;
	}

	if (ret_val == 0 && ! nc._complete_flag)
	{
		const mfx::cmd::lat::Node &   node = graph.use_node (node_index);
		cur_ts += node.get_latency ();

		const int      nbr_cnx = node.get_nbr_cnx (mfx::piapi::Dir_OUT);
		for (int cnx_cnt = 0; cnx_cnt < nbr_cnx && ret_val == 0; ++cnx_cnt)
		{
			const int      cnx_index = node.get_cnx (mfx::piapi::Dir_OUT, cnx_cnt);
			const mfx::cmd::lat::Cnx & cnx = graph.use_cnx (cnx_index);

			const int      next_ts         = cur_ts + cnx.get_comp_delay ();
			const int		next_node_index = cnx.get_node (mfx::piapi::Dir_IN);

			ret_val = check_graph_rec (graph, next_node_index, next_ts, nc_arr);
		}

		nc._complete_flag = true;
	}

	return ret_val;
}



std::default_random_engine	TestLatAlgo::_generator;



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
