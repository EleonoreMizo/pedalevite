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
#include "mfx/FileOpWav.h"
#include "test/TestSplitThiele8.h"
#include "test/TimerAccurate.h"

#include <vector>

#include <cassert>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestSplitThiele8::perform_test ()
{
	int            ret_val = 0;

	printf ("Testing mfx::dsp::iir::SplitThiele8...\n");

	const double   sample_freq = 44100.0;   // Sample frequency, base rate (decimated)
	const int      len         = fstb::round_int (sample_freq * 10);

	std::vector <float>  src (len);
	mfx::dsp::osc::SweepingSin ssin (sample_freq, 20.0, 20000.0);
	ssin.generate (src.data (), len);

	std::vector <float>  dst_lo (len);
	std::vector <float>  dst_hi (len);

	mfx::dsp::iir::SplitThiele8 xover;
	xover.set_sample_freq (sample_freq);
	xover.set_split_freq (1000);
	xover.set_thiele_coef (0.9f);
	xover.update_coef ();
	xover.clear_buffers ();

	TimerAccurate  tim;
	tim.reset ();
	tim.start ();

	xover.process_block_split (dst_lo.data (), dst_hi.data (), src.data (), len);

	tim.stop ();
	const double      spl_per_s = tim.get_best_rate (len);

	mfx::FileOpWav::save ("results/SplitThiele8-lo.wav", dst_lo, sample_freq, 0.5f);
	mfx::FileOpWav::save ("results/SplitThiele8-hi.wav", dst_hi, sample_freq, 0.5f);

	const double   kilo_sps  = spl_per_s / 1e3;
	const double   rt_mul    = spl_per_s / sample_freq;
	printf ("Speed: %12.3f kspl/s (x%.3f real-time).\n", kilo_sps, rt_mul);

	printf ("Done.\n\n");

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
