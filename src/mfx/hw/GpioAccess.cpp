/*****************************************************************************

        GpioAccess.cpp
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

#include "mfx/hw/GpioAccess.h"

#include <bcm_host.h>

#include <fcntl.h>

#include <chrono>
#include <thread>

#include <cassert>



namespace mfx
{
namespace hw
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



GpioAccess::GpioAccess ()
:	_periph_addr (::bcm_host_get_peripheral_address ())
,	_gpio_mptr (
		_periph_addr + bcm2837gpio::_gpio_ofs,
		bcm2837gpio::_gpio_len,
		"/dev/mem", O_RDWR | O_SYNC
	)
,	_gpio_ptr (_gpio_mptr.get ())
{
	// Nothing
}



// This function would require an atomic compare-and-swap to be thread-safe.
// Currently, it is not.
void	GpioAccess::set_fnc (int gpio, bcm2837gpio::PinFnc fnc) const
{
	assert (fnc >= 0);
	assert (fnc < bcm2837gpio::PinFnc_NBR_ELT);

	int            ofs_reg;
	int            shf_bit;
	find_addr_fnc (ofs_reg, shf_bit, gpio);

	const uint32_t msk_base = (1 << bcm2837gpio::_fnc_field_size) - 1;

	uint32_t       val = _gpio_ptr [ofs_reg];
	val &= ~(msk_base << shf_bit);
	val |= uint32_t (fnc) << shf_bit;
	_gpio_ptr [ofs_reg] = val;
}



void	GpioAccess::pull (int gpio, bcm2837gpio::Pull p) const
{
	assert (gpio >= 0);
	assert (gpio < _nbr_gpio);
	assert (p >= 0);
	assert (p < bcm2837gpio::Pull_NBR_ELT);

	// At least 150 cycles
	const std::chrono::microseconds  wait_time (10);

	const uint32_t mask = uint32_t (1) << gpio;
	_gpio_ptr [_reg_pull] = uint32_t (p);
	std::this_thread::sleep_for (wait_time);
	_gpio_ptr [_reg_pclk] = _gpio_ptr [_reg_pclk] | mask;
	std::this_thread::sleep_for (wait_time);
	_gpio_ptr [_reg_pull] = uint32_t (bcm2837gpio::Pull_NONE);
	_gpio_ptr [_reg_pclk] = _gpio_ptr [_reg_pclk] & ~mask;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace hw
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
