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

#include "mfx/ui/DisplayPi3St7920.h"

#include <wiringPi.h>
#include <wiringPiSPI.h>

#include <unistd.h>

#include <algorithm>
#include <stdexcept>

#include <cassert>



namespace mfx
{
namespace ui
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// Before calling:
// ::wiringPiSetupPhys ()
// ::pinMode (_pin_rst, OUTPUT);
// ::digitalWrite (_pin_rst, LOW);  ::delay (100);
// ::digitalWrite (_pin_rst, HIGH); ::delay (1);
DisplayPi3St7920::DisplayPi3St7920 (std::mutex &mutex_spi)
:	_mutex_spi (mutex_spi)
,	_screen_buf ()
,	_hnd_spi (::wiringPiSPISetupMode (_spi_port, _spi_rate, 0))
,	_msg_pool ()
,	_msg_queue ()
,	_quit_flag (false)
,	_refresher ()
{
	if (_hnd_spi == -1)
	{
		throw std::runtime_error ("DisplayPi3St7920: cannot open SPI port.");
	}

	_msg_pool.expand_to (256);

	::pinMode  (_pin_dc , OUTPUT);
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

	_refresher = std::thread (&DisplayPi3St7920::refresh_loop, this);
}



DisplayPi3St7920::~DisplayPi3St7920 ()
{
	if (_refresher.joinable ())
	{
		_quit_flag = true;
		_refresher.join ();
	}

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



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DisplayPi3St7920::send_byte_raw (uint8_t a)
{
	uint8_t        buffer [2] =
	{
		uint8_t (a & 0xF0),
		uint8_t (a << 4)
	};
	::wiringPiSPIDataRW (_spi_port, &buffer [0], sizeof (buffer));
}



void	DisplayPi3St7920::send_bytes_header (uint8_t rwrs, uint8_t a)
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
	{
		std::lock_guard <std::mutex>   lock (_mutex_spi);

		::digitalWrite (_pin_cs, HIGH);
		send_bytes_header (0, x);
		::digitalWrite (_pin_cs, LOW);
	}
	::delayMicroseconds (_delay_std);
}



void	DisplayPi3St7920::send_data (uint8_t x)
{
	{
		std::lock_guard <std::mutex>   lock (_mutex_spi);

		::digitalWrite (_pin_cs, HIGH);
		send_bytes_header (Serial_RS, x);
		::digitalWrite (_pin_cs, LOW);
	}
	::delayMicroseconds (_delay_std);
}



// col is in 16-pixel blocks, len too
void	DisplayPi3St7920::send_line (int col, int y, const uint8_t pix_ptr [], int len)
{
	assert (col >= 0);
	assert (y >= 0);
	assert (y < _scr_h);
	assert (len > 0);
	assert ((len + col) * 16 <= _scr_w);

	std::lock_guard <std::mutex>   lock (_mutex_spi);
	::digitalWrite (_pin_cs, HIGH);

	int            ofs_y = 0;
	int            ofs_x = col;
	if (y >= 32)
	{
		ofs_y = -32;
		ofs_x = 8;
	}
	send_bytes_header (0, Cmd_GDRAM_ADR | (y + ofs_y));
	send_bytes_header (0, Cmd_GDRAM_ADR | (    ofs_x));
	::delayMicroseconds (_delay_std);

	for (int c = 0; c < len; ++c)
	{
		uint16_t       val = 0;
		for (int r2 = 0; r2 < 16; ++r2)
		{
			val <<= 1;
			val |= *pix_ptr >> 7;
			++ pix_ptr;
		}

		const uint8_t  d1508 = val >> 8;
		const uint8_t  d0700 = val & 0xFF;
		send_bytes_header (Serial_RS, d1508);
		send_bytes_header (Serial_RS, d0700);
	}

	// For some reason we need to put the current address out of the screen
	// because sometimes parasite bytes are randomly written.
	::delayMicroseconds (_delay_std);
	send_bytes_header (0, Cmd_GDRAM_ADR | 48);
	send_bytes_header (0, Cmd_GDRAM_ADR | 0);
	::delayMicroseconds (_delay_std);
	send_bytes_header (Serial_RS, 0);
	send_bytes_header (Serial_RS, 0);

	::digitalWrite (_pin_cs, LOW);
	::delayMicroseconds (_delay_std);
}



void	DisplayPi3St7920::return_cell (MsgCell &cell)
{
	_msg_pool.return_cell (cell);
}



void	DisplayPi3St7920::refresh_loop ()
{
	while (! _quit_flag)
	{
		int            x1 = _scr_w;
		int            y1 = _scr_h;
		int            x2 = 0;
		int            y2 = 0;
		MsgCell *      cell_ptr = 0;
		do
		{
			cell_ptr = _msg_queue.dequeue ();
			if (cell_ptr != 0)
			{
				if (x2 - x1 <= 0)
				{
					x1 =      cell_ptr->_val._x;
					y1 =      cell_ptr->_val._y;
					x2 = x1 + cell_ptr->_val._w;
					y2 = y1 + cell_ptr->_val._h;
				}
				else
				{
					x1 = std::min (x1, cell_ptr->_val._x);
					y1 = std::min (y1, cell_ptr->_val._y);
					x2 = std::max (x2, cell_ptr->_val._x + cell_ptr->_val._w);
					y2 = std::max (y2, cell_ptr->_val._y + cell_ptr->_val._h);
				}

				return_cell (*cell_ptr);
			}
		}
		while (cell_ptr != 0 && ! _quit_flag);

		if (x2 - x1 > 0)
		{
			send_to_display (x1, y1, x2 - x1, y2 - y1);
		}

		// 20 Hz refresh. Should be enough for static display.
		::delay (50);
	}

	_quit_flag = false;
}



void	DisplayPi3St7920::send_to_display (int x, int y, int w, int h)
{
	assert (x >= 0);
	assert (y >= 0);
	assert (w > 0);
	assert (h > 0);
	assert (x + w <= _scr_w);
	assert (y + h <= _scr_h);

	const int        col_beg =  x           >> 4;
	const int        col_end = (x + w + 15) >> 4;
	const int        nbr_col = col_end - col_beg;
	const int        row_end =  y + h;
	const int        stride  = get_stride ();
	const uint8_t *  pix_ptr = &_screen_buf [col_beg * 16 + y * stride];

	for (int row = y; row < row_end; ++row)
	{
		send_line (col_beg, row, pix_ptr, nbr_col);
		pix_ptr += stride;
	}
}



}  // namespace ui
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
