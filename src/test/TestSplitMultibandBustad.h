/*****************************************************************************

        TestSplitMultibandBustad.h
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestSplitMultibandBustad_HEADER_INCLUDED)
#define TestSplitMultibandBustad_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



class TestSplitMultibandBustad
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static int     perform_test ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	template <class S>
	static int     perform_test_class (const char *classname_0, const char *filename_0, int nbr_bands, bool block_flag, bool save_flag);



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TestSplitMultibandBustad ()                               = delete;
	               TestSplitMultibandBustad (const TestSplitMultibandBustad &other) = delete;
	               TestSplitMultibandBustad (TestSplitMultibandBustad &&other)      = delete;
	TestSplitMultibandBustad &
	               operator = (const TestSplitMultibandBustad &other)        = delete;
	TestSplitMultibandBustad &
	               operator = (TestSplitMultibandBustad &&other)             = delete;
	bool           operator == (const TestSplitMultibandBustad &other) const = delete;
	bool           operator != (const TestSplitMultibandBustad &other) const = delete;

}; // class TestSplitMultibandBustad



//#include "test/TestSplitMultibandBustad.hpp"



#endif   // TestSplitMultibandBustad_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
