/*****************************************************************************

        TestRcClipGeneric.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestRcClipGeneric_HEADER_INCLUDED)
#define TestRcClipGeneric_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <vector>



class TestRcClipGeneric
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static int     perform_test ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	template <class T>
	static int     perform_test (const char classname_0 [], const char filename_0 []);

	static void    gen_saw (std::vector <float> &data, double sample_freq, double freq, int len);

#if defined (mfx_dsp_va_RcClipGeneric_STAT)
	template <class T>
	static void    print_stats (T &dclip, const char msg_0 []);
	static void    print_histo (int hist_arr [], int nbr_bars, int nbr_spl);
#endif // mfx_dsp_va_RcClipGeneric_STAT



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TestRcClipGeneric ()                               = delete;
	               TestRcClipGeneric (const TestRcClipGeneric &other) = delete;
	               TestRcClipGeneric (TestRcClipGeneric &&other)      = delete;
	               ~TestRcClipGeneric ()                              = delete;
	TestRcClipGeneric &
	               operator = (const TestRcClipGeneric &other)        = delete;
	TestRcClipGeneric &
	               operator = (TestRcClipGeneric &&other)             = delete;
	bool           operator == (const TestRcClipGeneric &other) const = delete;
	bool           operator != (const TestRcClipGeneric &other) const = delete;

}; // class TestRcClipGeneric



//#include "test/TestRcClipGeneric.hpp"



#endif   // TestRcClipGeneric_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

