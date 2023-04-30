/*****************************************************************************

        HigepioNative.hpp
        Author: Laurent de Soras, 2023

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_hw_HigepioNative_CODEHEADER_INCLUDED)
#define mfx_hw_HigepioNative_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace hw
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void   HigepioNative::set_pin_mode (int gpio, bcm2837gpio::PinFnc mode) noexcept
{
	assert (gpio >= 0);
	assert (gpio < bcm2837gpio::_nbr_gpio);
	assert (mode >= 0);
	assert (mode < bcm2837gpio::PinFnc_NBR_ELT);

	_ga.set_fnc (gpio, mode);
}



int   HigepioNative::read_pin (int gpio) const noexcept
{
	assert (gpio >= 0);
	assert (gpio < bcm2837gpio::_nbr_gpio);

	return _ga.read (gpio);
}



void   HigepioNative::write_pin (int gpio, int val) const noexcept
{
	assert (gpio >= 0);
	assert (gpio < bcm2837gpio::_nbr_gpio);
	assert (val >= 0);
	assert (val <= 1);

	_ga.write (gpio, val);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace hw
}  // namespace mfx



#endif   // mfx_hw_HigepioNative_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
