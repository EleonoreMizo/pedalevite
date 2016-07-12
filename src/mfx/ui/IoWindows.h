/*****************************************************************************

        IoWindows.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_ui_IoWindows_HEADER_INCLUDED)
#define mfx_ui_IoWindows_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include "conc/CellPool.h"
#include "mfx/ui/DisplayInterface.h"
#include "mfx/ui/LedInterface.h"
#include "mfx/ui/UserInputInterface.h"

#include <Windows.h>

#include <array>
#include <thread>
#include <vector>

#include <cstdint>



namespace mfx
{
namespace ui
{



class IoWindows
:	public DisplayInterface
,	public LedInterface
,	public UserInputInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       IoWindows (volatile bool &quit_request_flag);
	virtual        ~IoWindows ();



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

	// LedInterface
	virtual int    do_get_nbr_led () const;
	virtual void   do_set_led (int index, float val);

	// UserInputInterface
	virtual int    do_get_nbr_param (UserInputType type) const;
	virtual void   do_set_msg_recipient (UserInputType type, int index, MsgQueue *queue_ptr);
	virtual void   do_return_cell (MsgCell &cell);
	virtual int64_t
	               do_get_cur_date () const;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum MsgCustom
	{
		MsgCustom_REDRAW = WM_APP,
		MsgCustom_LED
	};

	class PixArgb
	{
	public:
		uint8_t        _b;
		uint8_t        _g;
		uint8_t        _r;
		uint8_t        _a;
	};

	class ScanEntry
	{
	public:
		bool           _scan_flag;
		int            _code;
		UserInputType  _type;
		int            _index;
		int            _dir;
	};

	static const int  _scr_w   = 128;
	static const int  _scr_h   =  64;
	static const int  _scr_s   = _scr_w;
	static const int  _nbr_led = 3;
	static const int  _zoom    = 4;

	static const int  _max_led_h = 32;
	static const int  _led_h_tmp = _scr_h * _zoom / 2;
	static const int  _led_h     = (_led_h_tmp < _max_led_h) ? _led_h_tmp : _max_led_h;
	static const int  _disp_w    = _scr_w * _zoom;
	static const int  _disp_h    = _scr_h * _zoom + _led_h;

	typedef std::array <uint8_t, _scr_s * _scr_h> ScreenBuffer;

	typedef conc::CellPool <UserInputMsg> MsgPool;

	typedef std::vector <MsgQueue *> QueueArray;
	typedef std::array <QueueArray, UserInputType_NBR_ELT> RecipientList;

	void           main_loop ();
	void           resize_win (int w, int h);
	void           init_bitmap (int w, int h);
	::LRESULT      winproc (::HWND hwnd, ::UINT message, ::WPARAM wparam, ::LPARAM lparam);
	bool           process_redraw (::HWND hwnd, ::WPARAM wparam, ::LPARAM lparam);
	bool           process_led (::HWND hwnd, ::WPARAM wparam, ::LPARAM lparam);
	bool           process_paint (::HWND hwnd, ::WPARAM wparam, ::LPARAM lparam);
	bool           process_key (::HWND hwnd, ::WPARAM wparam, ::LPARAM lparam, bool down_flag);
	void           enqueue_val (int64_t date, UserInputType type, int index, float val);
	void           redraw_main_screen (int x1, int y1, int x2, int y2);
	void           redraw_led (int x1, int y1, int x2, int y2, int led_cnt);

	static ::LRESULT CALLBACK
	               winproc_static (::HWND hwnd, ::UINT message, ::WPARAM wparam, ::LPARAM lparam);

	ScreenBuffer   _screen_buf;
	::HWND         _main_win;
	::HBITMAP      _bitmap;
	PixArgb *      _bitmap_data_ptr;
	std::array <float, _nbr_led>
	               _led_arr;
	std::array <std::array <int, _zoom>, _zoom>
	               _pix_table;

	MsgPool        _msg_pool;
	RecipientList  _recip_list;
	int64_t        _clock_freq; // Hz

	volatile bool& _quit_request_flag; // Application is requested to quit

	volatile bool  _quit_flag;         // Thread should stop
	std::thread    _msg_loop_thread;

	static const wchar_t
	               _window_class_name_0 [];
	static const ScanEntry
	               _scan_table [];



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               IoWindows ()                               = delete;
	               IoWindows (const IoWindows &other)         = delete;
	IoWindows &    operator = (const IoWindows &other)        = delete;
	bool           operator == (const IoWindows &other) const = delete;
	bool           operator != (const IoWindows &other) const = delete;

}; // class IoWindows



}  // namespace ui
}  // namespace mfx



//#include "mfx/ui/IoWindows.hpp"



#endif   // mfx_ui_IoWindows_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
