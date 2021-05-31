/*****************************************************************************

        TestOnsetNinos2.cpp
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "ffft/FFTRealFixLen.h"
#include "mfx/dsp/ana/OnsetNinos2.h"
#include "test/TestOnsetNinos2.h"
#include "mfx/FileOpWav.h"

#include <algorithm>
#include <vector>

#include <cassert>
#include <cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestOnsetNinos2::perform_test ()
{
	int            ret_val = 0;

	printf ("Testing mfx::dsp::ana::OnsetNinos2...\n");

	double         sample_freq;
	std::vector <std::vector <float> > chn_arr;
	ret_val = mfx::FileOpWav::load ("../../../src/test/samples/guitar-01.wav", chn_arr, sample_freq);
	if (ret_val != 0)
	{
		return ret_val;
	}

	std::vector <float> src;
	src.swap (chn_arr [0]);
	chn_arr.clear ();

	const int      len        = int (src.size ());
	constexpr int  block_size = 64;
	std::vector <float> dst (len, 0);

	mfx::dsp::ana::OnsetNinos2 <ffft::FFTRealFixLen <11> > os_det;
	os_det.set_sample_freq (sample_freq);
	os_det.set_thr_add (0.1f);
	os_det.set_thr_mul (1.5f);
	os_det.clear_buffers ();

	const int      latency_spl = os_det.compute_latency ();
	const double   latency_ms  = latency_spl * (1000.0 / sample_freq);
	printf ("Latency: %.1f ms\n", latency_ms);

	int            pos_blk = 0;
	do
	{
		const int      work_len   = std::min <int> (len - pos_blk, block_size);
		const bool     onset_flag = os_det.analyse_block (&src [pos_blk], work_len);
		if (onset_flag)
		{
			std::fill (
				dst.begin () + pos_blk,
				dst.begin () + pos_blk + work_len,
				1.f
			);
		}

		pos_blk += work_len;
	}
	while (pos_blk < len);

	mfx::FileOpWav::save ("results/onsetninos2_0.wav", dst, sample_freq, 0.5f);

	printf ("Done.\n\n");

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
