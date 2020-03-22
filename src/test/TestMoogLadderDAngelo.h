/*****************************************************************************

        TestMoogLadderDAngelo.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestMoogLadderDAngelo_HEADER_INCLUDED)
#define TestMoogLadderDAngelo_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



class TestMoogLadderDAngelo
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static int     perform_test ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	template <int N>
	static int     perform_test_n ();



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TestMoogLadderDAngelo ()                               = delete;
	               TestMoogLadderDAngelo (const TestMoogLadderDAngelo &other) = delete;
	               TestMoogLadderDAngelo (TestMoogLadderDAngelo &&other)      = delete;
	virtual        ~TestMoogLadderDAngelo ()                              = delete;
	TestMoogLadderDAngelo &
	               operator = (const TestMoogLadderDAngelo &other)        = delete;
	TestMoogLadderDAngelo &
	               operator = (TestMoogLadderDAngelo &&other)             = delete;
	bool           operator == (const TestMoogLadderDAngelo &other) const = delete;
	bool           operator != (const TestMoogLadderDAngelo &other) const = delete;

}; // class TestMoogLadderDAngelo



//#include "test/TestMoogLadderDAngelo.hpp"



#endif   // TestMoogLadderDAngelo_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

