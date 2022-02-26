/*****************************************************************************

        PeakDetectAccurate.cpp
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
#include "fstb/Poly.h"
#include "mfx/dsp/ana/r128/PeakDetectAccurate.h"

#include <array>
#include <algorithm>

#include <cassert>
#include <cmath>
#include <cstdlib>



namespace mfx
{
namespace dsp
{
namespace ana
{
namespace r128
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	PeakDetectAccurate::set_sample_freq (double sample_freq)
{
	assert (sample_freq > 0);

	// Nothing at the moment
	/*** To do: the function could be used to reduce the oversampling rate for
		base rates >= 96 kHz ***/
	fstb::unused (sample_freq);
}



float	PeakDetectAccurate::process_sample (float x) noexcept
{
	std::array <float, 2 + _ovrspl> buf;
	buf [0]      = _mem_arr [0];
	buf [1]      = _mem_arr [1];
	_upspl.process_sample_4x (buf.data () + 2, x);
	_peak_lvl    = find_peak (_peak_lvl, buf.data (), _ovrspl);
	_mem_arr [0] = buf [_ovrspl + 0];
	_mem_arr [1] = buf [_ovrspl + 1];

	return _peak_lvl;
}



float	PeakDetectAccurate::process_block (const float src_ptr [], int nbr_spl) noexcept
{
	constexpr int  max_blk_len = 64;
	constexpr int  buf_len     = max_blk_len * _ovrspl + 2;
	std::array <float, buf_len> buf;
	buf [0] = _mem_arr [0];
	buf [1] = _mem_arr [1];

	auto           peak_lvl     = _peak_lvl;
	int            blk_pos      = 0;
	int            work_len     = 0;
	int            work_len_ovr = 0;
	do
	{
		work_len = std::min (nbr_spl - blk_pos, max_blk_len);
		_upspl.process_block_4x (buf.data () + 2, src_ptr + blk_pos, work_len);
		work_len_ovr = work_len * _ovrspl;
		peak_lvl = find_peak (peak_lvl, buf.data (), work_len_ovr);
		blk_pos += work_len;
		buf [0]  = buf [work_len_ovr + 0];
		buf [1]  = buf [work_len_ovr + 1];
	}
	while (blk_pos < nbr_spl);
	_peak_lvl    = peak_lvl;
	_mem_arr [0] = buf [work_len_ovr + 0];
	_mem_arr [1] = buf [work_len_ovr + 1];

	return _peak_lvl;
}



float	PeakDetectAccurate::get_peak () const noexcept
{
	return _peak_lvl;
}



void	PeakDetectAccurate::clear_peak () noexcept
{
	_peak_lvl = 0;
}



void	PeakDetectAccurate::clear_buffers () noexcept
{
	_upspl.clear_buffers ();
	clear_peak ();
	_mem_arr.fill (0);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// data_ptr should contain nbr_spl + 2 samples
float	PeakDetectAccurate::find_peak (float peak_lvl, const float data_ptr [], int nbr_spl) noexcept
{
	assert (nbr_spl > 2);

	auto           y0 = data_ptr [0];
	auto           y1 = data_ptr [1];

	peak_lvl = std::max (peak_lvl, std::abs (y0));
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		const auto     y2 = data_ptr [pos + 2];

		const auto     d10 = y1 - y0;
		const auto     d12 = y1 - y2;
		if (d10 + d12 != 0)
		{
			const auto     x  = fstb::find_extremum_pos_parabolic (y0, y1, y2);
			if (x >= -0.5f && x < 0.5f)
			{
				const auto     a0 = y1;
				const auto     a1 = (y2 - y0) * 0.5f;
				const auto     a2 = (y2 + y0) * 0.5f - y1;
				const auto     y  = fstb::Poly::horner (x, a0, a1, a2);
				peak_lvl = std::max (peak_lvl, std::abs (y));
			}
		}
		else
		{
			peak_lvl = std::max (peak_lvl, std::abs (y1));
		}

		y0 = y1;
		y1 = y2;
	}

	return peak_lvl;
}



}  // namespace r128
}  // namespace ana
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
