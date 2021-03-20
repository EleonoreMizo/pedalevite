/*****************************************************************************

        TestOscSinCosStable.cpp
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

#include "mfx/dsp/osc/OscSinCosStable.h"
#include "test/TestOscSinCosStable.h"
#include "test/TimerAccurate.h"

#include <array>

#include <cassert>
#include <cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestOscSinCosStable::perform_test ()
{
	int            ret_val = 0;

	printf ("Testing mfx::dsp::osc::OscSinCosStable...\n");

	constexpr double  sample_freq = 44100; // Hz
	constexpr int     len         = 100 * 1000 * 1000;

	// Tested angles, rad
	std::array <float, 4> angle_arr {{ 1e-6f, 1e-3f, 0.1f, 2.f }};

	TimerAccurate  tim;

	double         spl_per_s = 0;
	for (auto angle : angle_arr)
	{
		printf ("Angle: %10g rad/spl... ", double (angle));
		fflush (stdout);

		mfx::dsp::osc::OscSinCosStable <float> osc;
		osc.clear_buffers ();
		osc.set_phase (0);
		osc.set_step (angle);

		tim.reset ();
		tim.start ();

		for (int pos = 0; pos < len; ++pos)
		{
			osc.step ();
		}

		tim.stop ();
		if (spl_per_s == 0)
		{
			spl_per_s = tim.get_best_rate (len);
		}

		const float    s = osc.get_sin ();
		const float    c = osc.get_cos ();
		const float    m = sqrtf (s * s + c * c);
		printf ("module = %8f after %d iterations.\n", m, len);
	}

	const double   kilo_sps  = spl_per_s / 1e3;
	const double   rt_mul    = spl_per_s / sample_freq;
	printf ("Speed: %12.3f kspl/s (x%.3f real-time).\n", kilo_sps, rt_mul);

	printf ("Done.\n\n");

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
