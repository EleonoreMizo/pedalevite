/*****************************************************************************

        TestRndXoroshiro128p.cpp
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

#include "fstb/RndXoroshiro128p.h"
#include "test/TestRndXoroshiro128p.h"
#include "test/TimerAccurate.h"

#include <cassert>
#include <cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestRndXoroshiro128p::perform_test ()
{
	int            ret_val = 0;

	printf ("Testing fstb::RndXoroshiro128p...\n");

	fstb::RndXoroshiro128p  rndgen;

	rndgen.jump_2_96 ();
	rndgen.jump_2_64 ();

	for (int k = 0; k < 16; ++k)
	{
		for (int i = 0; i < 4; ++i)
		{
			const uint64_t x = rndgen.gen_int ();
			printf ("%016llX ", x);
		}
		printf ("\n");
	}

	// Speed test
	constexpr double  sample_freq = 44100;
	constexpr int     len         = 100'000'000;
	TimerAccurate  tim;
	tim.start ();
	for (int k = 0; k < len; ++k)
	{
		rndgen.gen_int ();
	}
	tim.stop ();
	const auto     p = rndgen.gen_flt ();
	
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
