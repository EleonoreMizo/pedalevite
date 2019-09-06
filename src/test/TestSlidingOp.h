/*****************************************************************************

        TestSlidingOp.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestSlidingOp_HEADER_INCLUDED)
#define TestSlidingOp_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



class TestSlidingOp
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static int     perform_test ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	template <class SO>
	static int     perform_test_single ();

	template <typename T, typename OP>
	static T       compute_naive_combination (OP &op, const T src_ptr [], int len);



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TestSlidingOp ()                               = delete;
	               TestSlidingOp (const TestSlidingOp &other)     = delete;
	virtual        ~TestSlidingOp ()                              = delete;
	TestSlidingOp &
	               operator = (const TestSlidingOp &other)        = delete;
	bool           operator == (const TestSlidingOp &other) const = delete;
	bool           operator != (const TestSlidingOp &other) const = delete;

}; // class TestSlidingOp



//#include "test/TestSlidingOp.hpp"



#endif   // TestSlidingOp_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
