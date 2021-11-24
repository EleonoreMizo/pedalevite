/*****************************************************************************

        LimiterRms.cpp
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

#include "fstb/ToolsSimd.h"
#include "mfx/dsp/dyn/LimiterRms.h"

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace dyn
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	LimiterRms::set_sample_freq (double fs) noexcept
{
	_env.set_sample_freq (fs);
}



void	LimiterRms::set_time (float t) noexcept
{
	_env.set_time (t);
}



void	LimiterRms::set_level (float l) noexcept
{
	assert (l > 0);

	_lvl    = l;
	_lvl_sq = l * l;
}



void	LimiterRms::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	const auto     lvl4    = fstb::ToolsSimd::set1_f32 (_lvl);
	const auto     lvl4_sq = fstb::ToolsSimd::set1_f32 (_lvl_sq);

	int            block_pos = 0;
	do
	{
		const float *  src_loc_ptr = src_ptr + block_pos;
		float *        dst_loc_ptr = dst_ptr + block_pos;
		float *        buf_ptr     = &_buf [0];

		int            work_len = nbr_spl - block_pos;
		work_len = std::min (work_len, int (_tmp_buf_len));

		_env.process_block_no_sqrt (buf_ptr, src_loc_ptr, work_len);
		const int      m4 = work_len & ~3;
		for (int pos = 0; pos < m4; pos += 4)
		{
			const auto     vol_sq = fstb::ToolsSimd::load_f32 (buf_ptr + pos);
			auto           x      = fstb::ToolsSimd::loadu_f32 (src_loc_ptr + pos);
			const auto     vol_gt_lvl = (vol_sq > lvl4_sq);
			const auto     mult   = fstb::ToolsSimd::rsqrt_approx (vol_sq);
			const auto     xlim   = x * lvl4 * mult;
			x = fstb::ToolsSimd::select (vol_gt_lvl, xlim, x);
			fstb::ToolsSimd::storeu_f32 (dst_loc_ptr + pos, x);
		}
		for (int pos = m4; pos < work_len; ++pos)
		{
			const float    vol_sq = _buf [pos];
			float          x      = src_loc_ptr [pos];
			if (vol_sq > _lvl_sq)
			{
				x *= _lvl / sqrtf (vol_sq);
			}
			dst_loc_ptr [pos] = x;
		}

		block_pos += work_len;
	}
	while (block_pos < nbr_spl);
}



void	LimiterRms::clear_buffers () noexcept
{
	_env.clear_buffers ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace dyn
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
