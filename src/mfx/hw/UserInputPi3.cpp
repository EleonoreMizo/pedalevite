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

#include "mfx/hw/mcp23017.h"
#include "mfx/hw/UserInputPi3.h"
#include "mfx/ui/TimeShareThread.h"

#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <wiringPiI2C.h>

#include <unistd.h>

#include <chrono>
#include <stdexcept>
#include <thread>

#include <cassert>
#include <climits>
#include <ctime>



namespace mfx
{
namespace hw
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



const std::chrono::nanoseconds	UserInputPi3::_antibounce_time (
	std::chrono::milliseconds (30)
);

// Slave address, p. 8
const int	UserInputPi3::_i2c_dev_23017_arr [_nbr_dev_23017] =
{
	0x20 + 0,
	0x20 + 1
};

const int	UserInputPi3::_gpio_pin_arr [_nbr_sw_gpio] = { 7, 22 };

const UserInputPi3::SwitchSrc	UserInputPi3::_switch_arr [_nbr_switches] =
{
	{ BinSrc_GPIO    ,    0 },
	{ BinSrc_GPIO    ,    1 },
	{ BinSrc_PORT_EXP, 0x00 },
	{ BinSrc_PORT_EXP, 0x01 },
	{ BinSrc_PORT_EXP, 0x02 },
	{ BinSrc_PORT_EXP, 0x03 },
	{ BinSrc_PORT_EXP, 0x04 },
	{ BinSrc_PORT_EXP, 0x05 },
	{ BinSrc_PORT_EXP, 0x06 },
	{ BinSrc_PORT_EXP, 0x07 },
	{ BinSrc_PORT_EXP, 0x08 },
	{ BinSrc_PORT_EXP, 0x09 },
	{ BinSrc_PORT_EXP, 0x0A },
	{ BinSrc_PORT_EXP, 0x0B },
	{ BinSrc_PORT_EXP, 0x0C },
	{ BinSrc_PORT_EXP, 0x0D },
	{ BinSrc_PORT_EXP, 0x0E },
	{ BinSrc_PORT_EXP, 0x0F },
	{ BinSrc_PORT_EXP, 0x1C },
	{ BinSrc_PORT_EXP, 0x1F }
};

const UserInputPi3::RotEncSrc	UserInputPi3::_rotenc_arr [Cst::RotEnc_NBR_ELT] =
{
	{ BinSrc_PORT_EXP, 0x10, 0x11, -1 },
	{ BinSrc_PORT_EXP, 0x12, 0x13, -1 },
	{ BinSrc_PORT_EXP, 0x14, 0x15, -1 },
	{ BinSrc_PORT_EXP, 0x16, 0x17, -1 },
	{ BinSrc_PORT_EXP, 0x18, 0x19, -1 },

	{ BinSrc_PORT_EXP, 0x1A, 0x1B, -1 },
	{ BinSrc_PORT_EXP, 0x1D, 0x1E, -1 }
};

const int UserInputPi3::_pot_arr [Cst::_nbr_pot] =
{
	2, 3, 4
};


// Before calling:
// ::wiringPiSetupPhys ()
// ::pinMode (_pin_rst, OUTPUT);
// ::digitalWrite (_pin_rst, LOW);  ::delay (100);
// ::digitalWrite (_pin_rst, HIGH); ::delay (1);
UserInputPi3::UserInputPi3 (ui::TimeShareThread &thread_spi)
:	_thread_spi (thread_spi)
,	_hnd_23017_arr ()
,	_hnd_3008 (::wiringPiSPISetup (_spi_port, _spi_rate))
,	_recip_list ()
,	_switch_state_arr ()
,	_pot_state_arr ()
,	_rotenc_state_arr ()
,	_msg_pool ()
,	_quit_flag (false)
,	_polling_thread ()
,	_polling_count (0)
{
	for (int p = 0; p < _nbr_dev_23017; ++p)
	{
		_hnd_23017_arr [p] = ::wiringPiI2CSetup (_i2c_dev_23017_arr [p]);
		if (_hnd_23017_arr [p] == -1)
		{
			close_everything ();
			throw std::runtime_error ("Error initializing I2C");
		}
	}
	if (_hnd_3008 == -1)
	{
		close_everything ();
		throw std::runtime_error ("Error initializing SPI");
	}

	_msg_pool.expand_to (256);
	for (int i = 0; i < ui::UserInputType_NBR_ELT; ++i)
	{
		const int      nbr_dev =
			do_get_nbr_param (static_cast <ui::UserInputType> (i));
		_recip_list [i].resize (nbr_dev, 0);
	}

	for (int p = 0; p < _nbr_dev_23017; ++p)
	{
		::wiringPiI2CWriteReg8 (
			_hnd_23017_arr [p], mcp23017::cmd_iocona, mcp23017::iocon_mirror
		);

		// All the pins are set in read mode.
		::wiringPiI2CWriteReg16 (
			_hnd_23017_arr [p], mcp23017::cmd_iodira, 0xFFFF
		);
	}

	// Initial read
	read_data (true);

	_polling_thread = std::thread (&UserInputPi3::polling_loop, this);
	_thread_spi.register_cb (
		*this,
		std::chrono::microseconds (1000 * 1000 / 100) // 100 Hz refresh rate
	);
}



UserInputPi3::~UserInputPi3 ()
{
	_thread_spi.remove_cb (*this);
	close_everything ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	UserInputPi3::do_get_nbr_param (ui::UserInputType type) const
{
	int            nbr = 0;

	if (type == ui::UserInputType_POT)
	{
		nbr = Cst::_nbr_pot;
	}
	else if (type == ui::UserInputType_SW)
	{
		nbr = _nbr_switches;
	}
	else if (type == ui::UserInputType_ROTENC)
	{
		nbr = Cst::RotEnc_NBR_ELT;
	}

	return nbr;
}



void	UserInputPi3::do_set_msg_recipient (ui::UserInputType type, int index, MsgQueue *queue_ptr)
{
	const bool     send_flag =
		(queue_ptr != 0 &&_recip_list [type] [index] != queue_ptr);

	_recip_list [type] [index] = queue_ptr;

	if (send_flag)
	{
		switch (type)
		{
		case ui::UserInputType_SW:
			{
				// Creates an event only if the switch is ON (assumes OFF by default).
				const SwitchState &  state = _switch_state_arr [index];
				if (state.is_set () && state._flag)
				{
					enqueue_val (
						state._time_last,
						ui::UserInputType_SW,
						index,
						(state._flag) ? 1 : 0
					);
				}
			}
			break;
		case ui::UserInputType_POT:
			{
				const PotState &  state = _pot_state_arr [index];
				if (state.is_set ())
				{
					const float    val_flt  =
						state._cur_val * (1.0f / ((1 << _res_adc) - 1));
					const std::chrono::nanoseconds   cur_time (read_clock_ns ());
					enqueue_val (
						cur_time,
						ui::UserInputType_POT,
						index,
						val_flt
					);
				}
			}
			break;
		default:
			// Nothing
			break;
		}
	}
}



void	UserInputPi3::do_return_cell (MsgCell &cell)
{
	_msg_pool.return_cell (cell);
}



std::chrono::microseconds	UserInputPi3::do_get_cur_date () const
{
	return std::chrono::duration_cast <std::chrono::microseconds> (
		read_clock_ns ()
	);
}



bool	UserInputPi3::do_process_timeshare_op ()
{
	const std::chrono::nanoseconds   cur_time (read_clock_ns ());

	// Potentiometers
	for (int i = 0; i < Cst::_nbr_pot; ++i)
	{
		const int      adc_index = _pot_arr [i];
		const int      val       = read_adc (_spi_port, adc_index);
		if (val >= 0)
		{
			handle_pot (i, val, cur_time);
		}
	}

	return false;
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	UserInputPi3::close_everything ()
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
	for (int p = 0; p < _nbr_dev_23017; ++p)
	{
		if (_hnd_23017_arr [p] != -1)
		{
			close (_hnd_23017_arr [p]);
			_hnd_23017_arr [p] = -1;
		}
	}
}



void	UserInputPi3::polling_loop ()
{
	while (! _quit_flag)
	{
		const bool     low_freq_flag = ((_polling_count & 15) == 0);
		read_data (low_freq_flag);

		// 1 ms between updates. Not less because of the rotary encoders.
		std::this_thread::sleep_for (std::chrono::milliseconds (1));
		++ _polling_count;
	}

	_quit_flag = false;
}



void	UserInputPi3::read_data (bool low_freq_flag)
{
	typedef unsigned int InputState;
	static_assert (
		sizeof (InputState) * CHAR_BIT >= _nbr_sw_23017 * _nbr_dev_23017,
		"state capacity for MCP23017"
	);
	static_assert (
		sizeof (InputState) * CHAR_BIT >= _nbr_sw_gpio,
		"state capacity for GPIO"
	);

	const std::chrono::nanoseconds   cur_time (read_clock_ns ());
		
	// Reads all binary inputs first
	InputState     input_state_arr [BinSrc_NBR_ELT] = { 0, 0 };

	static const InputState mask = (1U << _nbr_sw_23017) - 1;
	for (int p = 0; p < _nbr_dev_23017; ++p)
	{
		InputState     dev_state = InputState (
			::wiringPiI2CReadReg16 (_hnd_23017_arr [p], mcp23017::cmd_gpioa)
		);
		dev_state ^= mask;
		input_state_arr [BinSrc_PORT_EXP] |= dev_state << (p * _nbr_sw_23017);
	}

	if (low_freq_flag)
	{
		for (int p = 0; p < _nbr_sw_gpio; ++p)
		{
			InputState     sw_val = InputState (
				::digitalRead (_gpio_pin_arr [p]) & 1
			);
			sw_val ^= 1;
			input_state_arr [BinSrc_GPIO] |= sw_val << p;
		}

		// Switches
		for (int s = 0; s < _nbr_switches; ++s)
		{
			const SwitchSrc & src = _switch_arr [s];
			const int      val    = (input_state_arr [src._type] >> src._pos) & 1;
			const bool     flag   = (val != 0);
			handle_switch (s, flag, cur_time);
		}
	}

	// Rotary incremental encoders
	for (int i = 0; i < Cst::RotEnc_NBR_ELT; ++i)
	{
		const RotEncSrc & src       = _rotenc_arr [i];
		const InputState  src_state = input_state_arr [src._type];
		const int         v0        = (src_state >> src._pos_0) & 1;
		const int         v1        = (src_state >> src._pos_1) & 1;
		handle_rotenc (i, (v0 != 0), (v1 != 0), cur_time);
	}
}



void	UserInputPi3::handle_switch (int index, bool flag, std::chrono::nanoseconds cur_time)
{
	SwitchState &  sw = _switch_state_arr [index];

	const std::chrono::nanoseconds   dist (cur_time - sw._time_last);
	if (flag != sw._flag && dist >= _antibounce_time)
	{
		sw._flag      = flag;
		sw._time_last = cur_time;
		enqueue_val (cur_time, ui::UserInputType_SW, index, (flag) ? 1 : 0);
	}
	else if (! sw.is_set ())
	{
		// Does noe generate any event when reading the initial value
		sw._flag      = flag;
		sw._time_last = cur_time;
	}
}



void	UserInputPi3::handle_rotenc (int index, bool f0, bool f1, std::chrono::nanoseconds cur_time)
{
	ui::RotEnc &   re  = _rotenc_state_arr [index];
	const int      dir = _rotenc_arr [index]._dir_mul;
	const int      inc = re.set_new_state (f0, f1) * dir;
	if (inc != 0)
	{
		enqueue_val (cur_time, ui::UserInputType_ROTENC, index, inc);
	}
}



void	UserInputPi3::handle_pot (int index, int val, std::chrono::nanoseconds cur_time)
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
		const float    val_flt = val * (1.0f / ((1 << _res_adc) - 1));
		enqueue_val (cur_time, ui::UserInputType_POT, index, val_flt);
	}
}



// date is in nanoseconds
void	UserInputPi3::enqueue_val (std::chrono::nanoseconds date, ui::UserInputType type, int index, float val)
{
	MsgQueue *     queue_ptr = _recip_list [type] [index];
	if (queue_ptr != 0)
	{
		MsgCell *      cell_ptr = _msg_pool.take_cell (true);
		if (cell_ptr == 0)
		{
			assert (false);
		}
		else
		{
			cell_ptr->_val.set (
				std::chrono::duration_cast <std::chrono::microseconds> (date),
				type,
				index,
				val
			);
			queue_ptr->enqueue (*cell_ptr);
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

	// MCP3008 doc, p. 21

	// Amount of bit shifting, from 0 to 7.
	// Only 2 and 3 are compatible with the 12864ZH (ST7920) Chip Select bug.
	static const int  s = 3;

	const int      msg_len = 3;
	const int      chns4   = chn << (s + 4);
	uint8_t        buffer [msg_len] =
	{
		uint8_t ((0x01 << s) + (chns4 >> 8)),
		uint8_t (chns4),
		0
	};

	int            ret_val = ::wiringPiSPIDataRW (port, &buffer [0], msg_len);
	if (ret_val != -1)
	{
		ret_val = (((buffer [1] << 8) + buffer [2]) >> s) & 0x3FF;
	}

	return ret_val;
}



std::chrono::nanoseconds	UserInputPi3::read_clock_ns () const
{
	timespec       tp;
	clock_gettime (CLOCK_REALTIME, &tp);

	const long     ns_mul = 1000L * 1000L * 1000L;

	return std::chrono::nanoseconds (int64_t (tp.tv_sec) * ns_mul + tp.tv_nsec);
}



UserInputPi3::SwitchState::SwitchState ()
:	_flag (false)
,	_time_last (INT64_MIN)
{
	// Nothing
}



bool	UserInputPi3::SwitchState::is_set () const
{
	return (_time_last.count () != INT64_MIN);
}



}  // namespace hw
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
