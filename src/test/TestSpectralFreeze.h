/*****************************************************************************

        TestSpectralFreeze.h
        Author: Laurent de Soras, 2022

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law.You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestSpectralFreeze_HEADER_INCLUDED)
#define TestSpectralFreeze_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



class TestSpectralFreeze
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

	               TestSpectralFreeze ()                              = delete;
	               TestSpectralFreeze (const TestSpectralFreeze &other) = delete;
	               TestSpectralFreeze (TestSpectralFreeze &&other)    = delete;
	TestSpectralFreeze &
	               operator = (const TestSpectralFreeze &other)       = delete;
	TestSpectralFreeze &
	               operator = (TestSpectralFreeze &&other)            = delete;
	bool           operator == (const TestSpectralFreeze &other) const = delete;
	bool           operator != (const TestSpectralFreeze &other) const = delete;

}; // class TestSpectralFreeze



//#include "test/TestSpectralFreeze.hpp"



#endif // TestSpectralFreeze_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
