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



#undef test_TestSplitMultiband_AUTOGEN



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"
#include "mfx/dsp/iir/DesignEq2p.h"
#include "mfx/dsp/iir/SplitMultiband.h"
#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/dsp/mix/Generic.h"
#include "mfx/dsp/osc/SweepingSin.h"
#include "mfx/FileOpWav.h"
#include "test/TestSplitMultiband.h"
#include "test/TimerAccurate.h"

#if defined (test_TestSplitMultiband_AUTOGEN)
	#include "test/TestSplitMultiband_generated.h"
#endif // test_TestSplitMultiband_AUTOGEN

#include <cassert>
#include <cmath>
#include <cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestSplitMultiband::perform_test ()
{
	int            ret_val = 0;

	if (ret_val == 0)
	{
		ret_val = perform_test_class <
			mfx::dsp::iir::SplitMultiband <float, 2, 3>
		> (
			"mfx::dsp::iir::SplitMultiband",
			"splitmultiband",
			11,
			false, // block_flag
			true,  // save_flag
			[] (auto &s, int nbr_bands, float * const ptr_arr [])
			{
				s.set_nbr_bands (nbr_bands, ptr_arr);
			}
		);
	}

#if defined (test_TestSplitMultiband_AUTOGEN)
	if (ret_val == 0)
	{
		ret_val = perform_test_class <
			SplitMultibandSimd
		> (
			"SplitMultibandSimd",
			"splitmultibandsimd",
			SplitMultibandSimd::_nbr_bands,
			false, // block_flag
			true,  // save_flag
			[] (auto &s, int /*nbr_bands*/, float * const ptr_arr [])
			{
				s.set_band_ptr (ptr_arr);
			}
		);
	}
#endif // test_TestSplitMultiband_AUTOGEN

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class S, typename FB>
int	TestSplitMultiband::perform_test_class (const char *classname_0, const char *filename_0, int nbr_bands, bool block_flag, bool save_flag, FB set_band_fnc)
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
	constexpr int  order_ap0 = S::_nbr_2p_0 * 2 + S::_nbr_1p_0;
	constexpr int  order_ap1 = S::_nbr_2p_1 * 2 + S::_nbr_1p_1;

	typedef std::array <float, buf_size> Buffer;
	std::vector <Buffer> buf_arr (nbr_bands);

	std::vector <float *> band_ptr_arr (nbr_bands);
	for (int k = 0; k < nbr_bands; ++k)
	{
		band_ptr_arr [k] = buf_arr [k].data ();
	}

	set_band_fnc (splitter, nbr_bands, band_ptr_arr.data ());

	// Filter design
	printf ("Split frequencies: ");
	std::vector <std::array <float, 2> > eq0_arr;
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
		eq0_arr.push_back (eq_0);
	}
	printf ("\n");

	// Group delay
	double         group_delay  = 0;
	constexpr int  nbr_test_lat = 100;
	for (int k = 0; k < nbr_test_lat; ++k)
	{
		const double   f    = sample_freq * 0.5 * k / nbr_test_lat + 1e-3;
		double         gd_f = 0;
		for (const auto &eq0 : eq0_arr)
		{
			const std::array <float, 3> bz { eq0 [0], eq0 [1],       1 };
			const std::array <float, 3> az {       1, eq0 [1], eq0 [0] };
			const auto     gd_split = mfx::dsp::iir::DesignEq2p::compute_group_delay (
				bz.data (), az.data (), sample_freq, f
			);
			gd_f += gd_split;
		}
		group_delay = std::max (group_delay, gd_f);
	}
	const double   group_delay_ms = group_delay * 1000.0 / sample_freq;
	printf (
		"Group delay: %.1f ms (%.1f samples @ %.0f Hz)\n",
		group_delay_ms, group_delay, sample_freq
	);

	// Tests
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
			splitter.process_block (&src [pos], nbr_spl);
		}
		else
		{
			for (int k = 0; k < nbr_spl; ++k)
			{
				splitter.process_sample (src [pos + k]);
				splitter.offset_band_ptr (1);
			}
			splitter.offset_band_ptr (-nbr_spl);
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

	mfx::dsp::mix::Generic::copy_1_1 (dst.data (), band_dst [0].data(), len);
	for (int band_cnt = 1; band_cnt < nbr_bands; ++band_cnt)
	{
		mfx::dsp::mix::Generic::mix_1_1 (
			dst.data (), band_dst [band_cnt].data(), len
		);
	}

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
			splitter.process_block (&src [pos], nbr_spl);
			pos += nbr_spl;
		}
		while (pos < len);
	}
	else
	{
		assert ((len & 1) == 0);
		do
		{
			splitter.process_sample (src [pos]);
			splitter.offset_band_ptr (1);
			++ pos;
			splitter.process_sample (src [pos]);
			splitter.offset_band_ptr (-1);
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
