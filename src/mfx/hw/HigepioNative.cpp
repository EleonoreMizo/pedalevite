/*****************************************************************************

        HigepioNative.cpp
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
#include "fstb/fnc.h"
#include "mfx/hw/HigepioNative.h"

#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdexcept>
#include <system_error>

#include <cassert>
#include <climits>
#include <ctime>



namespace mfx
{
namespace hw
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



HigepioNative::HigepioNative ()
{
	// Nothing at the moment
}



void   HigepioNative::sleep (uint32_t dur_us) const noexcept
{
	assert (dur_us >= 0);

	constexpr auto us_per_s = uint32_t (1'000'000);

	if (dur_us == 0)
	{
		return;
	}

	// Short duration: active wait
	else if (dur_us < 100)
	{
		timeval        now {};
		gettimeofday (&now, nullptr);

		const auto     part_s   = dur_us / us_per_s;
		const auto     part_us  = dur_us - part_s * us_per_s;
		timeval        duration {};
		duration.tv_sec  = time_t (part_s);
		duration.tv_usec = suseconds_t (part_us);

		timeval        end {};
		timeradd (&now, &duration, &end);

		while (timercmp (&now, &end, < ))
		{
			gettimeofday (&now, nullptr);
		}
	}

	// Longer duration: system call
	else
	{
		const auto     part_s   = dur_us / us_per_s;
		const auto     part_us  = dur_us - part_s * us_per_s;
		timespec       duration {};
		duration.tv_sec  = time_t (part_s);
		duration.tv_nsec = long (part_us) * 1000;
		nanosleep (&duration, nullptr);
	}
}



HigepioNative::Spi::Spi (const HigepioNative &io, int chn, int speed, const char *err_0)
:	_speed (speed)
{
	fstb::unused (io);
	assert (chn >= 0);
	assert (chn < 2);
	assert (speed >= _speed_min);
	assert (speed <= _speed_max);

	char           dev_0 [99+1];
	fstb::snprintf4all (dev_0, sizeof (dev_0), "/dev/spidev0.%d", chn);
	_hnd = open (dev_0, O_RDWR);
	if (_hnd < 0)
	{
		if (err_0 == nullptr)
		{
			err_0 = "Cannot open SPI port";
		}
		throw std::system_error (
			std::error_code (errno, std::system_category ()), err_0
		);
	}

	try
	{
		int            mode = 0;
		if (ioctl (_hnd, SPI_IOC_WR_MODE, &mode) < 0)
		{
			throw std::system_error (
				std::error_code (errno, std::system_category ()),
				"Cannot set SPI mode"
			);
		}

		uint8_t        bits_per_word = CHAR_BIT;
		if (ioctl (_hnd, SPI_IOC_WR_BITS_PER_WORD, &bits_per_word) < 0)
		{
			throw std::system_error (
				std::error_code (errno, std::system_category ()),
				"Cannot set SPI word size"
			);
		}

		if (ioctl (_hnd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0)
		{
			throw std::system_error (
				std::error_code (errno, std::system_category ()),
				"Cannot set SPI maximum speed"
			);
		}
	}
	catch (...)
	{
		clean_up ();
		throw;
	}
}



HigepioNative::Spi::~Spi ()
{
	clean_up ();
}



// Returns 0 on success, negative on failure
int   HigepioNative::Spi::rw_data (uint8_t *data_ptr, int len) const noexcept
{
	assert (data_ptr != nullptr);
	assert (len > 0);

	int               ret_val = 0;

	spi_ioc_transfer  msg {}; // Defaults everything to 0
	msg.tx_buf        = reinterpret_cast <intptr_t> (data_ptr);
	msg.rx_buf        = reinterpret_cast <intptr_t> (data_ptr);
	msg.len           = len;
	msg.speed_hz      = _speed;
	msg.delay_usecs   = 0;
	msg.bits_per_word = CHAR_BIT;
	msg.cs_change     = 0;

	if (ioctl (_hnd, SPI_IOC_MESSAGE (1), &msg) < 0)
	{
		ret_val = -1;
	}

	return ret_val;
}



void   HigepioNative::Spi::clean_up () noexcept
{
	if (_hnd >= 0)
	{
		close (_hnd);
		_hnd = -1;
	}
}



HigepioNative::I2c::I2c (const HigepioNative &io, int dev_id, const char *err_0)
:	_hnd (open ("/dev/i2c-1", O_RDWR))
{
	fstb::unused (io);
	assert (dev_id >= 0);
	assert (dev_id <= 0x7F);

	if (_hnd < 0)
	{
		if (err_0 == nullptr)
		{
			err_0 = "Cannot open I2C port";
		}
		throw std::system_error (
			std::error_code (errno, std::system_category ()), err_0
		);
	}

	try
	{
		if (ioctl (_hnd, _ioctl_i2c_slave, dev_id) < 0)
		{
			throw std::system_error (
				std::error_code (errno, std::system_category ()),
				"Cannot select I2C device"
			);
		}
	}
	catch (...)
	{
		clean_up ();
		throw;
	}
}



HigepioNative::I2c::~I2c ()
{
	clean_up ();
}



uint8_t  HigepioNative::I2c::read_reg_8 (int reg) const noexcept
{
	assert (reg >= 0);
	assert (reg <= 0xFF);

	SmbusData      smbd;
	smbd._u8 = 0;
	const int      ret_val = access_smbus (_dir_r, reg, _byte_data, smbd);
	fstb::unused (ret_val);
	assert (ret_val == 0);

	return smbd._u8;
}



uint16_t HigepioNative::I2c::read_reg_16 (int reg) const noexcept
{
	assert (reg >= 0);
	assert (reg <= 0xFF);

	SmbusData      smbd;
	smbd._u16 = 0;
	const int      ret_val = access_smbus (_dir_r, reg, _word_data, smbd);
	fstb::unused (ret_val);
	assert (ret_val == 0);

	return smbd._u16;
}



void  HigepioNative::I2c::write_reg_8 (int reg, uint8_t val) const noexcept
{
	assert (reg >= 0);
	assert (reg <= 0xFF);

	SmbusData      smbd;
	smbd._u8 = val;
	const int      ret_val = access_smbus (_dir_w, reg, _byte_data, smbd);
	fstb::unused (ret_val);
	assert (ret_val == 0);
}



void  HigepioNative::I2c::write_reg_16 (int reg, uint16_t val) const noexcept
{
	assert (reg >= 0);
	assert (reg <= 0xFF);

	SmbusData      smbd;
	smbd._u16 = val;
	const int      ret_val = access_smbus (_dir_w, reg, _word_data, smbd);
	fstb::unused (ret_val);
	assert (ret_val == 0);
}



void	HigepioNative::I2c::clean_up () noexcept
{
	if (_hnd >= 0)
	{
		close (_hnd);
		_hnd = -1;
	}
}



int	HigepioNative::I2c::access_smbus (int dir, int cmd, int len, SmbusData &data) const noexcept
{
	SmbusIoctl arg { uint8_t (dir), uint8_t (cmd), uint32_t (len), &data };

	return ioctl (_hnd, _ioctl_i2c_smbus, &arg);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace hw
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
