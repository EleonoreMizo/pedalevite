/*****************************************************************************

        HigepioWiringPi.cpp
        Author: Laurent de Soras, 2023

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "mfx/hw/HigepioWiringPi.h"

#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <wiringPiI2C.h>

#include <stdexcept>

#include <cassert>



namespace mfx
{
namespace hw
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



HigepioWiringPi::HigepioWiringPi ()
{
	::wiringPiSetupGpio ();
}



void   HigepioWiringPi::set_pin_mode (int gpio, bcm2837gpio::PinFnc mode) noexcept
{
	assert (gpio >= 0);
	assert (gpio < bcm2837gpio::_nbr_gpio);
	assert (mode >= 0);
	assert (mode < bcm2837gpio::PinFnc_NBR_ELT);

	if (mode != bcm2837gpio::PinFnc_IN && mode != bcm2837gpio::PinFnc_OUT)
	{
		// Silently fails
		assert (false);
	}
	else
	{
		::pinMode (gpio, int (mode));
	}
}



int   HigepioWiringPi::read_pin (int gpio) const noexcept
{
	assert (gpio >= 0);
	assert (gpio < bcm2837gpio::_nbr_gpio);

	const auto     val = ::digitalRead (gpio);
	assert (val >= 0);
	assert (val <= 1);

	return val;
}



void   HigepioWiringPi::write_pin (int gpio, int val) const noexcept
{
	assert (gpio >= 0);
	assert (gpio < bcm2837gpio::_nbr_gpio);
	assert (val >= 0);
	assert (val <= 1);

	::digitalWrite (gpio, val);
}



void   HigepioWiringPi::sleep (uint32_t dur_us) const noexcept
{
	::delayMicroseconds (dur_us);
}



HigepioWiringPi::Spi::Spi (const HigepioWiringPi &io, int chn, int speed, const char *err_0)
:	_hnd (::wiringPiSPIGetFd (chn))
,	_chn (chn)
{
	fstb::unused (io);
	assert (chn >= 0);
	assert (chn < 2);
	assert (speed >= _speed_min);
	assert (speed <= _speed_max);

	// Checks if already open
	if (_hnd <= 0)
	{
		_hnd = ::wiringPiSPISetup (chn, speed);
		if (_hnd == -1)
		{
			assert (false);
			if (err_0 == nullptr)
			{
				err_0 = "Cannot open SPI port";
			}
			throw std::runtime_error (err_0);
		}
	}
}



// Returns 0 on success, negative on failure
int   HigepioWiringPi::Spi::rw_data (uint8_t *data_ptr, int len) const noexcept
{
	assert (data_ptr != nullptr);
	assert (len > 0);

	return ::wiringPiSPIDataRW (
		_chn, reinterpret_cast <unsigned char *> (data_ptr), len
	);
}



HigepioWiringPi::I2c::I2c (const HigepioWiringPi &io, int dev_id, const char *err_0)
:	_hnd (::wiringPiI2CSetup (dev_id))
{
	fstb::unused (io);
	assert (dev_id >= 0);
	assert (dev_id <= 0x7F);

	if (_hnd == -1)
	{
		assert (false);
		if (err_0 == nullptr)
		{
			err_0 = "Cannot open I2C port";
		}
		throw std::runtime_error (err_0);
	}
}



uint8_t  HigepioWiringPi::I2c::read_reg_8 (int reg) const noexcept
{
	assert (reg >= 0);
	assert (reg <= 0xFF);

	return uint8_t (::wiringPiI2CReadReg8 (_hnd, reg));
}



uint16_t HigepioWiringPi::I2c::read_reg_16 (int reg) const noexcept
{
	assert (reg >= 0);
	assert (reg <= 0xFF);

	return uint16_t (::wiringPiI2CReadReg16 (_hnd, reg));
}



void  HigepioWiringPi::I2c::write_reg_8 (int reg, uint8_t val) const noexcept
{
	assert (reg >= 0);
	assert (reg <= 0xFF);

	::wiringPiI2CWriteReg8 (_hnd, reg, val);
}



void  HigepioWiringPi::I2c::write_reg_16 (int reg, uint16_t val) const noexcept
{
	assert (reg >= 0);
	assert (reg <= 0xFF);

	::wiringPiI2CWriteReg16 (_hnd, reg, val);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace hw
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
