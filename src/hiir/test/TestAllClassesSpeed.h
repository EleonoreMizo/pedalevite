/*****************************************************************************

        TestAllClassesSpeed.h
        Author: Laurent de Soras, 2005

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (hiir_test_TestAllClassesSpeed_HEADER_INCLUDED)
#define hiir_test_TestAllClassesSpeed_HEADER_INCLUDED

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
class TestAllClassesSpeed
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum {         NBR_COEFS = NC };

	static void    perform_test () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TestAllClassesSpeed ()                                      = delete;
	               TestAllClassesSpeed (const TestAllClassesSpeed <NC> &other) = delete;
	               TestAllClassesSpeed (TestAllClassesSpeed <NC> &&other)      = delete;
	               ~TestAllClassesSpeed ()                                     = delete;
	TestAllClassesSpeed <NC> &
	               operator = (const TestAllClassesSpeed <NC> &other)          = delete;
	TestAllClassesSpeed <NC> &
	               operator = (TestAllClassesSpeed <NC> &&other)               = delete;
	bool           operator == (const TestAllClassesSpeed <NC> &other)         = delete;
	bool           operator != (const TestAllClassesSpeed <NC> &other)         = delete;

}; // class TestAllClassesSpeed



}  // namespace test
}  // namespace hiir



#include "hiir/test/TestAllClassesSpeed.hpp"



#endif   // hiir_test_TestAllClassesSpeed_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
