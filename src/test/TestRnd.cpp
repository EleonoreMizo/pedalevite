/*****************************************************************************

        TestRnd.cpp
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

#include "fstb/RndPcgXshRr6432.h"
#include "fstb/RndXoroshiro128p.h"
#include "test/TestRnd.h"
#include "test/TimerAccurate.h"

#include <cassert>
#include <cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestRnd::perform_test ()
{
	int            ret_val = 0;

	constexpr double  sample_freq = 44100;
	constexpr int     len         = 500'000'000;
	TimerAccurate  tim;

	{
		printf ("Testing fstb::RndXoroshiro128p...\n");

		fstb::RndXoroshiro128p  rndgen;

		rndgen.jump_2_96 ();
		rndgen.jump_2_64 ();

		for (int k = 0; k < 16; ++k)
		{
			for (int i = 0; i < 4; ++i)
			{
				const uint64_t x = rndgen.gen_int ();
				printf ("%016llX ", static_cast <long long> (x));
			}
			printf ("\n");
		}

		// Speed test
		uint64_t       sum = 0;
		tim.reset ();
		tim.start ();
		for (int k = 0; k < len; ++k)
		{
			sum ^= rndgen.gen_int ();
		}
		tim.stop ();

		const double   spl_per_s = tim.get_best_rate (len) + double (sum >> 56) * 1e-300;
		const double   mega_sps  = spl_per_s / 1e6;
		const double   rt_mul    = spl_per_s / sample_freq;
		printf ("Speed: %12.3f Mspl/s (x%.3f real-time).\n", mega_sps, rt_mul);
	}

	// -  -  -  -

	{
		printf ("Testing fstb::RndPcgXshRr6432...\n");
		fstb::RndPcgXshRr6432   rndgen;

		uint32_t       sum = 0;
		tim.reset ();
		tim.start ();
		for (int k = 0; k < len; ++k)
		{
			sum ^= rndgen.gen_int ();
		}
		tim.stop ();

		const double   spl_per_s = tim.get_best_rate (len) + double (sum >> 24) * 1e-300;
		const double   mega_sps  = spl_per_s / 1e6;
		const double   rt_mul    = spl_per_s / sample_freq;
		printf ("Speed: %12.3f Mspl/s (x%.3f real-time).\n", mega_sps, rt_mul);
	}

	printf ("Done.\n\n");

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
