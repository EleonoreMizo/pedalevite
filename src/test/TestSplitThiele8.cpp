/*****************************************************************************

        TestSplitThiele8.cpp
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
#include "mfx/dsp/iir/SplitThiele8.h"
#include "mfx/dsp/osc/SweepingSin.h"
#include "mfx/dsp/mix/Generic.h"
#include "mfx/FileOpWav.h"
#include "test/TestSplitThiele8.h"
#include "test/TimerAccurate.h"

#include <algorithm>
#include <vector>

#include <cassert>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestSplitThiele8::perform_test ()
{
	int            ret_val = 0;

	printf ("Testing mfx::dsp::iir::SplitThiele8...\n");

	mfx::dsp::mix::Generic::setup ();

	const double   sample_freq = 44100.0;   // Sample frequency, base rate (decimated)
	const int      len         = fstb::round_int (sample_freq * 10);

	std::vector <float>  src (len);
	mfx::dsp::osc::SweepingSin ssin (sample_freq, 20.0, 20000.0);
	ssin.generate (src.data (), len);

	std::vector <float>  dst_lo (len);
	std::vector <float>  dst_hi (len);
	std::vector <float>  dst_cp (len);

	mfx::dsp::iir::SplitThiele8 xover;
	xover.set_sample_freq (sample_freq);
	xover.set_split_freq (1000);
	xover.set_thiele_coef (0.9f);
	xover.update_coef ();
	xover.clear_buffers ();

	// Main processing
	xover.process_block_split (dst_lo.data (), dst_hi.data (), src.data (), len);

	// Compensates and subtracts. The result should be 0.
	xover.process_block_compensate (dst_cp.data (), src.data (), len);
	mfx::dsp::mix::Generic::mix_2_1_v (dst_cp.data (), dst_lo.data (), dst_hi.data (), len, -1);
	double         sum   = 0;
	std::for_each (dst_cp.begin (), dst_cp.end (), [&sum] (double x) { sum += x * x; });
	const double   dif_rms = sqrt (sum / len);
	// Arbitrary threshold, must take accumulation of num. errors into account.
	if (dif_rms > 1e-6)
	{
		mfx::FileOpWav::save (
			"results/splitthiele8-comp_dif.wav", dst_cp, sample_freq, 0.5f
		);
		printf (
			"\n*** Error: "
			"phase-compensated signal does not match the sum. ***\n\n"
		);
		ret_val = -1;
	}

	mfx::FileOpWav::save ("results/splitthiele8-lo.wav", dst_lo, sample_freq, 0.5f);
	mfx::FileOpWav::save ("results/splitthiele8-hi.wav", dst_hi, sample_freq, 0.5f);

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

	TimerAccurate  tim;
	tim.reset ();

#if defined (NDEBUG)
	const int      nbr_passes = 64;
#else
	const int      nbr_passes = 1;
#endif

	tim.start ();
	for (int p = 0; p < nbr_passes; ++p)
	{
		xover.process_block_split (dst_lo.data (), dst_hi.data (), src.data (), len);
		tim.stop_lap ();
	}

	const double      spl_per_s = tim.get_best_rate (len);

	const double   kilo_sps  = spl_per_s / 1e3;
	const double   rt_mul    = spl_per_s / sample_freq;
	printf ("Speed: %12.3f kspl/s (x%.3f real-time).\n", kilo_sps, rt_mul);

	printf ("Done.\n\n");

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
