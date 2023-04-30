/*****************************************************************************

        HigepioNative.h
        Author: Laurent de Soras, 2023

Only Pi 3 and above are supported.
Cannot access GPIO pins >= 31

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_hw_HigepioNative_HEADER_INCLUDED)
#define mfx_hw_HigepioNative_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/hw/bcm2837gpio.h"
#include "mfx/hw/GpioAccess.h"

#include <cstdint>



namespace mfx
{
namespace hw
{



class HigepioNative
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

		explicit       Spi (const HigepioNative &io, int chn, int speed, const char *err_0 = nullptr);
		               Spi (Spi &&other)        = default;
		               ~Spi ();
		Spi &          operator = (Spi &&other) = default;
		int            rw_data (uint8_t *data_ptr, int len) const noexcept;

	private:
		void           clean_up () noexcept;
		int            _hnd   = 0; // File handle, > 0
		int            _speed = 0; // Transmission speed, Hz, > 0

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
		explicit       I2c (const HigepioNative &io, int dev_id, const char *err_0 = nullptr);
		               I2c (I2c &&other)        = default;
		               ~I2c ();
		I2c &          operator = (I2c &&other) = default;
		uint8_t        read_reg_8 (int reg) const noexcept;
		uint16_t       read_reg_16 (int reg) const noexcept;
		void           write_reg_8 (int reg, uint8_t val) const noexcept;
		void           write_reg_16 (int reg, uint16_t val) const noexcept;

	private:
		// ioctl commands for I2C/smbus
		static constexpr int _ioctl_i2c_slave    = 0x0703;
		static constexpr int _ioctl_i2c_smbus    = 0x0720;
		// smbus command arguments
		static constexpr uint8_t   _dir_w        = 0;
		static constexpr uint8_t   _dir_r        = 1;
		static constexpr uint8_t   _byte_data    = 2;
		static constexpr uint8_t   _word_data    = 3;
		union SmbusData
		{
			uint8_t        _u8;
			uint16_t       _u16;
		};
		class SmbusIoctl
		{
		public:
			uint8_t        _rw;
			uint8_t        _cmd;
			uint32_t       _size;
			SmbusData *    _data_ptr;
		};
		void           clean_up () noexcept;
		inline int     access_smbus (int dir, int cmd, int len, SmbusData &data) const noexcept;
		int            _hnd   = 0; // File handle, > 0

	private:
		               I2c ()                               = delete;
		               I2c (const I2c &other)               = delete;
		I2c &          operator = (const I2c &other)        = delete;
		bool           operator == (const I2c &other) const = delete;
		bool           operator != (const I2c &other) const = delete;
	};

	               HigepioNative ();
	               ~HigepioNative () = default;

	inline void    set_pin_mode (int gpio, bcm2837gpio::PinFnc mode) noexcept;
	inline int     read_pin (int gpio) const noexcept;
	inline void    write_pin (int gpio, int val) const noexcept;
	void           sleep (uint32_t dur_us) const noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	GpioAccess     _ga;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               HigepioNative (const HigepioNative &other)     = delete;
	               HigepioNative (HigepioNative &&other)          = delete;
	HigepioNative& operator = (const HigepioNative &other)        = delete;
	HigepioNative& operator = (HigepioNative &&other)             = delete;
	bool           operator == (const HigepioNative &other) const = delete;
	bool           operator != (const HigepioNative &other) const = delete;

}; // class HigepioNative



}  // namespace hw
}  // namespace mfx



#include "mfx/hw/HigepioNative.hpp"



#endif   // mfx_hw_HigepioNative_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
