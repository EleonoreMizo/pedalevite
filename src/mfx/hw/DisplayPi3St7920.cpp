/*****************************************************************************

        DisplayPi3St7920.cpp
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

#include "mfx/hw/DisplayPi3St7920.h"
#include "mfx/ui/TimeShareThread.h"

#include <wiringPi.h>
#include <wiringPiSPI.h>

#include <unistd.h>

#include <algorithm>
#include <stdexcept>

#include <cassert>
#include <climits>



namespace mfx
{
namespace hw
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// Before calling:
// ::wiringPiSetupPhys ()
// ::pinMode (_pin_rst, OUTPUT);
// ::digitalWrite (_pin_rst, LOW);  ::delay (100);
// ::digitalWrite (_pin_rst, HIGH); ::delay (1);
DisplayPi3St7920::DisplayPi3St7920 (ui::TimeShareThread &thread_spi)
:	_thread_spi (thread_spi)
,	_state (State_INIT)
,	_screen_buf ()
,	_hnd_spi (::wiringPiSPISetupMode (_spi_port, _spi_rate, 0))
,	_msg_pool ()
,	_msg_queue ()
,	_redraw ()
{
	if (_hnd_spi == -1)
	{
		throw std::runtime_error ("DisplayPi3St7920: cannot open SPI port.");
	}

	_msg_pool.expand_to (256);

	_thread_spi.register_cb (
		*this,
		std::chrono::microseconds (1000 * 1000 / 20)
	); // 20 fps max refresh rate
}



DisplayPi3St7920::~DisplayPi3St7920 ()
{
	_thread_spi.remove_cb (*this);

	MsgCell *      cell_ptr = 0;
	do
	{
		cell_ptr = _msg_queue.dequeue ();
		if (cell_ptr != 0)
		{
			return_cell (*cell_ptr);
		}
	}
	while (cell_ptr != 0);

	close (_hnd_spi);
	_hnd_spi = -1;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	DisplayPi3St7920::do_get_width () const
{
	return _scr_w;
}



int	DisplayPi3St7920::do_get_height () const
{
	return _scr_h;
}



int	DisplayPi3St7920::do_get_stride () const
{
	return _scr_w;
}



uint8_t *	DisplayPi3St7920::do_use_screen_buf ()
{
	return &_screen_buf [0];
}



const uint8_t *	DisplayPi3St7920::do_use_screen_buf () const
{
	return &_screen_buf [0];
}



void	DisplayPi3St7920::do_refresh (int x, int y, int w, int h)
{
	MsgCell *      cell_ptr = _msg_pool.take_cell (true);
	if (cell_ptr == 0)
	{
		assert (false);
	}
	else
	{
		cell_ptr->_val._x = x;
		cell_ptr->_val._y = y;
		cell_ptr->_val._w = w;
		cell_ptr->_val._h = h;

		_msg_queue.enqueue (*cell_ptr);
	}
}



void	DisplayPi3St7920::do_force_reset ()
{
	_state = State_INIT;
}



bool	DisplayPi3St7920::do_process_timeshare_op ()
{
	const state_now = _state;
	if (state_now == State_INIT)
	{
		init_device ();
	}
	else if (state_now == State_REDRAW)
	{
		redraw_part ();
	}
	else
	{
		check_msg ();
	}

	return (_state == State_REDRAW);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DisplayPi3St7920::return_cell (MsgCell &cell)
{
	_msg_pool.return_cell (cell);
}



void	DisplayPi3St7920::init_device ()
{
	assert (_state == State_INIT);

	::pinMode  (_pin_cs , OUTPUT);

	send_cmd (Cmd_FNC_SET | Cmd_FNC_SET_DL); // Twice because RE cannot be set
	send_cmd (Cmd_FNC_SET | Cmd_FNC_SET_DL); // at the same time as other bits.

	send_cmd (Cmd_CLEAR);
	::delayMicroseconds (_delay_clr);
	send_cmd (Cmd_ENTRY     | Cmd_ENTRY_ID);
	send_cmd (Cmd_DISPLAY   | Cmd_DISPLAY_DISP);

	send_cmd (Cmd_FNC_SET_E | Cmd_FNC_SET_DL);
	send_cmd (Cmd_FNC_SET_E | Cmd_FNC_SET_DL | Cmd_FNC_SET_E_G);
	send_cmd (Cmd_RAM_SEL);

	_state = State_IDLE;
}



void	DisplayPi3St7920::check_msg ()
{
	int            x1 = INT_MAX;
	int            y1 = INT_MAX;
	int            x2 = INT_MIN;
	int            y2 = INT_MIN;
	MsgCell *      cell_ptr = 0;
	do
	{
		cell_ptr = _msg_queue.dequeue ();
		if (cell_ptr != 0)
		{
			x1 = std::min (x1, cell_ptr->_val._x);
			y1 = std::min (y1, cell_ptr->_val._y);
			x2 = std::max (x2, cell_ptr->_val._x + cell_ptr->_val._w);
			y2 = std::max (y2, cell_ptr->_val._y + cell_ptr->_val._h);

			return_cell (*cell_ptr);
		}
	}
	while (cell_ptr != 0);

	if (x1 < x2)
	{
		start_redraw (x1, y1, x2 - x1, y2 - y1);
		redraw_part ();
	}
}



void	DisplayPi3St7920::start_redraw (int x, int y, int w, int h)
{
	assert (_state == State_IDLE);
	assert (x >= 0);
	assert (y >= 0);
	assert (w > 0);
	assert (h > 0);
	assert (x + w <= _scr_w);
	assert (y + h <= _scr_h);

	_redraw._y       = y;
	_redraw._h       = h;
	_redraw._col_beg =   x           >> 4;
	_redraw._nbr_col = ((x + w + 15) >> 4) - _redraw._col_beg;
	const int        stride  = get_stride ();
	_redraw._pix_ptr = &_screen_buf [(_redraw._col_beg << 4) + y * stride];

	_redraw._nbr_pairs = 0;
	_redraw._pair_cnt  = 0;
	if (_redraw._col_beg == 0 && _redraw._nbr_col == _scr_w >> 4)
	{
		const int        half_h = _scr_h / 2;
		_redraw._nbr_pairs = h - half_h;
	}
	
	_state = State_REDRAW;
}



void	DisplayPi3St7920::redraw_part ()
{
	assert (_state == State_REDRAW);

	const int        stride = get_stride ();

	const int        max_nbr_lines = 8;
	for (int cnt = 0; cnt < max_nbr_lines && _state == State_REDRAW; ++cnt)
	{
		if (_redraw._pair_cnt < _redraw._nbr_pairs)
		{
			const int        half_h = _scr_h / 2;
			send_2_full_lines (
				_redraw._y,
				_redraw._pix_ptr,
				_redraw._pix_ptr + stride * half_h
			);
			_redraw._pix_ptr += stride;
			++ _redraw._y;
			++ _redraw._pair_cnt;
			_redraw._h -= 2;
		}
		else if (_redraw._h > 0)
		{
			send_line (
				_redraw._col_beg,
				_redraw._y,
				_redraw._pix_ptr,
				_redraw._nbr_col
			);
			_redraw._pix_ptr += stride;
			++ _redraw._y;
			-- _redraw._h;
		}

		// Finished?
		if (_redraw._h <= 0)
		{
			_state = State_IDLE;
		}
	}
}



void	DisplayPi3St7920::send_mode (Mode mode)
{
	uint8_t        buffer [1] = { uint8_t (mode) };
	::wiringPiSPIDataRW (_spi_port, &buffer [0], sizeof (buffer));
}



void	DisplayPi3St7920::send_byte_raw (uint8_t a)
{
	uint8_t        buffer [2] =
	{
		uint8_t (a & 0xF0),
		uint8_t (a << 4)
	};
	::wiringPiSPIDataRW (_spi_port, &buffer [0], sizeof (buffer));
}



void	DisplayPi3St7920::send_byte_header (uint8_t rwrs, uint8_t a)
{
	uint8_t        buffer [3] =
	{
		uint8_t (Serial_HEADER | rwrs),
		uint8_t (a & 0xF0),
		uint8_t (a << 4)
	};
	::wiringPiSPIDataRW (_spi_port, &buffer [0], sizeof (buffer));
}



void	DisplayPi3St7920::send_cmd (uint8_t x)
{
	::digitalWrite (_pin_cs, HIGH);
	send_byte_header (0, x);
	::digitalWrite (_pin_cs, LOW);
	::delayMicroseconds (_delay_std);
}



void	DisplayPi3St7920::send_data (uint8_t x)
{
	::digitalWrite (_pin_cs, HIGH);
	send_byte_header (Serial_RS, x);
	::digitalWrite (_pin_cs, LOW);
	::delayMicroseconds (_delay_std);
}



// col is in 16-pixel blocks, len too
void	DisplayPi3St7920::send_line (int col, int y, const uint8_t pix_ptr [], int len)
{
	assert (col >= 0);
	assert (y >= 0);
	assert (y < _scr_h);
	assert (pix_ptr != 0);
	assert (len > 0);
	assert ((len + col) * 16 <= _scr_w);

	int            ofs_y = 0;
	int            ofs_x = col;
	if (y >= _scr_h / 2)
	{
		ofs_y  = -(_scr_h / 2);
		ofs_x += _scr_w >> 4;
	}

	SpiBuffer       spibuf;
	int             spipos = 0;
	send_line_prologue (ofs_x, y + ofs_y, spibuf, spipos);

	prepare_line_data (spibuf, spipos, pix_ptr, len);
	::wiringPiSPIDataRW (_spi_port, &spibuf [0], spipos);

	send_line_epilogue ();
}



void	DisplayPi3St7920::send_2_full_lines (int y, const uint8_t pix1_ptr [], const uint8_t pix2_ptr [])
{
	assert (y >= 0);
	assert (y < _scr_h / 2);
	assert (pix1_ptr != 0);
	assert (pix2_ptr != 0);

	SpiBuffer       spibuf;
	int             spipos = 0;
	send_line_prologue (0, y, spibuf, spipos);

	const int       len = _scr_w >> 4;
	prepare_line_data (spibuf, spipos, pix1_ptr, len);
	prepare_line_data (spibuf, spipos, pix2_ptr, len);
	::wiringPiSPIDataRW (_spi_port, &spibuf [0], spipos);

	send_line_epilogue ();
}



void	DisplayPi3St7920::prepare_line_data (SpiBuffer &buf, int &pos, const uint8_t pix_ptr [], int len)
{
	for (int x = 0; x < len; ++x)
	{
		uint16_t       val = 0;
		for (int x2 = 0; x2 < 16; ++x2)
		{
			val <<= 1;
			val |= *pix_ptr >> 7;
			++ pix_ptr;
		}

		const uint8_t   d1508 = val >> 8;
		const uint8_t   d0700 = val & 0xFF;
		buf [pos + 0] = d1508 & 0xF0;
		buf [pos + 1] = d1508 << 4;
		buf [pos + 2] = d0700 & 0xF0;
		buf [pos + 3] = d0700 << 4;
		pos += 4;
	}
}



void	DisplayPi3St7920::send_line_prologue (int x, int y, SpiBuffer &spibuf, int &spipos)
{
	::digitalWrite (_pin_cs, HIGH);
	send_byte_header (0, Cmd_GDRAM_ADR | y);
	send_byte_raw (      Cmd_GDRAM_ADR | x);
	::delayMicroseconds (_delay_chg);

	spibuf [spipos] = Serial_HEADER | Serial_RS;
	++ spipos;
}



void	DisplayPi3St7920::send_line_epilogue ()
{
	::delayMicroseconds (_delay_chg);

#if 1 // s = 2 or 3
	// When we're done, we need to put the current address out of the screen
	// because of the Chip Select bug: the ST7920 reads SPI data from other
	// devices even when it is not selected.
	send_byte_header (0, Cmd_GDRAM_ADR | 63);
	send_byte_raw (      Cmd_GDRAM_ADR |  0);
//	::delayMicroseconds (_delay_chg);
	send_byte_header (Serial_RS, 0);
	send_byte_raw (              0);
#elif 1
	send_byte_header (0, Cmd_IRAM_ADR);
	send_byte_header (0, Cmd_IRAM_ADR);
	::delayMicroseconds (_delay_chg);
#endif

	::digitalWrite (_pin_cs, LOW);
	::delayMicroseconds (_delay_std);
}



}  // namespace hw
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
