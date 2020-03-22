/*****************************************************************************

        TestSvfAntisat.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestSvfAntisat_HEADER_INCLUDED)
#define TestSvfAntisat_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



class TestSvfAntisat
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static int     perform_test ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	template <class T>
	static int     perform_test (const char classname_0 [], const char filename_0 []);



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TestSvfAntisat ()                               = delete;
	               TestSvfAntisat (const TestSvfAntisat &other)    = delete;
	               TestSvfAntisat (TestSvfAntisat &&other)         = delete;
	virtual        ~TestSvfAntisat ()                              = delete;
	TestSvfAntisat &
	               operator = (const TestSvfAntisat &other)        = delete;
	TestSvfAntisat &
	               operator = (TestSvfAntisat &&other)             = delete;
	bool           operator == (const TestSvfAntisat &other) const = delete;
	bool           operator != (const TestSvfAntisat &other) const = delete;

}; // class TestSvfAntisat



//#include "test/TestSvfAntisat.hpp"



#endif   // TestSvfAntisat_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

