/*****************************************************************************

        Notch.cpp
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
#include "fstb/ToolsSimd.h"
#include "mfx/dsp/iir/DesignEq2p.h"
#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/nzbl/Notch.h"

#include <cassert>
#include <cmath>
#include <cstring>



namespace mfx
{
namespace pi
{
namespace nzbl
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Notch::reset (double sample_freq, int max_buf_len, float buf_0_ptr [], float buf_1_ptr [])
{
	assert (sample_freq > 0);
	assert (max_buf_len > 0);
	assert (fstb::is_ptr_align_nz (buf_0_ptr));
	assert (fstb::is_ptr_align_nz (buf_1_ptr));

	dsp::mix::Align::setup ();

	_sample_freq  = float (    sample_freq);
	_inv_fs       = float (1 / sample_freq);
	_max_buf_size = max_buf_len;

	_flt_dirty_flag = true;
	_bpf.clear_buffers ();

	_env.set_sample_freq (sample_freq);
	_env.clear_buffers ();

	_buf_env_ptr = buf_0_ptr;
	_buf_bpf_ptr = buf_1_ptr;
}



void	Notch::set_freq (float freq)
{
	assert (freq > 0);
	assert (freq < _sample_freq * 0.5f);

	_freq           = freq;
	_flt_dirty_flag = true;
}



void	Notch::set_q (float q)
{
	assert (q > 0);
	assert (q < 1e3); // Reasonable value

	_q              = q;
	_flt_dirty_flag = true;
}



void	Notch::set_lvl (float lvl)
{
	assert (lvl >= 0);

	_lvl = lvl;
}



// Can work in-place (this is more efficient for the bypass case)
// The source and destination buffer lengths should be aligned too.
void	Notch::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (_buf_env_ptr != 0);
	assert (fstb::is_ptr_align_nz (dst_ptr));
	assert (fstb::is_ptr_align_nz (src_ptr));
	assert (nbr_spl > 0);

	if (_flt_dirty_flag)
	{
		update_filter ();
	}

	if (_lvl < 1e-9f)
	{
		if (dst_ptr != src_ptr)
		{
			memcpy (dst_ptr, src_ptr, nbr_spl * sizeof (*dst_ptr));
		}
	}
	else
	{
		_bpf.process_block (_buf_bpf_ptr, src_ptr, nbr_spl);

		const auto     lvl   = fstb::ToolsSimd::set1_f32 (_lvl);
		const auto     thr   = fstb::ToolsSimd::set1_f32 (_rel_thr);
		const auto     mul   = fstb::ToolsSimd::set1_f32 (1.0f / (_rel_thr - 1));
		const auto     one   = fstb::ToolsSimd::set1_f32 (1);
		const auto     zero  = fstb::ToolsSimd::set_f32_zero ();
		const auto     c0123 = fstb::ToolsSimd::set_f32 (0, 0.25f, 0.5f, 0.75f);

/*
Optimisation: downsample src^2 with an average filter, here at x32.
Take the envelope and compute the gain with these data.
This reduces the computation load per sample (env + sqrt + rcp).
Linearly interpolate the gain between the points.
The exact rate is not important so we don't need to track the exact location
of the downsampling points.

Further possible optimisation: two pass, first one to collect e2_end with a
maximum of 4 sub-blocks, an intermediate step to vector-compute gt for all e2
collected, and the second pass to adjust the gain in the previous sub-blocks.
Not sure if the benefit is interesting, especially for small buffers.
*/

		int            block_pos = 0;
		do
		{
			const float *  bpf_ptr   = _buf_bpf_ptr + block_pos;
			const float *  sr2_ptr   = src_ptr      + block_pos;
			float *        ds2_ptr   = dst_ptr      + block_pos;
			const int      max_len   = _dspl_rate;
			const int      block_len = std::min (nbr_spl - block_pos, max_len);
			const float    blen_inv  = 1.0f / block_len;

			// Downsamples input^2 by averaging
			auto           sum_v     = fstb::ToolsSimd::set_f32_zero ();
			for (int pos = 0; pos < block_len; pos += 4)
			{
				const auto     x = fstb::ToolsSimd::load_f32 (bpf_ptr + pos);
				sum_v += x * x;
			}
			const float    avg = fstb::ToolsSimd::sum_h_flt (sum_v) * blen_inv;

			const float    e2_beg = _env.get_state_no_sqrt ();
			const float    e2_end = _env.analyse_block_raw_cst (avg, block_len);

			// g0 = lvl / max (env, lvl)
			const auto     e2 = fstb::ToolsSimd::set_2f32 (e2_beg, e2_end);
			const auto     e  = fstb::ToolsSimd::sqrt_approx (e2);
			const auto     et = fstb::ToolsSimd::max_f32 (e, lvl);
			const auto     g0 = fstb::ToolsSimd::rcp_approx (et) * lvl;

			// gain = max ((thr * g0 - 1) / (g0 - 1), 0)
			auto           gt = (thr * g0 - one) * mul;
			gt = fstb::ToolsSimd::max_f32 (gt, zero);

			const float    gt_beg = fstb::ToolsSimd::Shift <0>::extract (gt);
			const float    gt_end = fstb::ToolsSimd::Shift <1>::extract (gt);

			// Simple linear interpolation on the gain
			// Division can be avoided just by reading a table, block_len is
			// integer and bounded by _dspl_rate.
			const float    step4s = (gt_end - gt_beg) * 4 * blen_inv;
			const auto     step4  = fstb::ToolsSimd::set1_f32 (step4s);
			gt = fstb::ToolsSimd::Shift < 0>::spread (gt);
			fstb::ToolsSimd::mac (gt, step4, c0123);
			for (int pos = 0; pos < block_len; pos += 4)
			{
				const auto     s  = fstb::ToolsSimd::load_f32 (sr2_ptr + pos);
				const auto     b  = fstb::ToolsSimd::load_f32 (bpf_ptr + pos);
				const auto     y  = s - b * gt;
				fstb::ToolsSimd::store_f32 (ds2_ptr + pos, y);
				gt += step4;
			}

			block_pos += block_len;
		}
		while (block_pos < nbr_spl);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Notch::update_filter ()
{
	assert (_inv_fs > 0);

	const float    bw_rel = 1 / _q;
	const float    f_rel  = _freq * _inv_fs;
	const float    k      =
		dsp::iir::TransSZBilin::compute_k_approx (f_rel);

	// Cheaply fix the frequency warping caused by the BLT
	const float    q      = _q * (1 - 1.875f * f_rel * f_rel);

	float          b_s [3];
	float          a_s [3];
	dsp::iir::DesignEq2p::make_band_pass (b_s, a_s, q);

	float          b_z [3];
	float          a_z [3];
	dsp::iir::TransSZBilin::map_s_to_z_approx (
		b_z, a_z, b_s, a_s, k
	);
	_bpf.set_z_eq (b_z, a_z);

	// Sets the envelope rise and fall times at 16x the signal slope (about
	// 2.5 periods) so we can catch an accurate and stable volume while still
	// keeping a fast response.
	const int      mult  = 16;
	const float    delta = sqrt (4 * _q * _q + 1);
	const float    f1    = _freq * (delta - 1) * bw_rel * 0.5f; // Lowest freq at -3 dB
	const float    t     = float (mult / (2 * fstb::PI)) / f1;
	_env.set_times (t, t);

	_flt_dirty_flag = false;
}



}  // namespace nzbl
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
