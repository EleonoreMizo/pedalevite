/*****************************************************************************

        DisplayPi3Pcd8544.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_hw_DisplayPi3Pcd8544_HEADER_INCLUDED)
#define mfx_hw_DisplayPi3Pcd8544_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "conc/CellPool.h"
#include "conc/LockFreeCell.h"
#include "conc/LockFreeQueue.h"
#include "mfx/ui/DisplayInterface.h"
#include "mfx/ui/TimeShareCbInterface.h"

#include <array>



namespace mfx
{

namespace ui
{
	class TimeShareThread;
}

namespace hw
{



class DisplayPi3Pcd8544 final
:	public ui::DisplayInterface
,	public ui::TimeShareCbInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       DisplayPi3Pcd8544 (ui::TimeShareThread &thread_spi);
	virtual        ~DisplayPi3Pcd8544 ();

	static const int  _scr_w    = 84;
	static const int  _scr_h    = 48;

	static const int  _spi_port = 1;
	static const int  _spi_rate = 1 * 1000*1000; // Hz

	static const int  _pin_dc   = 12;
	static const int  _pin_cs   = 16;
	static const int  _pin_rst  = 18;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// ui::DisplayInterface
	int            do_get_width () const final;
	int            do_get_height () const final;
	int            do_get_stride () const final;
	uint8_t *      do_use_screen_buf () final;
	const uint8_t* do_use_screen_buf () const final;

	void           do_refresh (int x, int y, int w, int h) final;
	void           do_force_reset () final;

	// ui::TimeShareCbInterface
	bool           do_process_timeshare_op () final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum State
	{
		State_INIT = 0,            // SPI interface needs to be initialized
		State_IDLE,                // Waiting for redrawing messages

		State_NBR_ELT
	};

	// p. 14
	enum Cmd : uint8_t
	{
		Cmd_FUNC_SET  = 0x20,

		// Function Set
		Cmd_PD        = 0x04,
		Cmd_V         = 0x02,
		Cmd_H         = 0x01,

		// H = 0
		Cmd_SET_X     = 0x80,
		Cmd_SET_Y     = 0x40,
		Cmd_DISP_CTRL = 0x08,

		// Display Control
		Cmd_BLANK     = 0x00,
		Cmd_NORMAL    = 0x04,
		Cmd_ALL_ON    = 0x01,
		Cmd_INV_VID   = 0x05,

		// H = 1
		Cmd_SET_VOP   = 0x80,
		Cmd_BIAS_SYS  = 0x10,
		Cmd_TEMP_CTRL = 0x04,
	};

	class RefreshMsg
	{
	public:
		int            _x = 0;
		int            _y = 0;
		int            _w = _scr_w;
		int            _h = _scr_h;
	};
	typedef conc::LockFreeCell <RefreshMsg>  MsgCell;
	typedef conc::CellPool <RefreshMsg>      MsgPool;
	typedef conc::LockFreeQueue <RefreshMsg> MsgQueue;

	void           send_spi (uint8_t x);
	void           send_cmd (uint8_t x);
	void           send_data (uint8_t x);
	void           send_line (int x, int row, uint8_t data_ptr [], int len);

	void           return_cell (MsgCell &cell);

	void           init_device ();
	void           check_msg ();

	void           send_to_display (int x, int y, int w, int h);

	ui::TimeShareThread &
	               _thread_spi;
	volatile State _state;

	std::array <uint8_t, _scr_w * _scr_h>
	               _screen_buf;
	int            _hnd_spi;

	MsgPool        _msg_pool;
	MsgQueue       _msg_queue;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               DisplayPi3Pcd8544 ()                               = delete;
	               DisplayPi3Pcd8544 (const DisplayPi3Pcd8544 &other) = delete;
	               DisplayPi3Pcd8544 (DisplayPi3Pcd8544 &&other)      = delete;
	DisplayPi3Pcd8544 &
	               operator = (const DisplayPi3Pcd8544 &other)        = delete;
	DisplayPi3Pcd8544 &
	               operator = (DisplayPi3Pcd8544 &&other)             = delete;
	bool           operator == (const DisplayPi3Pcd8544 &other) const = delete;
	bool           operator != (const DisplayPi3Pcd8544 &other) const = delete;

}; // class DisplayPi3Pcd8544



}  // namespace hw
}  // namespace mfx



//#include "mfx/hw/DisplayPi3Pcd8544.hpp"



#endif   // mfx_hw_DisplayPi3Pcd8544_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
