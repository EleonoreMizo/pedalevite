/*****************************************************************************

        TestSplitMultibandLin.cpp
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
#include "mfx/dsp/iir/SplitMultibandLin.h"
#include "mfx/dsp/mix/Generic.h"
#include "mfx/dsp/osc/SweepingSin.h"
#include "mfx/FileOpWav.h"
#include "test/TestSplitMultibandLin.h"
#include "test/TimerAccurate.h"

#include <cassert>
#include <cmath>
#include <cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestSplitMultibandLin::perform_test ()
{
	int            ret_val = 0;

	printf ("Testing mfx::dsp::iir::SplitMultibandLin...\n");

	mfx::dsp::mix::Generic::setup ();

	constexpr double  sample_freq = 44100;
	constexpr int     buf_size    = 64;
	const int         len         = fstb::round_int (sample_freq * 10);

	std::vector <float>  src (len);
	mfx::dsp::osc::SweepingSin ssin (sample_freq, 20.0, 20000.0);
	ssin.generate (src.data (), len);

	constexpr int  order     = 4;
	constexpr int  nbr_biq   = order / 2;
	constexpr int  nbr_onep  = order - nbr_biq * 2; // Not used actually
	mfx::dsp::iir::SplitMultibandLin <float, order> splitter;
	constexpr int  nbr_bands = 11;

	typedef std::array <float, buf_size> Buffer;
	std::array <Buffer, nbr_bands> buf_arr;

	std::array <float *, nbr_bands> band_ptr_arr;
	for (int k = 0; k < nbr_bands; ++k)
	{
		band_ptr_arr [k] = buf_arr [k].data ();
	}

	splitter.set_sample_freq (sample_freq);
	splitter.set_nbr_bands (nbr_bands, band_ptr_arr.data ());

	// Filter design
	printf ("Target frequencies: ");
	constexpr double  thiele_k = 0.0f;
	const double      k2 = thiele_k * thiele_k;
	const double      k4 = k2 * k2;
	const double      d0 = 0.5 *      (4 - k2);
	const double      d1 = 0.5 * sqrt (8 + k4);
	const float       r1 = float (sqrt (d0 + d1));
	const float       r2 = float (sqrt (d0 - d1));
	const std::array <float, (nbr_biq * 3 + nbr_onep * 2) * 2> coef_arr =
	{
		1, 0, float (k2), // Biq 0, num
		1, r1, 1,         // Biq 0, den
		1, 0, 0,          // Biq 1, num
		1, r2, 1          // Biq 1, den
	};
	for (int split_idx = 0; split_idx < nbr_bands - 1; ++split_idx)
	{
		constexpr double fmin =    20.0;
		constexpr double fmax = 20000.0;
		const double     f    =
			fmin * pow (fmax / fmin, double (split_idx + 1) / nbr_bands);
		printf ("%7.1f ", f);
		splitter.set_splitter_coef (split_idx, float (f), coef_arr.data (), 0);
	}
	printf ("\nActual frequencies: ");
	for (int split_idx = 0; split_idx < nbr_bands - 1; ++split_idx)
	{
		const double   f = splitter.get_actual_xover_freq (split_idx);
		printf ("%7.1f ", f);
	}
	const int      latency = splitter.get_global_delay ();
	const double   latency_ms = latency * 1000.0 / sample_freq;
	printf (
		"\nGlobal delay: %.1f ms (%d samples @ %.0f Hz)\n",
		latency_ms, latency, sample_freq
	);

	std::vector <float>  dst (len);
	std::array <std::vector <float>, nbr_bands> band_dst;
	for (auto &v : band_dst)
	{
		v.resize (len);
	}

	int            pos = 0;
	do
	{
		const int      nbr_spl = std::min (len - pos, buf_size);
		splitter.process_block (&src [pos], nbr_spl);
		for (int band_cnt = 0; band_cnt < nbr_bands; ++band_cnt)
		{
			mfx::dsp::mix::Generic::mix_1_1 (
				&band_dst [band_cnt] [pos], buf_arr [band_cnt].data (), nbr_spl
			);
		}

		pos += nbr_spl;
	}
	while (pos < len);

	mfx::dsp::mix::Generic::copy_1_1 (dst.data (), band_dst [0].data(), len);
	for (int band_cnt = 1; band_cnt < nbr_bands; ++band_cnt)
	{
		mfx::dsp::mix::Generic::mix_1_1 (
			dst.data (), band_dst [band_cnt].data(), len
		);
	}

	mfx::FileOpWav::save ("results/splitmultibandlin-sum.wav", dst, sample_freq, 0.5f);
	for (int band_cnt = 0; band_cnt < nbr_bands; ++band_cnt)
	{
		char           filename_0 [1023+1];
		fstb::snprintf4all (filename_0, sizeof (filename_0),
			"results/splitmultibandlin-%02d.wav", band_cnt
		);
		mfx::FileOpWav::save (filename_0, band_dst [band_cnt], sample_freq, 0.5f);
	}

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Speed test

	TimerAccurate  tim;
	tim.reset ();

	tim.start ();
	pos = 0;
	do
	{
#if 1
		const int      nbr_spl = std::min (len - pos, buf_size);
		splitter.process_block (&src [pos], nbr_spl);
		pos += nbr_spl;
#else
		// Processing samples is faster
		splitter.process_sample (src [pos]);
		splitter.offset_band_ptr (1);
		++ pos;
		splitter.process_sample (src [pos]);
		splitter.offset_band_ptr (-1);
		++ pos;
#endif
	}
	while (pos < len);
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



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
