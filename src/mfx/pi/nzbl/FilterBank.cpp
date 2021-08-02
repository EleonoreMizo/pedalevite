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
#include "mfx/dsp/mix/Fpu.h"
#include "mfx/pi/nzbl/FilterBank.h"
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

	std::array <float *, _nbr_bands> band_ptr_arr {};
	for (int band_cnt = 0; band_cnt < _nbr_bands; ++band_cnt)
	{
		Band &         band = _band_arr [band_cnt];

		band_ptr_arr [band_cnt] = band._buf.data ();

		band._env.set_sample_freq (sample_freq);

		const auto     env_times = NgSetup::compute_env_times (band_cnt);
		band._env.set_times (env_times._at, env_times._rt);
	}

	_splitter.set_nbr_bands (_nbr_bands, band_ptr_arr.data ());

	latency = NgSetup::update_filters_compute_latency (
		_splitter, lat_flag ? 700 : -1, sample_freq, _ka, _kb
	);

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
	_splitter.process_block (src_ptr, nbr_spl);

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
	static_assert (_nbr_bands >= 2, "");
	dsp::mix::Fpu::copy_2_1 (
		dst_ptr,
		_band_arr [0]._buf.data (),
		_band_arr [1]._buf.data (),
		nbr_spl
	);
	constexpr int  pair_end = _nbr_bands & ~(2-1);
	for (int band_cnt = 2; band_cnt < pair_end; band_cnt += 2)
	{
		dsp::mix::Fpu::mix_2_1 (
			dst_ptr,
			_band_arr [band_cnt    ]._buf.data (),
			_band_arr [band_cnt + 1]._buf.data (),
			nbr_spl
		);
	}
	if (pair_end < _nbr_bands)
	{
		// Odd number of bands
		dsp::mix::Fpu::mix_1_1 (
			dst_ptr,
			_band_arr [pair_end]._buf.data (),
			nbr_spl
		);
	}
}



void	FilterBank::clear_buffers ()
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



/*
Other possible formula for the gain shape:
x = (limit (env / thr, 1, _thr_hi_rel) - 1) / (_thr_hi_rel - 1)
g = 0.5 * ((1 - (1 - x) ^ 8) ^ 2 + (1 - (1 - x) ^ 3) ^ 2)
No div, but 1 / thr must be precalculated too
*/

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

			// gain = (1 - max ((_thr_hi_rel * g0 - 1) / (_thr_hi_rel - 1), 0)) ^ 4
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



}  // namespace nzbl
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
