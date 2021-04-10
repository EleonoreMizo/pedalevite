/*****************************************************************************

        TestSplitMultibandBustad.cpp
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

#include "fstb/fnc.h"
#include "mfx/dsp/iir/SplitMultibandBustad.h"
#include "mfx/dsp/iir/DesignEq2p.h"
#include "mfx/dsp/mix/Generic.h"
#include "mfx/dsp/osc/SweepingSin.h"
#include "mfx/FileOpWav.h"
#include "test/TestSplitMultibandBustad.h"
#include "test/TimerAccurate.h"

#include <cassert>
#include <cmath>
#include <cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestSplitMultibandBustad::perform_test ()
{
	int            ret_val = 0;

	if (ret_val == 0)
	{
		ret_val = perform_test_class <
			mfx::dsp::iir::SplitMultibandBustad <4>
		> (
			"mfx::dsp::iir::SplitMultibandBustad",
			"splitmultibandbustad",
			11,
			true, // block_flag
			true  // save_flag
		);
	}

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class S>
int	TestSplitMultibandBustad::perform_test_class (const char *classname_0, const char *filename_0, int nbr_bands, bool block_flag, bool save_flag)
{
	assert (classname_0 != nullptr);
	assert (filename_0  != nullptr);

	int            ret_val = 0;

	printf ("Testing %s...\n", classname_0);

	mfx::dsp::mix::Generic::setup ();

	constexpr double  sample_freq = 44100;
	constexpr int     buf_size    = 64;
	const int         len         = fstb::round_int (sample_freq * 10);

	std::vector <float>  src (len);
	mfx::dsp::osc::SweepingSin ssin (sample_freq, 20.0, 20000.0);
	ssin.generate (src.data (), len);

	S              splitter;
	constexpr int  nbr_biq   = S::_nbr_2p;
	constexpr int  nbr_onep  = S::_nbr_1p;

	typedef std::array <float, buf_size> Buffer;
	std::vector <Buffer> buf_arr (nbr_bands);

	std::vector <float *> band_ptr_arr (nbr_bands);
	for (int k = 0; k < nbr_bands; ++k)
	{
		band_ptr_arr [k] = buf_arr [k].data ();
	}

	splitter.set_sample_freq (sample_freq);
	splitter.set_nbr_bands (nbr_bands, band_ptr_arr.data ());

	// Filter design
	printf ("Target frequencies: ");
	constexpr double  thiele_k = 0.5f;
	const double      k2 = thiele_k * thiele_k;
	std::array <float, (nbr_biq * 3 + nbr_onep * 2) * 2> coef_lpf_arr;
	std::array <float, (nbr_biq * 3 + nbr_onep * 2) * 2> coef_hpf_arr;
	for (int biq = 0; biq < nbr_biq; ++biq)
	{
		const double   a1 = mfx::dsp::iir::DesignEq2p::compute_thiele_coef_a1 (
			S::_order, biq, thiele_k
		);
		coef_lpf_arr [biq * 6    ] = 1;
		coef_lpf_arr [biq * 6 + 1] = 0;
		coef_lpf_arr [biq * 6 + 2] = (biq == 0) ? float (k2) : 0;
		coef_lpf_arr [biq * 6 + 3] = 1;
		coef_lpf_arr [biq * 6 + 4] = float (a1);
		coef_lpf_arr [biq * 6 + 5] = 1;

		coef_hpf_arr [biq * 6    ] = (biq == 0) ? float (k2) : 0;
		coef_hpf_arr [biq * 6 + 1] = 0;
		coef_hpf_arr [biq * 6 + 2] = 1;
		coef_hpf_arr [biq * 6 + 3] = 1;
		coef_hpf_arr [biq * 6 + 4] = float (a1);
		coef_hpf_arr [biq * 6 + 5] = 1;
	}
	if (nbr_onep > 0)
	{
		coef_lpf_arr [nbr_biq * 6    ] = 1;
		coef_lpf_arr [nbr_biq * 6 + 1] = 0;
		coef_lpf_arr [nbr_biq * 6 + 2] = 1;
		coef_lpf_arr [nbr_biq * 6 + 3] = 1;

		coef_hpf_arr [nbr_biq * 6    ] = 0;
		coef_hpf_arr [nbr_biq * 6 + 1] = 1;
		coef_hpf_arr [nbr_biq * 6 + 2] = 1;
		coef_hpf_arr [nbr_biq * 6 + 3] = 1;
	}
	for (int split_idx = 0; split_idx < nbr_bands - 1; ++split_idx)
	{
		constexpr double fmin =    20.0;
		constexpr double fmax = 20000.0;
		const double     f    =
			fmin * pow (fmax / fmin, double (split_idx + 1) / nbr_bands);
		printf ("%7.1f ", f);
		splitter.set_splitter_coef (
			split_idx, float (f),
			coef_lpf_arr.data (), coef_hpf_arr.data ()
		);
	}
	double         latency = 0;
	constexpr int  nbr_test_lat = 100;
	for (int k = 0; k < nbr_test_lat; ++k)
	{
		const double   f  = sample_freq * 0.5 * k / nbr_test_lat + 1e-3;
		const double   gd = splitter.compute_group_delay (float (f));
		latency = std::max (latency, gd);
	}
	const double   latency_ms = latency * 1000.0 / sample_freq;
	printf (
		"\nGroup delay: %.1f ms (%.1f samples @ %.0f Hz)\n",
		latency_ms, latency, sample_freq
	);

	std::vector <float>  dst (len);
	std::vector <std::vector <float> > band_dst (nbr_bands);
	for (auto &v : band_dst)
	{
		v.resize (len);
	}

	int            pos = 0;
	do
	{
		const int      nbr_spl = std::min (len - pos, buf_size);
		if (block_flag)
		{
			splitter.process_block_split (&src [pos], nbr_spl);
			splitter.process_block_merge (&dst [pos], nbr_spl);
		}
		else
		{
			for (int k = 0; k < nbr_spl; ++k)
			{
				splitter.process_sample_split (src [pos + k]);
				dst [pos + k] = splitter.process_sample_merge ();
			}
		}

		for (int band_cnt = 0; band_cnt < nbr_bands; ++band_cnt)
		{
			mfx::dsp::mix::Generic::copy_1_1 (
				&band_dst [band_cnt] [pos], buf_arr [band_cnt].data (), nbr_spl
			);
		}

		pos += nbr_spl;
	}
	while (pos < len);
	
	if (save_flag)
	{
		char           pathname_0 [1023+1];
		fstb::snprintf4all (pathname_0, sizeof (pathname_0),
			"results/%s-sum.wav", filename_0
		);
		mfx::FileOpWav::save (pathname_0, dst, sample_freq, 0.5f);
		for (int band_cnt = 0; band_cnt < nbr_bands; ++band_cnt)
		{
			fstb::snprintf4all (pathname_0, sizeof (pathname_0),
				"results/%s-%02d.wav", filename_0, band_cnt
			);
			mfx::FileOpWav::save (pathname_0, band_dst [band_cnt], sample_freq, 0.5f);
		}
	}

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Speed test

	TimerAccurate  tim;
	tim.reset ();

	tim.start ();
	pos = 0;
	if (block_flag)
	{
		do
		{
			const int      nbr_spl = std::min (len - pos, buf_size);
			splitter.process_block_split (&src [pos], nbr_spl);
			splitter.process_block_merge (&dst [pos], nbr_spl);
			pos += nbr_spl;
		}
		while (pos < len);
	}
	else
	{
		do
		{
			splitter.process_sample_split (src [pos]);
			dst [pos] = splitter.process_sample_merge ();
			++ pos;
		}
		while (pos < len);
	}
	tim.stop ();

	// Makes sure the results are not optimized out by the compiler
	double         p = 0;
	for (int band_cnt = 0; band_cnt < nbr_bands; ++band_cnt)
	{
		p += buf_arr [band_cnt] [0] +  buf_arr [band_cnt] [1];
	}

	const double   spl_per_s = tim.get_best_rate (len) + p * 1e-300;
	const double   kilo_sps  = spl_per_s / 1e3;
	const double   rt_mul    = spl_per_s / sample_freq;
	printf ("Speed: %12.3f kspl/s (x%.3f real-time).\n", kilo_sps, rt_mul);

	printf ("Done.\n\n");

	return ret_val;
}



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
