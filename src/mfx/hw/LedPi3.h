/*****************************************************************************

        LedPi3.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_hw_LedPi3_HEADER_INCLUDED)
#define mfx_hw_LedPi3_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/hw/GpioPwm.h"
#include "mfx/hw/Higepio.h"
#include "mfx/ui/LedInterface.h"

#include <array>
#include <atomic>
#include <thread>



namespace mfx
{
namespace hw
{



class LedPi3 final
:	public ui::LedInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       LedPi3 (Higepio &io);
	virtual        ~LedPi3 ();

	static const int  _nbr_led = 3;
	static const int  _gpio_pin_arr [_nbr_led];



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// LedInterface
	int            do_get_nbr_led () const final;
	void           do_set_led (int index, float val) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	class LedState
	{
	public:
		volatile float _val_cur  = 0;
		float          _val_prev = 0;
	};

	typedef std::array <LedState, _nbr_led> StateArray;

	static const int  _pwm_resol =     25; // Microseconds
	static const int  _pwm_cycle = 10'000; // Microseconds
	static const int  _pwm_chn   =      5; // Must be a free channel (see GpioPwm.h)

	void           refresh_loop ();

	Higepio &      _io;
	GpioPwm        _gpio_pwm;
	StateArray     _state_arr;

	std::atomic <bool>
	               _quit_flag;
	std::thread    _refresher;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               LedPi3 ()                               = delete;
	               LedPi3 (const LedPi3 &other)            = delete;
	               LedPi3 (LedPi3 &&other)                 = delete;
	LedPi3 &       operator = (const LedPi3 &other)        = delete;
	LedPi3 &       operator = (LedPi3 &&other)             = delete;
	bool           operator == (const LedPi3 &other) const = delete;
	bool           operator != (const LedPi3 &other) const = delete;

}; // class LedPi3



}  // namespace hw
}  // namespace mfx



//#include "mfx/hw/LedPi3.hpp"



#endif   // mfx_hw_LedPi3_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
