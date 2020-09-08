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

	printf ("Done.\n");

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
