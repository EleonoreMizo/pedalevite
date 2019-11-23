/*****************************************************************************

        TestLatAlgo.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestLatAlgo_HEADER_INCLUDED)
#define TestLatAlgo_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/cmd/lat/Node.h"

#include <random>
#include <vector>



namespace mfx
{
namespace cmd
{
namespace lat
{
	class Algo;
	class GraphInterface;
	class TreeList;
}
}
}

class TestLatAlgo
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static int     perform_test ();




/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	class CnxInfo
	{
	public:
		int            _node_src;
		int            _node_dst;
	};
	class NodeInfo
	{
	public:
		int            _latency;
		mfx::cmd::lat::Node::Nature
		               _nature;
	};

	class NodeChk
	{
	public:
		int            _timestamp      = -12345;
		bool           _timestamp_flag = false;
		bool           _complete_flag  = false;
	};

	static int     test_tree_only ();
	static int     test_full ();
	static void    build_graph (mfx::cmd::lat::Algo &graph, const CnxInfo cnx_info_arr [], const NodeInfo node_info_arr []);
	static void    build_graph_random (mfx::cmd::lat::Algo &graph, int nbr_nodes, int nbr_cnx);
	static void    display_tree_list (const mfx::cmd::lat::TreeList &tree_list);
	static void    display_graph (const mfx::cmd::lat::GraphInterface &graph);
	static int     check_graph (const mfx::cmd::lat::GraphInterface &graph);
	static int     check_graph_rec (const mfx::cmd::lat::GraphInterface &graph, int node_index, int cur_ts, std::vector <NodeChk> &nc_arr);

	static std::default_random_engine
	               _generator;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TestLatAlgo ()                               = delete;
	               TestLatAlgo (const TestLatAlgo &other)       = delete;
	virtual        ~TestLatAlgo ()                              = delete;
	TestLatAlgo &  operator = (const TestLatAlgo &other)        = delete;
	bool           operator == (const TestLatAlgo &other) const = delete;
	bool           operator != (const TestLatAlgo &other) const = delete;

}; // class TestLatAlgo



//#include "test/TestLatAlgo.hpp"



#endif   // TestLatAlgo_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
