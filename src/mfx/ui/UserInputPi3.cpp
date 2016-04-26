/*****************************************************************************

        UserInputPi3.cpp
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

#include "mfx/ui/UserInputPi3.h"

#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <wiringPiI2C.h>

#include <unistd.h>

#include <stdexcept>

#include <cassert>
#include <ctime>



namespace mfx
{
namespace ui
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



const int	UserInputPi3::_gpio_pin_arr [_nbr_sw_gpio] = { 7, 22 };



// Before calling:
// ::wiringPiSetupPhys ()
// ::pinMode (_pin_rst, OUTPUT);
// ::digitalWrite (_pin_rst, LOW);  ::delay (1);
// ::digitalWrite (_pin_rst, HIGH); ::delay (1);
UserInputPi3::UserInputPi3 (std::mutex &mutex_spi)
:	_mutex_spi (mutex_spi)
,	_hnd_23017 (::wiringPiI2CSetup (_i2c_dev_23017))
,	_hnd_3008 (::wiringPiSPISetup (_spi_port, _spi_rate))
,	_recip_list ()
,	_switch_state_arr (do_get_nbr_param (UserInputType_SW))
,	_pot_state_arr (do_get_nbr_param (UserInputType_POT))
,	_msg_pool ()
,	_quit_flag (false)
,	_polling_thread (&UserInputPi3::polling_loop, this)
{
	if (_hnd_23017 == -1)
	{
		throw std::runtime_error ("Error initializing I2C");
	}
	if (_hnd_3008 == -1)
	{
		throw std::runtime_error ("Error initializing SPI");
	}

	_msg_pool.expand_to (256);
	for (int i = 0; i < UserInputType_NBR_ELT; ++i)
	{
		const int      nbr_dev =
			do_get_nbr_param (static_cast <UserInputType> (i));
		_recip_list [i].resize (nbr_dev, 0);
	}

	::wiringPiI2CWriteReg8 (_hnd_23017, Cmd23017_IOCONA, IOCon_MIRROR);

	// All the pins are set in read mode.
	::wiringPiI2CWriteReg16 (_hnd_23017, Cmd23017_IODIRA, 0xFFFF);
}



UserInputPi3::~UserInputPi3 ()
{
	if (_polling_thread.joinable ())
	{
		_quit_flag = true;
		_polling_thread.join ();
	}

	if (_hnd_3008 != -1)
	{
		close (_hnd_3008);
		_hnd_3008 = -1;
	}
	if (_hnd_23017 != -1)
	{
		close (_hnd_23017);
		_hnd_23017 = -1;
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	UserInputPi3::do_get_nbr_param (UserInputType type) const
{
	int            nbr = 0;

	if (type == UserInputType_POT)
	{
		nbr = _nbr_adc;
	}
	else if (type == UserInputType_SW)
	{
		nbr = _nbr_sw_23017 + _nbr_sw_gpio;
	}

	return nbr;
}



void	UserInputPi3::do_set_msg_recipient (UserInputType type, int index, MsgQueue *queue_ptr)
{
	_recip_list [type] [index] = queue_ptr;
}



void	UserInputPi3::do_return_cell (MsgCell &cell)
{
	_msg_pool.return_cell (cell);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	UserInputPi3::polling_loop ()
{
	while (! _quit_flag)
	{
		const int64_t  cur_time = read_clock_ns ();

		// On the port expander
		uint16_t       state_all =
			::wiringPiI2CReadReg16 (_hnd_23017, Cmd23017_GPIOA);
		for (int i = 0; i < _nbr_sw_23017 && ! _quit_flag; ++i)
		{
			const bool     flag  = (((state_all >> i) & 1) == 0);
			const int      index = i;
			handle_switch (index, flag, cur_time);
		}

		// On the GPIO
		for (int i = 0; i < _nbr_sw_gpio && ! _quit_flag; ++i)
		{
			const int      sw_val = ::digitalRead (_gpio_pin_arr [i]);
			const bool     flag   = (sw_val == 0);
			const int      index  = _nbr_sw_23017 + i;
			handle_switch (index, flag, cur_time);
		}

		// Potentiometers
		for (int i = 0; i < _nbr_adc && ! _quit_flag; ++i)
		{
			const int      val = read_adc (_spi_port, i);
			if (val >= 0)
			{
				handle_adc (i, val, cur_time);
			}
		}

		// 10 ms between updates
		::delay (10);
	}

	_quit_flag = false;
}



void	UserInputPi3::handle_switch (int index, bool flag, int64_t cur_time)
{
	SwitchState &  sw = _switch_state_arr [index];

	if (flag != sw._flag)
	{
		const int64_t  dist = cur_time - sw._time_last;
		if (dist >= _antibounce_time)
		{
			sw._flag      = flag;
			sw._time_last = cur_time;

			MsgQueue *     queue_ptr = _recip_list [UserInputType_SW] [index];
			if (queue_ptr != 0)
			{
				MsgCell *      cell_ptr = _msg_pool.take_cell (true);
				if (cell_ptr == 0)
				{
					assert (false);
				}
				else
				{
					cell_ptr->_val.set (UserInputType_SW, index, (flag) ? 1 : 0);
					queue_ptr->enqueue (*cell_ptr);
				}
			}
		}
	}
}



void	UserInputPi3::handle_adc (int index, int val, int64_t cur_time)
{
	PotState &     pot      = _pot_state_arr [index];

	// Filters out the oscillations between two consecutive values
	bool           new_flag = false;
	const int      d_cur    = std::abs (val - pot._cur_val);
	const int      d_alt    = std::abs (val - pot._alt_val);
	if (d_cur != 0 && d_alt != 0)
	{
		new_flag = true;
		if (d_cur == 1)
		{
			pot._alt_val = pot._cur_val;
		}
		else if (d_alt > 1)
		{
			pot._alt_val = PotState::_val_none;
		}
		pot._cur_val = val;
	}
	else if (d_cur != 0)
	{
		assert (d_alt == 0);
		assert (d_cur == 1);
	}
	else
	{
		assert (d_cur == 0);
	}

	if (new_flag)
	{
		MsgQueue *     queue_ptr = _recip_list [UserInputType_SW] [index];
		if (queue_ptr != 0)
		{
			MsgCell *      cell_ptr = _msg_pool.take_cell (true);
			if (cell_ptr == 0)
			{
				assert (false);
			}
			else
			{
				const float    val_flt = val * (1.0f / ((1 << _res_adc) - 1));
				cell_ptr->_val.set (UserInputType_POT, index, val_flt);
				queue_ptr->enqueue (*cell_ptr);
			}
		}
	}
}



// Returns -1 on error
// Valid results are in range 0-1023
int	UserInputPi3::read_adc (int port, int chn)
{
	assert (port >= 0);
	assert (port < 2);
	assert (chn >= 0);
	assert (chn < 8);

	// MCP doc, p. 21
	// We could shift everything from 7 bits to improve the latency.
	const int      msg_len = 3;
	uint8_t        buffer [msg_len] =
	{
		0x01,
		uint8_t (chn << 4),
		0
	};

	std::lock_guard <std::mutex>   lock (_mutex_spi);

	int            ret_val = ::wiringPiSPIDataRW (port, &buffer [0], msg_len);
	if (ret_val != -1)
	{
		ret_val = ((buffer [1] & 3) << 8) + buffer [2];
	}

	return ret_val;
}



int64_t	UserInputPi3::read_clock_ns () const
{
	timespec       tp;
	clock_gettime (CLOCK_REALTIME, &tp);

	const long     ns_mul = 1000L * 1000L * 1000L;
	return int64_t (tp.tv_sec) * ns_mul + tp.tv_nsec;
}



}  // namespace ui
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
