/*****************************************************************************

        HigepioWiringPi.h
        Author: Laurent de Soras, 2023

Simple wrapper on the wiringPi library

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_hw_HigepioWiringPi_HEADER_INCLUDED)
#define mfx_hw_HigepioWiringPi_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/hw/bcm2837gpio.h"

#include <cstdint>



namespace mfx
{
namespace hw
{



class HigepioWiringPi
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	class Spi
	{
	public:
		// Maximum theoretical speed is higher (125 MHz) but the physical pins
		// will not follow.
		// https://forums.raspberrypi.com/viewtopic.php?p=1662572#p1662572
		static constexpr int   _speed_min =     32'000; // bit/s
		static constexpr int   _speed_max = 32'000'000; // bit/s

		explicit       Spi (const HigepioWiringPi &io, int chn, int speed, const char *err_0 = nullptr);
		               Spi (Spi &&other)        = default;
		               ~Spi ()                  = default;
		Spi &          operator = (Spi &&other) = default;
		int            rw_data (uint8_t *data_ptr, int len) const noexcept;

	private:
		int            _hnd = 0;
		int            _chn = -1;

	private:
		               Spi ()                               = delete;
		               Spi (const Spi &other)               = delete;
		Spi &          operator = (const Spi &other)        = delete;
		bool           operator == (const Spi &other) const = delete;
		bool           operator != (const Spi &other) const = delete;
	};

	class I2c
	{
	public:
		explicit       I2c (const HigepioWiringPi &io, int dev_id, const char *err_0 = nullptr);
		               I2c (I2c &&other)        = default;
		               ~I2c ()                  = default;
		I2c &          operator = (I2c &&other) = default;
		uint8_t        read_reg_8 (int reg) const noexcept;
		uint16_t       read_reg_16 (int reg) const noexcept;
		void           write_reg_8 (int reg, uint8_t val) const noexcept;
		void           write_reg_16 (int reg, uint16_t val) const noexcept;

	private:
		int            _hnd = 0;

	private:
		               I2c ()                               = delete;
		               I2c (const I2c &other)               = delete;
		I2c &          operator = (const I2c &other)        = delete;
		bool           operator == (const I2c &other) const = delete;
		bool           operator != (const I2c &other) const = delete;
	};

	               HigepioWiringPi ();
	               ~HigepioWiringPi () = default;

	void           set_pin_mode (int gpio, bcm2837gpio::PinFnc mode) noexcept;
	int            read_pin (int gpio) const noexcept;
	void           write_pin (int gpio, int val) const noexcept;
	void           sleep (uint32_t dur_us) const noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               HigepioWiringPi (const HigepioWiringPi &other)   = delete;
	               HigepioWiringPi (HigepioWiringPi &&other)        = delete;
	HigepioWiringPi &
	               operator = (const HigepioWiringPi &other)        = delete;
	HigepioWiringPi &
	               operator = (HigepioWiringPi &&other)             = delete;
	bool           operator == (const HigepioWiringPi &other) const = delete;
	bool           operator != (const HigepioWiringPi &other) const = delete;

}; // class HigepioWiringPi



}  // namespace hw
}  // namespace mfx



//#include "mfx/hw/HigepioWiringPi.hpp"



#endif   // mfx_hw_HigepioWiringPi_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
