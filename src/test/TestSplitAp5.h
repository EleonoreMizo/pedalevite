/*****************************************************************************

        TestSplitAp5.h
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestSplitAp5_HEADER_INCLUDED)
#define TestSplitAp5_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



class TestSplitAp5
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

	               TestSplitAp5 ()                               = delete;
	               TestSplitAp5 (const TestSplitAp5 &other)      = delete;
	               TestSplitAp5 (TestSplitAp5 &&other)           = delete;
	TestSplitAp5 & operator = (const TestSplitAp5 &other)        = delete;
	TestSplitAp5 & operator = (TestSplitAp5 &&other)             = delete;
	bool           operator == (const TestSplitAp5 &other) const = delete;
	bool           operator != (const TestSplitAp5 &other) const = delete;

}; // class TestSplitAp5



//#include "test/TestSplitAp5.hpp"



#endif   // TestSplitAp5_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
