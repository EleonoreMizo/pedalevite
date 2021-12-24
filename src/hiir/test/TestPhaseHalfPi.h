/*****************************************************************************

        TestPhaseHalfPi.h
        Author: Laurent de Soras, 2005

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (hiir_test_TestPhaseHalfPi_HEADER_INCLUDED)
#define hiir_test_TestPhaseHalfPi_HEADER_INCLUDED

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

template <class TO>
class TestPhaseHalfPi
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef TO TestedType;
	typedef typename TO::DataType DataType;
	static constexpr int _nbr_chn   = TO::_nbr_chn;
	static constexpr int _nbr_coefs = TO::NBR_COEFS;

	static int     perform_test (TO &phaser, const double coef_arr [_nbr_coefs], const SweepingSine &ss, const char *type_0, double transition_bw);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TestPhaseHalfPi ()                                  = delete;
	               TestPhaseHalfPi (const TestPhaseHalfPi <TO> &other) = delete;
	               TestPhaseHalfPi (TestPhaseHalfPi <TO> &&other)      = delete;
	               ~TestPhaseHalfPi ()                                 = delete;
	TestPhaseHalfPi <TO> &
	               operator = (const TestPhaseHalfPi <TO> &other)      = delete;
	TestPhaseHalfPi <TO> &
	               operator = (TestPhaseHalfPi <TO> &&other)           = delete;
	bool           operator == (const TestPhaseHalfPi <TO> &other)     = delete;
	bool           operator != (const TestPhaseHalfPi <TO> &other)     = delete;

}; // class TestPhaseHalfPi



}  // namespace test
}  // namespace hiir



#include "hiir/test/TestPhaseHalfPi.hpp"



#endif   // hiir_test_TestPhaseHalfPi_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
