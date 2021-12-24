/*****************************************************************************

        TestDelay.h
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (hiir_test_TestDelay_HEADER_INCLUDED)
#define hiir_test_TestDelay_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace hiir
{
namespace test
{



template <class TO>
class TestDelay
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef TO TestedType;
	typedef typename TO::DataType DataType;
	static constexpr int _nbr_chn   = TO::_nbr_chn;
	static constexpr int _nbr_coefs = TO::NBR_COEFS;

	static int     test_halfband (TO &filter, const double coef_arr [_nbr_coefs]);
	static int     test_downsampler (TO &filter, const double coef_arr [_nbr_coefs]);
	static int     test_upsampler (TO &filter, const double coef_arr [_nbr_coefs]);
	static int     test_phaser (TO &filter, const double coef_arr [_nbr_coefs]);




/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static constexpr double _f_fs = 1.f / 20;
	static constexpr int    _len  = 200000; // Must be even



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TestDelay ()                               = delete;
	               TestDelay (const TestDelay &other)         = delete;
	               TestDelay (TestDelay &&other)              = delete;
	TestDelay &    operator = (const TestDelay &other)        = delete;
	TestDelay &    operator = (TestDelay &&other)             = delete;
	bool           operator == (const TestDelay &other) const = delete;
	bool           operator != (const TestDelay &other) const = delete;

}; // class TestDelay



}  // namespace test
}  // namespace hiir



#include "hiir/test/TestDelay.hpp"



#endif   // hiir_test_TestDelay_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
