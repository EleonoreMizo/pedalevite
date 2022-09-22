/*****************************************************************************

        TestHelperDispNum.h
        Author: Laurent de Soras, 2022

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law.You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestHelperDispNum_HEADER_INCLUDED)
#define TestHelperDispNum_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace param
{
	class HelperDispNum;
}
}
}

class TestHelperDispNum
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static int     perform_test ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	using Hdn = mfx::pi::param::HelperDispNum;

	enum class Ret
	{
		FAIL
	};

	static void    test_txt (int &ret_val, const Hdn &hdn, const char txt_0 [], double val);
	static void    test_txt (int &ret_val, const Hdn &hdn, const char txt_0 [], Ret r);
	static void    test_val (int &ret_val, const Hdn &hdn, double val, const char txt_0 []);



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TestHelperDispNum ()                               = delete;
	               TestHelperDispNum (const TestHelperDispNum &other) = delete;
	               TestHelperDispNum (TestHelperDispNum &&other)      = delete;
	TestHelperDispNum &
	               operator = (const TestHelperDispNum &other)        = delete;
	TestHelperDispNum &
	               operator = (TestHelperDispNum &&other)             = delete;
	bool           operator == (const TestHelperDispNum &other) const = delete;
	bool           operator != (const TestHelperDispNum &other) const = delete;

}; // class TestHelperDispNum



//#include "TestHelperDispNum.hpp"



#endif // TestHelperDispNum_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
