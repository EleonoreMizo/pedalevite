/*****************************************************************************

        TestInterpPhase.h
        Author: Laurent de Soras, 2019

Template parameters:

- T: class to test (ohm::dsp::InterpPhase*)

- NPL2: Base-2 logarithm of the number of phases. > 0.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestInterpPhase_HEADER_INCLUDED)
#define TestInterpPhase_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class T, int NPL2>
class TestInterpPhase
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef T InterpPhaseUnaligned;

	static const int  PHASE_LEN     = InterpPhaseUnaligned::PHASE_LEN;
	static const int  NBR_PHASES_L2 = NPL2;

	static int		perform_test ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static int		perform_test_integrity ();
	static int		perform_test_performance ();



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TestInterpPhase ()                                         = delete;
	               TestInterpPhase (const TestInterpPhase <T, NPL2> &other)   = delete;
	virtual        ~TestInterpPhase ()                                        = delete;
	TestInterpPhase <T, NPL2> &
	               operator = (const TestInterpPhase <T, NPL2> &other)        = delete;
	bool           operator == (const TestInterpPhase <T, NPL2> &other) const = delete;
	bool           operator != (const TestInterpPhase <T, NPL2> &other) const = delete;

}; // class TestInterpPhase



#include "TestInterpPhase.hpp"



#endif   // TestInterpPhase_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
