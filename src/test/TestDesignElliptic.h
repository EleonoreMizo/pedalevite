/*****************************************************************************

        TestDesignElliptic.h
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestDesignElliptic_HEADER_INCLUDED)
#define TestDesignElliptic_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



class TestDesignElliptic
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

	               TestDesignElliptic ()                               = delete;
	               TestDesignElliptic (const TestDesignElliptic &other) = delete;
	               TestDesignElliptic (TestDesignElliptic &&other)      = delete;
	TestDesignElliptic &
	               operator = (const TestDesignElliptic &other)        = delete;
	TestDesignElliptic &
	               operator = (TestDesignElliptic &&other)             = delete;
	bool           operator == (const TestDesignElliptic &other) const = delete;
	bool           operator != (const TestDesignElliptic &other) const = delete;

}; // class TestDesignElliptic



//#include "test/TestDesignElliptic.hpp"



#endif   // TestDesignElliptic_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
