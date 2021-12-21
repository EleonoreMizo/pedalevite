/*****************************************************************************

        TestHalfBand.h
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (hiir_test_TestHalfBand_HEADER_INCLUDED)
#define hiir_test_TestHalfBand_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <vector>



namespace hiir
{
namespace test
{



class SweepingSine;

template <class TO>
class TestHalfBand
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef TO TestedType;
	typedef typename TO::DataType DataType;
	static constexpr int _nbr_chn = TO::_nbr_chn;

	enum {         NBR_COEFS = TestedType::NBR_COEFS };

	static int     perform_test (TO &filter, const double coef_arr [NBR_COEFS], const SweepingSine &ss, const char *type_0, double transition_bw, double stopband_at);




/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static int     check_band (std::vector <DataType> &dst_chk, const std::vector <DataType> &dest, bool hi_flag, int chn, const SweepingSine &ss, const char *type_0, double transition_bw, double stopband_at, const char *filename_fmt_0);



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TestHalfBand ()                               = delete;
	               TestHalfBand (const TestHalfBand &other)      = delete;
	               TestHalfBand (TestHalfBand &&other)           = delete;
	TestHalfBand & operator = (const TestHalfBand &other)        = delete;
	TestHalfBand & operator = (TestHalfBand &&other)             = delete;
	bool           operator == (const TestHalfBand &other) const = delete;
	bool           operator != (const TestHalfBand &other) const = delete;

}; // class TestHalfBand



}  // namespace test
}  // namespace hiir



#include "hiir/test/TestHalfBand.hpp"



#endif   // hiir_test_TestHalfBand_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
