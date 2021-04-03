/*****************************************************************************

        SplitMultiband.h
        Author: Laurent de Soras, 2021

This class uses allpass filter pairs as split filters and phase compensation
filters.
LPF  = (A0 + A1) * 0.5
HPF  = (A0 - A1) * 0.5
comp =  A0
Filter design is up to the class user. It is recommended to set the lightest
filter on A0, because it is used for phase compensation too.

Template parameters:

- T: processed data type (floating point)

- O0, O1: order of the A0 and A1 allpass filters, > 0.

Reference:

Alexis Favrot, Christof Faller,
Complementary N-Band IIR Filterbank Based on 2-Band Complementary Filters,
International Workshop on Acoustic Signal Enhancement, 2010

Filter design:

Sanjit K. Mitra,
Digital Signal Processing: A Computer-Based Approach, 2nd edition,
Chapter 6.10: Parallel Allpass Realization of IIR Transfer Functions,
McGraw-Hill, 2001

Ivan W. Selesnick,
Lowpass Filters Realizable As All-Pass Sums: Design Via New Flat Delay Filter,
IEEE Transactions on Circuits and Systems II: Analog and Digital Signal
Processing, volume 46, Issue 1, 1999-01

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_iir_SplitMultiband_HEADER_INCLUDED)
#define mfx_dsp_iir_SplitMultiband_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/iir/AllPassCascade.h"

#include <vector>

#include <cstddef>



namespace mfx
{
namespace dsp
{
namespace iir
{



template <typename T, int O0, int O1>
class SplitMultiband
{
	static_assert ((O0 > 0), "Filter order O0 must be > 0");
	static_assert ((O1 > 0), "Filter order O1 must be > 0");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef T DataType;
	typedef SplitMultiband <T, O0, O1> ThisType;

	               SplitMultiband ()                       = default;
	               SplitMultiband (SplitMultiband &&other) = default;
	SplitMultiband &
	               operator = (SplitMultiband &&other)     = default;

	void           set_nbr_bands (int nbr_bands, T * const band_ptr_arr []);

	void           set_splitter_coef (int split_idx, const T a0_arr [O0], const T a1_arr [O1]) noexcept;
	void           set_band_ptr_one (int band_idx, T *out_ptr) noexcept;
	void           set_band_ptr (T * const band_ptr_arr []) noexcept;
	void           offset_band_ptr (ptrdiff_t offset) noexcept;

	void           clear_buffers () noexcept;
	inline void    process_sample (T x) noexcept;
	void           process_block (const T src_ptr [], int nbr_spl) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	// Internal buffer size for block processing
	static constexpr int _max_buf_size  = 64;

	// Number of output for each splitter (convenience constant)
	static constexpr int _nbr_split_out = 2;

	// Number of 2-pole and 1-pole sections for each all-pass filter
	static constexpr int _nbr_2p_0 = O0 / 2;
	static constexpr int _nbr_1p_0 = O0 & 1;
	static constexpr int _nbr_2p_1 = O1 / 2;
	static constexpr int _nbr_1p_1 = O1 & 1;

	typedef AllPassCascade <T, _nbr_2p_0, _nbr_1p_0> ApfChain0;
	typedef AllPassCascade <T, _nbr_2p_1, _nbr_1p_1> ApfChain1;

	class Comp
	{
	public:
		ApfChain0      _apf;

		// Order of the splitter whose phase (_ap0) is taken from.
		int            _split_ord = 0;
	};
	typedef std::vector <Comp> CompArray;

	class Splitter
	{
	public:
		ApfChain0      _ap0;
		ApfChain1      _ap1;
		std::array <CompArray, _nbr_split_out>
		               _comp_arr;

		// Source buffer for the splitter
		const T *     _src_ptr = nullptr;

		// Destination buffers. 0 = low, 1 = high
		std::array <T *, _nbr_split_out>
		               _dst_ptr_arr { nullptr, nullptr };
	};
	typedef std::vector <Splitter> SplitterArray;

	// Type of a splitter output (another splitter or band)
	class SplitOut
	{
	public:
		enum class Type
		{
			UNDEF = -1,
			SPLIT,
			BAND
		};
		Type           _type = Type::UNDEF;
		int            _idx  = -1; // Index (node or band). -1 = not set
	};

	class Node
	{
	public:
		// Splitter order and index
		int            _ord = -1;
		int            _idx = -1;

		// Indexes in NodeArray
		std::array <SplitOut, _nbr_split_out>
		               _children;
	};
	typedef std::vector <Node> NodeArray;

	// Temporary buffers for block processing
	typedef std::array <T, _max_buf_size> Buffer;
	typedef std::vector <Buffer> BufferArray;
	typedef std::vector <T *> OutArray;

	void           build_layout_rec (int &split_ord, int &band_idx, int node_idx, int cur_depth, const T *src_ptr);
	std::vector <int>
	               collect_comp_rec (int node_idx);

	// Splitters. Sorted by processing order
	SplitterArray  _split_arr;

	// Temporary buffers
	BufferArray    _buf_arr;

	// [splitter index (sorted by frequency)] -> processing order
	std::vector <int>
	               _idx2ord_arr;

	// This is a temporary tree stored in a linear array.
	// Node order is abritrary here.
	NodeArray      _node_arr;

	// Stores the location of the pointers on each band output
	// This helps updating these pointers quickly.
	std::vector <T **>
	               _band_out_ptr_arr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	// Copy is disabled because of the internal pointer stuff.
	               SplitMultiband (const SplitMultiband &other)    = delete;
	SplitMultiband &
	               operator = (const SplitMultiband &other)        = delete;
	bool           operator == (const SplitMultiband &other) const = delete;
	bool           operator != (const SplitMultiband &other) const = delete;

}; // class SplitMultiband



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/iir/SplitMultiband.hpp"



#endif   // mfx_dsp_iir_SplitMultiband_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
