/*****************************************************************************

        TestHiirDesigner.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestHiirDesigner_HEADER_INCLUDED)
#define TestHiirDesigner_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



class TestHiirDesigner
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

	               TestHiirDesigner ()                               = delete;
	               TestHiirDesigner (const TestHiirDesigner &other)  = delete;
	               TestHiirDesigner (TestHiirDesigner &&other)       = delete;
	               ~TestHiirDesigner ()                              = delete;
	TestHiirDesigner &
	               operator = (const TestHiirDesigner &other)        = delete;
	TestHiirDesigner &
	               operator = (TestHiirDesigner &&other)             = delete;
	bool           operator == (const TestHiirDesigner &other) const = delete;
	bool           operator != (const TestHiirDesigner &other) const = delete;

}; // class TestHiirDesigner



//#include "test/TestHiirDesigner.hpp"



#endif   // TestHiirDesigner_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
