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
		template <typename OP>
		static void    test_op1 (const OP &op, const std::string &name, double min_val, double max_val);
	};

	template <typename T, bool REL_FLAG>
	class TestFncLogic
	{
	public:
		template <typename OPTST, typename OPREF>
		static void    test_op1 (int &ret_val, const OPREF &op_ref, const OPTST &op_tst, const std::string &name, double min_val, double max_val, double err_allowed);

		template <typename OPTST, typename OPREF>
		static void    test_op2 (int &ret_val, const OPREF &op_ref, const OPTST &op_tst, const std::string &name, double min_val1, double max_val1, double min_val2, double max_val2, double err_allowed);
	};

	template <bool REL_FLAG, typename OPREF, typename OPTSTS, typename OPTSTV>
	static void    test_op1_all_flt (int &ret_val, const OPREF &op_ref, const OPTSTS &op_s, const OPTSTV &op_v, const std::string &name, double min_val, double max_val, double err_allowed);
	template <typename T, bool REL_FLAG, typename OPREF, typename OPTSTS>
	static void    test_op1_all_s (int &ret_val, const OPREF &op_ref, const OPTSTS &op_s, const std::string &name, double min_val, double max_val, double err_allowed);
	template <bool REL_FLAG, typename OPREF, typename OPTSTV>
	static void    test_op1_all_flt_v (int &ret_val, const OPREF &op_ref, const OPTSTV &op_v, const std::string &name, double min_val, double max_val, double err_allowed);





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
