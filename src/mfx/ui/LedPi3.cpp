/*****************************************************************************

        LedPi3.cpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"
#include "mfx/ui/LedPi3.h"

#include <wiringPi.h>

#include <chrono>
#include <stdexcept>
#include <thread>

#include <cassert>



#define mfx_ui_LedPi3_REVERSE_ORDER


namespace mfx
{
namespace ui
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



const int	LedPi3::_gpio_pin_arr [_nbr_led] =
#if defined (mfx_ui_LedPi3_REVERSE_ORDER)
	{ 15, 13, 11 };
#else
	{ 11, 13, 15 };
#endif



// Before calling:
// ::wiringPiSetupPhys ()
LedPi3::LedPi3 ()
:	_gpio_pwm (_pwm_resol)
,	_state_arr ()
,	_quit_flag (false)
,	_refresher ()
{
	const int      ret_val = _gpio_pwm.init_chn (_pwm_chn, _pwm_cycle);
	if (ret_val != 0)
	{
		throw std::runtime_error ("Cannot initialize DMA channel");
	}

	for (int i = 0; i < _nbr_led; ++i)
	{
		::pinMode  (_gpio_pin_arr [i], OUTPUT);
		::digitalWrite (_gpio_pin_arr [i], LOW);
	}

	_refresher = std::thread (&LedPi3::refresh_loop, this);
}



LedPi3::~LedPi3 ()
{
	if (_refresher.joinable ())
	{
		_quit_flag = true;
		_refresher.join ();
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	LedPi3::do_get_nbr_led () const
{
	return _nbr_led;
}



void	LedPi3::do_set_led (int index, float val)
{
	_state_arr [index]._val_cur = val;
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	LedPi3::refresh_loop ()
{
	while (! _quit_flag)
	{
		for (int index = 0; index < _nbr_led && ! _quit_flag; ++index)
		{
			LedState &     state = _state_arr [index];
			if (state._val_cur != state._val_prev)
			{
				const float    val = state._val_cur;
				state._val_prev = val;
#if 1
				// Non-overlapping waveforms, half power
				const int      half_width = _pwm_cycle >> 1;
				const int      max_width  = half_width - _pwm_resol;
				const int      pw         = fstb::round_int (val * max_width);
				const int      start      = (index & 1) * half_width;
				_gpio_pwm.set_pulse (_pwm_chn, _gpio_pin_arr [index], start, pw);
#else
				const int      pw = fstb::round_int (val * _pwm_cycle);
				_gpio_pwm.set_pulse (_pwm_chn, _gpio_pin_arr [index], 0, pw);
#endif
			}
		}

		std::this_thread::sleep_for (std::chrono::milliseconds (20));
	}

	_quit_flag = false;
}



}  // namespace ui
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
