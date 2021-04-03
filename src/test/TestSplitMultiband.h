/*****************************************************************************

        TestSplitMultiband.h
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestSplitMultiband_HEADER_INCLUDED)
#define TestSplitMultiband_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



class TestSplitMultiband
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

	               TestSplitMultiband ()                               = delete;
	               TestSplitMultiband (const TestSplitMultiband &other) = delete;
	               TestSplitMultiband (TestSplitMultiband &&other)      = delete;
	TestSplitMultiband &
	               operator = (const TestSplitMultiband &other)        = delete;
	TestSplitMultiband &
	               operator = (TestSplitMultiband &&other)             = delete;
	bool           operator == (const TestSplitMultiband &other) const = delete;
	bool           operator != (const TestSplitMultiband &other) const = delete;

}; // class TestSplitMultiband



//#include "test/TestSplitMultiband.hpp"



#endif   // TestSplitMultiband_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
