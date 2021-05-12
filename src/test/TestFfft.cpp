/*****************************************************************************

        TestFfft.cpp
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

#include	"ffft/test/conf.h"
#include	"ffft/test/TestHelperFixLen.h"
#include	"ffft/test/TestHelperNormal.h"
#include "test/TestFfft.h"

#include <cassert>
#include <cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestFfft::perform_test ()
{
	int            ret_val = 0;

	printf ("Testing ffft...\n");

	if (ret_val == 0)
	{
		ret_val = perform_test_fnc ();
	}
	if (ret_val == 0)
	{
		ret_val = perform_test_spd ();
	}

	printf ("Done.\n\n");

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestFfft::perform_test_fnc ()
{
   int            ret_val = 0;

	ffft::test::TestHelperNormal <float >::perform_test_accuracy (ret_val);
	ffft::test::TestHelperNormal <double>::perform_test_accuracy (ret_val);

   ffft::test::TestHelperFixLen < 1>::perform_test_accuracy (ret_val);
   ffft::test::TestHelperFixLen < 2>::perform_test_accuracy (ret_val);
   ffft::test::TestHelperFixLen < 3>::perform_test_accuracy (ret_val);
   ffft::test::TestHelperFixLen < 4>::perform_test_accuracy (ret_val);
   ffft::test::TestHelperFixLen < 7>::perform_test_accuracy (ret_val);
   ffft::test::TestHelperFixLen < 8>::perform_test_accuracy (ret_val);
   ffft::test::TestHelperFixLen <10>::perform_test_accuracy (ret_val);
   ffft::test::TestHelperFixLen <12>::perform_test_accuracy (ret_val);
   ffft::test::TestHelperFixLen <13>::perform_test_accuracy (ret_val);

	return ret_val;
}



int	TestFfft::perform_test_spd ()
{
   int            ret_val = 0;

#if defined (ffft_test_SPEED_TEST_ENABLED)

	ffft::test::TestHelperNormal <float >::perform_test_speed (ret_val);
	ffft::test::TestHelperNormal <double>::perform_test_speed (ret_val);

   ffft::test::TestHelperFixLen < 1>::perform_test_speed (ret_val);
   ffft::test::TestHelperFixLen < 2>::perform_test_speed (ret_val);
   ffft::test::TestHelperFixLen < 3>::perform_test_speed (ret_val);
   ffft::test::TestHelperFixLen < 4>::perform_test_speed (ret_val);
   ffft::test::TestHelperFixLen < 7>::perform_test_speed (ret_val);
   ffft::test::TestHelperFixLen < 8>::perform_test_speed (ret_val);
   ffft::test::TestHelperFixLen <10>::perform_test_speed (ret_val);
   ffft::test::TestHelperFixLen <12>::perform_test_speed (ret_val);
   ffft::test::TestHelperFixLen <14>::perform_test_speed (ret_val);
   ffft::test::TestHelperFixLen <16>::perform_test_speed (ret_val);
   ffft::test::TestHelperFixLen <20>::perform_test_speed (ret_val);

#endif

   return (ret_val);
}



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
