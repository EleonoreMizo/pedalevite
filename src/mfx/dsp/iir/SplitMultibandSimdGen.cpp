/*****************************************************************************

        SplitMultibandSimdGen.cpp
        Author: Laurent de Soras, 2021

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

#include "fstb/fnc.h"
#include "mfx/dsp/iir/SplitMultibandSimdGen.h"

#include <algorithm>
#include <cassert>
#include <cctype>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



SplitMultibandSimdGen::SplitMultibandSimdGen (int nbr_bands, int order_a0, int order_a1, const std::string &main_classname, std::initializer_list <std::string> namespaces)
:	_nbr_bands (nbr_bands)
,	_nbr_split (nbr_bands - 1)
,	_order_a0 (order_a0)
,	_order_a1 (order_a1)
,	_classname (main_classname)
,	_namespaces (namespaces)
,	_node_arr ()
,	_split_arr (_nbr_split)
,	_simd_group_arr ()
,	_lvl_arr ()
{
	assert (nbr_bands >= 2);
	assert (order_a0 >= 0);
	assert (order_a1 >= 0);
	assert (order_a0 + order_a1 > 0);
	assert (! main_classname.empty ());
	assert (std::isalnum (main_classname.front ()));

	// Builds the crossover tree
	build_xover_tree ();

	// Finds and assigns the compensation filters to each crossover branch
	find_comp_filters_rec (0);
	assert (std::find_if (
		_split_arr.begin (), _split_arr.end (), [] (const Splitter &s) {
			return (s._node_idx < 0);
		}
	) == _split_arr.end ());

	// Tries to factorize the crossover butterfly gains
	factorize_gain_rec (0);

	// Makes SIMD groups from crossovers and phase compensation filters
	build_simd_group_array ();

	// Fills the FilterSpec objects for each SIMD group
	for (int grp_idx = 0; grp_idx < int (_simd_group_arr.size ()); ++grp_idx)
	{
		create_group_proc_map (grp_idx);
		optimize_group_comp (grp_idx);
		create_group_spec (grp_idx);
		optimize_group_gain (grp_idx);
	}

	// Generates class header and code
	_r = generate_main ();
}



SplitMultibandSimdGen::Result	SplitMultibandSimdGen::print_code () const
{
	return _r;
}



// Debugging function
std::string	SplitMultibandSimdGen::print_state () const
{
	std::string    m;
	char           txt_0 [1023+1];

	for (const auto &simd_group : _simd_group_arr)
	{
		fstb::snprintf4all (txt_0, sizeof (txt_0),
			"Group: max depth: %d, total footprint: %2d\n",
			simd_group.compute_max_depth (),
			simd_group.compute_footprint ()
		);
		m += txt_0;

		m += "\toutput  : ";
		for (int pos = 0; pos < int (simd_group._node_out_arr.size ()); ++pos)
		{
			if (pos > 0)
			{
				m += ", ";
			}
			const int      node_idx = simd_group._node_out_arr [pos];
			if (node_idx < 0)
			{
				m += "---";
			}
			else
			{
				const auto &   node     = _node_arr [node_idx];
				fstb::snprintf4all (txt_0, sizeof (txt_0),
					"%s%02d",
					(node._type == Node::Type::BAND) ? "B" : "S",
					node._idx
				);
				m += txt_0;
			}
		}
		m += "\n";

		for (const auto &simd_path : simd_group._path_list)
		{
			fstb::snprintf4all (txt_0, sizeof (txt_0),
				"\tsplitter: %3d, depth: %d, footprint: %2d\n",
				simd_path._split_idx,
				simd_path._depth,
				simd_path._footprint
			);
			m += txt_0;
		}

		m += "\tspecs   : " + simd_group._spec._classname + "\n";
		for (const auto &spec : simd_group._spec._spec_arr)
		{
			m += "\t+\tgains : ";
			for (const auto &gain : spec._gain_arr)
			{
				fstb::snprintf4all (txt_0, sizeof (txt_0), "%f ", gain);
				m += txt_0;
			}
			m += "\n\t\tnbr 2p: ";
			for (const auto &n : spec._nbr_p2)
			{
				fstb::snprintf4all (txt_0, sizeof (txt_0), "%d ", n);
				m += txt_0;
			}
			m += "\n\t\t1-pole: ";
			m += spec._p1_flag ? "yes" : "no";
			m += "\n\t\tuse 1p: ";
			for (const auto &s_flag : spec._skip_p1)
			{
				m += (! s_flag) ? "#" : ".";
				m += " ";
			}
			m += "\n\t\tbutfly: ";
			for (const auto &b_flag : spec._butterfly)
			{
				m += b_flag ? "#" : ".";
				m += " ";
			}
			m += "\n\t\texpand: ";
			m += spec._expand_flag ? "yes" : "no";
			m += "\n";
		}
	}

	return m;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	SplitMultibandSimdGen::FilterSpec::get_p2_len () const noexcept
{
	const int      nbr_p2_max = *(std::max_element (
		_nbr_p2.begin (), _nbr_p2.end ()
	));

	return nbr_p2_max;
}



int	SplitMultibandSimdGen::FilterSpec::get_p1_len () const noexcept
{
	return _p1_flag ? 1 : 0;
}



bool	SplitMultibandSimdGen::FilterSpec::has_gain () const noexcept
{
	return (std::find_if (
		_gain_arr.begin (),
		_gain_arr.end (),
		[] (float x) { return (x != 1); }
	) != _gain_arr.end ());
}



bool	SplitMultibandSimdGen::FilterSpec::operator == (const FilterSpec &other) const noexcept
{
	return (
		   _gain_arr    == other._gain_arr
		&& _nbr_p2      == other._nbr_p2
		&& _p1_flag     == other._p1_flag
		&& _skip_p1     == other._skip_p1
		&& _butterfly   == other._butterfly
		&& _expand_flag == other._expand_flag
	);
}



int	SplitMultibandSimdGen::ClassSpec::get_p2_ofs (int spec_idx) const noexcept
{
	assert (spec_idx >= 0);
	assert (spec_idx < int (_spec_arr.size ()));

	int            ofs = 0;
	for (int k = 0; k < spec_idx; ++k)
	{
		ofs += _spec_arr [k].get_p2_len ();
	}

	return ofs;
}



int	SplitMultibandSimdGen::ClassSpec::get_p1_ofs (int spec_idx) const noexcept
{
	assert (spec_idx >= 0);
	assert (spec_idx < int (_spec_arr.size ()));

	int            ofs = 0;
	for (int k = 0; k < spec_idx; ++k)
	{
		ofs += _spec_arr [k].get_p1_len ();
	}

	return ofs;
}



int	SplitMultibandSimdGen::SimdGroup::compute_footprint () const noexcept
{
	int            footprint = 0;

	std::for_each (
		_path_list.begin (), _path_list.end (),
		[&footprint] (const SimdPath &path) {
			footprint += path._footprint;
		}
	);

	return footprint;
}



int	SplitMultibandSimdGen::SimdGroup::compute_max_depth () const noexcept
{
	assert (! _path_list.empty ());

	const auto     it = std::max_element (
		_path_list.begin (),
		_path_list.end (),
		[] (const SimdPath &l, const SimdPath &r) { return l._depth < r._depth; }
	);
	assert (it != _path_list.end ());

	return it->_depth;
}



void	SplitMultibandSimdGen::build_xover_tree ()
{
	_node_arr.resize (1);
	build_xover_tree_rec (0, 0, _nbr_bands, 0);
}



void	SplitMultibandSimdGen::build_xover_tree_rec (int node_idx, int band_beg, int band_end, int level)
{
	assert (node_idx >= 0);
	assert (band_beg < band_end);
	assert (level >= 0);

	auto &         node = _node_arr [node_idx];
	node._level = level;

	if (level >= int (_lvl_arr.size ()))
	{
		assert (level == int (_lvl_arr.size ()));
		_lvl_arr.resize (level + 1);
	}
	_lvl_arr [level].push_back (node_idx);

	// Just one band: it's a leaf.
	if (band_end - band_beg == 1)
	{
		node._type = Node::Type::BAND;
		node._idx  = band_beg;
	}
	else
	{
		// When splitting an odd number of bands, we put the largest group first
		// (round to +oo) to have the deepest branch first. This will ease the
		// grouping into SIMD units.
		const int      band_mid = (band_beg + band_end + 1) >> 1;

		node._type         = Node::Type::SPLIT;
		node._idx          = band_mid - 1;
		const int      c_0 = int (_node_arr.size ());
		const int      c_1 = int (_node_arr.size ()) + 1;
		node._children [0] = c_0;
		node._children [1] = c_1;
		// 'node' reference becomes invalid from here
		_node_arr.resize (_node_arr.size () + _nbr_split_out);
		build_xover_tree_rec (c_0, band_beg, band_mid, level + 1);
		build_xover_tree_rec (c_1, band_mid, band_end, level + 1);
	}
}



// Returns the index list of all the splitters covered by node_idx
// Assigns recursively the splitter subtrees to their compensation list
// of their sibling
SplitMultibandSimdGen::CompArray	SplitMultibandSimdGen::find_comp_filters_rec (int node_idx)
{
	assert (node_idx >= 0);
	assert (node_idx < int (_node_arr.size ()));

	std::vector <int> comp_list;

	Node &         node  = _node_arr [node_idx];
	assert (node._type == Node::Type::SPLIT);
	Splitter &     split = _split_arr [node._idx];
	split._node_idx = node_idx;
	for (int child_cnt = 0; child_cnt < _nbr_split_out; ++child_cnt)
	{
		const auto     child_idx = node._children [child_cnt];
		if (child_idx >= 0)
		{
			Node &         child = _node_arr [child_idx];
			if (child._type == Node::Type::SPLIT)
			{
				const auto     child_list = find_comp_filters_rec (child_idx);
				split._path_arr [1 - child_cnt]._comp_arr = child_list;
				comp_list.insert (
					comp_list.end (),
					child_list.begin (), child_list.end ()
				);
			}
		}
	}

	comp_list.push_back (node._idx);

	return comp_list;
}



int	SplitMultibandSimdGen::factorize_gain_rec (int node_idx)
{
	assert (node_idx >= 0);
	assert (node_idx < int (_node_arr.size ()));

	Node &         node  = _node_arr [node_idx];
	if (node._type == Node::Type::BAND)
	{
		return 0;
	}

	std::array <int, _nbr_split_out> gain_arr {};
	for (int child_cnt = 0; child_cnt < _nbr_split_out; ++child_cnt)
	{
		const auto     child_idx = node._children [child_cnt];
		assert (child_idx >= 0);
		gain_arr [child_cnt] = factorize_gain_rec (child_idx);
	}
	const int      com = *(std::min_element (gain_arr.begin (), gain_arr.end ()));
	if (com > 0)
	{
		_split_arr [node._idx]._gain_l2 += com;

		for (int child_cnt = 0; child_cnt < _nbr_split_out; ++child_cnt)
		{
			const auto     child_idx = node._children [child_cnt];
			const auto &   child = _node_arr [child_idx];
			assert (child._type == Node::Type::SPLIT);
			_split_arr [child._idx]._gain_l2 -= com;
		}
	}

	return _split_arr [node._idx]._gain_l2;
}



/*
Pack several splitters into a single SIMD operation.
Splitters are packed:
- vertically: chained splitters and phase compensations, generating
multiple channels from a single source.
- horizontally, if there is room enough, to process multiple source
channels. This is mostly for the deepest levels, generating the final
band outputs.
To pack splitters, we start from the root. We pack everything at a given
level before packing splitters from the subsequent levels. This brings a
natural causal ordering for processing.
*/

void	SplitMultibandSimdGen::build_simd_group_array ()
{
	LevelArray     rem_nodes (_lvl_arr);

	int            lvl_base = 0;
	do
	{
		const LevelContent & lvl_cont = rem_nodes [lvl_base];
		SimdGroup      group;

		// Indicatest that the group is full or finished and that we have to
		// store it in the list, and start a new one if required.
		bool           validate_flag = false;

		while (! lvl_cont.empty () || ! group._path_list.empty ())
		{
			const int      footprint_group = group.compute_footprint ();

			// Validates the group if required
			if (footprint_group >= _simd_w || validate_flag)
			{
				assert (! group._path_list.empty ());
				// Checks if all footprints are equal within the group (required
				// later). This will be probably not the case with _simd_w > 4
				assert (std::find_if (
					group._path_list.begin (),
					group._path_list.end (),
					[&group] (const SimdPath &p) {
						return (p._footprint != group._path_list.front ()._footprint);
					}
				) == group._path_list.end ());

				// Gives the group a unique classname, for code generation
				char           txt_0 [255+1];
				fstb::snprintf4all (
					txt_0, sizeof (txt_0),
					"Filter%d", group._path_list.front ()._split_idx
				);
				group._spec._classname = txt_0;

				// Saves the current group and creates a new one
				_simd_group_arr.push_back (group);
				group         = SimdGroup {};
				validate_flag = false;
			}

			// Level done: validates the group if it is not empty, so we'll
			// start the next level with a fresh group.
			else if (lvl_cont.empty ())
			{
				validate_flag = (! group._path_list.empty ());
			}

			// Keeps on filling the current group with splitters.
			else
			{
				const int      node_idx         = lvl_cont.front ();
				const int      depth_from_there =
					get_node_descent_depth (node_idx);
				const int      depth_room       =
					fstb::get_prev_pow_2 (_simd_w - footprint_group);
				const int      footprint_l2     =
					std::min (depth_from_there, depth_room);
				if (footprint_l2 < 1)
				{
					assert (! group._path_list.empty ());
					validate_flag = true;
				}
				else
				{
					const int      footprint = 1 << footprint_l2;

					SimdPath       path;
					path._split_idx = _node_arr [node_idx]._idx;
					path._depth     = footprint_l2;
					path._footprint = footprint;
					group._path_list.push_back (path);
					erase_subtree (rem_nodes, node_idx, footprint_l2);
				}
			}
		}

		// Next level
		++ lvl_base;
	}
	while (lvl_base < int (rem_nodes.size ()));
}



// Returns the depth of the descendants of a node.
// A final band has a depth of 0.
int	SplitMultibandSimdGen::get_node_descent_depth (int node_idx) const
{
	assert (node_idx >= 0);
	assert (node_idx < int (_node_arr.size ()));

	int              depth = 0;

	const Node &   node = _node_arr [node_idx];
	if (node._type == Node::Type::SPLIT)
	{
		for (const auto &child_idx : node._children)
		{
			if (child_idx >= 0)
			{
				const int      depth_child = get_node_descent_depth (child_idx);
				depth = std::max (depth, depth_child + 1);
			}
		}
	}

	return depth;
}



// Erase 'depth' levels in the subtree beginning at node node_idx.
// Leafs (bands) are always erased when the parent splitter is erased.
void	SplitMultibandSimdGen::erase_subtree (LevelArray &rem_nodes, int node_idx, int depth)
{
	assert (node_idx >= 0);
	assert (node_idx < int (_node_arr.size ()));
	assert (depth >= 0);

	const Node &   node = _node_arr [node_idx];
	if (depth > 0)
	{
		for (const auto &child_idx : node._children)
		{
			if (child_idx >= 0)
			{
				erase_subtree (rem_nodes, child_idx, depth - 1);
			}
		}
	}

	if (depth > 0 || node._type == Node::Type::BAND)
	{
		LevelContent & lvl_cont = rem_nodes [node._level];
		const auto    it = std::find (lvl_cont.begin (), lvl_cont.end (), node_idx);
		assert (it != lvl_cont.end ());
		lvl_cont.erase (it);
	}
}



/*
Here, we create a table showing channel processing for each processing level.
The number of channels in _proc_map doubles each level.
_band_flag_arr is filled too.
*/

void	SplitMultibandSimdGen::create_group_proc_map (int group_idx)
{
	assert (group_idx >= 0);
	assert (group_idx < int (_simd_group_arr.size ()));

	SimdGroup &    simd_group = _simd_group_arr [group_idx];
	const int      max_depth = simd_group.compute_max_depth ();
	const int      nbr_paths = int (simd_group._path_list.size ());

	simd_group._proc_map.resize (
		max_depth, fstb::make_array <_max_nbr_chn, int> (-1)
	);

	// Indicates at least one channel output is a final band
	simd_group._band_flag_arr.resize (max_depth, false);

	// Inputs are packed together, and expanded twice after each split
	// Footprints for all paths should be equal
	for (int path_cnt = 0; path_cnt < nbr_paths; ++path_cnt)
	{
		simd_group._proc_map [0] [path_cnt] = simd_group._path_list [path_cnt]._split_idx;
	}

	// Builds _proc_map, fills _node_out_arr and _band_flag_arr
	for (int depth = 0; depth < max_depth; ++depth)
	{
		const int   nbr_parents = nbr_paths << depth;
		for (int par_cnt = 0; par_cnt < nbr_parents; ++par_cnt)
		{
			const int      path_idx  = par_cnt >> depth;
			const auto &   simd_path = simd_group._path_list [path_idx];

			const int      par_split_idx = simd_group._proc_map [depth] [par_cnt];
			if (par_split_idx >= 0)
			{
				const int      par_node_idx = _split_arr [par_split_idx]._node_idx;
				assert (par_node_idx >= 0);
				const auto &   par_node     = _node_arr [par_node_idx];
				if (par_node._type == Node::Type::SPLIT)
				{
					for (int child_idx = 0; child_idx < _nbr_split_out; ++child_idx)
					{
						const int   node_idx = par_node._children [child_idx];
						assert (node_idx >= 0);
						const auto &   node = _node_arr [node_idx];
						const int      depth_child = depth + 1;
						const int      chn_child = (par_cnt << 1) + child_idx;
						int            lane = chn_child << (max_depth - depth_child);
						// At the last level, both new channels are the A0/A1 lanes
						// of the original channels (no expand step)
						lane = (lane >> 1) + (lane & 1) * _max_nbr_chn;
						simd_group._node_out_arr [lane] = node_idx;

						if (depth_child < simd_path._depth)
						{
							if (node._type == Node::Type::SPLIT)
							{
								simd_group._proc_map [depth_child] [chn_child] = node._idx;
							}
							else
							{
								simd_group._band_flag_arr [depth_child] = true;
							}
						}
					}
				}
				else
				{
					simd_group._band_flag_arr [depth + 1] = true;
				}
			}
		}
	}
}



/*
We're trying to move phase compensation filters into the next crossovers
filter. Each candidate channel has to meet several conditions:
- The group should have at least 2 levels, we're not doing cross-group
	optimisations
- The channel requires at least one phase compensation filter
- The channel is not split again within the group, so its lanes are
	free in the next split
At this point we know we can move one of the candidate's compensation filter
to the next split, reducing the compensation level on the second stage.
A further optimisation can be done if these addtional conditions are met:
- All channels using compensation in the first level are either candidates
	or discarded channels
- Compensations are made of only one filter
In this case, there is no remaining compensation in the first stage and we can
completely eliminate it.

In this function, we target only the full optimisation (all conditions met).
*/

void	SplitMultibandSimdGen::optimize_group_comp (int group_idx)
{
	assert (group_idx >= 0);
	assert (group_idx < int (_simd_group_arr.size ()));

	SimdGroup &    simd_group = _simd_group_arr [group_idx];
	assert (! simd_group._proc_map.empty ());
	assert (! simd_group._band_flag_arr.empty ());

	const int      max_depth = simd_group.compute_max_depth ();
	const int      nbr_paths = int (simd_group._path_list.size ());

	// We leave the function as soon as a condition is not met
	if (max_depth < 2)
	{
		return;
	}

	// Level of the split whose compensation filters are checked
	const int      level   = max_depth - 2;
	const auto &   lvl_cur = simd_group._proc_map [level    ];
	const auto &   lvl_nxt = simd_group._proc_map [level + 1];

	// Number of channels at the given level, other channels are discarded
	const int      nbr_chn = nbr_paths << level;

	for (int chn_cnt = 0; chn_cnt < nbr_chn; ++chn_cnt)
	{
		const int      split_idx = lvl_cur [chn_cnt];
		if (split_idx >= 0)
		{
			const auto &   split = _split_arr [split_idx];

			// Checks if there is exactly one compensation filter attached to
			// this split.
			const auto     tot_comp =
				  split._path_arr [0]._comp_arr.size ()
				+ split._path_arr [1]._comp_arr.size ();
			if (tot_comp != 1)
			{
				return;
			}

			// Checks if the other branch is a split
			// comp_path is the path containing the compensation filter
			const int      comp_path = int (split._path_arr [1]._comp_arr.size ());
			if (lvl_nxt [(chn_cnt << 1) + comp_path   ] >= 0)
			{
				return;
			}

			// resplit_path is the path containing the potential split
			const int      resplit_path = 1 - comp_path;
			if (lvl_nxt [(chn_cnt << 1) + resplit_path] < 0)
			{
				return;
			}
		}
	}

	// Check successful
	simd_group._opt_comp_flag = true;
}



// If there are gains at different levels in the group, factorizes everything
// in the last level.
void	SplitMultibandSimdGen::optimize_group_gain (int group_idx)
{
	assert (group_idx >= 0);
	assert (group_idx < int (_simd_group_arr.size ()));

	SimdGroup &    simd_group = _simd_group_arr [group_idx];
	assert (! simd_group._proc_map.empty ());
	assert (! simd_group._band_flag_arr.empty ());

	// Counts the number of gains
	const int      nbr_stages = int (simd_group._spec._spec_arr.size ());
	assert (nbr_stages > 0);
	int            nbr_gains  = 0;
	for (int st_cnt = 0; st_cnt < nbr_stages; ++st_cnt)
	{
		const auto &   spec = simd_group._spec._spec_arr [st_cnt];
		if (spec.has_gain ())
		{
			++ nbr_gains;
		}
	}

	// We leave the function as soon as a condition is not met
	if (nbr_gains < 2)
	{
		return;
	}

	auto           gain_arr = fstb::make_array <_max_nbr_chn, float> (1.f);
	for (int st_cnt = 0; st_cnt < nbr_stages; ++st_cnt)
	{
		auto &         spec = simd_group._spec._spec_arr [st_cnt];
		if (st_cnt == nbr_stages - 1)
		{
			for (int k = 0; k < _max_nbr_chn; ++k)
			{
				spec._gain_arr [k] *= gain_arr [k];
			}
		}
		else
		{
			for (int k = 0; k < _max_nbr_chn; ++k)
			{
				gain_arr [k] *= spec._gain_arr [k];
				spec._gain_arr [k] = 1;
			}

			if (spec._expand_flag)
			{
				const auto     n_h = _max_nbr_chn >> 1;
				for (int k = n_h - 1; k >= 0; --k)
				{
					const auto     g = gain_arr [k];
					gain_arr [k * 2    ] = g;
					gain_arr [k * 2 + 1] = g;
				}
			}
		}
	}
}



// Makes a filter realisation layout from the SIMD group info.
void	SplitMultibandSimdGen::create_group_spec (int group_idx)
{
	assert (group_idx >= 0);
	assert (group_idx < int (_simd_group_arr.size ()));

	SimdGroup &    simd_group = _simd_group_arr [group_idx];
	assert (! simd_group._proc_map.empty ());
	assert (! simd_group._band_flag_arr.empty ());

	const int      max_depth = simd_group.compute_max_depth ();
	const int      nbr_paths = int (simd_group._path_list.size ());

	// Builds the filter spec
	const int      a0_nbr_p2 = _order_a0 / 2;
	const int      a0_nbr_p1 = _order_a0 & 1;
	const int      a1_nbr_p2 = _order_a1 / 2;
	const int      a1_nbr_p1 = _order_a1 & 1;
	const int      a1_ofs    = _max_nbr_chn;
	for (int depth = 0; depth < max_depth; ++depth)
	{
		const auto &   level = simd_group._proc_map [depth];
		FilterSpec     spec;

		const bool     expand_flag = (depth < max_depth - 1);
		const bool     opt_0_flag  =
			(simd_group._opt_comp_flag && depth == max_depth - 2);
		const bool     opt_1_flag  =
			(simd_group._opt_comp_flag && depth == max_depth - 1);

		// Splitters
		float          gain_prev = 1.f;
		spec._p1_flag = (a0_nbr_p1 + a1_nbr_p1 > 0);
		for (int chn_idx = 0; chn_idx < _max_nbr_chn; ++chn_idx)
		{
			spec._nbr_p2 [chn_idx         ]  = a0_nbr_p2;
			spec._nbr_p2 [chn_idx + a1_ofs]  = a1_nbr_p2;
			spec._skip_p1 [chn_idx         ] = (a0_nbr_p1 == 0);
			spec._skip_p1 [chn_idx + a1_ofs] = (a1_nbr_p1 == 0);
			const int      split_idx = level [chn_idx];

			if (split_idx >= 0)
			{
				Splitter &     split = _split_arr [split_idx];
				gain_prev = 1.f / (1 << split._gain_l2);
				spec._gain_arr [chn_idx] = gain_prev;
				split._coord._group_idx = group_idx;
				split._coord._lane      = chn_idx;
				split._coord._spec_pos  = int (simd_group._spec._spec_arr.size ());
			}

			else
			{
				if (chn_idx < (nbr_paths << depth))
				{
					// We moved the phase compensation filters here
					if (opt_1_flag)
					{
						const int      split_idx_prev =
							simd_group._proc_map [depth - 1] [chn_idx >> 1];
						Splitter &     split = _split_arr [split_idx_prev];
						const int      path_idx =
							int (split._path_arr [1]._comp_arr.size ());
						assert (path_idx <= 1);
						const int      lane = chn_idx;
						CompModule &   comp_mod = split._path_arr [path_idx];
						comp_mod._coord._group_idx = group_idx;
						comp_mod._coord._lane      = lane;
						comp_mod._coord._spec_pos  =
							int (simd_group._spec._spec_arr.size ());
					}
					else
					{
						// If there is no split but the channel is kept (for band
						// output), we have to neutralize this section.
						spec._nbr_p2 [chn_idx         ]  = 0;
						spec._nbr_p2 [chn_idx + a1_ofs]  = 0;
						spec._skip_p1 [chn_idx         ] = true;
						spec._skip_p1 [chn_idx + a1_ofs] = true;
					}
				}

				// We try to get the same gain on all the lanes, if possible. To
				// do so, we can safely report the gain of the previous splitter
				// on non-splitter channels for the first level of this group.
				if (gain_prev != 1 && depth == 0 && chn_idx >= nbr_paths)
				{
					spec._gain_arr [chn_idx] = gain_prev;
				}
			}

			// Try to enable or disable completely the butterfly operations
			spec._butterfly [chn_idx] =
				(split_idx >= 0 || ! simd_group._band_flag_arr [depth]);
		}
		if (opt_0_flag)
		{
			spec._expand_flag = expand_flag;
		}
		simd_group._spec._spec_arr.push_back (spec);

		// Phase compensations
		if (! opt_0_flag)
		{
			spec              = FilterSpec {};
			spec._expand_flag = expand_flag;
			bool           comp_flag = false;
			for (int chn_idx = 0; chn_idx < _max_nbr_chn; ++chn_idx)
			{
				const int      split_idx = level [chn_idx];
				if (split_idx < 0)
				{
					// If the channel is being discarded, uses the same settings
					// as the previous channels to minimize unique settings.
					if (expand_flag && chn_idx >= nbr_paths << depth)
					{
						spec._nbr_p2 [chn_idx         ]  = spec._nbr_p2 [chn_idx - 1         ];
						spec._nbr_p2 [chn_idx + a1_ofs]  = spec._nbr_p2 [chn_idx - 1 + a1_ofs];
						spec._skip_p1 [chn_idx         ] = spec._skip_p1 [chn_idx - 1         ];
						spec._skip_p1 [chn_idx + a1_ofs] = spec._skip_p1 [chn_idx - 1 + a1_ofs];
					}
					else
					{
						spec._nbr_p2 [chn_idx         ]  = 0;
						spec._nbr_p2 [chn_idx + a1_ofs]  = 0;
						spec._skip_p1 [chn_idx         ] = true;
						spec._skip_p1 [chn_idx + a1_ofs] = true;
					}
				}
				else
				{
					auto &         split = _split_arr [split_idx];
					for (int path_cnt = 0; path_cnt < _nbr_split_out; ++path_cnt)
					{
						CompModule &   comp_mod = split._path_arr [path_cnt];
						const int      nbr_comp = int (comp_mod._comp_arr.size ());
						const int      order    = _order_a0 * nbr_comp;
						const int      nbr_p2   = order / 2;
						const int      nbr_p1   = order & 1;
						const int      lane     = chn_idx + path_cnt * a1_ofs;
						spec._nbr_p2 [lane]  = nbr_p2;
						spec._skip_p1 [lane] = (nbr_p1 == 0);
						if (order > 0)
						{
							comp_mod._coord._group_idx = group_idx;
							comp_mod._coord._lane      = lane;
							comp_mod._coord._spec_pos  =
								int (simd_group._spec._spec_arr.size ());
							spec._p1_flag |= (nbr_p1 > 0);
							comp_flag      = true;

							// We are merging one-pole filters into 2nd-order sections:
							// First come the original 2nd-order sections, then the
							// merged sections, in the same order.
							if ((_order_a0 & 1) != 0 && nbr_comp > 1)
							{
								const int      nbr_merge = nbr_comp & ~1;
								for (int comp_cnt = 0; comp_cnt < nbr_merge; ++comp_cnt)
								{
									// We need to memorize the original b0 coefficient
									const int      split_idx_comp =
										comp_mod._comp_arr [comp_cnt];
									_split_arr [split_idx_comp]._p1_b0_flag = true;
								}
							}
						} // if order > 0
					} // for path_cnt
				} // if split_idx
			} // for chn_idx
			if (comp_flag)
			{
				simd_group._spec._spec_arr.push_back (spec);
			}
		}
	}
}



SplitMultibandSimdGen::Result	SplitMultibandSimdGen::generate_main () const
{
	Result         r;
	char           txt_0 [1023+1];

	fstb::snprintf4all (txt_0, sizeof (txt_0), "%d", _nbr_bands);
	const std::string nbr_bands = txt_0;

	const auto     r_setup { generate_filter_setup () };
	const auto     r_proc { generate_filter_proc () };
	const auto     r_subf { generate_subfilters () };

	r._decl += "class " + _classname + "\n{\npublic:\n\n";
	fstb::snprintf4all (txt_0, sizeof (txt_0),
		"\tstatic constexpr int _nbr_bands = %d;\n"
		"\tstatic constexpr int _nbr_2p_0  = %d;\n"
		"\tstatic constexpr int _nbr_1p_0  = %d;\n"
		"\tstatic constexpr int _nbr_2p_1  = %d;\n"
		"\tstatic constexpr int _nbr_1p_1  = %d;\n",
		_nbr_bands,
		_order_a0 / 2,
		_order_a0 & 1,
		_order_a1 / 2,
		_order_a1 & 1
	);
	r._decl += txt_0;
	r._decl +=
"\tinline void    set_band_ptr (float * const band_ptr_arr [" + nbr_bands + "]) noexcept;\n"
"\tinline constexpr int\n"
"\t               get_nbr_bands () const noexcept { return _nbr_bands; };\n"
"\tinline void    offset_band_ptr (ptrdiff_t offset) noexcept;\n"
		+ r_setup._decl +
"\tvoid           clear_buffers () noexcept;\n"
"\tvoid           process_block (const float *src_ptr, int nbr_spl) noexcept;\n"
		+ r_proc._decl +
"\nprivate:\n\n"
		+ r_subf._decl;
	const int      nbr_groups = int (_simd_group_arr.size ());
	for (int group_idx = 0; group_idx < nbr_groups; ++group_idx)
	{
		const auto &   group = _simd_group_arr [group_idx];
		fstb::snprintf4all (txt_0, sizeof (txt_0),
			"\t%s _filter_%d;\n",
			group._spec._classname.c_str (),
			group_idx
		);
		r._decl += txt_0;
	}
	r._decl +=
"\tfloat * _out_ptr_arr [" + nbr_bands + "] {};\n"
"};\n\n";

	r._cinl += r_subf._cinl;
	r._cinl +=
"void\t" + _classname + "::set_band_ptr (float * const band_ptr_arr [" + nbr_bands + "]) noexcept\n"
"{\n"
"\tstd::copy (band_ptr_arr, band_ptr_arr + " + nbr_bands + ", _out_ptr_arr);\n"
"}\n"
"\n"
"void\t" + _classname + "::offset_band_ptr (ptrdiff_t offset) noexcept\n"
"{\n"
"\tstd::for_each (\n"
"\t\tstd::begin (_out_ptr_arr), std::end (_out_ptr_arr),\n"
"\t\t[offset] (float * &ptr) { ptr += offset; }\n"
"\t);\n"
"}\n"
"\n";
	r._cinl += r_setup._cinl;
	r._cinl += r_proc._cinl;

	r._code += r_subf._code;
	r._code += r_setup._code;
	r._code +=
"void\t" + _classname + "::clear_buffers () noexcept\n"
"{\n";
	for (int group_idx = 0; group_idx < nbr_groups; ++group_idx)
	{
		fstb::snprintf4all (txt_0, sizeof (txt_0),
			"\t_filter_%d.clear_buffers ();\n", group_idx
		);
		r._code += txt_0;
	}
	r._code +=
"}\n"
"\n"
"void\t" + _classname + "::process_block (const float *src_ptr, int nbr_spl) noexcept\n"
"{\n"
"\tfor (int pos = 0; pos < nbr_spl; ++pos)\n"
"\t{\n"
"\t\tprocess_sample (src_ptr [pos]);\n"
"\t\toffset_band_ptr (1);\n"
"\t}\n"
"\toffset_band_ptr (-nbr_spl);\n"
"}\n"
"\n";
	r._code += r_proc._code;

	r._decl  = add_namespaces (r._decl);
	r._cinl  = add_namespaces (r._cinl);
	r._code  = add_namespaces (r._code);

	r._decl  = get_autogen_header () + generate_include () + r._decl;
	r._cinl  = get_autogen_header () + r._cinl;
	r._code  = get_autogen_header () + r._code;

	return r;
}



SplitMultibandSimdGen::Result	SplitMultibandSimdGen::generate_subfilters () const
{
	Result         r;

	const int      nbr_groups = int (_simd_group_arr.size ());
	for (int grp_idx = 0; grp_idx < nbr_groups; ++grp_idx)
	{
		const auto &   simd_group = _simd_group_arr [grp_idx];

		// Checks if the filter structure from this group is identical to
		// another one
		const auto &   ref_arr = simd_group._spec._spec_arr;
		int            id_idx  = -1;
		for (int tst_idx = 0; tst_idx < grp_idx && id_idx < 0; ++tst_idx)
		{
			if (_simd_group_arr [tst_idx]._spec._spec_arr == ref_arr)
			{
				id_idx = tst_idx;
			}
		}

		if (id_idx >= 0)
		{
			r._decl +=
				  "\tusing " + simd_group._spec._classname
				+ " = " + _simd_group_arr [id_idx]._spec._classname
				+ ";\n\n";
		}

		else
		{
			const auto     r_sub =
				generate_code (simd_group._spec, _classname + "::");

			r._decl += "\tclass " + simd_group._spec._classname + "\n\t{\n";
			r._decl += r_sub._decl;
			r._decl += "\t};\n\n";

			r._cinl += r_sub._cinl;

			r._code += r_sub._code;
		}
	}

	return r;
}



SplitMultibandSimdGen::Result	SplitMultibandSimdGen::generate_filter_setup () const
{
	Result         r;

	const std::string fncname = "set_splitter_coef";

	char           txt_0 [1023+1];
	fstb::snprintf4all (txt_0, sizeof (txt_0),
		"%s (int split_idx"
		", const float a0_arr [%d]"
		", const float a1_arr [%d]) noexcept",
		fncname.c_str (),
		_order_a0,
		_order_a1
	);
	const std::string fncarg = txt_0;

	r._decl  = "\tvoid           " + fncarg + ";\n";

	r._code  = "void\t" + _classname + "::" + fncarg + "\n{\n";
	r._code += "\tswitch (split_idx)\n\t{\n";

	const int      a0_nbr_p2 = _order_a0 / 2;
	const int      a0_nbr_p1 = _order_a0 & 1;
	const int      a1_nbr_p2 = _order_a1 / 2;
	const int      a1_nbr_p1 = _order_a1 & 1;
	const int      a1_ofs    = _max_nbr_chn;

	for (int split_idx = 0; split_idx < _nbr_split; ++split_idx)
	{
		const auto &   split  = _split_arr [split_idx];
		const auto &   s_grp  = _simd_group_arr [split._coord._group_idx];
		const int      ofs_p2 = s_grp._spec.get_p2_ofs (split._coord._spec_pos);
		const int      ofs_p1 = s_grp._spec.get_p1_ofs (split._coord._spec_pos);

		fstb::snprintf4all (txt_0, sizeof (txt_0),
			"\tcase %d:\n", split_idx
		);
		r._code += txt_0;

		if (split._p1_b0_flag)
		{
			assert (a0_nbr_p1 > 0);
			fstb::snprintf4all (txt_0, sizeof (txt_0),
				"\t\t_b0_%d = a0_arr [%d];\n",
				split_idx, a0_nbr_p2 * 2
			);
			r._code += txt_0;
		}

		// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
		// Crossover filters

		// 2-pole sections
		for (int p2_cnt = 0; p2_cnt < a0_nbr_p2; ++p2_cnt)
		{
			fstb::snprintf4all (txt_0, sizeof (txt_0),
				"\t\t_filter_%d.set_apf_2p (%d, %d, a0_arr [%d], a0_arr [%d]);\n",
				split._coord._group_idx,
				ofs_p2 + p2_cnt,
				split._coord._lane,
				p2_cnt * 2,
				p2_cnt * 2 + 1
			);
			r._code += txt_0;
		}
		for (int p2_cnt = 0; p2_cnt < a1_nbr_p2; ++p2_cnt)
		{
			fstb::snprintf4all (txt_0, sizeof (txt_0),
				"\t\t_filter_%d.set_apf_2p (%d, %d, a1_arr [%d], a1_arr [%d]);\n",
				split._coord._group_idx,
				ofs_p2 + p2_cnt,
				split._coord._lane + a1_ofs,
				p2_cnt * 2,
				p2_cnt * 2 + 1
			);
			r._code += txt_0;
		}

		// 1-pole sections
		if (a0_nbr_p1 > 0)
		{
			fstb::snprintf4all (txt_0, sizeof (txt_0),
				"\t\t_filter_%d.set_apf_1p (%d, %d, a0_arr [%d]);\n",
				split._coord._group_idx,
				ofs_p1,
				split._coord._lane,
				a0_nbr_p2 * 2
			);
			r._code += txt_0;
		}
		if (a1_nbr_p1 > 0)
		{
			fstb::snprintf4all (txt_0, sizeof (txt_0),
				"\t\t_filter_%d.set_apf_1p (%d, %d, a1_arr [%d]);\n",
				split._coord._group_idx,
				ofs_p1,
				split._coord._lane + a1_ofs,
				a1_nbr_p2 * 2
			);
			r._code += txt_0;
		}

		// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
		// Phase compensation filters

		for (int split_c_idx = 0; split_c_idx < _nbr_split; ++split_c_idx)
		{
			auto &         split_c = _split_arr [split_c_idx];
			for (int path_cnt = 0; path_cnt < _nbr_split_out; ++path_cnt)
			{
				// Checks if the path compensates the phase of the current splitter
				const CompModule &   comp_mod = split_c._path_arr [path_cnt];
				const int      nbr_comp = int (comp_mod._comp_arr.size ());
				const auto     it  = std::find (
					comp_mod._comp_arr.begin (), comp_mod._comp_arr.end (),
					split_idx
				);
				const int      pos =
					int (std::distance (comp_mod._comp_arr.begin (), it));
				if (pos < nbr_comp)
				{
					const auto &   s_grp_c  =
						_simd_group_arr [comp_mod._coord._group_idx];
					const int      ofs_p2_c =
						s_grp_c._spec.get_p2_ofs (comp_mod._coord._spec_pos);
					const int      ofs_p1_c =
						s_grp_c._spec.get_p1_ofs (comp_mod._coord._spec_pos);

					// 2-pole sections
					for (int p2_cnt = 0; p2_cnt < a0_nbr_p2; ++p2_cnt)
					{
						fstb::snprintf4all (txt_0, sizeof (txt_0),
							"\t\t_filter_%d.set_apf_2p (%d, %d, a0_arr [%d], a0_arr [%d]);\n",
							comp_mod._coord._group_idx,
							ofs_p2_c + p2_cnt + pos * a0_nbr_p2,
							comp_mod._coord._lane,
							p2_cnt * 2,
							p2_cnt * 2 + 1
						);
						r._code += txt_0;
					}

					// 1-pole section
					if (a0_nbr_p1 > 0)
					{
						// True 1-pole section
						if (pos == nbr_comp - 1 && (nbr_comp & 1) == 1)
						{
							fstb::snprintf4all (txt_0, sizeof (txt_0),
								"\t\t_filter_%d.set_apf_1p (%d, %d, a0_arr [%d]);\n",
								comp_mod._coord._group_idx,
								ofs_p1_c,
								comp_mod._coord._lane,
								a0_nbr_p2 * 2
							);
							r._code += txt_0;
						}

						// Merged section
						else
						{
							const int      pos_alt   = pos ^ 1;
							const int      split_alt = comp_mod._comp_arr [pos_alt];
							fstb::snprintf4all (txt_0, sizeof (txt_0),
								"\t\t_filter_%d.set_apf_2p (%d, %d, "
								"a0_arr [%d] * _b0_%d, a0_arr [%d] + _b0_%d);\n",
								comp_mod._coord._group_idx,
								ofs_p2_c + nbr_comp * a0_nbr_p2 + (pos >> 1),
								comp_mod._coord._lane,
								a0_nbr_p2 * 2, split_alt,
								a0_nbr_p2 * 2, split_alt
							);
							r._code += txt_0;
						}
					} // if a0_nbr_p1 > 0
				} // if pos < nbr_comp
			} // for path_cnt
		} // for split_c_idx

		r._code += "\t\tbreak;\n";
	}

	r._code += "\t}\n}\n\n";

	return r;
}



SplitMultibandSimdGen::Result	SplitMultibandSimdGen::generate_filter_proc () const
{
	Result         r;

	const std::string fncname = "process_sample";

	char           txt_0 [1023+1];
	fstb::snprintf4all (txt_0, sizeof (txt_0),
		"%s (float x) noexcept", fncname.c_str ()
	);
	const std::string fncarg = txt_0;

	r._decl  = "\tinline void    " + fncarg + ";\n";

	r._cinl  = "void\t" + _classname + "::" + fncarg + "\n{\n";
	r._cinl += "\tconst auto v0i = fstb::ToolsSimd::set1_f32 (x);\n";

	const int      nbr_groups = int (_simd_group_arr.size ());

	for (int grp_cnt = 0; grp_cnt < nbr_groups; ++grp_cnt)
	{
		// Prepares the stage input from previous stage outputs
		if (grp_cnt > 0)
		{
			fstb::snprintf4all (txt_0, sizeof (txt_0),
				"\tconst auto v%di = ", grp_cnt
			);
			const std::string vi_beg = txt_0;

			const auto     coord_arr = find_simd_group_inputs (grp_cnt);
			const int      nbr_chn   = int (coord_arr.size ());

			fstb::snprintf4all (txt_0, sizeof (txt_0),
				"v%do", coord_arr [0]._group_idx
			);
			const std::string vo0 = txt_0;

			if (nbr_chn == 1)
			{
				const int      lane = coord_arr [0]._lane_idx;
				if (lane == 0)
				{
					r._cinl +=
						  vi_beg + "fstb::ToolsSimd::deinterleave_f32_lo ("
						+ vo0 + ", " + vo0 + ");\n";
				}
				else if (lane == 1)
				{
					r._cinl +=
						  vi_beg + "fstb::ToolsSimd::deinterleave_f32_hi ("
						+ vo0 + ", " + vo0 + ");\n";
				}
				else
				{
					fstb::snprintf4all (txt_0, sizeof (txt_0),
						"fstb::ToolsSimd::set1_f32 (\n"
						"\t\tfstb::ToolsSimd::Shift <%d>::extract (v%do)\n"
						"\t);\n",
						coord_arr [0]._lane_idx,
						coord_arr [0]._group_idx
					);
					r._cinl += vi_beg + txt_0;
				}
			}

			else if (nbr_chn == 2)
			{
				const int      lane = coord_arr [0]._lane_idx;
				if (   coord_arr [1]._group_idx == coord_arr [0]._group_idx
				    && coord_arr [1]._lane_idx  == lane + _max_nbr_chn)
				{
					assert (coord_arr [0]._lane_idx < _max_nbr_chn);
					if (lane == 0)
					{
						r._cinl +=
							  vi_beg + "fstb::ToolsSimd::deinterleave_f32_lo ("
							+ vo0 + ", " + vo0 + ");\n";
					}
					else
					{
						assert (lane == 1);
						r._cinl +=
							  vi_beg + "fstb::ToolsSimd::deinterleave_f32_hi ("
							+ vo0 + ", " + vo0 + ");\n";
					}
				}

				else
				{
					fstb::snprintf4all (txt_0, sizeof (txt_0),
						"fstb::ToolsSimd::set_2f32_fill (\n"
						"\t\tfstb::ToolsSimd::Shift <%d>::extract (v%do),\n"
						"\t\tfstb::ToolsSimd::Shift <%d>::extract (v%do)\n"
						"\t);\n",
						coord_arr [0]._lane_idx,
						coord_arr [0]._group_idx,
						coord_arr [1]._lane_idx,
						coord_arr [1]._group_idx
					);
					r._cinl += vi_beg + txt_0;
				}
			}

			else
			{
				// Shouldn't happen with _max_nbr_chn == 2
				assert (false);
			}
		}

		fstb::snprintf4all (txt_0, sizeof (txt_0),
			"\tconst auto v%do = _filter_%d.process_sample (v%di);\n",
			grp_cnt, grp_cnt, grp_cnt
		);
		r._cinl += txt_0;
	}

	// Stores band outputs
	std::string    out;
	bool           trash_flag = false;
	for (int grp_cnt = 0; grp_cnt < nbr_groups; ++grp_cnt)
	{
		const auto     band_idx_arr { get_band_out (grp_cnt) };
		if (! band_idx_arr.empty ())
		{
			out += "\tfstb::ToolsSimd::explode (\n";
			for (int lane = 0; lane < _simd_w; ++lane)
			{
				const int      band_idx = band_idx_arr [lane];
				if (band_idx < 0)
				{
					fstb::snprintf4all (txt_0, sizeof (txt_0),
						"\t\ttrash [%d],\n", lane
					);
					trash_flag = true;
				}
				else
				{
					fstb::snprintf4all (txt_0, sizeof (txt_0),
						"\t\t*(_out_ptr_arr [%d]),\n", band_idx
					);
				}
				out += txt_0;
			}
			fstb::snprintf4all (txt_0, sizeof (txt_0),
				"\t\tv%do\n\t);\n", grp_cnt
			);
			out += txt_0;
		}
	}
	if (trash_flag)
	{
		fstb::snprintf4all (txt_0, sizeof (txt_0),
			"\talignas (%d) std::array <float, %d> trash;\n",
			_simd_w * sizeof (float),
			_simd_w
		);
		r._cinl += txt_0;
	}
	r._cinl += out;

	r._cinl += "}\n\n";

	return r;
}



std::string	SplitMultibandSimdGen::add_namespaces (const std::string &s) const
{
	if (_namespaces.empty ())
	{
		return s;
	}

	std::string    pre;
	std::string    post;
	for (const auto &ns : _namespaces)
	{
		pre += "namespace " + ns + " {\n";
		post = "} // namespace " + ns + "\n" + post;
	}

	return pre + "\n" + s + post + "\n";
}



// Cannot be called on the root SIMD group (index 0)
SplitMultibandSimdGen::GroupOutArray	SplitMultibandSimdGen::find_simd_group_inputs (int grp_idx) const
{
	assert (grp_idx > 0); // not >=
	assert (grp_idx < int (_simd_group_arr.size ()));

	GroupOutArray  coord_arr;

	const int      nbr_groups = int (_simd_group_arr.size ());
	const auto &   grp_dst    = _simd_group_arr [grp_idx];
	for (const SimdPath &path : grp_dst._path_list)
	{
		const auto &   split = _split_arr [path._split_idx];

		// Finds the node index in the group outputs
		CoordGroupOut  coord;
		for (int grp_idx_tst = 0; grp_idx_tst < nbr_groups; ++grp_idx_tst)
		{
			const SimdGroup & grp_src = _simd_group_arr [grp_idx_tst];
			const auto     it = std::find (
				grp_src._node_out_arr.begin (),
				grp_src._node_out_arr.end (),
				split._node_idx
			);
			if (it != grp_src._node_out_arr.end ())
			{
				coord._group_idx = grp_idx_tst;
				coord._lane_idx  =
					int (std::distance (grp_src._node_out_arr.begin (), it));
				break;
			}
		}
		assert (coord._group_idx >= 0);

		coord_arr.push_back (coord);
	}

	return coord_arr;
}



// If there is no band output on this group, the vector is empty
// Otherwise, each element contains the band index or -1 if it does not
// feed a band.
std::vector <int>	SplitMultibandSimdGen::get_band_out (int grp_idx) const
{
	assert (grp_idx >= 0);
	assert (grp_idx < int (_simd_group_arr.size ()));

	const auto &   group = _simd_group_arr [grp_idx];

	bool           band_flag = false;
	std::vector <int> band_idx_arr (_simd_w, -1);
	for (int lane = 0; lane < _simd_w; ++lane)
	{
		const int      node_idx = group._node_out_arr [lane];
		if (node_idx >= 0)
		{
			const auto &   node = _node_arr [node_idx];
			if (node._type == Node::Type::BAND)
			{
				band_idx_arr [lane] = node._idx;
				band_flag           = true;
			}
		}
	}

	if (! band_flag)
	{
		band_idx_arr.clear ();
	}

	return band_idx_arr;
}



std::string	SplitMultibandSimdGen::get_autogen_header ()
{
	return "// Code automatically generated.\n\n";
}



std::string	SplitMultibandSimdGen::generate_include ()
{
	return
"#pragma once\n"
"\n"
"#include <algorithm>\n"
"#include <iterator>\n"
#if 1
"#include \"fstb/ToolsSimd.h\"\n\n";
#else
// Stand-alone x86/SSE2 version.
// Mainly for checking the result with Complier Explorer.
"#include <cstdint>\n"
"#include <emmintrin.h>\n"
"\n"
"namespace fstb\n"
"{\n"
"class ToolsSimd\n"
"{\n"
"public:\n"
"\ttypedef __m128i VectS32;\n"
"\ttypedef __m128  VectF32;\n"
"\tstatic VectF32 set_f32_zero () noexcept { return _mm_setzero_ps (); }\n"
"\tstatic VectF32 set1_f32 (float a) noexcept { return _mm_set1_ps (a); }\n"
"\tstatic VectF32 set_f32 (float a0, float a1, float a2, float a3) noexcept { return _mm_set_ps (a3, a2, a1, a0); }\n"
"\tstatic VectS32 set_s32 (int32_t a0, int32_t a1, int32_t a2, int32_t a3) noexcept { return _mm_set_epi32 (a3, a2, a1, a0); }\n"
"\tstatic VectF32 cast_f32 (VectS32 x) noexcept { return _mm_castsi128_ps (x); }\n"
"\tstatic VectF32 select (VectF32 cond, VectF32 v_t, VectF32 v_f) noexcept\n"
"\t{\n"
"\t\tconst auto     cond_1 = _mm_and_ps (cond, v_t);\n"
"\t\tconst auto     cond_0 = _mm_andnot_ps (cond, v_f);\n"
"\t\treturn _mm_or_ps (cond_0, cond_1);\n"
"\t}\n"
"\tstatic VectF32 and_f32 (VectF32 lhs, VectF32 rhs) noexcept { return _mm_and_ps (lhs, rhs); }\n"
"\tstatic VectF32 or_f32 (VectF32 lhs, VectF32 rhs) noexcept { return _mm_or_ps (lhs, rhs); }\n"
"\tstatic VectF32 butterfly_f32_w64 (VectF32 x) noexcept\n"
"\t{\n"
"\t\tconst auto sign = _mm_castsi128_ps (_mm_setr_epi32 (0, 0, 1 << 31, 1 << 31));\n"
"\t\tconst auto x0   = _mm_shuffle_ps (x, x, (2<<0) + (3<<2) + (0<<4) + (1<<6)); // c, d, a, b\n"
"\t\tconst auto x1   = _mm_xor_ps (x, sign); // a, b, -c, -d\n"
"\t\treturn _mm_add_ps (x0, x1);\n"
"\t}\n"
"\tstatic VectF32 deinterleave_f32_lo (VectF32 i0, VectF32 i1) noexcept\n"
"\t{\n"
"\t\treturn _mm_shuffle_ps (i0, i1, (0<<0) | (2<<2) | (0<<4) | (2<<6));\n"
"\t}\n"
"\tstatic VectF32 deinterleave_f32_hi (VectF32 i0, VectF32 i1) noexcept\n"
"\t{\n"
"\t\treturn _mm_shuffle_ps (i0, i1, (1<<0) | (3<<2) | (1<<4) | (3<<6));\n"
"\t}\n"
"\tstatic void explode (float &a0, float &a1, float &a2, float &a3, VectF32 x) noexcept\n"
"\t{\n"
"\t\tconst auto     tmp = _mm_movehl_ps (x, x);\n"
"\t\ta0 = _mm_cvtss_f32 (x);\n"
"\t\ta2 = _mm_cvtss_f32 (tmp);\n"
"\t\ta1 = _mm_cvtss_f32 (_mm_shuffle_ps (x, x, (1<<0)));\n"
"\t\ta3 = _mm_cvtss_f32 (_mm_shuffle_ps (tmp, tmp, (1<<0)));\n"
"\t}\n"
"\ttemplate <int SHIFT> class Shift\n"
"\t{\n"
"\tpublic:\n"
"\t\tstatic float extract (VectF32 a) noexcept\n"
"\t\t{\n"
"\t\t\tswitch (SHIFT & 3)\n"
"\t\t\t{\n"
"\t\t\tcase 1:  a = _mm_shuffle_ps (a, a, 1); break;\n"
"\t\t\tcase 2:  a = _mm_shuffle_ps (a, a, 2); break;\n"
"\t\t\tcase 3:  a = _mm_shuffle_ps (a, a, 3); break;\n"
"\t\t\tdefault: /* Nothing */                 break;\n"
"\t\t\t}\n"
"\t\t\treturn _mm_cvtss_f32 (a);\n"
"\t\t}\n"
"\t};\n"
"};\n"
"}\n"
"\n"
"#if defined (_MSC_VER)\n"
"fstb::ToolsSimd::VectF32 &\toperator += (fstb::ToolsSimd::VectF32 &lhs, const fstb::ToolsSimd::VectF32 &rhs) noexcept\n"
"{\n"
"\tlhs = _mm_add_ps (lhs, rhs);\n"
"\treturn lhs;\n"
"}\n"
"fstb::ToolsSimd::VectF32 &\toperator -= (fstb::ToolsSimd::VectF32 &lhs, const fstb::ToolsSimd::VectF32 &rhs) noexcept\n"
"{\n"
"\tlhs = _mm_sub_ps (lhs, rhs);\n"
"\treturn lhs;\n"
"}\n"
"fstb::ToolsSimd::VectF32 &\toperator *= (fstb::ToolsSimd::VectF32 &lhs, const fstb::ToolsSimd::VectF32 &rhs) noexcept\n"
"{\n"
"\tlhs = _mm_mul_ps (lhs, rhs);\n"
"\treturn lhs;\n"
"}\n"
"fstb::ToolsSimd::VectF32\toperator + (fstb::ToolsSimd::VectF32 lhs, const fstb::ToolsSimd::VectF32 &rhs) noexcept\n"
"{\n"
"\treturn _mm_add_ps (lhs, rhs);\n"
"}\n"
"fstb::ToolsSimd::VectF32\toperator - (fstb::ToolsSimd::VectF32 lhs, const fstb::ToolsSimd::VectF32 &rhs) noexcept\n"
"{\n"
"\treturn _mm_sub_ps (lhs, rhs);\n"
"}\n"
"fstb::ToolsSimd::VectF32\toperator * (fstb::ToolsSimd::VectF32 lhs, const fstb::ToolsSimd::VectF32 &rhs) noexcept\n"
"{\n"
"\treturn _mm_mul_ps (lhs, rhs);\n"
"}\n"
"#endif\n"
"\n";
#endif
}



// Turns a filter spec list into code
SplitMultibandSimdGen::Result	SplitMultibandSimdGen::generate_code (const ClassSpec &spec_list, const std::string &class_scope)
{
	std::string    pub_code; // Code of public functions
	std::string    pub_decl; // Public function declarations
	std::string    prv_decl; // Private declarations
	std::string    m_code;   // Main function code

	// Names of the z^-1 and z^-2 filter states, as member arrays
	const std::string n_z1      = "_z1";
	const std::string n_z2      = "_z2";

	// Names of the all-pass coefficients, as member arrays
	const std::string n_ap1     = "_ap1"; // b0 for the 1-pole APF
	const std::string n_ap2_arr = "_ap2"; // b0 and b1 for the 2-pole APF

	int            z1      = 0; // Number of z^-1 memories
	int            z2      = 0; // Number of z^-2 memories
	int            nbr_ap1 = 0; // Number of 1-pole allpass filters
	int            nbr_ap2 = 0; // Number of 2-pole allpass filters

	// An intermediate filter state has been saved for later recombination
	bool           save_flag = false;

	// z^-1 and z^-2 memories have been declared within the function scope
	bool           x1z_flag  = false;
	bool           x2z_flag  = false;

	// current z^-1 and z^-2 memories contain something meaningful (as output
	// of the previous stage and input of the next stage)
	bool           x1z_valid_flag = false;
	bool           x2z_valid_flag = false;

	const std::string fnc_name_arg =
		"process_sample (fstb::ToolsSimd::VectF32 x) noexcept";

	pub_decl += "\t\tinline fstb::ToolsSimd::VectF32 " + fnc_name_arg + ";\n";

	m_code +=
		  "fstb::ToolsSimd::VectF32\t" + class_scope + spec_list._classname
		+ "::" + fnc_name_arg + "\n{\n";

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

	char           txt_0 [1023+1];

	const int      nbr_spec = int (spec_list._spec_arr.size ());
	for (int spec_cnt = 0; spec_cnt < nbr_spec; ++spec_cnt)
	{
	
	const auto &   spec = spec_list._spec_arr [spec_cnt];

	if (nbr_spec > 1)
	{
		if (spec_cnt > 0)
		{
			m_code += "\n";
		}
		fstb::snprintf4all (txt_0, sizeof (txt_0),
			"\t// Stage %d\n", spec_cnt
		);
		m_code += txt_0;
	}

	// Collects global features
	const bool     gain_flag = spec.has_gain ();

	const float    gain_min =
		*(std::min_element (spec._gain_arr.begin (), spec._gain_arr.end ()));
	const float    gain_max =
		*(std::max_element (spec._gain_arr.begin (), spec._gain_arr.end ()));
	const bool     same_gain_flag = (gain_flag && gain_min == gain_max);

	const int      nbr_p2_min =
		*(std::min_element (spec._nbr_p2.begin (), spec._nbr_p2.end ()));
	const int      nbr_p2_max =
		*(std::max_element (spec._nbr_p2.begin (), spec._nbr_p2.end ()));
	const bool     p2_flag     = (nbr_p2_max > 0);
	const bool     sel_p2_flag = (nbr_p2_min < nbr_p2_max);

	const bool     skip_1p_flag = (std::find (
		spec._skip_p1.begin (), spec._skip_p1.end (), true
	) != spec._skip_p1.end ());

	const bool     bf_flag = (std::find (
		spec._butterfly.begin (), spec._butterfly.end (), true
	) != spec._butterfly.end ());
	const bool     bypass_bf_flag = (bf_flag && std::find (
		spec._butterfly.begin (), spec._butterfly.end (), false
	) != spec._butterfly.end ());

	if ((p2_flag | spec._p1_flag) & ! x1z_flag)
	{
		m_code += "\tauto x1z = " + n_z1 + " [0];\n";
		x1z_flag       = true;
		x1z_valid_flag = true;
	}
	if (p2_flag && ! x2z_flag)
	{
		m_code += "\tauto x2z = " + n_z2 + " [0];\n";
		x2z_flag       = true;
		x2z_valid_flag = true;
	}
	if ((sel_p2_flag || skip_1p_flag || bf_flag) && ! save_flag)
	{
		m_code += "\tfstb::ToolsSimd::VectF32 mask;\n";
		m_code += "\tfstb::ToolsSimd::VectF32 x_save;\n";
		save_flag = true;
	}

	// Gain
	if (gain_flag)
	{
		if (same_gain_flag)
		{
			fstb::snprintf4all (txt_0, sizeof (txt_0),
				"set1_f32 (%#gf);\n", spec._gain_arr [0]
			);
		}
		else
		{
			fstb::snprintf4all (txt_0, sizeof (txt_0),
				"set_f32 (%#gf, %#gf, %#gf, %#gf);\n",
				spec._gain_arr [0], spec._gain_arr [1],
				spec._gain_arr [0], spec._gain_arr [1]
			);
		}
		m_code += "\t{\n";
		m_code += "\t\tconst auto gain = fstb::ToolsSimd::";
		m_code += txt_0;
		m_code += "\t\tx *= gain;\n";
		m_code += "\t}\n";
	}

	// 2-pole sections
	if (p2_flag)
	{
		// Number of saved 2-pole states, for later selection
		int            nbr_save_p2   = 0;
		int            mask_save_tot = 0;
		std::array <int, _simd_w - 1> mask_save_p2 { };

		for (int p2_idx = 0; p2_idx < nbr_p2_max; ++p2_idx)
		{
			// Check if we have to save and mask the intermediate state
			for (int path = 0; path < _simd_w; ++path)
			{
				if (spec._nbr_p2 [path] == p2_idx)
				{
					mask_save_p2 [nbr_save_p2] |= 1 << path;
				}
			}
			if (mask_save_p2 [nbr_save_p2] != 0)
			{
				assert (sel_p2_flag);

				const std::string e_mask = build_mask (mask_save_p2 [nbr_save_p2]);
				if (nbr_save_p2 == 0)
				{
					m_code += "\tx_save = x;\n";
					m_code += "\tmask   = " + e_mask + ";\n";
				}
				else
				{
					if (nbr_save_p2 == 1)
					{
						m_code +=
							  "\tx_save = fstb::ToolsSimd::and_f32 (mask, x_save);\n";
					}
					m_code += "\tmask   = " + e_mask + ";\n";
					m_code +=
						"\tx_save = fstb::ToolsSimd::or_f32 (\n"
						"\t\tx_save, fstb::ToolsSimd::and_f32 (mask, x)\n"
						"\t);\n";
				}

				mask_save_tot |= mask_save_p2 [nbr_save_p2];
				++ nbr_save_p2;
			}

			// Filtering
			if (! x1z_valid_flag)
			{
				m_code += "\tx1z = " + use_arr_elt (n_z1, z1) + ";\n";
				x1z_valid_flag = true;
			}
			if (! x2z_valid_flag)
			{
				m_code += "\tx2z = " + use_arr_elt (n_z2, z2) + ";\n";
				x2z_valid_flag = true;
			}
			const auto     n_ap2 = use_arr_elt (n_ap2_arr, nbr_ap2);
			const auto     n_b0  = n_ap2 + " [0]";
			const auto     n_b1  = n_ap2 + " [1]";
			const auto     n_x1z = add_arr_elt (n_z1, z1);
			const auto     n_x2z = add_arr_elt (n_z2, z2);
			const auto     n_y1z = use_arr_elt (n_z1, z1);
			const auto     n_y2z = use_arr_elt (n_z2, z2);
			m_code += "\t{\n";
			m_code += "\t\tconst auto b0  = " + n_b0 + ";\n";
			m_code += "\t\tconst auto b1  = " + n_b1 + ";\n";
			m_code += "\t\tconst auto y1z = " + n_y1z + ";\n";
			m_code += "\t\tconst auto y2z = " + n_y2z + ";\n";
			m_code += "\t\tconst auto y   = b0 * (x - y2z) + b1 * (x1z - y1z) + x2z;\n";
			m_code += "\t\t" + n_x1z + " = x;\n";
			m_code += "\t\t" + n_x2z + " = x1z;\n";
			m_code += "\t\tx   = y;\n";
			m_code += "\t\tx1z = y1z;\n";
			m_code += "\t\tx2z = y2z;\n";
			m_code += "\t}\n";

			++ nbr_ap2;
		}

		// Output selection
		if (sel_p2_flag)
		{
			const auto     n_y1z = add_arr_elt (n_z1, z1);
			const auto     n_y2z = add_arr_elt (n_z2, z2);
			m_code += "\t" + n_y1z + " = x;\n";
			m_code += "\t" + n_y2z + " = x1z;\n";
			x1z_valid_flag = false;
			x2z_valid_flag = false;

			if (nbr_save_p2 == 1)
			{
				m_code += "\tx = fstb::ToolsSimd::select (mask, x_save, x);\n";
			}
			else
			{
				const std::string e_mask = build_mask (~mask_save_tot);
				m_code += "\tmask   = " + e_mask + ";\n";
				m_code +=
					"\tx = fstb::ToolsSimd::or_f32 (\n"
					"\t\tx_save, fstb::ToolsSimd::and_f32 (mask, x)\n"
					"\t);\n";
			}
		}
	}

	if (x2z_valid_flag)
	{
		const auto     n_y2z = add_arr_elt (n_z2, z2);
		m_code += "\t" + n_y2z + " = x1z;\n";
		x2z_valid_flag = false;
	}

	// 1-pole section
	if (spec._p1_flag)
	{
		if (skip_1p_flag)
		{
			m_code += "\tx_save = x;\n";
		}

		{
			if (! x1z_valid_flag)
			{
				m_code += "\tx1z = " + use_arr_elt (n_z1, z1) + ";\n";
				x1z_valid_flag = true;
			}

			const std::string n_b0  = use_arr_elt (n_ap1, nbr_ap1);

			const std::string n_x1z = add_arr_elt (n_z1, z1);
			const std::string n_y1z = use_arr_elt (n_z1, z1);
			m_code += "\t{\n";
			m_code += "\t\tconst auto b0  = " + n_b0 + ";\n";
			m_code += "\t\tconst auto y1z = " + n_y1z + ";\n";
			m_code += "\t\tconst auto y   = b0 * (x - y1z) + x1z;\n";
			m_code += "\t\t" + n_x1z + " = x;\n";
			m_code += "\t\tx   = y;\n";
			m_code += "\t\tx1z = y1z;\n";
			m_code += "\t}\n";
		}

		if (skip_1p_flag)
		{
			const auto     n_y1z = add_arr_elt (n_z1, z1);
			m_code += "\t" + n_y1z + " = x;\n";
			x1z_valid_flag = false;

			int         mask_save_p1 = 0;
			for (int path = 0; path < _simd_w; ++path)
			{
				if (spec._skip_p1 [path])
				{
					mask_save_p1 |= 1 << path;
				}
			}
			const std::string e_mask = build_mask (mask_save_p1);
			m_code += "\tmask = " + e_mask + ";\n";
			m_code += "\tx    = fstb::ToolsSimd::select (mask, x_save, x);\n";
		}

		++ nbr_ap1;
	}

	if (x1z_valid_flag)
	{
		const auto     n_y1z = add_arr_elt (n_z1, z1);
		m_code += "\t" + n_y1z + " = x;\n";
		x1z_valid_flag = false;
	}

	// Butterfly
	if (bf_flag)
	{
		if (bypass_bf_flag)
		{
			m_code += "\tx_save = x;\n";
		}

		m_code += "\tx = fstb::ToolsSimd::butterfly_f32_w64 (x);\n";

		if (bypass_bf_flag)
		{
			int         mask_save_bf = 0;
			for (int chn_cnt = 0; chn_cnt < _max_nbr_chn; ++chn_cnt)
			{
				if (! spec._butterfly [chn_cnt])
				{
					mask_save_bf |= (1 + (1 << _max_nbr_chn)) << chn_cnt;
				}
			}
			const std::string e_mask = build_mask (mask_save_bf);
			m_code += "\tmask = " + e_mask + ";\n";
			m_code += "\tx    = fstb::ToolsSimd::select (mask, x_save, x);\n";
		}
	}

	// Expansion
	if (spec._expand_flag)
	{
		m_code += "\tx = fstb::ToolsSimd::deinterleave_f32_lo (x, x);\n";
	}

	} // for spec

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

	m_code += "\n\treturn x;\n";
	m_code += "}\n\n";

	// Class members
	if (nbr_ap1 > 0)
	{
		// Variables
		prv_decl += "\t\tfstb::ToolsSimd::VectF32 " + use_arr_elt (n_ap1, nbr_ap1) + " {};\n";

		// Setters
		pub_decl += "\t\tvoid set_apf_1p (int idx, int lane, float b0) noexcept;\n";
		pub_code +=
			"void\t" + class_scope + spec_list._classname + "::set_apf_1p (int idx, int lane, float b0) noexcept\n"
			"{\n"
			"\t" + set_scalar (n_ap1 + " [idx]", "lane", "b0") + "\n"
			"}\n\n";
	}
	if (nbr_ap2 > 0)
	{
		// Variables
		const auto     n_ap2 = use_arr_elt (use_arr_elt (n_ap2_arr, nbr_ap2), 2);
		prv_decl += "\t\tfstb::ToolsSimd::VectF32 " + n_ap2 + " {};\n";

		// Setters
		const auto     n_ap2_s = n_ap2_arr + " [idx]";
		pub_decl += "\t\tvoid set_apf_2p (int idx, int lane, float b0, float b1) noexcept;\n";
		pub_code +=
			"void\t" + class_scope + spec_list._classname + "::set_apf_2p (int idx, int lane, float b0, float b1) noexcept\n"
			"{\n"
			"\t" + set_scalar (use_arr_elt (n_ap2_s, 0), "lane", "b0") + "\n"
			"\t" + set_scalar (use_arr_elt (n_ap2_s, 1), "lane", "b1") + "\n"
			"}\n\n";
	}
	pub_decl += "\t\tvoid clear_buffers () noexcept;\n";
	std::string clbuf =
		"void\t" + class_scope + spec_list._classname + "::clear_buffers () noexcept\n"
		"{\n";
	if (z1 > 0)
	{
		prv_decl += "\t\tfstb::ToolsSimd::VectF32 " + use_arr_elt (n_z1 , z1) + " {};\n";
		clbuf    +=
			  "\tstd::fill (std::begin (" + n_z1 + "), std::end ("
			+ n_z1 + "), fstb::ToolsSimd::set_f32_zero ());\n";
	}
	if (z2 > 0)
	{
		prv_decl += "\t\tfstb::ToolsSimd::VectF32 " + use_arr_elt (n_z2 , z2) + " {};\n";
		clbuf    +=
			  "\tstd::fill (std::begin (" + n_z2 + "), std::end ("
			+ n_z2 + "), fstb::ToolsSimd::set_f32_zero ());\n";
	}
	clbuf += "}\n\n";
	pub_code += clbuf;

	// Put the code bits together
	Result         r;
	r._decl =
		  "\tpublic:\n"  + pub_decl
		+ "\tprivate:\n" + prv_decl;
	r._cinl = m_code;
	r._code = pub_code;

	return r;
}



std::string	SplitMultibandSimdGen::add_arr_elt (const std::string &var_name, int &k)
{
	assert (! var_name.empty ());
	assert (k >= 0);

	char           txt_0 [1023+1];
	fstb::snprintf4all (txt_0, sizeof (txt_0), "%s [%d]", var_name.c_str (), k);
	++ k;
	
	return txt_0;
}



std::string	SplitMultibandSimdGen::use_arr_elt (const std::string &var_name, int k)
{
	assert (! var_name.empty ());
	assert (k >= 0);

	char           txt_0 [1023+1];
	fstb::snprintf4all (txt_0, sizeof (txt_0), "%s [%d]", var_name.c_str (), k);

	return txt_0;
}



std::string	SplitMultibandSimdGen::set_scalar (const std::string &var_name, const std::string &lane, const std::string &expr)
{
	assert (! var_name.empty ());
	assert (! lane.empty ());
	assert (! expr.empty ());

	return
		  "(reinterpret_cast <float *> (&" + var_name + ")) [" + lane + "] = "
		+ expr + ";";
}



std::string	SplitMultibandSimdGen::build_mask (int m)
{
	char           txt_0 [1023+1];
	fstb::snprintf4all (txt_0, sizeof (txt_0),
		"fstb::ToolsSimd::cast_f32 (fstb::ToolsSimd::set_s32 (%d, %d, %d, %d))",
		-((m >> 0) & 1),
		-((m >> 1) & 1),
		-((m >> 2) & 1),
		-((m >> 3) & 1)
	);

	return txt_0;
}



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
