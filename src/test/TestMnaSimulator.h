/*****************************************************************************

        TestMnaSimulator.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestMnaSimulator_HEADER_INCLUDED)
#define TestMnaSimulator_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/va/mna/Simulator.h"

#include <vector>



class TestMnaSimulator
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static int     perform_test ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static void    gen_saw (std::vector <float> &data, double sample_freq, double freq, int len);

#if defined (mfx_dsp_va_mna_Simulator_STATS)
	static void    print_stats (mfx::dsp::va::mna::Simulator &mna);
	static void    print_histo (int hist_arr [], int nbr_bars, int nbr_spl);
#endif // mfx_dsp_va_mna_Simulator_STATS



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TestMnaSimulator ()                               = delete;
	               TestMnaSimulator (const TestMnaSimulator &other)  = delete;
	               TestMnaSimulator (TestMnaSimulator &&other)       = delete;
	               ~TestMnaSimulator ()                              = delete;
	TestMnaSimulator &
	               operator = (const TestMnaSimulator &other)        = delete;
	TestMnaSimulator &
	               operator = (TestMnaSimulator &&other)             = delete;
	bool           operator == (const TestMnaSimulator &other) const = delete;
	bool           operator != (const TestMnaSimulator &other) const = delete;

}; // class TestMnaSimulator



//#include "test/TestMnaSimulator.hpp"



#endif   // TestMnaSimulator_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
