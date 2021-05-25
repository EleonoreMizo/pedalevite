/*****************************************************************************

        TestVelvetConv.h
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestVelvetConv_HEADER_INCLUDED)
#define TestVelvetConv_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



class TestVelvetConv
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

	               TestVelvetConv ()                               = delete;
	               TestVelvetConv (const TestVelvetConv &other)    = delete;
	               TestVelvetConv (TestVelvetConv &&other)         = delete;
	TestVelvetConv &
	               operator = (const TestVelvetConv &other)        = delete;
	TestVelvetConv &
	               operator = (TestVelvetConv &&other)             = delete;
	bool           operator == (const TestVelvetConv &other) const = delete;
	bool           operator != (const TestVelvetConv &other) const = delete;

}; // class TestVelvetConv



//#include "test/TestVelvetConv.hpp"



#endif   // TestVelvetConv_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
