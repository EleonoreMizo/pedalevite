/*****************************************************************************

        FilterBank.cpp
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

#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/nzbl/FilterBank.h"

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



FilterBank::FilterBank ()
:	_sample_freq (0)
,	_inv_fs (0)
,	_max_block_size (0)
,	_rel_thr (20)
,	_split_arr (_nbr_bands - 1)
,	_band_arr (_nbr_bands)
,	_buf ()
{
	dsp::mix::Align::setup ();
}



void	FilterBank::reset (double sample_freq, int max_buf_len)
{
	assert (sample_freq > 0);
	assert (max_buf_len > 0);

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	_max_block_size = max_buf_len;
	const int      mbs_align = (max_buf_len + 3) & -4;
	_buf.resize (mbs_align * 2);

	for (int band_cnt = 0; band_cnt < _nbr_bands; ++band_cnt)
	{
		Band &         band = _band_arr [band_cnt];
		band._buf.resize (mbs_align);

		band._env.set_sample_freq (sample_freq);

		float          f = 30;
		if (band_cnt > 0)
		{
			f = compute_split_freq (band_cnt - 1);
		}
		const int      mult  = 16;
		float          t     = float (mult / (2 * fstb::PI)) / f;
		t = std::max (t, 0.005f);
		band._env.set_times (t, t);
	}

	const float          k        = 0.65f;
	const float          alpha    = float (sqrt (2 * (1 - k * k)));
	static const float   as [3]   = { 1, alpha, 1 };
	static const float   bl0s [3] = { 1, 0, 0 };
	static const float   bl1s [3] = { 1, 0, k };
	static const float   bh0s [3] = { 0, 0, 1 };
	static const float   bh1s [3] = { k, 0, 1 };

	for (int split_cnt = 0; split_cnt < _nbr_split; ++split_cnt)
	{
		Split &        split = _split_arr [split_cnt];
		const float    f0    = compute_split_freq (split_cnt);
		const float    kf    =
			dsp::iir::TransSZBilin::compute_k_approx (f0 * _inv_fs);

		split._fix.neutralise ();

		float          bz [3];
		float          az [3];

		// LP
		dsp::iir::TransSZBilin::map_s_to_z_approx (bz, az, bl0s, as, kf);
		split._main.set_z_eq_one (0, bz, az);
		split._fix.set_z_eq_one (0, bz, az);
		dsp::iir::TransSZBilin::map_s_to_z_approx (bz, az, bl1s, as, kf);
		split._main.set_z_eq_one (2, bz, az);
		split._fix.set_z_eq_one (1, bz, az);

		// HP
		dsp::iir::TransSZBilin::map_s_to_z_approx (bz, az, bh0s, as, kf);
		split._main.set_z_eq_one (1, bz, az);
		dsp::iir::TransSZBilin::map_s_to_z_approx (bz, az, bh1s, as, kf);
		split._main.set_z_eq_one (3, bz, az);
	}

	clear_buffers ();
}



void	FilterBank::set_level (int band_idx, float lvl)
{
	assert (band_idx >= 0);
	assert (band_idx < _nbr_bands);
	assert (lvl >= 0);

	_band_arr [band_idx]._lvl = lvl;
}



// Can work in-place
void	FilterBank::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (_sample_freq > 0);
	assert (fstb::DataAlign <true>::check_ptr (dst_ptr));
	assert (fstb::DataAlign <true>::check_ptr (src_ptr));
	assert (nbr_spl > 0);
	assert (nbr_spl <= _max_block_size);

	float *           buf_ptr = &_buf [0];

	// Prepare the interleaved input buffer for the first splitter
	for (int pos = 0; pos < nbr_spl; pos += 4)
	{
		const auto     x = fstb::ToolsSimd::load_f32 (src_ptr + pos);
		fstb::ToolsSimd::VectF32   xi0;
		fstb::ToolsSimd::VectF32   xi1;
		fstb::ToolsSimd::interleave_f32 (xi0, xi1, x, x);
		fstb::ToolsSimd::store_f32 (buf_ptr + pos * 2    , xi0);
		fstb::ToolsSimd::store_f32 (buf_ptr + pos * 2 + 4, xi1);
	}

	// Splits the signal in several bands
	for (int split_idx = 0; split_idx < _nbr_split; ++split_idx)
	{
		Split &        split = _split_arr [split_idx];

		// Splits in place
		split._main.process_block_2x2_immediate (buf_ptr, buf_ptr, nbr_spl);

		if (split_idx == _nbr_split - 1)
		{
			// Last split: distributes the low and high parts to the current
			// and the last bands
			dsp::mix::Align::copy_2i_2 (
				&_band_arr [split_idx    ]._buf [0],
				&_band_arr [split_idx + 1]._buf [0],
				buf_ptr,
				nbr_spl
			);
		}
		else
		{
			dsp::mix::Align::copy_2i_1 (
				&_band_arr [split_idx]._buf [0],
				buf_ptr,
				nbr_spl
			);

			// Stores the lower part into the band buffer and spreads the high
			// part in both channels
			float *        band_ptr   = &_band_arr [split_idx]._buf [0];
			for (int pos = 0; pos < nbr_spl; pos += 4)
			{
				const int      pos_2 = pos * 2;
				const auto     x0    = fstb::ToolsSimd::load_f32 (buf_ptr + pos_2    );
				const auto     x1    = fstb::ToolsSimd::load_f32 (buf_ptr + pos_2 + 4);
				const auto     l     = fstb::ToolsSimd::deinterleave_f32_lo (x0, x1);
				const auto     h0    = fstb::ToolsSimd::monofy_2f32_hi (x0);
				const auto     h1    = fstb::ToolsSimd::monofy_2f32_hi (x1);
				fstb::ToolsSimd::store_f32 (band_ptr + pos      , l);
				fstb::ToolsSimd::store_f32 (buf_ptr  + pos_2    , h0);
				fstb::ToolsSimd::store_f32 (buf_ptr  + pos_2 + 4, h1);
			}
		}
	}

	// Band processing
	for (int band_cnt = 0; band_cnt < _nbr_bands; ++band_cnt)
	{
		process_band (band_cnt, nbr_spl);
	}

	// Band merging
	for (int split_idx = 1; split_idx < _nbr_split; ++split_idx)
	{
		Split &        split = _split_arr [split_idx];

		split._fix.process_block_serial_immediate (
			&_band_arr [split_idx - 1]._buf [0],
			&_band_arr [split_idx - 1]._buf [0],
			nbr_spl
		);
		dsp::mix::Align::mix_1_1 (
			&_band_arr [split_idx    ]._buf [0],
			&_band_arr [split_idx - 1]._buf [0],
			nbr_spl
		);
	}
	// The last two bands don't need compensation processing
	dsp::mix::Align::copy_2_1 (
		dst_ptr,
		&_band_arr [_nbr_bands - 2]._buf [0],
		&_band_arr [_nbr_bands - 1]._buf [0],
		nbr_spl
	);
}



void	FilterBank::clear_buffers ()
{
	for (int split_cnt = 0; split_cnt < _nbr_split; ++split_cnt)
	{
		Split &        split = _split_arr [split_cnt];
		split._main.clear_buffers ();
		split._fix.clear_buffers ();
	}

	for (int band_cnt = 0; band_cnt < _nbr_bands; ++band_cnt)
	{
		Band &         band = _band_arr [band_cnt];
		band._env.clear_buffers ();
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	FilterBank::process_band (int band_idx, int nbr_spl)
{
	assert (band_idx >= 0);
	assert (band_idx < _nbr_bands);
	assert (nbr_spl > 0);
	assert (nbr_spl <= _max_block_size);

	Band &         band = _band_arr [band_idx];

	if (band._lvl >= 1e-9f)
	{
		float *        buf_ptr = &band._buf [0];

		const auto     lvl   = fstb::ToolsSimd::set1_f32 (band._lvl);
		const auto     thr   = fstb::ToolsSimd::set1_f32 (_rel_thr);
		const auto     mul   = fstb::ToolsSimd::set1_f32 (1.0f / (_rel_thr - 1));
		const auto     one   = fstb::ToolsSimd::set1_f32 (1);
		const auto     zero  = fstb::ToolsSimd::set_f32_zero ();

		int            block_pos = 0;
		do
		{
			float *        buf2_ptr  = buf_ptr + block_pos;
			const int      max_len   = _dspl_rate;
			const int      block_len = std::min (nbr_spl - block_pos, max_len);
			const float    blen_inv  = 1.0f / block_len;

			// Downsamples input^2 by averaging
			auto           sum_v     = fstb::ToolsSimd::set_f32_zero ();
			for (int pos = 0; pos < block_len; pos += 4)
			{
				const auto     x = fstb::ToolsSimd::load_f32 (buf_ptr + pos);
				sum_v += x * x;
			}
			const float    avg = fstb::ToolsSimd::sum_h_flt (sum_v) * blen_inv;

			const float    e2_beg = band._env.get_state_no_sqrt ();
			const float    e2_end = band._env.analyse_block_raw_cst (avg, block_len);

			// g0 = lvl / max (env, lvl)
			const auto     e2 = fstb::ToolsSimd::set_2f32 (e2_beg, e2_end);
			const auto     e  = fstb::ToolsSimd::sqrt_approx (e2);
			const auto     et = fstb::ToolsSimd::max_f32 (e, lvl);
			const auto     g0 = fstb::ToolsSimd::rcp_approx (et) * lvl;

			// gain = (1 - max ((thr * g0 - 1) / (g0 - 1), 0)) ^ 4
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



float	FilterBank::compute_split_freq (int split_idx)
{
	return float (125 << split_idx);
}



}  // namespace nzbl
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
