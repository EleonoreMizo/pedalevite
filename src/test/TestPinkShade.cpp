/*****************************************************************************

        TestPinkShade.cpp
        Author: Laurent de Soras, 2020

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
#include "test/TestPinkShade.h"
#include "mfx/dsp/nz/PinkShade.h"
#include "mfx/FileOpWav.h"
#include "test/TimerAccurate.h"

#include <cassert>
#include <cstdlib>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestPinkShade::perform_test ()
{
	int            ret_val = 0;

	printf ("Testing mfx::dsp::nz::PinkShade...\n");

	const double   sample_freq = 44100;
	const int      len         = fstb::round_int (sample_freq * 10);
	mfx::dsp::nz::PinkShade gen;

	std::vector <float> dst (len);
	gen.process_block (dst.data (), len);

	mfx::FileOpWav::save ("results/pinkshade1.wav", dst, sample_freq, 0.5f);

	// Speed test
	constexpr int  nbr_loops = 100;
	TimerAccurate  tim;

	tim.start ();
	for (int loop_cnt = 0; loop_cnt < nbr_loops; ++loop_cnt)
	{
		gen.process_block (dst.data (), len);
		tim.stop_lap ();
	}

	const auto     p         = dst.back ();
	const double   spl_per_s = tim.get_best_rate (len) + p * 1e-300;
	const double   mega_sps  = spl_per_s / 1e6;
	const double   rt_mul    = spl_per_s / sample_freq;
	printf ("Speed: %12.3f Mspl/s (x%.3f real-time).\n", mega_sps, rt_mul);

	printf ("Done.\n\n");

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
