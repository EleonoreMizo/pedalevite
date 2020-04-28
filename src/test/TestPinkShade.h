/*****************************************************************************

        TestPinkShade.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestPinkShade_HEADER_INCLUDED)
#define TestPinkShade_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



class TestPinkShade
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

	               TestPinkShade ()                               = delete;
	               TestPinkShade (const TestPinkShade &other)     = delete;
	               TestPinkShade (TestPinkShade &&other)          = delete;
	               ~TestPinkShade ()                              = delete;
	TestPinkShade &
	               operator = (const TestPinkShade &other)        = delete;
	TestPinkShade &
	               operator = (TestPinkShade &&other)             = delete;
	bool           operator == (const TestPinkShade &other) const = delete;
	bool           operator != (const TestPinkShade &other) const = delete;

}; // class TestPinkShade



//#include "test/TestPinkShade.hpp"



#endif   // TestPinkShade_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
