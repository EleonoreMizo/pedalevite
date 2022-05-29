/*****************************************************************************

        TestOscSinCosStableSimd.h
        Author: Laurent de Soras, 2022

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law.You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestOscSinCosStableSimd_HEADER_INCLUDED)
#define TestOscSinCosStableSimd_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



class TestOscSinCosStableSimd
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static int     perform_test ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	static int     test_block_size ();
	static int     test_stability_speed ();



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TestOscSinCosStableSimd ()                         = delete;
	               TestOscSinCosStableSimd (const TestOscSinCosStableSimd &other) = delete;
	               TestOscSinCosStableSimd (TestOscSinCosStableSimd &&other) = delete;
	TestOscSinCosStableSimd &
	               operator = (const TestOscSinCosStableSimd &other)  = delete;
	TestOscSinCosStableSimd &
	               operator = (TestOscSinCosStableSimd &&other)       = delete;
	bool           operator == (const TestOscSinCosStableSimd &other) const = delete;
	bool           operator != (const TestOscSinCosStableSimd &other) const = delete;

}; // class TestOscSinCosStableSimd



//#include "test/TestOscSinCosStableSimd.hpp"



#endif // TestOscSinCosStableSimd_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
