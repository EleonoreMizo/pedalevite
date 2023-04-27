/*****************************************************************************

        TestLedSimple.cpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/hw/GpioPin.h"
#include "test/TestLedSimple.h"

#include <unistd.h>

#include <wiringPi.h>

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <ctime>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestLedSimple::perform_test ()
{
	int            ret_val = 0;

	static const int  nbr_leds = 3;
	static const int  led_pin_arr [nbr_leds] =
	{
		mfx::hw::GpioPin::_led_0,
		mfx::hw::GpioPin::_led_1,
		mfx::hw::GpioPin::_led_2
	};

	::wiringPiSetupGpio ();

	for (int i = 0; i < nbr_leds; ++i)
	{
		::pinMode  (led_pin_arr [i], OUTPUT);
		::digitalWrite (led_pin_arr [i], LOW);
	}

	int            active = 0;
	double         t      = 1000;
	double         r      = 0.99;
	while (true)
	{
		::digitalWrite (led_pin_arr [active], HIGH);
		::delay (int (t + 0.5));
		::digitalWrite (led_pin_arr [active], LOW);

		active = (active + 1) % nbr_leds;
		t *= r;
		if (t <= 1 || t >= 1000)
		{
			r = 1 / r;
		}
	}


	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
