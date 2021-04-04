/*****************************************************************************

        SplitMultiband.hpp
        Author: Laurent de Soras, 2021

The splitter tree is kept balanced with all the leafs at level L or L-1.

6 bands, 5 splitters:
Band    | 0   1   2   3   4   5
--------+----------------------
Split 0 |               0
Split 1 |       1       |   4
Split 2 |   2   |   3	|   |

11 bands, 10 splitters:
Band    | 0   1   2   3   4   5   6   7   8   9   A
--------+------------------------------------------
Split 0 |                           0
Split 1 |               1           |       7
Split 2 |       2       |       5   |   8   |   9
Split 3 |   3   |   4   |   6   |   |   |   |   |

The numbers indicate the processing order, in depth-first search order to
minimize the number of required buffers and optimize cache usage.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_iir_SplitMultiband_CODEHEADER_INCLUDED)
#define mfx_dsp_iir_SplitMultiband_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"
#if defined (mfx_dsp_iir_SplitMultiband_TRUE_BLOCK_PROC)
	#include "mfx/dsp/mix/Generic.h"
#endif // mfx_dsp_iir_SplitMultiband_TRUE_BLOCK_PROC

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/




template <typename T, int O0, int O1>
SplitMultiband <T, O0, O1>::SplitMultiband ()
{
#if defined (mfx_dsp_iir_SplitMultiband_TRUE_BLOCK_PROC)
	mix::Generic::setup ();
#endif // mfx_dsp_iir_SplitMultiband_TRUE_BLOCK_PROC
}



/*
==============================================================================
Name: reserve
Description:
	Reserves memory for a given maximum number of bands.
	Once this function is called, set_nbr_band() should be allocation-free
	for any specified number of bands lower or equal to this function
	parameter.
Input parameters:
	- nbr_bands: Maximum number of preallocated bands, >= 2
Throws: std::vector-related exceptions
==============================================================================
*/

template <typename T, int O0, int O1>
void	SplitMultiband <T, O0, O1>::reserve (int nbr_bands)
{
	assert (nbr_bands >= 2);

	const int      nbr_split   = nbr_bands - 1;
	const int      nbr_tmp_buf = calc_nbr_tmp_buf (nbr_bands);

	_split_arr.reserve (nbr_split);
	_comp_arr.reserve (nbr_bands * 2);
	_buf_arr.reserve (nbr_tmp_buf);
	_idx2ord_arr.reserve (nbr_split);
	_node_arr.reserve (nbr_split);
	_band_out_ptr_arr.reserve (nbr_bands);
}



/*
==============================================================================
Name: set_nbr_bands
Description:
	Sets the number of bands and the corresponding output buffers.
	Output buffers are required even for single-sample processing, in which
	case the buffers are 1-sample long.
	Important notes:
	- This is a mandatory call before calling any other function.
	- This function allocates memory so it is not RT-safe. However it is
		possible to preallocate memory with the reserve() function.
	- If the number of bands is changed, there is not signal continuity.
		You should rebuild the crossover filters with as many calls to
		set_splitter_coef() as necessary.
Input parameters:
	- nbr_bands: Number of bands, >= 2
	- band_ptr_arr: array containing pointers on the output buffers for each
		band. It should contain nbr_bands pointers.
Throws: std::vector-related exceptions, if memory is allocated
==============================================================================
*/

template <typename T, int O0, int O1>
void	SplitMultiband <T, O0, O1>::set_nbr_bands (int nbr_bands, T * const band_ptr_arr [])
{
	assert (nbr_bands >= 2);
	assert (band_ptr_arr != nullptr);
	assert (std::find (
		band_ptr_arr, band_ptr_arr + nbr_bands, nullptr
	) == band_ptr_arr + nbr_bands);

	if (nbr_bands != int (_band_out_ptr_arr.size ()))
	{
		// Makes sure the array is filled with nullptr
		_band_out_ptr_arr.assign (nbr_bands, nullptr);

		const int      nbr_split = nbr_bands - 1;
		_split_arr.resize (nbr_split);
		_comp_arr.clear ();

		const int      nbr_tmp_buf = calc_nbr_tmp_buf (nbr_bands);
		_buf_arr.resize (nbr_tmp_buf);

		// Makes sure the array is filled with default-initialised data
		_node_arr.assign (nbr_split, Node ());
		_idx2ord_arr.assign (nbr_split, -1);

		// Initialises the tree structure. Root is index 0
		for (int k = 1; k < nbr_split; ++k)
		{
			const int      src = (k - 1) >> 1;
			const int      chl = (k - 1) &  1;
			_node_arr [src]._children [chl] =
				SplitOut { SplitOut::Type::SPLIT, k };
		}

		// Finds the processing order for all the nodes and finds the
		// output band indexes
		int           split_ord = 0;
		int           band_idx  = 0;
		build_layout_rec (split_ord, band_idx, 0, 0, 0);
		assert (split_ord == nbr_split);
		assert (band_idx == nbr_bands);
		assert (std::count_if (
			_idx2ord_arr.begin (),
			_idx2ord_arr.end (),
			[] (int x) { return (x < 0); }
		) == 0);

		// Builds the compensation filters
		collect_comp_rec (0);

		// Sets the band output buffers
		set_band_ptr (band_ptr_arr);

		// Wipes all the filter states.
		clear_buffers ();
	}
}



/*
==============================================================================
Name: get_nbr_bands
Description:
	Returns the current number of bands.
	It must have been explicitly set before.
Returns: The number of bands, >= 2
Throws: Nothing
==============================================================================
*/

template <typename T, int O0, int O1>
int	SplitMultiband <T, O0, O1>::get_nbr_bands () const noexcept
{
	assert (! _band_out_ptr_arr.empty ());

	return int (_band_out_ptr_arr.size ());
}



/*
==============================================================================
Name: set_splitter_coef
Description:
	Sets the z-equation coefficients for the all-pass filters of a given
	crossover splitter. For consistent results, splitters should be kept sorted
	by ascending cutoff frequencies.
Input parameters:
	- split_idx: splitter index, [0 ; nbr_bands-2]
	- a0_arr: pointer on the coefficients of the first all-pass filter.
		First come the 2nd order coefficients by pairs of { b0, b1 }, if any.
		Then the 1st order coefficient b0, if the filter order is odd.
	- a1_arr: pointer on the coefficients of the second all-pass filter.
		Layout is the same as a0_arr.
Throws: Nothing
==============================================================================
*/

template <typename T, int O0, int O1>
void	SplitMultiband <T, O0, O1>::set_splitter_coef (int split_idx, const T a0_arr [O0], const T a1_arr [O1]) noexcept
{
	assert (split_idx >= 0);
	assert (split_idx < int (_split_arr.size ()));
	assert (a0_arr != nullptr);
	assert (a1_arr != nullptr);

	const int      split_ord = _idx2ord_arr [split_idx];
	auto &         split_cur = _split_arr [split_ord];
	split_cur._ap0.set_coefs (a0_arr);
	split_cur._ap1.set_coefs (a1_arr);
	for (auto &comp : _comp_arr)
	{
		if (comp._split_ord == split_ord)
		{
			comp._apf.set_coefs (a0_arr);
		}
	}
}



/*
==============================================================================
Name: set_band_ptr_one
Description:
	Sets the output buffer for a single band.
Input parameters:
	- band_idx: index of the band in question, [0 ; nbr_bands - 1].
	- out_ptr: Pointer on the output buffer for this band.
Throws: Nothing
==============================================================================
*/

template <typename T, int O0, int O1>
void	SplitMultiband <T, O0, O1>::set_band_ptr_one (int band_idx, T *out_ptr) noexcept
{
	assert (! _split_arr.empty ());
	assert (band_idx >= 0);
	assert (band_idx < int (_band_out_ptr_arr.size ()));
	assert (out_ptr != nullptr);

	auto           ptrptr = _band_out_ptr_arr [band_idx];
	assert (ptrptr != nullptr);
	*ptrptr = out_ptr;
}



/*
==============================================================================
Name: set_band_ptr
Description:
	Sets the output buffers for all the bands.
Input parameters:
	- band_ptr_arr: array containing pointers on the output buffers for each
		band. It should contain nbr_bands pointers.
Throws: Nothing
==============================================================================
*/

template <typename T, int O0, int O1>
void	SplitMultiband <T, O0, O1>::set_band_ptr (T * const band_ptr_arr []) noexcept
{
	assert (! _split_arr.empty ());
	const int      nbr_bands = int (_band_out_ptr_arr.size ());
	assert (band_ptr_arr != nullptr);
	assert (std::find (
		band_ptr_arr, band_ptr_arr + nbr_bands, nullptr
	) == band_ptr_arr + nbr_bands);

	for (int band_idx = 0; band_idx < nbr_bands; ++band_idx)
	{
		auto           ptrptr = _band_out_ptr_arr [band_idx];
		assert (ptrptr != nullptr);
		*ptrptr = band_ptr_arr [band_idx];
	}
}



/*
==============================================================================
Name: offset_band_ptr
Description:
	Adds a value to the pointers of all the band output buffers.
	Important: the offset is measured in T, not bytes.
Input parameters:
	- offset: value to add to the pointers
Throws: Nothing
==============================================================================
*/

template <typename T, int O0, int O1>
void	SplitMultiband <T, O0, O1>::offset_band_ptr (ptrdiff_t offset) noexcept
{
	assert (! _split_arr.empty ());

	const int      nbr_bands = int (_band_out_ptr_arr.size ());
	for (int band_idx = 0; band_idx < nbr_bands; ++band_idx)
	{
		auto           ptrptr = _band_out_ptr_arr [band_idx];
		assert (ptrptr != nullptr);
		*ptrptr += offset;
	}
}



/*
==============================================================================
Name: clear_buffers
Description:
	Clears all the filter states.
Throws: Nothing
==============================================================================
*/

template <typename T, int O0, int O1>
void	SplitMultiband <T, O0, O1>::clear_buffers () noexcept
{
	for (auto &split : _split_arr)
	{
		split._ap0.clear_buffers ();
		split._ap1.clear_buffers ();
	}

	for (auto &comp : _comp_arr)
	{
		comp._apf.clear_buffers ();
	}
}



/*
==============================================================================
Name: process_sample
Description:
	Splits a single sample into several bands. The output samples are stored
	at the beginning of the band output buffers.
Input parameters:
	- x: sample to process.
Throws: Nothing
==============================================================================
*/

template <typename T, int O0, int O1>
void	SplitMultiband <T, O0, O1>::process_sample (T x) noexcept
{
	assert (! _split_arr.empty ());

	_split_arr [0]._src_ptr = &x;

	for (auto &split : _split_arr)
	{
		auto           src_ptr = split._src_ptr;
		auto           lo_ptr  = split._dst_ptr_arr [0];
		auto           hi_ptr  = split._dst_ptr_arr [1];
		auto           v       = *src_ptr;

		// Band splitting
		v *= 0.5f;
		const auto     v0 = split._ap0.process_sample (v);
		const auto     v1 = split._ap1.process_sample (v);
		auto           lo = v0 + v1;
		auto           hi = v0 - v1;

		// Phase compensation
		for (int k = split._comp_ref_arr [0]._beg
		;	k < split._comp_ref_arr [0]._end
		;	++ k)
		{
			lo = _comp_arr [k]._apf.process_sample (lo);
		}
		for (int k = split._comp_ref_arr [1]._beg
		;	k < split._comp_ref_arr [1]._end
		;	++ k)
		{
			hi = _comp_arr [k]._apf.process_sample (hi);
		}

		*lo_ptr = lo;
		*hi_ptr = hi;
	}
}



/*
==============================================================================
Name: process_block
Description:
	Splits a block of samples into several bands. The output blocks are stored
	in the band output buffers; make sure they are large enough.
Input parameters:
	- src_ptr: Pointer on the input samples.
	- nbr_spl: Number of samples to process, > 0.
Throws: Nothing
==============================================================================
*/

template <typename T, int O0, int O1>
void	SplitMultiband <T, O0, O1>::process_block (const T src_ptr [], int nbr_spl) noexcept
{
	assert (! _split_arr.empty ());
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

#if defined (mfx_dsp_iir_SplitMultiband_TRUE_BLOCK_PROC)

	// Slow version kept for reference
	auto           tmp_ptr  = _buf_arr.back ().data ();
	int            pos      = 0;
	do
	{
		const int      work_len = std::min (nbr_spl - pos, _max_buf_size);
		_split_arr [0]._src_ptr = src_ptr + pos;

		for (auto &split : _split_arr)
		{
			auto           lo_ptr  = split._dst_ptr_arr [0];
			auto           hi_ptr  = split._dst_ptr_arr [1];

			// Band splitting
			mix::Generic::copy_1_1_v (tmp_ptr, split._src_ptr, work_len, 0.5f);
			split._ap0.process_block (lo_ptr, tmp_ptr, work_len);
			split._ap1.process_block (hi_ptr, tmp_ptr, work_len);
			mix::Generic::add_sub_ip_2_2 (lo_ptr, hi_ptr, work_len);

			// Phase compensation
			for (int k = split._comp_ref_arr [0]._beg
			;	k < split._comp_ref_arr [0]._end
			;	++ k)
			{
				_comp_arr [k]._apf.process_block (lo_ptr, lo_ptr, work_len);
			}
			for (int k = split._comp_ref_arr [1]._beg
			;	k < split._comp_ref_arr [1]._end
			;	++ k)
			{
				_comp_arr [k]._apf.process_block (hi_ptr, hi_ptr, work_len);
			}
		}

		// Next sub-block
		const int      pos_next = pos + work_len;
		if (pos_next < nbr_spl)
		{
			offset_band_ptr (work_len);
		}
		else if (nbr_spl > _max_buf_size && pos > 0)
		{
			offset_band_ptr (-pos);
		}
		pos = pos_next;
	}
	while (pos < nbr_spl);

#else // mfx_dsp_iir_SplitMultiband_TRUE_BLOCK_PROC

	// This version is actually faster than the true block processing
	process_sample (src_ptr [0]);
	for (int pos = 1; pos < nbr_spl; ++pos)
	{
		offset_band_ptr (1);
		process_sample (src_ptr [pos]);
	}
	offset_band_ptr (1 - nbr_spl);

#endif // mfx_dsp_iir_SplitMultiband_TRUE_BLOCK_PROC
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// split_ord: order index of the current split
// band_idx: index of the last processed band
// node_idx: index of the node in dat._node_arr
// src_ptr: source buffer, can be nullptr for the first node
template <typename T, int O0, int O1>
void	SplitMultiband <T, O0, O1>::build_layout_rec (int &split_ord, int &band_idx, int node_idx, int cur_depth, const T *src_ptr)
{
	Node &         node  = _node_arr [node_idx];
	node._ord = split_ord;
	++ split_ord;

	Splitter &     split = _split_arr [node._ord];
	split._src_ptr = src_ptr;

	for (int child_cnt = 0; child_cnt < _nbr_split_out; ++child_cnt)
	{
		SplitOut &     child = node._children [child_cnt];
		if (child._type == SplitOut::Type::SPLIT)
		{
			auto           buf_ptr = _buf_arr [cur_depth * 2 + child_cnt].data ();
			split._dst_ptr_arr [child_cnt] = buf_ptr;
			build_layout_rec (
				split_ord, band_idx, child._idx, cur_depth + 1, buf_ptr
			);
		}
		else
		{
			child._type = SplitOut::Type::BAND;
			child._idx  = band_idx;
			_band_out_ptr_arr [band_idx] = &split._dst_ptr_arr [child_cnt];
			++ band_idx;
		}

		// After the first child branch has been traversed,
		// we know the split index
		if (node._idx < 0)
		{
			assert (band_idx > 0);
			node._idx = band_idx - 1;
			_idx2ord_arr [node._idx] = node._ord;
		}
	}
}



// Returns the order list of all the splitters covered by node_idx
// Assigns recursively the splitter subtrees to their compensation list
// of their sibling
template <typename T, int O0, int O1>
std::vector <int>	SplitMultiband <T, O0, O1>::collect_comp_rec (int node_idx)
{
	std::vector <int> comp_list;

	Node &         node  = _node_arr [node_idx];
	Splitter &     split = _split_arr [node._ord];
	for (int child_cnt = 0; child_cnt < _nbr_split_out; ++child_cnt)
	{
		SplitOut &    child = node._children [child_cnt];
		if (child._type == SplitOut::Type::SPLIT)
		{
			const auto    child_list = collect_comp_rec (child._idx);
			const int     nbr_comp   = int (child_list.size ());
			auto &        comp_ref   = split._comp_ref_arr [1 - child_cnt];
			comp_ref._beg = int (_comp_arr.size ());
			comp_ref._end = comp_ref._beg + nbr_comp;
			_comp_arr.resize (_comp_arr.size () + nbr_comp);
			for (int comp_cnt = 0; comp_cnt < nbr_comp; ++comp_cnt)
			{
				_comp_arr [comp_ref._beg + comp_cnt]._split_ord =
					child_list [comp_cnt];
			}

			comp_list.insert (
				comp_list.end (),
				child_list.begin (), child_list.end ()
			);
		}
	}

	comp_list.push_back (node._ord);

	return comp_list;
}



template <typename T, int O0, int O1>
int	SplitMultiband <T, O0, O1>::calc_nbr_tmp_buf (int nbr_bands)
{
	assert (nbr_bands >= 2);

	// Depth of the split tree
	const int      depth       = fstb::get_next_pow_2 (nbr_bands);

	// Number of required temporary buffers
	// We need 2 output buffers per depth level, excepted for the last depth
	// because the splitter outputs are the final bands.
	// And one more buffer for temporary operations (filter input * 0.5)
	const int      nbr_tmp_buf = _nbr_split_out * (depth - 1) + 1;

	return nbr_tmp_buf;
}



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_iir_SplitMultiband_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
