/*****************************************************************************

        HistoLu.h
        Author: Laurent de Soras, 2022

Builds a histogram of per-block LU values. This can be used as a helper to
compute the integrated loundness (BT.1770) or the loudness range (R 128).
This class is RT-compatible.

References:
- ITU-R BS.1770-4
- EBU R 128
- EBU Tech 3342-2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_ana_r128_HistoLu_HEADER_INCLUDED)
#define mfx_dsp_ana_r128_HistoLu_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "mfx/dsp/ana/r128/FilterK.h"

#include <array>



namespace mfx
{
namespace dsp
{
namespace ana
{
namespace r128
{



class HistoLu
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	// Absolute gate threshold, in LUFS
	static constexpr float  _gate_abs_lufs = -70;

	// Base-2 log of the minimum stored mean square value (incl.)
	// Corresponding value in LUFS should be equal or below the absolute gate
	// threshold.
	static constexpr int _msq_l2_min = -24;

	// Base-2 log of the maximum stored mean square value (incl.)
	static constexpr int _msq_l2_max =  +4;

	// Histogram resolution per power of 2 (linear).
	static constexpr int _msq_res_l2 = 8;
	static constexpr int _msq_res    = 1 << _msq_res_l2;

	void           add_block (float msq) noexcept;
	void           clear_buffers () noexcept;

	float          compute_loudness_integrated () const noexcept;
	float          compute_loudness_range () const noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static_assert (
		   double (_msq_l2_min)
		<= (_gate_abs_lufs - FilterK::_ofs_997) * (fstb::LOG2_10 / 10.0),
		""
	);

	static constexpr int _hist_size = (_msq_l2_max - _msq_l2_min) * _msq_res + 1;
	static constexpr int _idx_0lufs = -_msq_l2_min << _msq_res_l2;

	typedef double SumType;

	typedef std::array <int, _hist_size> OccurArray;
	typedef std::array <SumType, _hist_size> SumArray;

	float          compute_lufs_gated_abs () const noexcept;
	float          compute_lufs_gated (int gate_idx) const noexcept;
	float          compute_lufs_nth (int thr_idx, int nbr_blocks, float fractile) const noexcept;

	static inline int
	               clip_index (int idx) noexcept;
	static inline int
	               conv_msq_to_index (float msq) noexcept;
	static inline int
	               conv_lu_to_offset (float lu) noexcept;
	static inline float
	               conv_offset_to_lu (float ofs) noexcept;
	static inline int
	               conv_lufs_to_index (float lufs) noexcept;
	static inline float
	               conv_index_to_lufs (int idx) noexcept;

	// Number of blocks in each LUFS segment
	OccurArray     _occ_arr {};

	// Sum of the block values in each LUFS segment
	SumArray       _sum_arr {};

	static const float
	               _gate_abs_msq;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const HistoLu &other) const = delete;
	bool           operator != (const HistoLu &other) const = delete;

}; // class HistoLu



}  // namespace r128
}  // namespace ana
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/ana/r128/HistoLu.hpp"



#endif   // mfx_dsp_ana_r128_HistoLu_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

