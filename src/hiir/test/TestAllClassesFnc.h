/*****************************************************************************

        TestAllClassesFnc.h
        Author: Laurent de Soras, 2005

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (hiir_test_TestAllClassesFnc_HEADER_INCLUDED)
#define hiir_test_TestAllClassesFnc_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace hiir
{
namespace test
{



template <int NC>
class TestAllClassesFnc
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum {         NBR_COEFS = NC };

	static int     perform_test (double transition_bw);
	static void    perform_test_r (int &ret_val, double transition_bw);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TestAllClassesFnc ()                                    = delete;
	               TestAllClassesFnc (const TestAllClassesFnc <NC> &other) = delete;
	               TestAllClassesFnc (TestAllClassesFnc <NC> &&other)      = delete;
	               ~TestAllClassesFnc ()                                   = delete;
	TestAllClassesFnc <NC> &
	               operator = (const TestAllClassesFnc <NC> &other)        = delete;
	TestAllClassesFnc <NC> &
	               operator = (TestAllClassesFnc <NC> &&other)             = delete;
	bool           operator == (const TestAllClassesFnc <NC> &other)       = delete;
	bool           operator != (const TestAllClassesFnc <NC> &other)       = delete;

}; // class TestAllClassesFnc



}  // namespace test
}  // namespace hiir



#include "hiir/test/TestAllClassesFnc.hpp"



#endif   // hiir_test_TestAllClassesFnc_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
