/*****************************************************************************

        Algo.cpp
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
#include "mfx/cmd/lat/Tools.h"

#include <algorithm>

#include <cassert>
#include <climits>
#include <cstdlib>



namespace mfx
{
namespace cmd
{
namespace lat
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Algo::Algo (const Algo &other)
:	_cnx_list (other._cnx_list)
,	_node_list (other._node_list)
,	_bm_nbr_rec_calls (other._bm_nbr_rec_calls)
,	_tree_list ()          // Temporary, not copied
,	_term_node_list_arr () // Temporary, not copied
,	_visited_flag_list ()  // Temporary, not copied
{
	// Nothing
}



Algo &	Algo::operator = (const Algo &other)
{
	if (&other != this)
	{
		_cnx_list         = other._cnx_list;
		_node_list        = other._node_list;
		_bm_nbr_rec_calls = other._bm_nbr_rec_calls;
	}

	return *this;
}



void	Algo::reset ()
{
	_node_list.clear ();
	_cnx_list.clear ();
}



// Previous data is lost
void	Algo::set_nbr_elt (int nbr_nodes, int nbr_cnx)
{
	assert (nbr_nodes >= 0);
	assert (nbr_cnx >= 0);

	reset ();

	_node_list.resize (nbr_nodes);
	_cnx_list.resize (nbr_cnx);
}



void	Algo::run ()
{
	_bm_nbr_rec_calls = 0;

	_tree_list.init (*this);

	// Allocates memory for terminal node lists
	const int      nbr_nodes_tot = get_nbr_nodes ();
	for (auto &tnl : _term_node_list_arr)
	{
		tnl._list.resize (nbr_nodes_tot);
	}

	_visited_flag_list.resize (nbr_nodes_tot);

	// Finds all timestamps
	const int      nbr_trees = _tree_list.get_nbr_trees ();
	for (int tree_cnt = 0; tree_cnt < nbr_trees; ++tree_cnt)
	{
		build_term_node_list (tree_cnt);
		set_initial_timestamps ();
		compute_remaining_timestamps (tree_cnt);
	}

	_visited_flag_list.clear ();

	for (auto &tnl : _term_node_list_arr)
	{
		tnl._list.clear ();
	}

	_tree_list.restore ();
}



int	Algo::get_bm_nbr_rec_calls () const
{
	return _bm_nbr_rec_calls;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	Algo::do_get_nbr_nodes () const
{
	return int (_node_list.size ());
}



Node &	Algo::do_use_node (int index)
{
	return _node_list [index];
}



const Node &	Algo::do_use_node (int index) const
{
	return _node_list [index];
}



int	Algo::do_get_nbr_cnx () const
{
	return int (_cnx_list.size ());
}



Cnx &	Algo::do_use_cnx (int index)
{
	return _cnx_list [index];
}



const Cnx &	Algo::do_use_cnx (int index) const
{
	return _cnx_list [index];
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Algo::build_term_node_list (int tree_cnt)
{
	assert (tree_cnt >= 0);
	assert (tree_cnt < _tree_list.get_nbr_trees ());

	const int      nbr_nodes = _tree_list.get_nbr_nodes (tree_cnt);

	// Builds source and sink node lists for this tree
	for (int dir_cnt = 0; dir_cnt < piapi::Dir_NBR_ELT; ++dir_cnt)
	{
		const piapi::Dir  dir = static_cast <piapi::Dir> (dir_cnt);

		TermNodeList &    tnl = _term_node_list_arr [dir_cnt];
		tnl._nbr_nodes = 0;

		for (int node_cnt = 0; node_cnt < nbr_nodes; ++node_cnt)
		{
			const int      node_index = _tree_list.get_node (tree_cnt, node_cnt);
			const Node &   node       = use_node (node_index);
			if (node.is_pure (dir))
			{
				tnl._list [tnl._nbr_nodes] = node_index;
				++ tnl._nbr_nodes;
			}
		}

		// We must have terminal nodes because we don't allow loops
		assert (tnl._nbr_nodes > 0);
	}
}



void	Algo::set_initial_timestamps ()
{
	// Sets audio input timestamps to 0
	const TermNodeList & tnl_out = _term_node_list_arr [piapi::Dir_OUT];
	assert (tnl_out._nbr_nodes > 0);
	bool        audio_in_flag = false;
	for (int node_cnt = 0; node_cnt < tnl_out._nbr_nodes; ++node_cnt)
	{
		const int      node_index = tnl_out._list [node_cnt];
		Node &         node       = use_node (node_index);
		const Node::Nature   nature = node.get_nature ();
		if (nature == Node::Nature_SOURCE)
		{
			node.set_timestamp (0);
			audio_in_flag = true;
		}
	}

	// If there is no audio input in the tree, pick arbitrarily a node
	// and set its timestamp to 0.
	if (! audio_in_flag)
	{
		const int      first_index = tnl_out._list [0];
		Node &         node        = use_node (first_index);
		node.set_timestamp (0);
	}
}



void	Algo::compute_remaining_timestamps (int tree_cnt)
{
	assert (tree_cnt >= 0);
   assert (tree_cnt < _tree_list.get_nbr_trees ());

	bool           smth_done_flag;
	do
	{
		smth_done_flag = false;

		// Starts with pure inputs (sink nodes)
		for (int dir_cnt = 0; dir_cnt < piapi::Dir_NBR_ELT; ++dir_cnt)
		{
			const piapi::Dir     dir = static_cast <piapi::Dir> (dir_cnt);
			const TermNodeList & tnl = _term_node_list_arr [dir_cnt];

			int            pos = 0;
			while (pos < tnl._nbr_nodes)
			{
				// Skips nodes whose timestamp is known
				const bool     found_flag =
				skip_term_nodes_known_timestamps (pos, tnl);

				if (found_flag)
				{
					reset_visit_list ();

					const int      node_index = tnl._list [pos];
					const bool		flag = compute_timestamp_rec (node_index, dir);
					smth_done_flag |= flag;
					++ pos;
				}
			}
		}
	}
	while (smth_done_flag);
}



bool	Algo::skip_term_nodes_known_timestamps (int &pos, const TermNodeList &tnl)
{
	assert (&tnl != 0);
	assert (pos >= 0);
   assert (pos < tnl._nbr_nodes);

	bool           found_flag = false;
	do
	{
		const int      node_index = tnl._list [pos];
		const Node &   node       = use_node (node_index);
		if (node.is_timestamp_set ())
		{
			++ pos;
		}
		else
		{
			found_flag = true;
		}
	}
	while (! found_flag && pos < tnl._nbr_nodes);

	assert (pos < tnl._nbr_nodes || ! found_flag);

	return found_flag;
}



void	Algo::reset_visit_list ()
{
	const int      nbr_nodes = get_nbr_nodes ();

	for (int node_cnt = 0; node_cnt < nbr_nodes; ++node_cnt)
	{
		_visited_flag_list [node_cnt] = false;
	}
}



// Starts from a pure sink node, against the current (Dir_IN) or
// from a pure source node (Dir_OUT)
bool	Algo::compute_timestamp_rec (int node_index, piapi::Dir dir)
{
	assert (node_index >= 0);
   assert (node_index < get_nbr_nodes ());
	assert (dir >= 0);
	assert (dir < piapi::Dir_NBR_ELT);

	++ _bm_nbr_rec_calls;

	bool           progress_flag = false;

	_visited_flag_list [node_index] = true;
   Node &         node = use_node (node_index);

	if (! node.is_timestamp_set ())
	{
		const piapi::Dir  dir_inv = Tools::invert_dir (dir);
		const int   nbr_next      = node.get_nbr_cnx (dir);

		// Computes timestamps of all source nodes, if possible.
		// Finds the biggest one
		int         biggest_timestamp =
			  (dir == piapi::Dir_IN) ? INT_MIN : INT_MAX;
		bool        biggest_found_flag = false;
		for (int next_cnt = 0; next_cnt < nbr_next; ++next_cnt)
		{
			const int      next_node_index = Tools::get_next_node (
				*this,
				node_index,
				dir,
				next_cnt
			);

			// Computes timestamp if never visited
			const Node &  next_node = use_node (next_node_index);
			if (! _visited_flag_list [next_node_index])
			{
				const bool     flag =
					compute_timestamp_rec (next_node_index, dir);
				progress_flag |= flag;
			}

			// Collects maximum timestamp
			if (next_node.is_timestamp_set ())
			{
				const int      next_timestamp =
					get_timestamp_at (next_node, dir_inv);
				biggest_timestamp = get_biggest_timestamp (
					biggest_timestamp,
					next_timestamp,
					dir
				);
				biggest_found_flag = true;
			}
		}

		if (biggest_found_flag)
		{
			// Computes node timestamp
			long				node_timestamp = biggest_timestamp;
			if (dir == piapi::Dir_IN)
			{
				const int      latency = node.get_latency ();
				node_timestamp += latency;
			}
			node.set_timestamp (node_timestamp);

			// Synchronises inputs by adding a delay on connexions
			for (int next_cnt = 0; next_cnt < nbr_next; ++next_cnt)
			{
				const int      cnx_index       = node.get_cnx (dir, next_cnt);
				Cnx &          cnx             = use_cnx (cnx_index);
				const int      next_node_index = cnx.get_node (dir_inv);
				const Node &   next_node       = use_node (next_node_index);

				if (next_node.is_timestamp_set ())
				{
					const int      delay =
						compute_delay_between (node, next_node, dir);
					assert (cnx.get_comp_delay () == 0);
					cnx.set_comp_delay (delay);
				}
			}

			progress_flag = true;
		}
	}

	return progress_flag;
}



bool	Algo::compute_timestamp_bck_rec (int node_index)
{
	assert (node_index >= 0);
   assert (node_index < get_nbr_nodes ());

	bool           progress_flag = false;

	/*** To do ***/
	assert (false);

	return progress_flag;
}



int	Algo::get_timestamp_at (const Node &node, piapi::Dir dir)
{
	assert (&node != 0);
	assert (node.is_timestamp_set ());
	assert (dir >= 0);
	assert (dir < piapi::Dir_NBR_ELT);

	int            timestamp = node.get_timestamp ();
	if (dir == piapi::Dir_IN)
	{
		timestamp -= node.get_latency ();
	}

	return timestamp;
}



int	Algo::get_biggest_timestamp (long ts_1, long ts_2, piapi::Dir dir)
{
	assert (dir >= 0);
	assert (dir < piapi::Dir_NBR_ELT);

	if (dir == piapi::Dir_IN)
	{
		ts_1 = std::max (ts_1, ts_2);
	}
	else
	{
		ts_1 = std::min (ts_1, ts_2);
	}

	return ts_1;
}



int	Algo::compute_delay_between (const Node &node_1, const Node &node_2, piapi::Dir node_1_end)
{
	assert (&node_1 != 0);
	assert (node_1.is_timestamp_set ());
	assert (&node_2 != 0);
	assert (node_2.is_timestamp_set ());
	assert (node_1_end >= 0);
	assert (node_1_end < piapi::Dir_NBR_ELT);

	const piapi::Dir  node_2_end = Tools::invert_dir (node_1_end);
	const int      ts_1  = get_timestamp_at (node_1, node_1_end);
	const int      ts_2  = get_timestamp_at (node_2, node_2_end);
	const int      delay = std::abs (ts_2 - ts_1);

	return delay;
}



}  // namespace lat
}  // namespace cmd
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
