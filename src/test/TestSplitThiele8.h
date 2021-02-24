/*****************************************************************************

        TestSplitThiele8.h
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestSplitThiele8_HEADER_INCLUDED)
#define TestSplitThiele8_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



class TestSplitThiele8
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

	               TestSplitThiele8 ()                               = delete;
	               TestSplitThiele8 (const TestSplitThiele8 &other)  = delete;
	               TestSplitThiele8 (TestSplitThiele8 &&other)       = delete;
	TestSplitThiele8 &
	               operator = (const TestSplitThiele8 &other)        = delete;
	TestSplitThiele8 &
	               operator = (TestSplitThiele8 &&other)             = delete;
	bool           operator == (const TestSplitThiele8 &other) const = delete;
	bool           operator != (const TestSplitThiele8 &other) const = delete;

}; // class TestSplitThiele8



//#include "test/TestSplitThiele8.hpp"



#endif   // TestSplitThiele8_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
