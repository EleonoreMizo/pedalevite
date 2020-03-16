/*****************************************************************************

        TestDiodeClipJcm.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestDiodeClipJcm_HEADER_INCLUDED)
#define TestDiodeClipJcm_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <vector>



class TestDiodeClipJcm
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static int     perform_test ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static void    gen_saw (std::vector <float> &data, double sample_freq, double freq, int len);



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TestDiodeClipJcm ()                               = delete;
	               TestDiodeClipJcm (const TestDiodeClipJcm &other) = delete;
	               TestDiodeClipJcm (TestDiodeClipJcm &&other)      = delete;
	               ~TestDiodeClipJcm ()                              = delete;
	TestDiodeClipJcm &
	               operator = (const TestDiodeClipJcm &other)        = delete;
	TestDiodeClipJcm &
	               operator = (TestDiodeClipJcm &&other)             = delete;
	bool           operator == (const TestDiodeClipJcm &other) const = delete;
	bool           operator != (const TestDiodeClipJcm &other) const = delete;

}; // class TestDiodeClipJcm



//#include "test/TestDiodeClipJcm.hpp"



#endif   // TestDiodeClipJcm_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

