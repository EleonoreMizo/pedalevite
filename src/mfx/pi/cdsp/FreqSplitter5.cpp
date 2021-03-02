/*****************************************************************************

        FreqSplitter5.cpp
        Author: Laurent de Soras, 2016

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

#include "mfx/pi/cdsp/FreqSplitter5.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace cdsp
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	FreqSplitter5::clear_buffers ()
{
	for (auto &band : _band_split_arr)
	{
		band.clear_buffers ();
	}
}



void	FreqSplitter5::set_sample_freq (double sample_freq)
{
	assert (sample_freq > 0);

	for (auto &band : _band_split_arr)
	{
		band.set_sample_freq (sample_freq);
	}
}



void	FreqSplitter5::set_split_freq (float freq)
{
	assert (freq > 0);

	for (auto &band : _band_split_arr)
	{
		band.set_split_freq (freq);
	}
}



void	FreqSplitter5::copy_z_eq (const FreqSplitter5 &other)
{
	for (size_t b_cnt = 0; b_cnt < _band_split_arr.size (); ++b_cnt)
	{
		_band_split_arr [b_cnt].copy_param_from (other._band_split_arr [b_cnt]);
	}
}



void	FreqSplitter5::process_block (int chn, float dst_l_ptr [], float dst_h_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (chn >= 0);
	assert (chn < int (_band_split_arr.size ()));
	assert (dst_l_ptr != nullptr);
	assert (dst_h_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	auto &         splitter = _band_split_arr [chn];
	if (splitter.is_dirty ())
	{
		splitter.update_coef ();
	}
	splitter.process_block_split (dst_l_ptr, dst_h_ptr, src_ptr, nbr_spl);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace cdsp
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
