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

	static int     check_dspl (const SweepingSine &ss, double bw, double at, const T out_ptr [], bool hiband_flag);
	static int     check_uspl (const SweepingSine &ss, double bw, double at, const T out_ptr []);
	static int     check_phase (const SweepingSine &ss, double bw, const T out_0_ptr [], const T out_1_ptr []);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



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
