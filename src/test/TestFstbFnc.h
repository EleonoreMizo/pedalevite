/*****************************************************************************

        TestFstbFnc.h
        Author: Laurent de Soras, 2023

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law.You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestFstbFnc_HEADER_INCLUDED)
#define TestFstbFnc_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace param
{
	class HelperDispNum;
}
}
}

class TestFstbFnc
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static int     perform_test ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static int     test_get_x_pow_2 ();



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TestFstbFnc ()                               = delete;
	               TestFstbFnc (const TestFstbFnc &other)       = delete;
	               TestFstbFnc (TestFstbFnc &&other)            = delete;
	TestFstbFnc &  operator = (const TestFstbFnc &other)        = delete;
	TestFstbFnc &  operator = (TestFstbFnc &&other)             = delete;
	bool           operator == (const TestFstbFnc &other) const = delete;
	bool           operator != (const TestFstbFnc &other) const = delete;

}; // class TestFstbFnc



//#include "TestFstbFnc.hpp"



#endif // TestFstbFnc_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
