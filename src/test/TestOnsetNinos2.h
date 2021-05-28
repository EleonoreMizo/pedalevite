/*****************************************************************************

        TestOnsetNinos2.h
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestOnsetNinos2_HEADER_INCLUDED)
#define TestOnsetNinos2_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



class TestOnsetNinos2
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

	               TestOnsetNinos2 ()                               = delete;
	               TestOnsetNinos2 (const TestOnsetNinos2 &other)   = delete;
	               TestOnsetNinos2 (TestOnsetNinos2 &&other)        = delete;
	TestOnsetNinos2 &
	               operator = (const TestOnsetNinos2 &other)        = delete;
	TestOnsetNinos2 &
	               operator = (TestOnsetNinos2 &&other)             = delete;
	bool           operator == (const TestOnsetNinos2 &other) const = delete;
	bool           operator != (const TestOnsetNinos2 &other) const = delete;

}; // class TestOnsetNinos2



//#include "test/TestOnsetNinos2.hpp"



#endif   // TestOnsetNinos2_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
