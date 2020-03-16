/*****************************************************************************

        TestDiodeClipScreamer.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestDiodeClipScreamer_HEADER_INCLUDED)
#define TestDiodeClipScreamer_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <vector>



class TestDiodeClipScreamer
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

	               TestDiodeClipScreamer ()                               = delete;
	               TestDiodeClipScreamer (const TestDiodeClipScreamer &other) = delete;
	               TestDiodeClipScreamer (TestDiodeClipScreamer &&other)      = delete;
	               ~TestDiodeClipScreamer ()                              = delete;
	TestDiodeClipScreamer &
	               operator = (const TestDiodeClipScreamer &other)        = delete;
	TestDiodeClipScreamer &
	               operator = (TestDiodeClipScreamer &&other)             = delete;
	bool           operator == (const TestDiodeClipScreamer &other) const = delete;
	bool           operator != (const TestDiodeClipScreamer &other) const = delete;

}; // class TestDiodeClipScreamer



//#include "test/TestDiodeClipScreamer.hpp"



#endif   // TestDiodeClipScreamer_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

