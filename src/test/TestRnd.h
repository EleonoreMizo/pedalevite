/*****************************************************************************

        TestRndXoroshiro128p.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestRndXoroshiro128p_HEADER_INCLUDED)
#define TestRndXoroshiro128p_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



class TestRndXoroshiro128p
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static int     perform_test ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TestRndXoroshiro128p ()                               = delete;
	               TestRndXoroshiro128p (const TestRndXoroshiro128p &other) = delete;
	               TestRndXoroshiro128p (TestRndXoroshiro128p &&other)      = delete;
	               ~TestRndXoroshiro128p ()                              = delete;
	TestRndXoroshiro128p &
	               operator = (const TestRndXoroshiro128p &other)        = delete;
	TestRndXoroshiro128p &
	               operator = (TestRndXoroshiro128p &&other)             = delete;
	bool           operator == (const TestRndXoroshiro128p &other) const = delete;
	bool           operator != (const TestRndXoroshiro128p &other) const = delete;

}; // class TestRndXoroshiro128p



//#include "test/TestRndXoroshiro128p.hpp"



#endif   // TestRndXoroshiro128p_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
