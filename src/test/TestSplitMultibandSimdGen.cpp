/*****************************************************************************

        TestSplitMultibandSimdGen.cpp
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

#include "mfx/dsp/iir/SplitMultibandSimdGen.h"
#include "test/TestSplitMultibandSimdGen.h"

#include <cassert>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestSplitMultibandSimdGen::perform_test ()
{
	int            ret_val = 0;

//	printf ("Testing mfx::dsp::iir::SplitMultibandSimdGen...\n");

// cd "src\test"
// "..\..\build\win\test\DebugWin32\test.exe" > "TestSplitMultiband_generated.h"

	mfx::dsp::iir::SplitMultibandSimdGen a (11, 2, 3, "SplitMultibandSimd", {});

	const auto     r = a.print_code ();
	printf ("// DECLARATIONS ///////////////////\n\n%s\n\n", r._decl.c_str ());
	printf ("// INLINE CODE ////////////////////\n\n%s\n\n", r._cinl.c_str ());
	printf ("// COMPILATION UNIT ///////////////\n\n%s\n\n", r._code.c_str ());

	const std::string m = a.print_state ();
	printf ("/*\n%s*/\n", m.c_str ());

//	printf ("Done.\n\n");

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
