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
#if ! defined (mfx_ui_LedPi3_HEADER_INCLUDED)
#define mfx_ui_LedPi3_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/ui/LedInterface.h"
#include "mfx/GpioPwm.h"

#include <array>
#include <thread>



namespace mfx
{
namespace ui
{



class LedPi3
:	public LedInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               LedPi3 ();
	virtual        ~LedPi3 ();

	static const int  _nbr_led = 3;
	static const int  _gpio_pin_arr [_nbr_led];



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// LedInterface
	virtual int    do_get_nbr_led () const;
	virtual void   do_set_led (int index, float val);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	class LedState
	{
	public:
		volatile float _val_cur  = 0;
		float          _val_prev = 0;
	};

	typedef std::array <LedState, _nbr_led> StateArray;

	static const int  _pwm_resol =       100; // Microseconds
	static const int  _pwm_cycle = 10 * 1000; // Microseconds
	static const int  _pwm_chn   = 0;

	void           refresh_loop ();

	GpioPwm        _gpio_pwm;
	StateArray     _state_arr;

	volatile bool  _quit_flag;
	std::thread    _refresher;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               LedPi3 (const LedPi3 &other)            = delete;
	LedPi3 &       operator = (const LedPi3 &other)        = delete;
	bool           operator == (const LedPi3 &other) const = delete;
	bool           operator != (const LedPi3 &other) const = delete;

}; // class LedPi3



}  // namespace ui
}  // namespace mfx



//#include "mfx/ui/LedPi3.hpp"



#endif   // mfx_ui_LedPi3_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
