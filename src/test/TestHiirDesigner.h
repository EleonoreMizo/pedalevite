/*****************************************************************************

        TestHiirDesigner.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestHiirDesigner_HEADER_INCLUDED)
#define TestHiirDesigner_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <vector>



class TestHiirDesigner
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static int     perform_test ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static std::vector <double>
	               build_filter (int nbr_coef, double fs, double f_tst, double tg_pdly);
	template <int R, typename U, typename D>
	static void    measure_phase_delay (U &us, D &ds, double fs, double f_tst);
	static double  find_delay (const std::vector <float> &ref, const std::vector <float> &tst, int skip);
	static double  evaluate_mag_lpf (const std::vector <double> &coef_arr, double f_fs);



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TestHiirDesigner ()                               = delete;
	               TestHiirDesigner (const TestHiirDesigner &other)  = delete;
	               TestHiirDesigner (TestHiirDesigner &&other)       = delete;
	               ~TestHiirDesigner ()                              = delete;
	TestHiirDesigner &
	               operator = (const TestHiirDesigner &other)        = delete;
	TestHiirDesigner &
	               operator = (TestHiirDesigner &&other)             = delete;
	bool           operator == (const TestHiirDesigner &other) const = delete;
	bool           operator != (const TestHiirDesigner &other) const = delete;

}; // class TestHiirDesigner



//#include "test/TestHiirDesigner.hpp"



#endif   // TestHiirDesigner_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
