/*****************************************************************************

        TestSplitMultiband.cpp
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
#include "mfx/dsp/iir/SplitMultiband.h"
#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/dsp/mix/Generic.h"
#include "mfx/dsp/osc/SweepingSin.h"
#include "mfx/FileOpWav.h"
#include "test/TestSplitMultiband.h"
#include "test/TimerAccurate.h"

#include <cassert>
#include <cmath>
#include <cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestSplitMultiband::perform_test ()
{
	int            ret_val = 0;

	printf ("Testing mfx::dsp::iir::SplitMultiband...\n");

	mfx::dsp::mix::Generic::setup ();

	constexpr double  sample_freq = 44100;
	constexpr int     buf_size    = 64;
	const int         len         = fstb::round_int (sample_freq * 10);

	std::vector <float>  src (len);
	mfx::dsp::osc::SweepingSin ssin (sample_freq, 20.0, 20000.0);
	ssin.generate (src.data (), len);

	constexpr int  order_ap0 = 2;
	constexpr int  order_ap1 = 3;
	mfx::dsp::iir::SplitMultiband <float, order_ap0, order_ap1> splitter;
	constexpr int  nbr_bands = 11;

	typedef std::array <float, buf_size> Buffer;
	std::array <Buffer, nbr_bands> buf_arr;

	std::array <float *, nbr_bands> band_ptr_arr;
	for (int k = 0; k < nbr_bands; ++k)
	{
		band_ptr_arr [k] = buf_arr [k].data ();
	}

	splitter.set_nbr_bands (nbr_bands, band_ptr_arr.data ());

	// Filter design
	printf ("Split frequencies: ");
	for (int split_idx = 0; split_idx < nbr_bands - 1; ++split_idx)
	{
		constexpr double fmin =    20.0;
		constexpr double fmax = 20000.0;
		const double     f    =
			fmin * pow (fmax / fmin, double (split_idx + 1) / nbr_bands);
		printf ("%7.1f ", f);

		constexpr double ka   = 0.6180339887498948482; // 0.5 * (sqrt (5) - 1)
		constexpr double kb   = 1.0;
		constexpr float  x1   = float (ka + kb);
		constexpr float  x2   = float (ka);

		// Bilinear transform
		const float    k = mfx::dsp::iir::TransSZBilin::compute_k_approx (
			float (f / sample_freq)
		);

		float          zb_1o [3];
		float          zb_1e [2];
		float          zb_0 [3];
		mfx::dsp::iir::TransSZBilin::map_s_to_z_ap1_approx (zb_1e,     k);
		mfx::dsp::iir::TransSZBilin::map_s_to_z_ap2_approx (zb_1o, x2, k);
		mfx::dsp::iir::TransSZBilin::map_s_to_z_ap2_approx (zb_0 , x1, k);

		const std::array <float, order_ap0> eq_0 =
		{
			zb_0 [0], zb_0 [1]
		};
		const std::array <float, order_ap1> eq_1 =
		{
			zb_1o [0], zb_1o [1],
			zb_1e [0]
		};

		splitter.set_splitter_coef (split_idx, eq_0.data (), eq_1.data ());
	}
	printf ("\n");

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

	mfx::FileOpWav::save ("results/splitmultiband-sum.wav", dst, sample_freq, 0.5f);
	for (int band_cnt = 0; band_cnt < nbr_bands; ++band_cnt)
	{
		char           filename_0 [1023+1];
		fstb::snprintf4all (filename_0, sizeof (filename_0),
			"results/splitmultiband-%02d.wav", band_cnt
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
#if 0
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
