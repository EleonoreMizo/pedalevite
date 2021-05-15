/*****************************************************************************

        TestHash.cpp
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

#include "fstb/Hash.h"
#include "test/TestHash.h"
#include "test/TimerAccurate.h"

#include <cassert>
#include <cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestHash::perform_test ()
{
	int            ret_val = 0;

	printf ("Testing fstb::Hash...\n");

	for (int k = 0; k < 10000 && ret_val == 0; ++k)
	{
		uint32_t       v32 = k * k;
		uint64_t       v64 = uint64_t (v32) * uint64_t (v32);

		const uint32_t h32 = fstb::Hash::hash (v32);
		const uint64_t h64 = fstb::Hash::hash (v64);

		const uint32_t t32 = fstb::Hash::hash_inv (h32);
		const uint64_t t64 = fstb::Hash::hash_inv (h64);

		if (t32 != v32)
		{
			printf ("Error for the 32-bit hash/hash_inv.\n");
			assert (false);
			ret_val = -1;
		}
		else if (t64 != v64)
		{
			printf ("Error for the 64-bit hash/hash_inv.\n");
			assert (false);
			ret_val = -1;
		}
	}

	// Speed test
	constexpr double  sample_freq = 44100;
	constexpr int     len         = 100'000'000;
	TimerAccurate  tim;

	{
		tim.start ();
		uint32_t       p32 = 0;
		for (uint32_t k = 0; k < len; ++k)
		{
			p32 += fstb::Hash::hash (k);
		}
		tim.stop ();
	
		const double   spl_per_s = tim.get_best_rate (len) + double (p32 >> 24) * 1e-300;
		const double   mega_sps  = spl_per_s / 1e6;
		const double   rt_mul    = spl_per_s / sample_freq;
		printf ("u32 speed: %12.3f Mspl/s (x%.3f real-time).\n", mega_sps, rt_mul);
	}

	{
		tim.start ();
		uint64_t       p64 = 0;
		for (uint64_t k = 0; k < len; ++k)
		{
			p64 += fstb::Hash::hash (k);
		}
		tim.stop ();
	
		const double   spl_per_s = tim.get_best_rate (len) + double (p64 >> 56) * 1e-300;
		const double   mega_sps  = spl_per_s / 1e6;
		const double   rt_mul    = spl_per_s / sample_freq;
		printf ("u64 speed: %12.3f Mspl/s (x%.3f real-time).\n", mega_sps, rt_mul);
	}

	printf ("Done.\n\n");

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
