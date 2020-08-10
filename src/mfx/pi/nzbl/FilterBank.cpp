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

#include "fstb/def.h"
#include "mfx/dsp/iir/DesignEq2p.h"
#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/dsp/mix/Fpu.h"
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



// Does not compute the latency if latency < 0 as input
void	FilterBank::reset (double sample_freq, int max_buf_len, double &latency)
{
	assert (sample_freq > 0);
	assert (max_buf_len > 0);

	fstb::unused (max_buf_len);

	const bool     lat_flag = (latency >= 0);
	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	for (int band_cnt = 0; band_cnt < _nbr_bands; ++band_cnt)
	{
		Band &         band = _band_arr [band_cnt];

		band._env.set_sample_freq (sample_freq);

		// Computes the envelope times.
		// For the lowest band, we use 30 Hz as reference to make sure that the
		// lowest frequencies are not distorded. Consequently, the gate will be
		// slow to react. But low frequency rumble is generally steady (it comes
		// from the power supply) therefore this is not a problem.
		float          f = 30;
		if (band_cnt > 0)
		{
			f = compute_split_freq (band_cnt - 1);
		}
		const int      mult   = 16;
		float          t      = float (mult / (2 * fstb::PI)) / f;
		// Longer release helps preventing amplitude modulation on periodic
		// noise bursts
		const float    min_at = 0.005f;
		const float    min_rt = 0.050f;
		const float    at = std::max (t, min_at);
		const float    rt = std::max (t, min_rt);
		band._env.set_times (at, rt);
	}

	constexpr float      k      = 0.65f; // Thiele coefficient
	const float          alpha  = float (sqrt (2 * (1 - k * k)));
	static const float   as [3] = { 1, alpha, 1 }; // Shared poles
	static const float   bls [_nbr_stages] [3] = { { 1, 0, 0 }, { 1, 0, k } };
	static const float   bhs [_nbr_stages] [3] = { { 0, 0, 1 }, { k, 0, 1 } };

	for (int split_cnt = 0; split_cnt < _nbr_split; ++split_cnt)
	{
		Split &        split = _split_arr [split_cnt];
		const float    f0    = compute_split_freq (split_cnt);
		const float    kf    =
			mfx::dsp::iir::TransSZBilin::compute_k_approx (f0 * _inv_fs);

		for (int stage = 0; stage < _nbr_stages; ++stage)
		{
			float          bz [3];
			float          az [3];

			// LP
			mfx::dsp::iir::TransSZBilin::map_s_to_z_approx (
				bz, az, bls [stage], as, kf
			);
			split._lpf [stage].set_z_eq (bz, az);
			split._fix [stage].set_z_eq (bz, az);

			// HP
			mfx::dsp::iir::TransSZBilin::map_s_to_z_approx (
				bz, az, bhs [stage], as, kf
			);
			split._hpf [stage].set_z_eq (bz, az);

			// Evaluates the group delay
			if (lat_flag)
			{
				// Base frequency for latency evaluation. Hz
				constexpr double  f_lat = 700;

				// Uses the HPs or LPs depending on the tested frequency
				if (f0 < f_lat)
				{
					split._hpf [stage].get_z_eq (bz, az);
				}
				else
				{
					split._lpf [stage].get_z_eq (bz, az);
				}
				az [0] = 1;
				const double      gd = mfx::dsp::iir::DesignEq2p::compute_group_delay (
					bz, az, sample_freq, f_lat
				);
				latency += gd;
			}
		}
	}

	clear_buffers ();
}



// thr is a linear value
void	FilterBank::set_threshold (int band_idx, float thr)
{
	assert (band_idx >= 0);
	assert (band_idx < _nbr_bands);
	assert (thr >= 0);

	_band_arr [band_idx]._thr = thr;
}



// Can work in-place
void	FilterBank::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (_sample_freq > 0);
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);
	assert (nbr_spl <= _max_blk_size);

	// Splits the signal in several bands
	for (int split_idx = 0; split_idx < _nbr_split; ++split_idx)
	{
		Split &        split  = _split_arr [split_idx];

		// The lower part goes to the current band, and the higher part
		// propagates to the next band
		float *        lo_ptr = _band_arr [split_idx    ]._buf.data ();
		float *        hi_ptr = _band_arr [split_idx + 1]._buf.data ();

		split._hpf [0].process_block (hi_ptr, src_ptr, nbr_spl);
		split._hpf [1].process_block (hi_ptr, hi_ptr, nbr_spl);

		split._lpf [0].process_block (lo_ptr, src_ptr, nbr_spl);
		split._lpf [1].process_block (lo_ptr, lo_ptr, nbr_spl);

		// Next bands will be filtered in-place.
		src_ptr = hi_ptr;
	}

	// Band processing
	// Divides the current block in almost equal sub-blocks, fitting in
	// the maximum processing length.
	const int      nbr_sub_blocks = (nbr_spl + _dspl_rate - 1) >> _dspl_rate_l2;
	const int      sub_block_len  = (nbr_spl + nbr_sub_blocks - 1) / nbr_sub_blocks;
	for (int band_cnt = 0; band_cnt < _nbr_bands; ++band_cnt)
	{
		process_band (band_cnt, nbr_spl, sub_block_len);
	}

	// Band merging
	for (int split_idx = 1; split_idx < _nbr_split; ++split_idx)
	{
		Split &        split   = _split_arr [split_idx];
		float *        prv_ptr = _band_arr [split_idx - 1]._buf.data ();
		float *        cur_ptr = _band_arr [split_idx    ]._buf.data ();

		split._fix [0].process_block (prv_ptr, prv_ptr, nbr_spl);
		split._fix [1].process_block (prv_ptr, prv_ptr, nbr_spl);
		mfx::dsp::mix::Fpu::mix_1_1 (cur_ptr, prv_ptr, nbr_spl);
	}

	// The last two bands don't need compensation processing
	mfx::dsp::mix::Fpu::copy_2_1 (
		dst_ptr,
		_band_arr [_nbr_bands - 2]._buf.data (),
		_band_arr [_nbr_bands - 1]._buf.data (),
		nbr_spl
	);
}



void	FilterBank::clear_buffers ()
{
	for (int split_cnt = 0; split_cnt < _nbr_split; ++split_cnt)
	{
		Split &        split = _split_arr [split_cnt];
		for (int stage = 0; stage < _nbr_stages; ++stage)
		{
			split._lpf [stage].clear_buffers ();
			split._hpf [stage].clear_buffers ();
			split._fix [stage].clear_buffers ();
		}
	}

	for (int band_cnt = 0; band_cnt < _nbr_bands; ++band_cnt)
	{
		Band &         band = _band_arr [band_cnt];
		band._env.clear_buffers ();
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	FilterBank::process_band (int band_idx, int nbr_spl, int sub_block_len)
{
	assert (band_idx >= 0);
	assert (band_idx < _nbr_bands);
	assert (nbr_spl > 0);
	assert (nbr_spl <= _max_blk_size);
	assert (sub_block_len > 0);
	assert (sub_block_len <= nbr_spl);

	Band &         band = _band_arr [band_idx];

	if (band._thr >= 1e-9f)
	{
		float *        buf_ptr = band._buf.data ();

		const float    thr       = band._thr;
		int            block_pos = 0;
		do
		{
			float *        buf2_ptr  = buf_ptr + block_pos;
			const int      block_len = std::min (nbr_spl - block_pos, sub_block_len);
			const float    blen_inv  = fstb::rcp_uint <float> (block_len);

			// Downsamples input^2 by averaging
			float          sum = 0;
			for (int pos = 0; pos < block_len; ++pos)
			{
				const auto     x = buf2_ptr [pos];
				sum += x * x;
			}
			const float    avg = sum * blen_inv;

			const float    e2  = band._env.analyse_block_raw_cst (avg, block_len);
			const float    env = sqrtf (e2);

			// g0 = thr / max (env, thr)
			const float    g0 = thr / std::max (env, thr);

			// gain = (1 - max ((_thr_hi_rel * g0 - 1) / (g0 - 1), 0)) ^ 4
			float          g  = (_thr_hi_rel * g0 - 1) * _mul_thr_hi;
			g = fstb::ipowpc <4> (1 - std::max (g, 0.f));

			// Volume
			mfx::dsp::mix::Fpu::scale_1_vlr (buf2_ptr, block_len, band._g_old, g);

			// Next sub-block
			band._g_old = g;
			block_pos  += block_len;
		}
		while (block_pos < nbr_spl);
	}
}



constexpr float	FilterBank::compute_split_freq (int split_idx)
{
	return float (125 << split_idx);
}



}  // namespace nzbl
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
