/*****************************************************************************

        TestHardclipBl.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestHardclipBl_HEADER_INCLUDED)
#define TestHardclipBl_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <vector>



class TestHardclipBl
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

	               TestHardclipBl ()                               = delete;
	               TestHardclipBl (const TestHardclipBl &other)    = delete;
	               TestHardclipBl (TestHardclipBl &&other)         = delete;
	               ~TestHardclipBl ()                              = delete;
	TestHardclipBl &
	               operator = (const TestHardclipBl &other)        = delete;
	TestHardclipBl &
	               operator = (TestHardclipBl &&other)             = delete;
	bool           operator == (const TestHardclipBl &other) const = delete;
	bool           operator != (const TestHardclipBl &other) const = delete;

}; // class TestHardclipBl



//#include "test/TestHardclipBl.hpp"



#endif   // TestHardclipBl_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

