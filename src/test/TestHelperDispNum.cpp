/*****************************************************************************

        TestHelperDispNum.cpp
        Author: Laurent de Soras, 2022

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law.You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/txt/neutral/fnc.h"
#include "fstb/fnc.h"
#include "mfx/pi/param/HelperDispNum.h"
#include "test/TestHelperDispNum.h"

#include <cassert>
#include <cstdio>
#include <cstring>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestHelperDispNum::perform_test ()
{
	int            ret_val = 0;

	printf ("Testing mfx::pi::param::HelperDispNum...\n");

	Hdn            hdn;
	hdn.set_preset (Hdn::Preset_MIDI_NOTE);
	hdn.set_range (0, 127);

	test_txt (ret_val, hdn, ""        , Ret::FAIL);
	test_txt (ret_val, hdn, "xyz"     , Ret::FAIL);
	test_txt (ret_val, hdn, "C5"      , 60.0 );
	test_txt (ret_val, hdn, "C5xyz"   , Ret::FAIL);
	test_txt (ret_val, hdn, "C5+25"   , 60.25);
	test_txt (ret_val, hdn, "C5+50"   , 60.5 );
	test_txt (ret_val, hdn, "C#5-25"  , 60.75);
	test_txt (ret_val, hdn, "C# 5 -33", 60.67);
	test_txt (ret_val, hdn, "C5+10xyz", Ret::FAIL);

	test_val (ret_val, hdn, 60.0    , "C5"    );
	test_val (ret_val, hdn, 60.25   , "C5+25" );
	test_val (ret_val, hdn, 60.49   , "C5+49" );
	test_val (ret_val, hdn, 60.50   , "C5+50" );
	test_val (ret_val, hdn, 60.50499, "C5+50" );
	test_val (ret_val, hdn, 60.50501, "C#5-49");
	test_val (ret_val, hdn, 60.51   , "C#5-49");
	test_val (ret_val, hdn, 60.75   , "C#5-25");

	if (ret_val == 0)
	{
		printf ("Done.\n");
	}

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	TestHelperDispNum::test_txt (int &ret_val, const Hdn &hdn, const char txt_0 [], double val)
{
	assert (txt_0 != nullptr);

	if (ret_val == 0)
	{
		double         val_tst = 0;
		const int      ret_loc = hdn.conv_from_str (txt_0, val_tst);
		if (ret_loc != Hdn::Err_OK)
		{
			printf ("*** Error: conversion failure for \"%s\".\n", txt_0);
			ret_val = -1;
		}
		else if (! fstb::is_eq (val_tst, val, 1e-9))
		{
			printf (
				"*** Error: wrong value for \"%s\": found: %f, expected: %f).\n",
				txt_0, val_tst, val
			);
			ret_val = -1;
		}
	}
}



void	TestHelperDispNum::test_txt (int &ret_val, const Hdn &hdn, const char txt_0 [], Ret r)
{
	fstb::unused (r);
	assert (txt_0 != nullptr);

	if (ret_val == 0)
	{
		double         val_tst = 0;
		const int      ret_loc = hdn.conv_from_str (txt_0, val_tst);
		if (ret_loc == Hdn::Err_OK)
		{
			printf ("*** Error: conversion was expected to fail but succeeded.\n");
			ret_val = -1;
		}
	}
}



void	TestHelperDispNum::test_val (int &ret_val, const Hdn &hdn, double val, const char txt_0 [])
{
	assert (txt_0 != nullptr);

	constexpr long    max_len = 1023;
	char              buf_0 [max_len+1] {};
	if (ret_val == 0)
	{
		const int         ret_loc = hdn.conv_to_str (val, buf_0, max_len);
		if (ret_loc != Hdn::Err_OK)
		{
			printf ("*** Error: conversion failure for %f.\n", val);
			ret_val = -1;
		}
		else
		{
			const std::string tst { fstb::txt::neutral::trim_spaces (buf_0) };
			if (tst != txt_0)
			{
				printf (
					"*** Error: wrong string for %f, found: \"%s\", expected: \"%s\".\n",
					val, buf_0, txt_0
				);
				ret_val = -1;
			}
		}
	}
}



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
