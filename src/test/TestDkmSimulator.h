/*****************************************************************************

        TestDkmSimulator.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestDkmSimulator_HEADER_INCLUDED)
#define TestDkmSimulator_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/va/dkm/Simulator.h"

#include <vector>



class TestDkmSimulator
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static int     perform_test ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static void    gen_saw (std::vector <float> &data, double sample_freq, double freq, int len);
	static void    gen_sq (std::vector <float> &data, double sample_freq, double freq, int len);
	static void    gen_sine (std::vector <float> &data, double sample_freq, double freq, int len);
	static void    gen_silence (std::vector <float> &data, int len);
	static void    scale_vect (std::vector <float> &data, float scale);
	static void    cat_vect (std::vector <float> &data, const std::vector <float> &more);

#if defined (mfx_dsp_va_dkm_Simulator_STATS)
	template <class S>
	static void    print_stats (S &dkm);
	static void    print_histo (const int hist_arr [], int nbr_bars, int nbr_spl);
	static void    print_pivot_list (const std::map <std::vector <int>, int> &piv_map);
#endif // mfx_dsp_va_dkm_Simulator_STATS



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TestDkmSimulator ()                               = delete;
	               TestDkmSimulator (const TestDkmSimulator &other)  = delete;
	               TestDkmSimulator (TestDkmSimulator &&other)       = delete;
	               ~TestDkmSimulator ()                              = delete;
	TestDkmSimulator &
	               operator = (const TestDkmSimulator &other)        = delete;
	TestDkmSimulator &
	               operator = (TestDkmSimulator &&other)             = delete;
	bool           operator == (const TestDkmSimulator &other) const = delete;
	bool           operator != (const TestDkmSimulator &other) const = delete;

}; // class TestDkmSimulator



//#include "test/TestDkmSimulator.hpp"



#endif   // TestDkmSimulator_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
