/*****************************************************************************

        HistoLu.cpp
        Author: Laurent de Soras, 2022

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"
#include "mfx/dsp/ana/r128/HistoLu.h"
#include "mfx/dsp/ana/r128/MeanSq.h"

#include <numeric>

#include <cstdint>
#include <cassert>



namespace mfx
{
namespace dsp
{
namespace ana
{
namespace r128
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



constexpr float	HistoLu::_gate_abs_lufs;
constexpr int	HistoLu::_msq_l2_min;
constexpr int	HistoLu::_msq_l2_max;
constexpr int	HistoLu::_msq_res_l2;
constexpr int	HistoLu::_msq_res;



void	HistoLu::clear_buffers () noexcept
{
	_occ_arr.fill (0);
	_sum_arr.fill (0);
}



void	HistoLu::add_block (float msq) noexcept
{
	assert (msq >= 0);

	// Registers only blocks above the absolute threshold
	if (msq > _gate_abs_msq)
	{
		const int   idx = conv_msq_to_index (msq);
		++ _occ_arr [idx];
		_sum_arr [idx] += msq;
	}
}



float	HistoLu::compute_loudness_integrated () const noexcept
{
	const auto     loud_abs = compute_lufs_gated_abs ();
	const auto     thr_lufs = loud_abs - 10;
	auto           thr_idx  = conv_lufs_to_index (thr_lufs);
	thr_idx = clip_index (thr_idx);

	return compute_lufs_gated (thr_idx);
}



float	HistoLu::compute_loudness_range () const noexcept
{
	const auto     loud_abs = compute_lufs_gated_abs ();
	const auto     thr_lufs = loud_abs - 20;
	auto           thr_idx  = conv_lufs_to_index (thr_lufs);
	thr_idx = clip_index (thr_idx);

	const auto     nbr_blocks = std::accumulate (
		std::next (_occ_arr.begin (), thr_idx), _occ_arr.end (), 0
	);
	const auto     lufs_lo = compute_lufs_nth (thr_idx, nbr_blocks, 0.10f);
	const auto     lufs_hi = compute_lufs_nth (thr_idx, nbr_blocks, 0.95f);
	const auto     lra     = lufs_hi - lufs_lo;
	assert (lra >= 0);

	return lra;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



constexpr int	HistoLu::_hist_size;
constexpr int	HistoLu::_idx_0lufs;



float	HistoLu::compute_lufs_gated_abs () const noexcept
{
	return compute_lufs_gated (0);
}



float	HistoLu::compute_lufs_gated (int gate_idx) const noexcept
{
	assert (gate_idx >= 0);
	assert (gate_idx < _hist_size);

	const auto     msq_sum    = std::accumulate (
		std::next (_sum_arr.begin (), gate_idx), _sum_arr.end (), 0.0
	);
	const auto     nbr_blocks = std::accumulate (
		std::next (_occ_arr.begin (), gate_idx), _occ_arr.end (), 0
	);

	const auto     msq  = (nbr_blocks > 0) ? msq_sum / double (nbr_blocks) : 0;
	const auto     lufs = MeanSq::conv_mean_sq_to_lufs (float (msq));

	return lufs;
}



float	HistoLu::compute_lufs_nth (int thr_idx, int nbr_blocks, float fractile) const noexcept
{
	assert (thr_idx >= 0);
	assert (thr_idx < _hist_size);
	assert (nbr_blocks >= 0);
	assert (fractile >= 0);
	assert (fractile <= 1);

	const int      cnt_stop = fstb::round_int (fractile * float (nbr_blocks));
	int            count    = 0;
	int            idx      = thr_idx;
	while (count < cnt_stop && idx < _hist_size)
	{
		count += _occ_arr [idx];
		++ idx;
	}
	auto           lufs = conv_index_to_lufs (idx);

	// Refines the LUFS value, assuming a rectangular spread of the loudness
	// values across the histogram segment
	if (count > cnt_stop && idx > 0)
	{
		const auto     lu_unit    = conv_offset_to_lu (1);
		const auto     n_blk_seg  = _occ_arr [idx - 1];
		const auto     ratio_back = float (count - cnt_stop) / float (n_blk_seg);
		lufs -= lu_unit * ratio_back;
	}

	return lufs;
}



int	HistoLu::clip_index (int idx) noexcept
{
	return fstb::limit (idx, 0, _hist_size - 1);
}



int	HistoLu::conv_msq_to_index (float msq) noexcept
{
	assert (msq >= 0);

	union Combo
	{
		float          _f;
		uint32_t       _i;
	};

	constexpr auto    mnt_len = 23;
	constexpr auto    exp_ofs = 127;
	constexpr auto    shf_amt = mnt_len - _msq_res_l2;
	constexpr auto    round_n = 1 << (shf_amt - 1);
	constexpr auto    add_raw = round_n - ((exp_ofs + _msq_l2_min) << mnt_len);

	const Combo       c { msq };
	auto              idx = (c._i + add_raw) >> shf_amt;
	idx = clip_index (idx);

	return idx;
}



int	HistoLu::conv_lu_to_offset (float lu) noexcept
{
	const auto     ofs = fstb::round_int (
		_msq_res * lu * (fstb::LOG2_10 / 10.0)
	);

	return ofs;
}



float	HistoLu::conv_offset_to_lu (float ofs) noexcept
{
	const auto     lu = ofs * float (10 / (fstb::LOG2_10 * _msq_res));

	return lu;
}



int	HistoLu::conv_lufs_to_index (float lufs) noexcept
{
	const auto     ofs = conv_lu_to_offset (lufs - FilterK::_ofs_997);
	auto           idx = ofs + _idx_0lufs;
	idx = clip_index (idx);

	return idx;
}



float	HistoLu::conv_index_to_lufs (int idx) noexcept
{
	idx -= _idx_0lufs;
	const float    lufs = conv_offset_to_lu (float (idx)) + FilterK::_ofs_997;

	return lufs;
}



const float	HistoLu::_gate_abs_msq = MeanSq::conv_lufs_to_mean_sq (_gate_abs_lufs);



}  // namespace r128
}  // namespace ana
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
