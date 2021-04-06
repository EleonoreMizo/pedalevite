/*****************************************************************************

        TestSplitMultibandLin.h
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestSplitMultibandLin_HEADER_INCLUDED)
#define TestSplitMultibandLin_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



class TestSplitMultibandLin
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

	               TestSplitMultibandLin ()                               = delete;
	               TestSplitMultibandLin (const TestSplitMultibandLin &other) = delete;
	               TestSplitMultibandLin (TestSplitMultibandLin &&other)      = delete;
	TestSplitMultibandLin &
	               operator = (const TestSplitMultibandLin &other)        = delete;
	TestSplitMultibandLin &
	               operator = (TestSplitMultibandLin &&other)             = delete;
	bool           operator == (const TestSplitMultibandLin &other) const = delete;
	bool           operator != (const TestSplitMultibandLin &other) const = delete;

}; // class TestSplitMultibandLin



//#include "test/TestSplitMultibandLin.hpp"



#endif   // TestSplitMultibandLin_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
