/*****************************************************************************

        ResultCheck.h
        Author: Laurent de Soras, 2005

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (hiir_test_ResultCheck_HEADER_INCLUDED)
#define hiir_test_ResultCheck_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <vector>



namespace hiir
{
namespace test
{



class SweepingSine;

template <typename T>
class ResultCheck
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static std::vector <T>
	               generate_test_signal (const SweepingSine &ss, long len, int nbr_chn);

	static int     check_halfband (const SweepingSine &ss, double bw, double at, const T out_ptr [], bool hiband_flag, int rate_div);
	static int     check_uspl (const SweepingSine &ss, double bw, double at, const T out_ptr []);
	static int     check_phase (const SweepingSine &ss, double bw, const T out_0_ptr [], const T out_1_ptr []);

	static int     check_delay (const T out_ptr [], const T ref_ptr [], long nbr_spl, double dly_expect, double f_fs);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static double  find_delay (const T tst_ptr [], const T ref_ptr [], long len, long skip, int start);
	static double  find_extremum_pos_parabolic (double r1, double r2, double r3) noexcept;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               ResultCheck ()                             = delete;
	               ResultCheck (const ResultCheck <T> &other) = delete;
	               ResultCheck (ResultCheck <T> &&other)      = delete;
	ResultCheck <T> &
	               operator = (const ResultCheck <T> &other)  = delete;
	ResultCheck <T> &
	               operator = (ResultCheck <T> &&other)       = delete;
	bool           operator == (const ResultCheck <T> &other) = delete;
	bool           operator != (const ResultCheck <T> &other) = delete;

}; // class ResultCheck



}  // namespace test
}  // namespace hiir



#include "hiir/test/ResultCheck.hpp"



#endif   // hiir_test_ResultCheck_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
