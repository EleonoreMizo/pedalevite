/*****************************************************************************

        TestMoogLadderMystran.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestMoogLadderMystran_HEADER_INCLUDED)
#define TestMoogLadderMystran_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



class TestMoogLadderMystran
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

	               TestMoogLadderMystran ()                               = delete;
	               TestMoogLadderMystran (const TestMoogLadderMystran &other) = delete;
	               TestMoogLadderMystran (TestMoogLadderMystran &&other)      = delete;
	virtual        ~TestMoogLadderMystran ()                              = delete;
	TestMoogLadderMystran &
	               operator = (const TestMoogLadderMystran &other)        = delete;
	TestMoogLadderMystran &
	               operator = (TestMoogLadderMystran &&other)             = delete;
	bool           operator == (const TestMoogLadderMystran &other) const = delete;
	bool           operator != (const TestMoogLadderMystran &other) const = delete;

}; // class TestMoogLadderMystran



//#include "test/TestMoogLadderMystran.hpp"



#endif   // TestMoogLadderMystran_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

