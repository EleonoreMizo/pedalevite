/*****************************************************************************

        FilterBankSimd.cpp
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/DataAlign.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/nzbl/FilterBankSimd.h"
#include "mfx/pi/nzbl/NgSetup.h"

#include <algorithm>

#include <cassert>
#include <cmath>



namespace mfx
{
namespace pi
{
namespace nzbl
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



FilterBankSimd::FilterBankSimd ()
{
	dsp::mix::Align::setup ();
}



// Does not compute the latency if latency < 0 as input
void	FilterBankSimd::reset (double sample_freq, int max_buf_len, double &latency)
{
	assert (sample_freq > 0);
	assert (max_buf_len > 0);

	const bool     lat_flag = (latency >= 0);
	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	_max_block_size = max_buf_len;
	const int      mbs_align = (max_buf_len + 3) & -4;
	_buf.resize (mbs_align * 2);

	std::array <float *, _nbr_bands> band_ptr_arr {};
	for (int band_cnt = 0; band_cnt < _nbr_bands; ++band_cnt)
	{
		Band &         band = _band_arr [band_cnt];
		band._buf.resize (mbs_align);
		band_ptr_arr [band_cnt] = band._buf.data ();

		band._env.set_sample_freq (sample_freq);

		const auto     env_times = NgSetup::compute_env_times (band_cnt);
		band._env.set_times (env_times._at, env_times._rt);
	}

	_splitter.set_band_ptr (band_ptr_arr.data ());

	latency = NgSetup::update_filters_compute_latency (
		_splitter, lat_flag ? 700 : -1, sample_freq, _ka, _kb);

	clear_buffers ();
}



void	FilterBankSimd::set_level (int band_idx, float lvl)
{
	assert (band_idx >= 0);
	assert (band_idx < _nbr_bands);
	assert (lvl >= 0);

	_band_arr [band_idx]._lvl = lvl;
}



// Can work in-place
void	FilterBankSimd::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (_sample_freq > 0);
	assert (fstb::DataAlign <true>::check_ptr (dst_ptr));
	assert (fstb::DataAlign <true>::check_ptr (src_ptr));
	assert (nbr_spl > 0);
	assert (nbr_spl <= _max_block_size);

	// Splits the signal in several bands
	_splitter.process_block (src_ptr, nbr_spl);

	// Band processing
	for (int band_cnt = 0; band_cnt < _nbr_bands; ++band_cnt)
	{
		process_band (band_cnt, nbr_spl);
	}

	// Band merging
	static_assert (_nbr_bands >= 2, "");
	dsp::mix::Align::copy_2_1 (
		dst_ptr,
		_band_arr [0]._buf.data (),
		_band_arr [1]._buf.data (),
		nbr_spl
	);
	constexpr int  pair_end = _nbr_bands & ~(2-1);
	for (int band_cnt = 2; band_cnt < pair_end; band_cnt += 2)
	{
		dsp::mix::Align::mix_2_1 (
			dst_ptr,
			_band_arr [band_cnt    ]._buf.data (),
			_band_arr [band_cnt + 1]._buf.data (),
			nbr_spl
		);
	}
	if (pair_end < _nbr_bands)
	{
		// Odd number of bands
		dsp::mix::Align::mix_1_1 (
			dst_ptr,
			_band_arr [pair_end]._buf.data (),
			nbr_spl
		);
	}
}



void	FilterBankSimd::clear_buffers ()
{
	_splitter.clear_buffers ();

	for (int band_cnt = 0; band_cnt < _nbr_bands; ++band_cnt)
	{
		Band &         band = _band_arr [band_cnt];
		band._env.clear_buffers ();
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	FilterBankSimd::process_band (int band_idx, int nbr_spl)
{
	assert (band_idx >= 0);
	assert (band_idx < _nbr_bands);
	assert (nbr_spl > 0);
	assert (nbr_spl <= _max_block_size);

	Band &         band = _band_arr [band_idx];

	if (band._lvl >= 1e-9f)
	{
		float *        buf_ptr = &band._buf [0];

		const auto     lvl  = fstb::ToolsSimd::set1_f32 (band._lvl);
		const auto     thr  = fstb::ToolsSimd::set1_f32 (_rel_thr);
		const auto     mul  = fstb::ToolsSimd::set1_f32 (1.0f / (_rel_thr - 1));
		const auto     one  = fstb::ToolsSimd::set1_f32 (1);
		const auto     zero = fstb::ToolsSimd::set_f32_zero ();

		int            block_pos = 0;
		do
		{
			float *        buf2_ptr  = buf_ptr + block_pos;
			const int      max_len   = _dspl_rate;
			const int      block_len = std::min (nbr_spl - block_pos, max_len);
			const float    blen_inv  = 1.f / float (block_len);

			// Downsamples input^2 by averaging
			auto           sum_v     = fstb::ToolsSimd::set_f32_zero ();
			for (int pos = 0; pos < block_len; pos += 4)
			{
				const auto     x = fstb::ToolsSimd::load_f32 (buf2_ptr + pos);
				sum_v += x * x;
			}
			const float    avg = sum_v.sum_h () * blen_inv;

			const float    e2_beg = band._env.get_state_no_sqrt ();
			const float    e2_end = band._env.analyse_block_raw_cst (avg, block_len);

			// g0 = lvl / max (env, lvl)
			const auto     e2 = fstb::ToolsSimd::set_2f32 (e2_beg, e2_end);
			const auto     e  = fstb::ToolsSimd::sqrt_approx (e2);
			const auto     et = fstb::ToolsSimd::max_f32 (e, lvl);
			const auto     g0 = lvl * et.rcp_approx ();

			// gain = (1 - max ((thr * g0 - 1) / (_rel_thr - 1), 0)) ^ 4
			auto           gt = (thr * g0 - one) * mul;
			gt = one - fstb::ToolsSimd::max_f32 (gt, zero);
			gt = gt * gt;
			gt = gt * gt;

			// Volume
			const float    gt_beg = fstb::ToolsSimd::Shift <0>::extract (gt);
			const float    gt_end = fstb::ToolsSimd::Shift <1>::extract (gt);
			dsp::mix::Align::scale_1_vlr (buf2_ptr, block_len, gt_beg, gt_end);

			block_pos += block_len;
		}
		while (block_pos < nbr_spl);
	}
}



}  // namespace nzbl
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
