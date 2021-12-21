/*****************************************************************************

        main.cpp
        Author: Laurent de Soras, 2005

- First, edit conf.h to configure the test bench for your CPU/compiler.

- Tests block processing for each object with a sweeping sine. Optionally
saves the result as 16-bit raw files (mono, or stereo for PhaseHalfPi*
objects). Results are automatically tested, however these tests are rough.
You should ensure yourself the correctness of the results. The easiest way is
to display a 2-D colored spectrum (intensity vs time and frequency).

- Tests the block processing speed of each object. Gives the "best case"
performance: long blocks and cache misses reduced.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (4 : 4786) // "identifier was truncated to '255' characters in the debug information"
	#pragma warning (4 : 4800) // "forcing value to bool 'true' or 'false' (performance warning)"
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "hiir/test/conf.h"
#include "hiir/test/TestAllClassesFnc.h"
#include "hiir/test/TestAllClassesSpeed.h"
#include "hiir/def.h"
#include "hiir/fnc.h"

#if defined (_MSC_VER)
#include <crtdbg.h>
#include <new.h>
#endif   // _MSC_VER

#include <new>
#include <vector>

#include <cassert>
#include <cmath>
#include <cstdio>



/*\\\ FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



static int	HIIR_test_functionalities ();
static int	HIIR_test_speed ();



#if defined (_MSC_VER)
static int __cdecl	HIIR_new_handler_cb (size_t dummy)
{
	throw std::bad_alloc ();
	return (0);
}
#endif	// _MSC_VER



#if defined (_MSC_VER) && ! defined (NDEBUG)
static int	__cdecl	HIIR_debug_alloc_hook_cb (int alloc_type, void *user_data_ptr, size_t size, int block_type, long request_nbr, const unsigned char *filename_0, int line_nbr)
{
	if (block_type != _CRT_BLOCK)	// Ignore CRT blocks to prevent infinite recursion
	{
		switch (alloc_type)
		{
		case _HOOK_ALLOC:
		case _HOOK_REALLOC:
		case _HOOK_FREE:

			// Put some debug code here

			break;

		default:
			assert (false);	// Undefined allocation type
			break;
		}
	}

	return (1);
}
#endif



#if defined (_MSC_VER) && ! defined (NDEBUG)
static int	__cdecl	HIIR_debug_report_hook_cb (int report_type, char *user_msg_0, int *ret_val_ptr)
{
	*ret_val_ptr = 0;	// 1 to override the CRT default reporting mode

	switch (report_type)
	{
	case _CRT_WARN:
	case _CRT_ERROR:
	case _CRT_ASSERT:

// Put some debug code here

		break;
	}

	return (*ret_val_ptr);
}
#endif



static void	HIIR_main_prog_init ()
{
#if defined (_MSC_VER)
	::_set_new_handler (::HIIR_new_handler_cb);
#endif   // _MSC_VER

#if defined (_MSC_VER) && ! defined (NDEBUG)
	{
		const int   mode =   (1 * _CRTDBG_MODE_DEBUG)
		                   | (1 * _CRTDBG_MODE_WNDW);
		::_CrtSetReportMode (_CRT_WARN, mode);
		::_CrtSetReportMode (_CRT_ERROR, mode);
		::_CrtSetReportMode (_CRT_ASSERT, mode);

		const int	old_flags = ::_CrtSetDbgFlag (_CRTDBG_REPORT_FLAG);
		::_CrtSetDbgFlag (  old_flags
		                  | (1 * _CRTDBG_LEAK_CHECK_DF)
		                  | (1 * _CRTDBG_CHECK_ALWAYS_DF));
		::_CrtSetBreakAlloc (-1);	// Specify here a memory bloc number
		::_CrtSetAllocHook (HIIR_debug_alloc_hook_cb);
		::_CrtSetReportHook (HIIR_debug_report_hook_cb);

		// Speed up I/O but breaks C stdio compatibility
//		std::cout.sync_with_stdio (false);
//		std::cin.sync_with_stdio (false);
//		std::cerr.sync_with_stdio (false);
//		std::clog.sync_with_stdio (false);
	}
#endif	// _MSC_VER, NDEBUG
}



static void	HIIR_main_prog_end ()
{
#if defined (_MSC_VER) && ! defined (NDEBUG)
	{
		const int   mode =   (1 * _CRTDBG_MODE_DEBUG)
		                   | (0 * _CRTDBG_MODE_WNDW);
		::_CrtSetReportMode (_CRT_WARN, mode);
		::_CrtSetReportMode (_CRT_ERROR, mode);
		::_CrtSetReportMode (_CRT_ASSERT, mode);

		::_CrtMemState	mem_state;
		::_CrtMemCheckpoint (&mem_state);
		::_CrtMemDumpStatistics (&mem_state);
	}
#endif	// _MSC_VER, NDEBUG
}



int main (int /* argc */, char * /* argv */ [])
{
	HIIR_main_prog_init ();

	try
	{
		int            ret_val = 0;

		if (ret_val == 0)
		{
			ret_val = HIIR_test_functionalities ();
		}
		if (ret_val == 0)
		{
			ret_val = HIIR_test_speed ();
		}

		if (ret_val == 0)
		{
			printf ("All tests passed successfully.\n\n");
		}
		else
		{
			printf ("*** Test failed ***\n\n");
		}
	}

	catch (std::exception &e)
	{
		printf ("*** main(): Exception (std::exception) : %s\n", e.what ());
		throw;
	}

	catch (...)
	{
		printf ("*** main(): Undefined exception\n");
		throw;
	}

	HIIR_main_prog_end ();

	return 0;
}



int	HIIR_test_functionalities ()
{
	int            ret_val = 0;

#if defined (hiir_test_LONG_FUNC_TESTS)
	hiir::test::TestAllClassesFnc < 1>::perform_test_r (ret_val, 0.1);
	hiir::test::TestAllClassesFnc < 2>::perform_test_r (ret_val, 0.1);
	hiir::test::TestAllClassesFnc < 3>::perform_test_r (ret_val, 0.05);
	hiir::test::TestAllClassesFnc < 4>::perform_test_r (ret_val, 0.05);
	hiir::test::TestAllClassesFnc < 5>::perform_test_r (ret_val, 0.02);
	hiir::test::TestAllClassesFnc < 6>::perform_test_r (ret_val, 0.02);
	hiir::test::TestAllClassesFnc < 7>::perform_test_r (ret_val, 0.01);
#endif   // hiir_test_LONG_FUNC_TESTS

	hiir::test::TestAllClassesFnc < 8>::perform_test_r (ret_val, 0.01);

#if defined (hiir_test_LONG_FUNC_TESTS)
	hiir::test::TestAllClassesFnc < 1>::perform_test_r (ret_val, 0.49);
	hiir::test::TestAllClassesFnc <32>::perform_test_r (ret_val, 0.0001);
#endif   // hiir_test_LONG_FUNC_TESTS

	return ret_val;
}



int	HIIR_test_speed ()
{
	int            ret_val = 0;

	hiir::test::TestAllClassesSpeed < 1>::perform_test ();
	hiir::test::TestAllClassesSpeed < 2>::perform_test ();
	hiir::test::TestAllClassesSpeed < 3>::perform_test ();
	hiir::test::TestAllClassesSpeed < 4>::perform_test ();
	hiir::test::TestAllClassesSpeed < 8>::perform_test ();

#if defined (hiir_test_LONG_SPEED_TESTS)
	// If you enable this section, it may take much longer
	// to compile because of recursive template expansion.
	hiir::test::TestAllClassesSpeed <12>::perform_test ();
	hiir::test::TestAllClassesSpeed <16>::perform_test ();
	hiir::test::TestAllClassesSpeed <24>::perform_test ();
	hiir::test::TestAllClassesSpeed <32>::perform_test ();
#endif   // hiir_test_LONG_SPEED_TESTS

	return ret_val;
}



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
