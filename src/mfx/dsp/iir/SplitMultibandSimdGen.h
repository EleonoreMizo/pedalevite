/*****************************************************************************

        SplitMultibandSimdGen.h
        Author: Laurent de Soras, 2021

This is a code generator to build a SIMD multi-band splitter made of pairs of
all-pass filters.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_iir_SplitMultibandSimdGen_HEADER_INCLUDED)
#define mfx_dsp_iir_SplitMultibandSimdGen_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"

#include <array>
#include <initializer_list>
#include <vector>
#include <string>



namespace mfx
{
namespace dsp
{
namespace iir
{



class SplitMultibandSimdGen
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	class Result
	{
	public:
		// Declarations (header)
		std::string    _decl;

		// Inline code (header)
		std::string    _cinl;

		// Standard code (compilation unit)
		std::string    _code;
	};

	explicit       SplitMultibandSimdGen (int nbr_bands, int order_a0, int order_a1, const std::string &main_classname, std::initializer_list <std::string> namespaces);

	Result         print_code () const;

	// Debugging function
	std::string    print_state () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	// Number of output for each splitter (convenience constant)
	static constexpr int _nbr_split_out = 2;

	// SIMD vector width
	static constexpr int _simd_w = 4;

	// Maximum number of channels per SIMD vector
	static constexpr int _max_nbr_chn = _simd_w / _nbr_split_out;

	/*
	Low-level building block structure for filtering.
	Data is arranged in channels.
	Each channel occupies 2 lanes (for the A0/A1 filters).
	Both lanes are spaced by half a vector width.
	Operation order is:
	- Gain, per channel (*** To do: per lane ***)
	- 2-pole sections
	- 1-pole section
	- Butterfly operation { A0 + A1, A0 - A1 }
	- Channel expansion
	*/
	class FilterSpec
	{
	public:
		int            get_p2_len () const noexcept;
		int            get_p1_len () const noexcept;
		bool           has_gain () const noexcept;
		bool           operator == (const FilterSpec &other) const noexcept;

		// Gain for each channel
		std::array <float, _max_nbr_chn>
		               _gain_arr { fstb::make_array <_max_nbr_chn, float> (1.f) };

		// Number of 2-pole sections for each path.
		std::array <int, _simd_w>
		               _nbr_p2 { };

		// If there is a 1-pole section
		bool           _p1_flag = false;

		// Paths skipping the 1-pole section (if any)
		std::array <bool, _simd_w>
		               _skip_p1 { };

		// Indicates if there is a butterfly operation on the channel
		std::array <bool, _max_nbr_chn>
		               _butterfly { };

		// Indicates if the A0/A1 pair of the lower channel(s) are expanded to
		// full channel pairs (for example: 1-channel split output to 2-channel
		// split input)
		bool           _expand_flag = false;
	};
	typedef std::vector <FilterSpec> SpecArray;

	// Chain of several filtering building blocks
	// This is directly turned into a C++ class (data structure + code).
	class ClassSpec
	{
	public:
		int            get_p2_ofs (int spec_idx) const noexcept;
		int            get_p1_ofs (int spec_idx) const noexcept;

		// Unique name for the generated class
		std::string    _classname;

		SpecArray      _spec_arr;
	};

	// From a single source channel
	class SimdPath
	{
	public:
		// Root crossover
		int            _split_idx = -1;

		// Number of depth levels covered by this filter chain.
		// 1 = one split/comp. -1 = not set
		int            _depth     = -1;

		// Number of outputs rounded to the next power of 2, >= 2. -1 = not set
		int            _footprint = -1;
	};

	// Description of a SIMD group, generating several crossover filters
	// Contains temporary data for optimisations and code generation.
	class SimdGroup
	{
	public:
		int            compute_footprint () const noexcept;
		int            compute_max_depth () const noexcept;

		std::vector <SimdPath>
		               _path_list;

		// List of the filter chain specs for the group
		ClassSpec      _spec;

		// Node indexes of each output scalar.
		// -1: unused output
		std::array <int, _simd_w>
		               _node_out_arr { fstb::make_array <_simd_w, int> (-1) };

		// Splitter indexes for all the channels of every level covered by
		// the group
		std::vector <std::array <int, _max_nbr_chn> >
		               _proc_map;

		// Indicates for each level, that at least one channel output is a final
		// band
		std::vector <bool>
		               _band_flag_arr;

		// Indicates that all compensation filters of the penultimate level
		// are moved into the splitters of the last level.
		bool           _opt_comp_flag = false;
	};
	typedef std::vector <SimdGroup> SimdGroupArray;

	// Coordinates to locate filter coeffients
	class FilterCoord
	{
	public:
		// SIMD group index. -1 = not set
		int            _group_idx = -1;

		// SIMD lane. -1 = not set
		// Unique for compensation filters, A0 for the crossover filters (A1
		// lane can be deduced trivially).
		int            _lane      = -1;

		// Spec index within the spec list. -1 = not set
		int            _spec_pos  = -1;
	};

	// Simple node for the tree describing the crossover structure.
	// The crossovers are nodes with children and the final bands are
	// leaf nodes.
	class Node
	{
	public:
		enum class Type
		{
			SPLIT = 0,
			BAND         // A BAND node is implicitely a leaf
		};

		Type           _type  = Type::BAND;

		// Splitter index or band index. < 0 = not set
		int            _idx   = -1;

		// Node depth level within the tree. 0 = root. -1 = not set
		int            _level = -1;

		// Node index or -1 if no child
		std::array <int, 2>
		               _children { -1, -1 };
	};
	typedef std::vector <Node> NodeArray;

	// Indexes of the splitters used for the compensation of a single path
	typedef std::vector <int> CompArray;

	// A compensation path with the coordinates of its coefficients in the
	// SIMD groups
	class CompModule
	{
	public:
		CompArray      _comp_arr;
		FilterCoord    _coord;
	};

	class Splitter
	{
	public:
		typedef std::array <CompModule, _nbr_split_out> PathArray;

		// Corresponding node index. -1 = not set.
		int            _node_idx   = -1;

		// Corrective gain for the all-pass sum. Actually -log2 (gain).
		// The gains of several crossovers may be grouped together.
		int            _gain_l2    = 1;

		// Compensation filters for both paths
		PathArray      _path_arr;

		// Direct coordinates for the filter
		FilterCoord    _coord;

		// We need to store the b0 coefficient for the 1-pole part of the filter,
		// because it is combined with other 1-pole filters to form a 2-pole
		// filter
		bool           _p1_b0_flag = false;
	};
	typedef std::vector <Splitter> SplitterArray;

	// Node indexes for a given level
	typedef std::vector <int> LevelContent;

	// Node indexes for all the levels
	typedef std::vector <LevelContent> LevelArray;

	// Coordinates of a SIMD group output
	class CoordGroupOut
	{
	public:
		int            _group_idx = -1;
		int            _lane_idx  = -1;
	};
	typedef std::vector <CoordGroupOut> GroupOutArray;

	void           build_xover_tree ();
	void           build_xover_tree_rec (int node_idx, int band_beg, int band_end, int level);
	CompArray      find_comp_filters_rec (int node_idx);
	int            factorize_gain_rec (int node_idx);
	void           build_simd_group_array ();
	int            get_node_descent_depth (int node_idx) const;
	void           erase_subtree (LevelArray &rem_nodes, int node_idx, int depth);
	void           create_group_proc_map (int group_idx);
	void           optimize_group_comp (int group_idx);
	void           optimize_group_gain (int group_idx);
	void           create_group_spec (int group_idx);
	Result         generate_main () const;
	Result         generate_subfilters () const;
	Result         generate_filter_setup () const;
	Result         generate_filter_proc () const;
	std::string    add_namespaces (const std::string &s) const;
	GroupOutArray  find_simd_group_inputs (int grp_idx) const;
	std::vector <int>
	               get_band_out (int grp_idx) const;

	static std::string
	               get_autogen_header ();
	static std::string
	               generate_include ();
	static Result  generate_code (const ClassSpec &spec_list, const std::string &class_scope);
	static std::string
	               add_arr_elt (const std::string &var_name, int &k);
	static std::string
	               use_arr_elt (const std::string &var_name, int k);
	static std::string
	               set_scalar (const std::string &var_name, const std::string &lane, const std::string &expr);
	static std::string
	               build_mask (int m);

	const int      _nbr_bands;
	const int      _nbr_split; // _nbr_bands - 1
	const int      _order_a0;
	const int      _order_a1;
	const std::string
	               _classname;
	const std::vector <std::string>
	               _namespaces;
	NodeArray      _node_arr;
	SplitterArray  _split_arr;
	SimdGroupArray _simd_group_arr;
	LevelArray     _lvl_arr;
	Result         _r;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

#if 0
	               SplitMultibandSimdGen ()                               = delete;
	               SplitMultibandSimdGen (const SplitMultibandSimdGen &other) = delete;
	               SplitMultibandSimdGen (SplitMultibandSimdGen &&other)      = delete;
	SplitMultibandSimdGen &
	               operator = (const SplitMultibandSimdGen &other)        = delete;
	SplitMultibandSimdGen &
	               operator = (SplitMultibandSimdGen &&other)             = delete;
#endif
	bool           operator == (const SplitMultibandSimdGen &other) const = delete;
	bool           operator != (const SplitMultibandSimdGen &other) const = delete;

}; // class SplitMultibandSimdGen



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/iir/SplitMultibandSimdGen.hpp"



#endif   // mfx_dsp_iir_SplitMultibandSimdGen_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
