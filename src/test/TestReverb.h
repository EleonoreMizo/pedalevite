/*****************************************************************************

        TestReverb.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestReverb_HEADER_INCLUDED)
#define TestReverb_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



class TestReverb
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static constexpr int _max_block_len = 1024;
	static constexpr int _nbr_chn       = 2;

	static int     perform_test ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static int     test_freeverb ();
	static int     test_bigverb ();
	static int     test_delay_frac ();
	static int     test_delay_apf ();



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TestReverb ()                               = delete;
	               TestReverb (const TestReverb &other)        = delete;
	               TestReverb (TestReverb &&other)             = delete;
	TestReverb &   operator = (const TestReverb &other)        = delete;
	TestReverb &   operator = (TestReverb &&other)             = delete;
	bool           operator == (const TestReverb &other) const = delete;
	bool           operator != (const TestReverb &other) const = delete;

}; // class TestReverb



//#include "test/TestReverb.hpp"



#endif   // TestReverb_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
