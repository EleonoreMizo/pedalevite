/*****************************************************************************

        FreqSplitter.cpp
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

#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/pi/dist2/FreqSplitter.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace dist2
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	FreqSplitter::clear_buffers ()
{
	for (auto &band : _band_split_arr)
	{
		band.clear_buffers ();
	}
}



void	FreqSplitter::set_sample_freq (double sample_freq)
{
	assert (sample_freq > 0);

	_sample_freq = float (      sample_freq);
	_inv_fs      = float (1.0 / sample_freq);
	update_filter ();
}



void	FreqSplitter::set_split_freq (float freq)
{
	assert (freq > 0);
	assert (freq < _sample_freq * 0.5f);

	_split_freq = freq;
	update_filter ();
}



void	FreqSplitter::process_block (int chn, float dst_l_ptr [], float dst_h_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (chn >= 0);
	assert (chn < int (_band_split_arr.size ()));
	assert (dst_l_ptr != nullptr);
	assert (dst_h_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	BandSplitApp & splitter = _band_split_arr [chn];

	splitter.split_block (dst_l_ptr, dst_h_ptr, src_ptr, nbr_spl);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	FreqSplitter::update_filter ()
{
	assert (_sample_freq > 0);
	assert (_inv_fs      > 0);
	assert (_split_freq  > 0);

	// Filter design           ka     , kb
	// Butterworth -30 dB/oct: 0.61804, 1.0
	// Steep with ripples:     0.35   , 0.99
	// Compromise:             0.5    , 1.0
	const float		ka = 0.61804f;
	const float		kb = 1.0f;
	const float		x1 = ka + kb;
	const float		x2 = ka;

	// Bilinear transform
	const float    k =
		dsp::iir::TransSZBilin::compute_k_approx (_split_freq * _inv_fs);

	float          zb_0o [3];
	float          zb_0e [2];
	float          zb_1 [3];
	dsp::iir::TransSZBilin::map_s_to_z_ap1_approx (zb_0e,     k);
	dsp::iir::TransSZBilin::map_s_to_z_ap2_approx (zb_0o, x2, k);
	dsp::iir::TransSZBilin::map_s_to_z_ap2_approx (zb_1 , x1, k);

	// Sets all the channels
	for (auto &splitter : _band_split_arr)
	{
		Filter0 &	filter_0 = splitter.use_filter_0 ();
		Filter1 &	filter_1 = splitter.use_filter_1 ();
		filter_0.use_filter ().set_coef (zb_0e [0]);
		filter_0.use_filter (0).set_z_eq (zb_0o [0], zb_0o [1]);
		filter_1.set_z_eq (zb_1 [0], zb_1 [1]);
	}
}



}  // namespace dist2
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
