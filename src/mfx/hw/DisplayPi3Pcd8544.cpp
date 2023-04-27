/*****************************************************************************

        DisplayPi3Pcd8544.cpp
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

#include "mfx/hw/DisplayPi3Pcd8544.h"
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
// ::wiringPiSetup* ()
// ::pinMode (_pin_rst, OUTPUT);
// ::digitalWrite (_pin_rst, LOW);  ::delay (100);
// ::digitalWrite (_pin_rst, HIGH); ::delay (1);
DisplayPi3Pcd8544::DisplayPi3Pcd8544 (ui::TimeShareThread &thread_spi)
:	_thread_spi (thread_spi)
,	_state (State_INIT)
,	_screen_buf ()
,	_hnd_spi (::wiringPiSPISetup (_spi_port, _spi_rate))
,	_msg_pool ()
,	_msg_queue ()
{
	if (_hnd_spi == -1)
	{
		throw std::runtime_error ("DisplayPi3Pcd8544: cannot open SPI port.");
	}

	_msg_pool.expand_to (256);

	_thread_spi.register_cb (
		*this,
		std::chrono::microseconds (1'000'000 / 20)
	); // 20 fps max refresh rate
}



DisplayPi3Pcd8544::~DisplayPi3Pcd8544 ()
{
	_thread_spi.remove_cb (*this);

	MsgCell *      cell_ptr = nullptr;
	do
	{
		cell_ptr = _msg_queue.dequeue ();
		if (cell_ptr != nullptr)
		{
			return_cell (*cell_ptr);
		}
	}
	while (cell_ptr != nullptr);

	close (_hnd_spi);
	_hnd_spi = -1;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	DisplayPi3Pcd8544::do_get_width () const
{
	return _scr_w;
}



int	DisplayPi3Pcd8544::do_get_height () const
{
	return _scr_h;
}



int	DisplayPi3Pcd8544::do_get_stride () const
{
	return _scr_w;
}



uint8_t *	DisplayPi3Pcd8544::do_use_screen_buf ()
{
	return &_screen_buf [0];
}



const uint8_t *	DisplayPi3Pcd8544::do_use_screen_buf () const
{
	return &_screen_buf [0];
}



void	DisplayPi3Pcd8544::do_refresh (int x, int y, int w, int h)
{
	MsgCell *      cell_ptr = _msg_pool.take_cell (true);
	if (cell_ptr == nullptr)
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



void	DisplayPi3Pcd8544::do_force_reset ()
{
	_state = State_INIT;
}



bool	DisplayPi3Pcd8544::do_process_timeshare_op ()
{
	if (_state == State_INIT)
	{
		init_device ();
	}
	else
	{
		check_msg ();
	}

	return (false);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// Not locked
void	DisplayPi3Pcd8544::send_spi (uint8_t x)
{
	uint8_t        buffer [1] = { x };
	::wiringPiSPIDataRW (_spi_port, &buffer [0], 1);
}



void	DisplayPi3Pcd8544::send_cmd (uint8_t c)
{
	::digitalWrite (_pin_dc, LOW);
	::digitalWrite (_pin_cs, LOW);
	send_spi (c);
	::digitalWrite (_pin_cs, HIGH);
}



void	DisplayPi3Pcd8544::send_data (uint8_t a)
{
	::digitalWrite (_pin_dc, HIGH);
	::digitalWrite (_pin_cs, LOW);
	send_spi (a);
	::digitalWrite (_pin_cs, HIGH);
}



// data_ptr is not const because the input buffer will be altered.
void	DisplayPi3Pcd8544::send_line (int x, int row, uint8_t data_ptr [], int len)
{
	send_cmd (uint8_t (Cmd_SET_Y | row));
	send_cmd (uint8_t (Cmd_SET_X | x  ));

	::digitalWrite (_pin_dc, HIGH);
	::digitalWrite (_pin_cs, LOW);
	::wiringPiSPIDataRW (_spi_port, &data_ptr [0], len);
	::digitalWrite (_pin_cs, HIGH);
}



void	DisplayPi3Pcd8544::return_cell (MsgCell &cell)
{
	_msg_pool.return_cell (cell);
}



void	DisplayPi3Pcd8544::init_device ()
{
	::pinMode  (_pin_dc , OUTPUT);
	::pinMode  (_pin_cs , OUTPUT);

	send_cmd (Cmd_FUNC_SET  | Cmd_H);
	send_cmd (Cmd_TEMP_CTRL | 0x00);  // See 7.8
	send_cmd (Cmd_BIAS_SYS  | 0x03);  // See 8.8
	send_cmd (Cmd_SET_VOP   | 0x3A);  // See 8.9, and modified manually
	send_cmd (Cmd_FUNC_SET);
	send_cmd (Cmd_DISP_CTRL | Cmd_NORMAL);

	_state = State_IDLE;
}



void	DisplayPi3Pcd8544::check_msg ()
{
	assert (_state == State_IDLE);

	int            x1 = INT_MAX;
	int            y1 = INT_MAX;
	int            x2 = INT_MIN;
	int            y2 = INT_MIN;
	MsgCell *      cell_ptr = nullptr;
	do
	{
		cell_ptr = _msg_queue.dequeue ();
		if (cell_ptr != nullptr)
		{
			x1 = std::min (x1, cell_ptr->_val._x);
			y1 = std::min (y1, cell_ptr->_val._y);
			x2 = std::max (x2, cell_ptr->_val._x + cell_ptr->_val._w);
			y2 = std::max (y2, cell_ptr->_val._y + cell_ptr->_val._h);

			return_cell (*cell_ptr);
		}
	}
	while (cell_ptr != nullptr);

	if (x1 < x2)
	{
		send_to_display (x1, y1, x2 - x1, y2 - y1);
	}
}



void	DisplayPi3Pcd8544::send_to_display (int x, int y, int w, int h)
{
	assert (x >= 0);
	assert (y >= 0);
	assert (w > 0);
	assert (h > 0);
	assert (x + w <= _scr_w);
	assert (y + h <= _scr_h);

	const int     row_beg =  y          >> 3;
	const int     row_end = (y + h + 7) >> 3;
	const int     col_end =  x + w;
	const int     stride  = get_stride ();
	uint8_t *     pix_ptr = &_screen_buf [0];
	std::array <uint8_t, _scr_w>   data_arr;
	for (int row = row_beg; row < row_end; ++row)
	{
		const int      row_pos = (row << 3) * stride;
		for (int col = x; col < col_end; ++col)
		{
			uint8_t        val = 0;
			int            ofs = row_pos + col;
			for (int r2 = 0; r2 < 8; ++r2)
			{
				val |= (pix_ptr [ofs] >> 7) << r2;
				ofs += stride;
			}
			data_arr [col] = val;
		}

		send_line (x, row, &data_arr [x], w);
	}

	// The last data byte is not taken into account if we don't do this.
	send_cmd (Cmd_SET_Y | 0);
}



}  // namespace hw
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
