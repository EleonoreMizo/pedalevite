/*****************************************************************************

        DisplayPi3St7920.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_ui_DisplayPi3St7920_HEADER_INCLUDED)
#define mfx_ui_DisplayPi3St7920_HEADER_INCLUDED

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

class DisplayPi3St7920
:	public DisplayInterface
,	public TimeShareCbInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       DisplayPi3St7920 (TimeShareThread &thread_spi);
	virtual        ~DisplayPi3St7920 ();

	static const int  _scr_w    = 128;
	static const int  _scr_h    = 64;

	static const int  _spi_port = 1;
	static const int  _spi_rate = 1 * 1000*1000; // Hz.

	static const int  _pin_cs   = 16;   // Chip select (RS)
	static const int  _pin_rst  = 18;   // Reset (RST)



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// DisplayInterface
	virtual int    do_get_width () const;
	virtual int    do_get_height () const;
	virtual int    do_get_stride () const;
	virtual uint8_t *
	               do_use_screen_buf ();
	virtual const uint8_t *
	               do_use_screen_buf () const;

	virtual void   do_refresh (int x, int y, int w, int h);

	// TimeShareCbInterface
	virtual bool   do_process_timeshare_op ();



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum State
	{
		State_INIT = 0,            // SPI interface needs to be initialized
		State_IDLE,                // Waiting for redrawing messages
		State_REDRAW,              // Drawing in progress

		State_NBR_ELT
	};

	class StateRedraw
	{
	public:
		int            _col_beg;
		int            _nbr_col;
		int            _y;
		int            _h;
		const uint8_t *
		               _pix_ptr;
		int            _nbr_pairs;
		int            _pair_cnt;
		int            _row;
	};

	// p. 16
	enum Cmd : uint8_t
	{
		// Basic commands (RE=0)
		Cmd_CLEAR         = 0x01,
		Cmd_HOME          = 0x02,
		Cmd_ENTRY         = 0x04,
		Cmd_DISPLAY       = 0x08,
		Cmd_CURS_CTRL     = 0x10,
		Cmd_FNC_SET       = 0x20,
		Cmd_CGRAM_ADR     = 0x40,
		Cmd_DDRAM_ADR     = 0x80,

		// Extended commands (RE=1)
		Cmd_STANDBY       = 0x01,
		Cmd_RAM_SEL       = 0x02,
		Cmd_SCROLL        = 0x03,
		Cmd_REVERSE       = 0x04,
		Cmd_FNC_SET_E     = 0x24,  // With RE flag set
		Cmd_IRAM_ADR      = 0x40,
		Cmd_SCROLL_ADR    = 0x40,
		Cmd_GDRAM_ADR     = 0x80,

		// Cmd_ENTRY
		Cmd_ENTRY_S       = 0x01,
		Cmd_ENTRY_ID      = 0x02,

		// Cmd_DISPLAY
		Cmd_DISPLAY_BLINK = 0x01,
		Cmd_DISPLAY_CURS  = 0x02,
		Cmd_DISPLAY_DISP  = 0x04,

		// Cmd_CURS_CTRL
		Cmd_CURS_CTRL_RL  = 0x04,
		Cmd_CURS_CTRL_SC  = 0x08,

		// Cmd_FNC_SET / Cmd_FNC_SET_E
		Cmd_FNC_SET_RE    = 0x04,
		Cmd_FNC_SET_DL    = 0x10,

		// Cmd_FNC_SET_E
		Cmd_FNC_SET_E_G   = 0x02,
	};

	// p. 26
	enum Serial : uint8_t
	{
		Serial_HEADER     = 0xF8,
		Serial_RW         = 0x04,
		Serial_RS         = 0x02
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
	typedef std::array <uint8_t, (_scr_w >> 4) * 4 * 2 + 1> SpiBuffer;

	void           return_cell (MsgCell &cell);

	void           init_device ();
	void           check_msg ();
	void           start_redraw (int x, int y, int w, int h);
	void           redraw_part ();

	void           send_byte_raw (uint8_t a);
	void           send_byte_header (uint8_t rwrs, uint8_t a);
	void           send_cmd (uint8_t x);
	void           send_data (uint8_t x);
	void           send_line (int col, int y, const uint8_t pix_ptr [], int len);
	void           send_2_full_lines (int y, const uint8_t pix1_ptr [], const uint8_t pix2_ptr []);
	void           prepare_line_data (SpiBuffer &buf, int &pos, const uint8_t pix_ptr [], int len);
	void           send_line_prologue (int x, int y, SpiBuffer &spibuf, int &spipos);
	void           send_line_epilogue ();

	TimeShareThread &
	               _thread_spi;
	volatile State _state;

	std::array <uint8_t, _scr_w * _scr_h>
	               _screen_buf;
	int            _hnd_spi;

	MsgPool        _msg_pool;
	MsgQueue       _msg_queue;

	StateRedraw    _redraw;

	static const int _delay_std =  101; // Microseconds. 72 us are required but wiringPi would use a cpu-consuming spinlock.
	static const int _delay_clr = 1600; // Microseconds.
	static const int _delay_chg =    2; // Microseconds. Short delay between address setting and data feeding



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               DisplayPi3St7920 ()                               = delete;
	               DisplayPi3St7920 (const DisplayPi3St7920 &other)  = delete;
	DisplayPi3St7920 &
	               operator = (const DisplayPi3St7920 &other)        = delete;
	bool           operator == (const DisplayPi3St7920 &other) const = delete;
	bool           operator != (const DisplayPi3St7920 &other) const = delete;

}; // class DisplayPi3St7920



}  // namespace ui
}  // namespace mfx



//#include "mfx/ui/DisplayPi3St7920.hpp"



#endif   // mfx_ui_DisplayPi3St7920_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
