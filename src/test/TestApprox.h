/*****************************************************************************

        TestApprox.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestApprox_HEADER_INCLUDED)
#define TestApprox_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <string>



class TestApprox
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static int     perform_test ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	template <typename T, int ILL2>
	class TestFncSpeed
	{
		static_assert (ILL2 >= 0, "ILL2 must be in [0 ; 3]");
		static_assert (ILL2 <= 3, "ILL2 must be in [0 ; 3]");
	public:
		template <typename OP, typename S>
		static void    test_op1 (const OP &op, const std::string &name, S min_val, S max_val);
	};

	template <typename T, bool REL_FLAG>
	class TestFncLogic
	{
	public:
		template <typename OPTST, typename OPREF, typename S>
		static void    test_op1 (const OPREF &op_ref, const OPTST &op_tst, const std::string &name, S min_val, S max_val);

		template <typename OPTST, typename OPREF, typename S>
		static void    test_op2 (const OPREF &op_ref, const OPTST &op_tst, const std::string &name, S min_val1, S max_val1, S min_val2, S max_val2);
	};

	template <bool REL_FLAG, typename OPREF, typename OPTSTS, typename OPTSTV>
	static void    test_op1_all_flt (const OPREF &op_ref, const OPTSTS &op_s, const OPTSTV &op_v, const std::string &name, float min_val, float max_val);
	template <bool REL_FLAG, typename OPREF, typename OPTSTS>
	static void    test_op1_all_flt_s (const OPREF &op_ref, const OPTSTS &op_s, const std::string &name, float min_val, float max_val);
	template <bool REL_FLAG, typename OPREF, typename OPTSTV>
	static void    test_op1_all_flt_v (const OPREF &op_ref, const OPTSTV &op_v, const std::string &name, float min_val, float max_val);





/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TestApprox ()                               = delete;
	               TestApprox (const TestApprox &other)        = delete;
	virtual        ~TestApprox ()                              = delete;
	TestApprox &   operator = (const TestApprox &other)        = delete;
	bool           operator == (const TestApprox &other) const = delete;
	bool           operator != (const TestApprox &other) const = delete;

}; // class TestApprox



//#include "test/TestApprox.hpp"



#endif   // TestApprox_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
