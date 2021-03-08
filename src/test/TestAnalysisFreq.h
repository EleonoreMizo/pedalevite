/*****************************************************************************

        TestAnalysisFreq.h
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestAnalysisFreq_HEADER_INCLUDED)
#define TestAnalysisFreq_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



class TestAnalysisFreq
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

	               TestAnalysisFreq ()                               = delete;
	               TestAnalysisFreq (const TestAnalysisFreq &other)  = delete;
	               TestAnalysisFreq (TestAnalysisFreq &&other)       = delete;
	TestAnalysisFreq &
	               operator = (const TestAnalysisFreq &other)        = delete;
	TestAnalysisFreq &
	               operator = (TestAnalysisFreq &&other)             = delete;
	bool           operator == (const TestAnalysisFreq &other) const = delete;
	bool           operator != (const TestAnalysisFreq &other) const = delete;

}; // class TestAnalysisFreq



//#include "test/TestAnalysisFreq.hpp"



#endif   // TestAnalysisFreq_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
