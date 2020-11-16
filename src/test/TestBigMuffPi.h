/*****************************************************************************

        TestBigMuffPi.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestBigMuffPi_HEADER_INCLUDED)
#define TestBigMuffPi_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/va/BigMuffPi.h"

#include <vector>



class TestBigMuffPi
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static int     perform_test ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static void    gen_saw (std::vector <float> &data, double sample_freq, double freq, int len);

#if defined (mfx_dsp_va_dkm_Simulator_STATS)
	static void    print_stats (const mfx::dsp::va::BigMuffPi &muff);
	static void    print_stats (mfx::dsp::va::dkm::Simulator::Stats &stats, const char *title_0);
	static void    print_histo (const int hist_arr [], int nbr_bars, int nbr_spl);
#endif // mfx_dsp_va_dkm_Simulator_STATS



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TestBigMuffPi ()                               = delete;
	               TestBigMuffPi (const TestBigMuffPi &other)     = delete;
	               TestBigMuffPi (TestBigMuffPi &&other)          = delete;
	               ~TestBigMuffPi ()                              = delete;
	TestBigMuffPi& operator = (const TestBigMuffPi &other)        = delete;
	TestBigMuffPi& operator = (TestBigMuffPi &&other)             = delete;
	bool           operator == (const TestBigMuffPi &other) const = delete;
	bool           operator != (const TestBigMuffPi &other) const = delete;

}; // class TestBigMuffPi



//#include "test/TestBigMuffPi.hpp"



#endif   // TestBigMuffPi_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
