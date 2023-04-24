/*****************************************************************************

        TestFstbFnc.cpp
        Author: Laurent de Soras, 2023

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law.You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"
#include "test/TestFstbFnc.h"

#include <cassert>
#include <cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestFstbFnc::perform_test ()
{
	int            ret_val = 0;

	printf ("Testing fstb functions...\n");

	if (ret_val == 0)
	{
		ret_val = test_get_x_pow_2 ();
	}

	if (ret_val == 0)
	{
		printf ("Done.\n");
	}

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestFstbFnc::test_get_x_pow_2 ()
{
	printf ("get_prev_pow_2() / get_next_pow_2()...\n");

	constexpr auto one  = uint64_t (1);
	constexpr auto last = (one << 32) - 1;

	for (uint64_t ux = 1; ux <= last; ++ux)
	{
		const auto     x   = uint32_t (ux);

		const auto     p   = fstb::get_prev_pow_2 (x);
		if (p < 0 || (one << p) > ux || (one << (p + 1)) <= ux)
		{
			printf ("*** Error: get_prev_pow_2 (0x%X) returned %d\n", x, p);
			return -1;
		}

		const auto     n   = fstb::get_next_pow_2 (x);
		if (n < 0 || (one << n) < ux || (one << n) >= (ux << 1))
		{
			printf ("*** Error: get_next_pow_2 (0x%X) returned %d\n", x, n);
			return -1;
		}
	}

	return 0;
}



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
