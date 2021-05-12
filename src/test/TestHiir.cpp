/*****************************************************************************

        TestHiir.cpp
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

#include "hiir/test/conf.h"
#include "hiir/test/TestAllClassesFnc.h"
#include "hiir/test/TestAllClassesSpeed.h"
#include "test/TestHiir.h"

#include <cassert>
#include <cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestHiir::perform_test ()
{
	int            ret_val = 0;

	printf ("Testing hiir...\n");

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



int	TestHiir::perform_test_fnc ()
{
	int            ret_val = 0;

#if defined (hiir_test_LONG_FUNC_TESTS)
	hiir::test::TestAllClassesFnc < 1>::perform_test_r (ret_val, 0.1);
	hiir::test::TestAllClassesFnc < 2>::perform_test_r (ret_val, 0.1);
	hiir::test::TestAllClassesFnc < 3>::perform_test_r (ret_val, 0.05);
	hiir::test::TestAllClassesFnc < 4>::perform_test_r (ret_val, 0.05);
	hiir::test::TestAllClassesFnc < 5>::perform_test_r (ret_val, 0.02);
	hiir::test::TestAllClassesFnc < 6>::perform_test_r (ret_val, 0.02);
	hiir::test::TestAllClassesFnc < 7>::perform_test_r (ret_val, 0.01);
#endif   // hiir_test_LONG_FUNC_TESTS

	hiir::test::TestAllClassesFnc < 8>::perform_test_r (ret_val, 0.01);

#if defined (hiir_test_LONG_FUNC_TESTS)
	hiir::test::TestAllClassesFnc < 1>::perform_test_r (ret_val, 0.49);
	hiir::test::TestAllClassesFnc <32>::perform_test_r (ret_val, 0.0001);
#endif   // hiir_test_LONG_FUNC_TESTS

	return ret_val;
}



int	TestHiir::perform_test_spd ()
{
	int            ret_val = 0;

	hiir::test::TestAllClassesSpeed < 1>::perform_test ();
	hiir::test::TestAllClassesSpeed < 2>::perform_test ();
	hiir::test::TestAllClassesSpeed < 3>::perform_test ();
	hiir::test::TestAllClassesSpeed < 4>::perform_test ();
	hiir::test::TestAllClassesSpeed < 8>::perform_test ();

#if defined (hiir_test_LONG_SPEED_TESTS)
	// If you enable this section, it may take much longer
	// to compile because of recursive template expansion.
	hiir::test::TestAllClassesSpeed <12>::perform_test ();
	hiir::test::TestAllClassesSpeed <16>::perform_test ();
	hiir::test::TestAllClassesSpeed <24>::perform_test ();
	hiir::test::TestAllClassesSpeed <32>::perform_test ();
#endif   // hiir_test_LONG_SPEED_TESTS

	return ret_val;
}



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
