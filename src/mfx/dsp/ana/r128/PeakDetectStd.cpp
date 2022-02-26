/*****************************************************************************

        PeakDetectStd.cpp
        Author: Laurent de Soras, 2022

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/ana/r128/PeakDetectStd.h"

#include <array>
#include <algorithm>

#include <cassert>
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



void	PeakDetectStd::set_sample_freq (double sample_freq)
{
	assert (sample_freq > 0);

	// Nothing at the moment
	/*** To do: the function could be used to reduce the oversampling rate for
		base rates >= 96 kHz ***/
	fstb::unused (sample_freq);
}



float	PeakDetectStd::process_sample (float x) noexcept
{
	std::array <float, _ovrspl> buf;
	_upspl.process_sample_4x (buf.data (), x);
	_peak_lvl = compute_peak4 (_peak_lvl, buf.data ());

	return _peak_lvl;
}



float	PeakDetectStd::process_block (const float src_ptr [], int nbr_spl) noexcept
{
	constexpr int  max_blk_len = 64;
	constexpr int  buf_len     = max_blk_len * _ovrspl;
	std::array <float, buf_len> buf;

	auto           peak_lvl = _peak_lvl;
	int            blk_pos   = 0;
	do
	{
		const int      work_len = std::min (nbr_spl - blk_pos, max_blk_len);
		_upspl.process_block_4x (buf.data (), src_ptr + blk_pos, work_len);
		for (int pos = 0; pos < work_len; ++pos)
		{
			peak_lvl = compute_peak4 (peak_lvl, buf.data () + pos * _ovrspl);
		}
		blk_pos += work_len;
	}
	while (blk_pos < nbr_spl);
	_peak_lvl = peak_lvl;

	return _peak_lvl;
}



float	PeakDetectStd::get_peak () const noexcept
{
	return _peak_lvl;
}



void	PeakDetectStd::clear_peak () noexcept
{
	_peak_lvl = 0;
}



void	PeakDetectStd::clear_buffers () noexcept
{
	_upspl.clear_buffers ();
	clear_peak ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



float	PeakDetectStd::compute_peak4 (float peak_lvl, const float data_ptr [4])
{
	const float    peak_spl = std::max (
		std::max (std::abs (data_ptr [0]), std::abs (data_ptr [1])),
		std::max (std::abs (data_ptr [2]), std::abs (data_ptr [3]))
	);
	peak_lvl = std::max (peak_lvl, peak_spl);

	return peak_lvl;
}



}  // namespace r128
}  // namespace ana
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
