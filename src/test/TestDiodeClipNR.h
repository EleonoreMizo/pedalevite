/*****************************************************************************

        TestDiodeClipNR.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestDiodeClipNR_HEADER_INCLUDED)
#define TestDiodeClipNR_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <vector>



class TestDiodeClipNR
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

	               TestDiodeClipNR ()                               = delete;
	               TestDiodeClipNR (const TestDiodeClipNR &other) = delete;
	               TestDiodeClipNR (TestDiodeClipNR &&other)      = delete;
	               ~TestDiodeClipNR ()                              = delete;
	TestDiodeClipNR &
	               operator = (const TestDiodeClipNR &other)        = delete;
	TestDiodeClipNR &
	               operator = (TestDiodeClipNR &&other)             = delete;
	bool           operator == (const TestDiodeClipNR &other) const = delete;
	bool           operator != (const TestDiodeClipNR &other) const = delete;

}; // class TestDiodeClipNR



//#include "test/TestDiodeClipNR.hpp"



#endif   // TestDiodeClipNR_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

