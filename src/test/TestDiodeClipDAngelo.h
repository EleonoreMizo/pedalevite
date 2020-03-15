/*****************************************************************************

        TestDiodeClipDAngelo.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestDiodeClipDAngelo_HEADER_INCLUDED)
#define TestDiodeClipDAngelo_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <vector>



class TestDiodeClipDAngelo
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

	               TestDiodeClipDAngelo ()                               = delete;
	               TestDiodeClipDAngelo (const TestDiodeClipDAngelo &other) = delete;
	               TestDiodeClipDAngelo (TestDiodeClipDAngelo &&other)      = delete;
	               ~TestDiodeClipDAngelo ()                              = delete;
	TestDiodeClipDAngelo &
	               operator = (const TestDiodeClipDAngelo &other)        = delete;
	TestDiodeClipDAngelo &
	               operator = (TestDiodeClipDAngelo &&other)             = delete;
	bool           operator == (const TestDiodeClipDAngelo &other) const = delete;
	bool           operator != (const TestDiodeClipDAngelo &other) const = delete;

}; // class TestDiodeClipDAngelo



//#include "test/TestDiodeClipDAngelo.hpp"



#endif   // TestDiodeClipDAngelo_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

