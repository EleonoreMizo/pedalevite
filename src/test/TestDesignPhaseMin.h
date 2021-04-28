/*****************************************************************************

        TestDesignPhaseMin.h
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestDesignPhaseMin_HEADER_INCLUDED)
#define TestDesignPhaseMin_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <vector>



class TestDesignPhaseMin
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static int     perform_test ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static float   fir_process_sample (const std::vector <float> &src, int pos, const float fir_ptr [], int fir_len);



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TestDesignPhaseMin ()                               = delete;
	               TestDesignPhaseMin (const TestDesignPhaseMin &other) = delete;
	               TestDesignPhaseMin (TestDesignPhaseMin &&other)      = delete;
	TestDesignPhaseMin &
	               operator = (const TestDesignPhaseMin &other)        = delete;
	TestDesignPhaseMin &
	               operator = (TestDesignPhaseMin &&other)             = delete;
	bool           operator == (const TestDesignPhaseMin &other) const = delete;
	bool           operator != (const TestDesignPhaseMin &other) const = delete;

}; // class TestDesignPhaseMin



//#include "test/TestDesignPhaseMin.hpp"



#endif   // TestDesignPhaseMin_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
