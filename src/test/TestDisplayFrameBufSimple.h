/*****************************************************************************

        TestDisplayFrameBufSimple.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestDisplayFrameBufSimple_HEADER_INCLUDED)
#define TestDisplayFrameBufSimple_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



class TestDisplayFrameBufSimple
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

	               TestDisplayFrameBufSimple ()                               = delete;
	               TestDisplayFrameBufSimple (const TestDisplayFrameBufSimple &other) = delete;
	virtual        ~TestDisplayFrameBufSimple ()                              = delete;
	TestDisplayFrameBufSimple &
	               operator = (const TestDisplayFrameBufSimple &other)        = delete;
	bool           operator == (const TestDisplayFrameBufSimple &other) const = delete;
	bool           operator != (const TestDisplayFrameBufSimple &other) const = delete;

}; // class TestDisplayFrameBufSimple



//#include "test/TestDisplayFrameBufSimple.hpp"



#endif   // TestDisplayFrameBufSimple_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
