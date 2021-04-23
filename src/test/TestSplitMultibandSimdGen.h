/*****************************************************************************

        TestSplitMultibandSimdGen.h
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestSplitMultibandSimdGen_HEADER_INCLUDED)
#define TestSplitMultibandSimdGen_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



class TestSplitMultibandSimdGen
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

	               TestSplitMultibandSimdGen ()                               = delete;
	               TestSplitMultibandSimdGen (const TestSplitMultibandSimdGen &other) = delete;
	               TestSplitMultibandSimdGen (TestSplitMultibandSimdGen &&other)      = delete;
	TestSplitMultibandSimdGen &
	               operator = (const TestSplitMultibandSimdGen &other)        = delete;
	TestSplitMultibandSimdGen &
	               operator = (TestSplitMultibandSimdGen &&other)             = delete;
	bool           operator == (const TestSplitMultibandSimdGen &other) const = delete;
	bool           operator != (const TestSplitMultibandSimdGen &other) const = delete;

}; // class TestSplitMultibandSimdGen



//#include "test/TestSplitMultibandSimdGen.hpp"



#endif   // TestSplitMultibandSimdGen_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
