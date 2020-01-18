/*****************************************************************************

        IoWindows.cpp
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

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include "fstb/txt/Conv.h"
#include "fstb/def.h"
#include "fstb/fnc.h"
#include "mfx/hw/IoWindows.h"
#include "mfx/Cst.h"
#include "mfx/Model.h"
#include "mfx/ModelMsgCmdConfLdSv.h"

#include	<commdlg.h>
#include <Windowsx.h>

#include <stdexcept>

#include <cassert>
#include <cmath>
#include <cstring>



namespace mfx
{
namespace hw
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



IoWindows::IoWindows (volatile bool &quit_request_flag)
:	_model_ptr (nullptr)
,	_screen_buf ()
,	_main_win (nullptr)
,	_bitmap (nullptr)
,	_bitmap_data_ptr (nullptr)
,	_led_arr ()
,	_pix_table ()
,	_msg_pool ()
,	_recip_list ()
,	_clock_freq (0)
,	_pressed_sw_ptr (nullptr)
,	_sw_states (0)
,	_quit_request_flag (quit_request_flag)
,	_quit_flag ()
#if defined (_MSC_VER)
#pragma warning (push)
#pragma warning (disable : 4355)
#endif // 'this': used in base member initializer list
,	_msg_loop_thread (&IoWindows::main_loop, this)
#if defined (_MSC_VER)
#pragma warning (pop)
#endif
{
	for (auto &l : _led_arr)
	{
		l = 0;
	}

	_msg_pool.expand_to (256);
	for (int i = 0; i < ui::UserInputType_NBR_ELT; ++i)
	{
		const int      nbr_dev =
			do_get_nbr_param (static_cast <ui::UserInputType> (i));
		_recip_list [i].resize (nbr_dev, nullptr);
	}

	::LARGE_INTEGER   freq;
	::QueryPerformanceFrequency (&freq);
	_clock_freq = freq.QuadPart;

	const int      sc = std::max (12 / _zoom, 1);
	const int      pe = std::max (_zoom - 1, 1);
	for (int y = 0; y < _zoom; ++y)
	{
		const int      ky = (y < pe) ? sc : sc - 1;
		for (int x = 0; x < _zoom; ++x)
		{
			const int       kx = (x < pe) ? sc : sc - 1;
			const float     r  = float (kx * ky) / (sc * sc);
			_pix_table [y] [x] = fstb::round_int (0x100 * pow (r, 1 / 2.2));
		}
	}
}



IoWindows::~IoWindows ()
{
	if (_msg_loop_thread.joinable ())
	{
		_quit_flag = true;
		_msg_loop_thread.join ();
	}

	if (_bitmap != nullptr)
	{
		::DeleteObject (_bitmap);
		_bitmap          = nullptr;
		_bitmap_data_ptr = nullptr;
	}
}



void	IoWindows::set_model (Model &model)
{
	_model_ptr = &model;
	auto &         cmd_mgr = _model_ptr->use_async_cmd ();
	_ret_queue_sptr = cmd_mgr.create_new_ret_queue ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	IoWindows::do_get_width () const
{
	return _scr_w;
}



int	IoWindows::do_get_height () const
{
	return _scr_h;
}



int	IoWindows::do_get_stride () const
{
	return _scr_s;
}



uint8_t *	IoWindows::do_use_screen_buf ()
{
	return &_screen_buf [0];
}



const uint8_t *	IoWindows::do_use_screen_buf () const
{
	return &_screen_buf [0];
}



void	IoWindows::do_refresh (int x, int y, int w, int h)
{
	x *= _zoom;
	y *= _zoom;
	w *= _zoom;
	h *= _zoom;

	const ::WPARAM wparam = (y << 16) + x;
	const ::LPARAM lparam = (h << 16) + w;
	::PostMessageW (_main_win, MsgCustom_REDRAW, wparam, lparam);
}



void	IoWindows::do_force_reset ()
{
	// Nothing more at this point
	do_refresh (0, 0, _scr_w, _scr_h);
}



int	IoWindows::do_get_nbr_led () const
{
	return _nbr_led;
}



void	IoWindows::do_set_led (int index, float val)
{
	const ::WPARAM wparam = index;
	const ::LPARAM lparam =
		fstb::limit (fstb::round_int (val * 65535), 0, 65535);
	::PostMessageW (_main_win, MsgCustom_LED, wparam, lparam);
}



int	IoWindows::do_get_nbr_param (ui::UserInputType /*type*/) const
{
	return Cst::_max_input_param;
}



void	IoWindows::do_set_msg_recipient (ui::UserInputType type, int index, MsgQueue *queue_ptr)
{
	_recip_list [type] [index] = queue_ptr;
}



void	IoWindows::do_return_cell (MsgCell &cell)
{
	_msg_pool.return_cell (cell);
}



std::chrono::microseconds	IoWindows::do_get_cur_date () const
{
	::LARGE_INTEGER   d;
	::QueryPerformanceCounter (&d);
	const int64_t  date_us = int64_t (
		double (d.QuadPart) * (1000 * 1000) / _clock_freq
	);

	return std::chrono::microseconds (date_us);
}



void	IoWindows::do_notify_model_error_code (int ret_val)
{
	if (ret_val != 0)
	{
		::MessageBoxW (
			_main_win,
			L"Operation failed.",
			L"Error",
			MB_ICONERROR | MB_OK
		);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



const wchar_t	IoWindows::_window_class_name_0 [] =
	L"pedalevite_mfx_ui_iowindows_main";

const IoWindows::ScanEntry	IoWindows::_scan_table [] =
{
	// Pedals, top row (6)
	{ true , 0x10      , ui::UserInputType_SW    ,  2,  0 }, // A
	{ true , 0x11      , ui::UserInputType_SW    ,  3,  0 }, // Z
	{ true , 0x12      , ui::UserInputType_SW    ,  4,  0 }, // E
	{ true , 0x13      , ui::UserInputType_SW    ,  5,  0 }, // R
	{ true , 0x14      , ui::UserInputType_SW    ,  6,  0 }, // T
	{ true , 0x15      , ui::UserInputType_SW    ,  7,  0 }, // Y

	// Pedals, bottom row (6)
	{ true , 0x1E      , ui::UserInputType_SW    ,  8,  0 }, // Q
	{ true , 0x1F      , ui::UserInputType_SW    ,  9,  0 }, // S
	{ true , 0x20      , ui::UserInputType_SW    , 14,  0 }, // D
	{ true , 0x21      , ui::UserInputType_SW    , 15,  0 }, // F
	{ true , 0x22      , ui::UserInputType_SW    , 16,  0 }, // G
	{ true , 0x23      , ui::UserInputType_SW    , 17,  0 }, // H

	// Rotary encoders with push button (2)
	{ true , 0x2C      , ui::UserInputType_ROTENC,  5, -1 }, // W
	{ true , 0x2D      , ui::UserInputType_ROTENC,  5, +1 }, // X
	{ true , 0x2E      , ui::UserInputType_SW    , 18,  0 }, // C
	{ true , 0x2F      , ui::UserInputType_ROTENC,  6, -1 }, // V
	{ true , 0x30      , ui::UserInputType_ROTENC,  6, +1 }, // B
	{ true , 0x31      , ui::UserInputType_SW    , 19,  0 }, // N

	// Common rotary encoders
	{ false, 0x31      , ui::UserInputType_ROTENC,  0, -1 }, // 1
	{ false, 0x32      , ui::UserInputType_ROTENC,  0, +1 }, // 2
	{ false, 0x33      , ui::UserInputType_ROTENC,  1, -1 }, // 3
	{ false, 0x34      , ui::UserInputType_ROTENC,  1, +1 }, // 4
	{ false, 0x35      , ui::UserInputType_ROTENC,  2, -1 }, // 5
	{ false, 0x36      , ui::UserInputType_ROTENC,  2, +1 }, // 6
	{ false, 0x37      , ui::UserInputType_ROTENC,  3, -1 }, // 7
	{ false, 0x38      , ui::UserInputType_ROTENC,  3, +1 }, // 8
	{ false, 0x39      , ui::UserInputType_ROTENC,  4, -1 }, // 9
	{ false, 0x30      , ui::UserInputType_ROTENC,  4, +1 }, // 0

	// Panel buttons
	{ false, VK_RETURN , ui::UserInputType_SW    ,  0,  0 }, // Enter (main)
	{ false, VK_ESCAPE , ui::UserInputType_SW    ,  1,  0 }, // Esc
	{ false, VK_UP     , ui::UserInputType_SW    , 10,  0 }, // Up
	{ false, VK_DOWN   , ui::UserInputType_SW    , 11,  0 }, // Down
	{ false, VK_LEFT   , ui::UserInputType_SW    , 12,  0 }, // Left
	{ false, VK_RIGHT  , ui::UserInputType_SW    , 13,  0 }, // Right

	// Expression pedal position
	{ false, VK_NUMPAD0, ui::UserInputType_POT   ,  0,  0 },
	{ false, VK_NUMPAD1, ui::UserInputType_POT   ,  0,  1 },
	{ false, VK_NUMPAD2, ui::UserInputType_POT   ,  0,  2 },
	{ false, VK_NUMPAD3, ui::UserInputType_POT   ,  0,  3 },
	{ false, VK_NUMPAD4, ui::UserInputType_POT   ,  0,  4 },
	{ false, VK_NUMPAD5, ui::UserInputType_POT   ,  0,  5 },
	{ false, VK_NUMPAD6, ui::UserInputType_POT   ,  0,  6 },
	{ false, VK_NUMPAD7, ui::UserInputType_POT   ,  0,  7 },
	{ false, VK_NUMPAD8, ui::UserInputType_POT   ,  0,  8 },
	{ false, VK_NUMPAD9, ui::UserInputType_POT   ,  0,  9 }
};

const IoWindows::SwLoc	IoWindows::_switch_pos_table [] =
{
	// Pedals, top row (6)
	{  2, SwType_FOOT, 0, 0 },
	{  3, SwType_FOOT, 1, 0 },
	{  4, SwType_FOOT, 2, 0 },
	{  5, SwType_FOOT, 3, 0 },
	{  6, SwType_FOOT, 4, 0 },
	{  7, SwType_FOOT, 5, 0 },

	// Pedals, bottom row (6)
	{  8, SwType_FOOT, 0, 1 },
	{  9, SwType_FOOT, 1, 1 },
	{ 14, SwType_FOOT, 2, 1 },
	{ 15, SwType_FOOT, 3, 1 },
	{ 16, SwType_FOOT, 4, 1 },
	{ 17, SwType_FOOT, 5, 1 },

	// Panel buttons
	{  0, SwType_CTRL, 0, 0 }, // Enter (main)
	{  1, SwType_CTRL, 2, 0 }, // Esc
	{ 10, SwType_CTRL, 1, 0 }, // Up
	{ 11, SwType_CTRL, 1, 1 }, // Down
	{ 12, SwType_CTRL, 0, 1 }, // Left
	{ 13, SwType_CTRL, 2, 1 }  // Right
};


void	IoWindows::main_loop ()
{
	::HMODULE      module = ::GetModuleHandleW (nullptr);

	::WNDCLASSEXW  wcex;
	memset (&wcex, 0, sizeof (wcex));

	// Create the window class
	wcex.cbSize			= sizeof (WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc	= &winproc_static;
	wcex.cbClsExtra	= 0;
	wcex.cbWndExtra	= 0;
	wcex.hInstance		= module;
	wcex.hIcon			= nullptr;
	wcex.hCursor		= ::LoadCursor (nullptr, IDC_ARROW);
	wcex.hbrBackground= HBRUSH (COLOR_APPWORKSPACE + 1);
	wcex.lpszMenuName	= nullptr;
	wcex.lpszClassName= _window_class_name_0;
	wcex.hIconSm		= nullptr;
	::RegisterClassExW (&wcex);	// No check

	_main_win = ::CreateWindowW (
		_window_class_name_0,
		L"P\x00E9" L"dale Vite",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, 0,
		320, 200,
		nullptr,
		nullptr,
		module,
		nullptr
	);
	if (_main_win == nullptr)
	{
		throw std::runtime_error (
			"Cannot create the window."
		);
	}

	::SetWindowLongPtrW (
		_main_win,
		GWLP_USERDATA,
		reinterpret_cast <::LONG_PTR> (this)
	);

	// Resizes the window to the desired client size
	resize_win (_disp_w, _disp_h);

	init_bitmap (_disp_w, _disp_h);

	::ShowWindow (_main_win, SW_SHOW);

	while (! _quit_flag)
	{
		::MSG          msg;
		const ::BOOL   gm_res = ::GetMessageW (&msg, nullptr, 0, 0);
		if (gm_res == 0 || gm_res == -1)
		{
			_quit_flag         = true;
			_quit_request_flag = true;
		}
		else
		{
			TranslateMessage (&msg); 
			DispatchMessageW (&msg); 
		}
	}

	::SetWindowLongPtrW (_main_win, GWLP_USERDATA, 0);
	::DestroyWindow (_main_win);
	_main_win = nullptr;

	_quit_flag = false;
}



void	IoWindows::resize_win (int w, int h)
{
	::RECT         win_rect;
	::RECT         old_rect;
	::SetRect (&win_rect, 0, 0, w, h);
	::AdjustWindowRectEx (
		&win_rect,
		::GetWindowLongW (_main_win, GWL_STYLE),
		::GetMenu (_main_win) != 0,
		::GetWindowLongW (_main_win, GWL_EXSTYLE)
	);
	::GetWindowRect (_main_win, &old_rect);
	::MoveWindow (
		_main_win,
		old_rect.left,
		old_rect.top,
		win_rect.right - win_rect.left,
		win_rect.bottom - win_rect.top,
		TRUE
	);
}



void	IoWindows::init_bitmap (int w, int h)
{
	::HDC          hdc = ::GetDC (_main_win);

	::BITMAPINFOHEADER	bitmap_info_header =
	{
		sizeof (bitmap_info_header),
		w,
		-h,
		1,
		32,
		BI_RGB,
		0,
		0,
		0,
		0,
		0
	};

	::BITMAPINFO	bitmap_info;
	bitmap_info.bmiHeader = bitmap_info_header;

	_bitmap = ::CreateDIBSection (
		hdc,
		&bitmap_info,
		DIB_RGB_COLORS,
		(void **) &_bitmap_data_ptr,
		0,
		0
	);

	::ReleaseDC (_main_win, hdc);
}



::LRESULT	IoWindows::winproc (::HWND hwnd, ::UINT message, ::WPARAM wparam, ::LPARAM lparam)
{
	::LRESULT      ret_val = 0;
	bool           def_proc_flag = false;

	switch (message)
	{
	case WM_CLOSE:
		_quit_request_flag = true;
		::PostQuitMessage (0);
		break;

	case WM_PAINT:
		def_proc_flag = process_paint (hwnd, wparam, lparam);
		break;

	case WM_KEYDOWN:
		def_proc_flag = process_key (hwnd, wparam, lparam, true);
		break;
	case WM_KEYUP:
		def_proc_flag = process_key (hwnd, wparam, lparam, false);
		break;

	case WM_LBUTTONDOWN:
		def_proc_flag = process_lbuttondown (hwnd, wparam, lparam);
		break;

	case WM_LBUTTONUP:
		def_proc_flag = process_lbuttonup (hwnd, wparam, lparam);
		break;

	case WM_CAPTURECHANGED:
		release_mouse_pressed_sw ();
		def_proc_flag = false;
		break;

	case MsgCustom_REDRAW:
		def_proc_flag = process_redraw (hwnd, wparam, lparam);
		break;

	case MsgCustom_LED:
		def_proc_flag = process_led (hwnd, wparam, lparam);
		break;

	case MsgCustom_SWITCH:
		def_proc_flag = process_switch (hwnd, wparam);
		break;

	default:
		def_proc_flag = true;
		break;
	}

	if (def_proc_flag)
	{
		ret_val = ::DefWindowProcW (hwnd, message, wparam, lparam);
	}

	/*** To do: move this into a timer callback ***/
	if (_model_ptr != nullptr)
	{
		_model_ptr->use_async_cmd ().flush_ret_queue (*_ret_queue_sptr);
	}

	return ret_val;
}



bool	IoWindows::process_redraw (::HWND hwnd, ::WPARAM wparam, ::LPARAM lparam)
{
	fstb::unused (hwnd);

	::RECT         rect;
	rect.left   = ::LONG (wparam & 65535);
	rect.top    = ::LONG (wparam >> 16  );
	rect.right  = ::LONG (lparam & 65535) + rect.left;
	rect.bottom = ::LONG (lparam >> 16  ) + rect.top;
	::InvalidateRect (_main_win, &rect, FALSE);

	return false;
}



bool	IoWindows::process_led (::HWND hwnd, ::WPARAM wparam, ::LPARAM lparam)
{
	fstb::unused (hwnd);

	const int      index = int (wparam);
	const float    val   = lparam / 65535.f;
	_led_arr [index] = val;

	const int      led_x1 =  index      * _disp_w / _nbr_led;
	const int      led_x2 = (index + 1) * _disp_w / _nbr_led;
	const int      led_w  = led_x2 - led_x1;
	const int      led_y1 = _scr_h * _zoom;

	const ::WPARAM wparam_r = (led_y1 << 16) + led_x1;
	const ::LPARAM lparam_r = (_led_h << 16) + led_w;
	::PostMessageW (_main_win, MsgCustom_REDRAW, wparam_r, lparam_r);

	return false;
}



bool	IoWindows::process_switch (::HWND hwnd, ::WPARAM wparam)
{
	fstb::unused (hwnd);

	const int      index = int (wparam);
	const SwLoc *  loc_ptr = find_sw_from_index (index);
	if (loc_ptr == nullptr)
	{
		assert (false);
	}
	else
	{
		int            sw_x;
		int            sw_y;
		compute_sw_coord (sw_x, sw_y, loc_ptr->_type, loc_ptr->_x, loc_ptr->_y);

		const ::WPARAM wparam_r = ( sw_y << 16) +  sw_x;
		const ::LPARAM lparam_r = (_sw_h << 16) + _sw_w;
		::PostMessageW (_main_win, MsgCustom_REDRAW, wparam_r, lparam_r);
	}

	return false;
}



bool	IoWindows::process_paint (::HWND hwnd, ::WPARAM wparam, ::LPARAM lparam)
{
	fstb::unused (wparam, lparam);

	if (hwnd != _main_win)
	{
		return true;
	}

	::PAINTSTRUCT  paint_data;
	::HDC          hdc  = ::BeginPaint (hwnd, &paint_data);
	const int      x1 = paint_data.rcPaint.left;
	const int      y1 = paint_data.rcPaint.top;
	const int      x2 = paint_data.rcPaint.right;
	const int      y2 = paint_data.rcPaint.bottom;
	const int      w  = x2 - x1;
	const int      h  = y2 - y1;

	redraw_main_screen (x1, y1, x2, y2);
	for (int led_cnt = 0; led_cnt < _nbr_led; ++led_cnt)
	{
		redraw_led (x1, y1, x2, y2, led_cnt);
	}
	redraw_sw_all (x1, y1, x2, y2);
	redraw_lsc_all (x1, y1, x2, y2);

	::HDC          context = ::CreateCompatibleDC (nullptr);
	::HGDIOBJ      old_obj = ::SelectObject (context, _bitmap);
	::BitBlt (hdc, x1, y1, w, h, context, x1, y1, SRCCOPY);
	::SelectObject (context, old_obj);

	::DeleteDC (context);

	::EndPaint (hwnd, &paint_data);

	return false;
}



bool	IoWindows::process_key (::HWND hwnd, ::WPARAM wparam, ::LPARAM lparam, bool down_flag)
{
	fstb::unused (hwnd);

	const int      virt      = int (wparam);
	const int      scan      =   (lparam >> 16) & 255;
	const bool     prev_flag = (((lparam >> 30) &   1) != 0);

	const ScanEntry * se_ptr = nullptr;
	const int      nbr_keys  =
		int (sizeof (_scan_table) / sizeof (_scan_table [0]));
	for (int k = 0; k < nbr_keys && se_ptr == nullptr; ++ k)
	{
		const ScanEntry & se = _scan_table [k];
		if (   (  se._scan_flag && scan == se._code)
			   || (! se._scan_flag && virt == se._code))
		{
			se_ptr = &se;
		}
	}

	if (se_ptr != nullptr)
	{
		const int64_t     date = get_date ();

		if (se_ptr->_type == ui::UserInputType_SW)
		{
			if (! (down_flag && prev_flag))
			{
				update_sw_state (se_ptr->_index, down_flag);
				const float    val = (down_flag) ? 1.f : 0.f;
				enqueue_val (date, se_ptr->_type, se_ptr->_index, val);
			}
		}
		else if (se_ptr->_type == ui::UserInputType_POT)
		{
			if (down_flag && ! prev_flag)
			{
				const float    val = float (se_ptr->_dir) / 9.f;
				enqueue_val (date, se_ptr->_type, se_ptr->_index, val);
			}
		}
		else if (se_ptr->_type == ui::UserInputType_ROTENC)
		{
			if (down_flag)
			{
				const float    val = float (se_ptr->_dir);
				enqueue_val (date, se_ptr->_type, se_ptr->_index, val);
			}
		}
		else
		{
			assert (false);
		}
	}

	return true;
}



bool	IoWindows::process_lbuttondown (::HWND hwnd, ::WPARAM wparam, ::LPARAM lparam)
{
	fstb::unused (wparam);

	::SetFocus (hwnd);
	::SetCapture (hwnd);

	const int      mx = GET_X_LPARAM (lparam);
	const int      my = GET_Y_LPARAM (lparam);
	const SwLoc *  sw_loc_ptr = find_sw_from_coord (mx, my);
	if (sw_loc_ptr != nullptr)
	{
		release_mouse_pressed_sw ();
		enqueue_sw_msg (sw_loc_ptr->_index, true);
		_pressed_sw_ptr = sw_loc_ptr;
	}
	else
	{
		if (_model_ptr == nullptr)
		{
			assert (false);
		}
		else
		{
			std::string    pathname;
			const Lsc      type = find_lsc_from_coord (mx, my);
			if (type != Lsc_INVALID)
			{
				int            ret_val = select_file (
					pathname,
					(type == Lsc_SAVE),
					(type == Lsc_SAVE) ? "Save config" : "Load config"
				);
				if (ret_val == 0)
				{
					auto &         cmd_mgr  = _model_ptr->use_async_cmd ();
					auto *         cell_ptr = cmd_mgr.use_pool ().take_cell (true);
					cell_ptr->_val._content._msg_sptr =
						std::static_pointer_cast <ModelMsgCmdInterface> (
							std::make_shared <ModelMsgCmdConfLdSv> (
								  (type == Lsc_SAVE)
								? ModelMsgCmdConfLdSv::Type_SAVE
								: ModelMsgCmdConfLdSv::Type_LOAD,
								pathname,
								this
							)
						);
					cmd_mgr.enqueue (*cell_ptr, _ret_queue_sptr);
				}
			}
		}
	}

	return false;
}



bool	IoWindows::process_lbuttonup (::HWND hwnd, ::WPARAM wparam, ::LPARAM lparam)
{
	fstb::unused (hwnd, wparam, lparam);

	::ReleaseCapture ();

	return false;
}



// date in _clock_freq
void	IoWindows::enqueue_val (int64_t date, ui::UserInputType type, int index, float val)
{
	MsgQueue *     queue_ptr = _recip_list [type] [index];
	if (queue_ptr != nullptr)
	{
		MsgCell *      cell_ptr = _msg_pool.take_cell (true);
		if (cell_ptr == nullptr)
		{
			assert (false);
		}
		else
		{
			const std::chrono::microseconds  date_us (int64_t (
				double (date) * (1000 * 1000) / _clock_freq
			));
			cell_ptr->_val.set (date_us, type, index, val);
			queue_ptr->enqueue (*cell_ptr);
		}
	}
}



void	IoWindows::redraw_main_screen (int x1, int y1, int x2, int y2)
{
	const int      xd1 =  x1              / _zoom;
	const int      yd1 =  y1              / _zoom;
	const int      xd2 = (x2 + _zoom - 1) / _zoom;
	const int      yd2 = (y2 + _zoom - 1) / _zoom;

	PixArgb        bkg;
	bkg._r = (_col_bkg >> 16) & 255;
	bkg._g = (_col_bkg >>  8) & 255;
	bkg._b =  _col_bkg        & 255;
	PixArgb        black;
	black._r = (_col_min >> 16) & 255;
	black._g = (_col_min >>  8) & 255;
	black._b =  _col_min        & 255;
	PixArgb        w_dif;
	w_dif._r = ((_col_max >> 16) & 255) - black._r;
	w_dif._g = ((_col_max >>  8) & 255) - black._g;
	w_dif._b = ( _col_max        & 255) - black._b;

	if (xd1 < _scr_w && yd1 < _scr_h)
	{
		PixArgb *      dst_ptr = &_bitmap_data_ptr [yd1 * _disp_w * _zoom + xd1 * _zoom];
		const uint8_t* src_ptr = &_screen_buf [yd1 * _scr_s + xd1];
		const int      wd      = std::min (xd2, _scr_w) - xd1;
		const int      hd      = std::min (yd2, _scr_h) - yd1;
		for (int y = 0; y < hd; ++y)
		{
			for (int yz = 0; yz < _zoom; ++yz)
			{
				for (int x = 0; x < wd; ++x)
				{
					const int      v_base   = src_ptr [x];
					const int      coldif_r = black._r + ((v_base * w_dif._r) >> 8) - bkg._r;
					const int      coldif_g = black._g + ((v_base * w_dif._g) >> 8) - bkg._g;
					const int      coldif_b = black._b + ((v_base * w_dif._b) >> 8) - bkg._b;
					PixArgb *      dst2_ptr = dst_ptr + x * _zoom;
					for (int xz = 0; xz < _zoom; ++xz)
					{
						const int      v = _pix_table [yz] [xz];
						dst2_ptr [xz]._b = uint8_t (bkg._b + ((v * coldif_b) >> 8));
						dst2_ptr [xz]._g = uint8_t (bkg._g + ((v * coldif_g) >> 8));
						dst2_ptr [xz]._r = uint8_t (bkg._r + ((v * coldif_r) >> 8));
						dst2_ptr [xz]._a = 255;
					}
				}

				dst_ptr += _disp_w;
			}

			src_ptr += _scr_s;
		}
	}
}



void	IoWindows::redraw_led (int x1, int y1, int x2, int y2, int led_cnt)
{
#if 1 // Fixed size
	const int      led_w  = _led_h;
	const int      led_x1 = led_cnt * led_w;
	const int      led_x2 = led_x1 + led_w;
#else // Fills all the screen width
	const int      led_x1 =  led_cnt      * _disp_w / _nbr_led;
	const int      led_x2 = (led_cnt + 1) * _disp_w / _nbr_led;
	const int      led_w  = led_x2 - led_x1;
#endif
	const int      led_y1 = _scr_h * _zoom;
	const int      led_y2 = led_y1 + _led_h;

	x1 = std::max (x1, led_x1);
	if (led_cnt < _nbr_led - 1)
	{
		x2 = std::min (x2, led_x2);
	}
	y1 = std::max (y1, led_y1);
	y2 = std::min (y2, led_y2);

	if (x1 < x2 && y1 < y2)
	{
		const uint8_t  intensity = uint8_t (fstb::round_int (
			pow (_led_arr [led_cnt], 1.0f / 2.2f) * 255
		));
		const int      mid_pos = (_nbr_led - 1) >> 1;
		PixArgb        color;
		color._a = 255;
		color._r = (led_cnt != mid_pos) ? intensity : 0;
		color._g = (led_cnt == mid_pos) ? intensity : 0;
		color._b = 0;

		const int      rad  = std::min (led_w, _led_h) >> 1;
		const int      rad2 = rad * (rad + 1); // (rad + 0.5) ^ 2
		const int      c_x  = (led_x1 + led_x2) >> 1;
		const int      c_y  = (led_y1 + led_y2) >> 1;

		PixArgb *      dst_ptr = &_bitmap_data_ptr [y1 * _disp_w];
		for (int y = y1; y < y2; ++y)
		{
			const int      dy  = y - c_y;
			const int      dy2 = dy * dy;
			for (int x = x1; x < x2; ++x)
			{
				const int      dx  = x - c_x;
				const int      dx2 = dx * dx;
				const int      d2  = dx2 + dy2;
				if (d2 <= rad2)
				{
					dst_ptr [x] = color;
				}
				else
				{
					dst_ptr [x]._b = 64;
					dst_ptr [x]._g = 64;
					dst_ptr [x]._r = 64;
					dst_ptr [x]._a = 255;
				}
			}

			dst_ptr += _disp_w;
		}
	}
}



void	IoWindows::redraw_sw_all (int x1, int y1, int x2, int y2)
{
	const int      nbr_sw =
		int (sizeof (_switch_pos_table) / sizeof (_switch_pos_table [0]));
	for (int sw_cnt = 0; sw_cnt < nbr_sw; ++sw_cnt)
	{
		const SwLoc &  loc     = _switch_pos_table [sw_cnt];
		const bool     on_flag = is_sw_pressed (loc._index);

		int            sw_x;
		int            sw_y;
		compute_sw_coord (sw_x, sw_y, loc._type, loc._x, loc._y);
		redraw_button (x1, y1, x2, y2, sw_x, sw_y, on_flag);
	}
}



void	IoWindows::redraw_lsc_all (int x1, int y1, int x2, int y2)
{
	int            sw_x;
	int            sw_y;
	compute_lsc_coord (sw_x, sw_y, Lsc_LOAD);
	redraw_button (x1, y1, x2, y2, sw_x, sw_y, false);
	compute_lsc_coord (sw_x, sw_y, Lsc_SAVE);
	redraw_button (x1, y1, x2, y2, sw_x, sw_y, false);
}



void	IoWindows::redraw_button (int x1, int y1, int x2, int y2, int sw_x, int sw_y, bool on_flag)
{
	const int      sw_end_x = sw_x + _sw_w;
	const int      sw_end_y = sw_y + _sw_h;

	x1 = std::max (x1, sw_x    );
	x2 = std::min (x2, sw_end_x);
	y1 = std::max (y1, sw_y    );
	y2 = std::min (y2, sw_end_y);

	if (x1 < x2 && y1 < y2)
	{
		static const uint8_t c_outl = 130;
		static const uint8_t c_outs =  70;
		static const uint8_t c_fill = 100;

		const uint8_t  c_oul = (on_flag) ? 255 - c_outl : c_outl;
		const uint8_t  c_ous = (on_flag) ? 255 - c_outs : c_outs;
		const uint8_t  c_fil = (on_flag) ? 255 - c_fill : c_fill;
		const PixArgb  p_oul = { c_oul, c_oul, c_oul, 255 };
		const PixArgb  p_ous = { c_ous, c_ous, c_ous, 255 };
		const PixArgb  p_fil = { c_fil, c_fil, c_fil, 255 };

		draw_line_h (
			sw_x, sw_y            , _sw_w    , p_oul, x1, y1, x2, y2
		);
		draw_line_h (
			sw_x, sw_end_y - 1    , _sw_w    , p_ous, x1, y1, x2, y2
		);
		draw_line_v (
			sw_x        , sw_y + 1, _sw_h - 2, p_oul, x1, y1, x2, y2
		);
		draw_line_v (
			sw_end_x - 1, sw_y + 1, _sw_h - 2, p_ous, x1, y1, x2, y2
		);

		fill_block (
			sw_x + 1, sw_y + 1, _sw_w - 2, _sw_h - 2, p_fil, x1, y1, x2, y2
		);
	}
}



void	IoWindows::draw_line_h (int xo, int yo, int l, const PixArgb &c, int x1, int y1, int x2, int y2)
{
	if (yo >= y1 && yo < y2)
	{
		int            xe = xo + l;
		xo = std::max (xo, x1);
		xe = std::min (xe, x2);
		if (xo < xe)
		{
			PixArgb *      dst_ptr = &_bitmap_data_ptr [yo * _disp_w];
			for (int x = xo; x < xe; ++x)
			{
				dst_ptr [x] = c;
			}
		}
	}
}



void	IoWindows::draw_line_v (int xo, int yo, int l, const PixArgb &c, int x1, int y1, int x2, int y2)
{
	if (xo >= x1 && xo < x2)
	{
		int            ye = yo + l;
		yo = std::max (yo, y1);
		ye = std::min (ye, y2);
		if (yo < ye)
		{
			PixArgb *      dst_ptr = &_bitmap_data_ptr [0];
			for (int y = yo; y < ye; ++y)
			{
				dst_ptr [y * _disp_w + xo] = c;
			}
		}
	}
}



void	IoWindows::fill_block (int xo, int yo, int w, int h, const PixArgb &c, int x1, int y1, int x2, int y2)
{
	int            xe = xo + w;
	int            ye = yo + h;
	xo = std::max (xo, x1);
	xe = std::min (xe, x2);
	yo = std::max (yo, y1);
	ye = std::min (ye, y2);
	if (xo < xe && yo < ye)
	{
		for (int y = yo; y < ye; ++y)
		{
			PixArgb *      dst_ptr = &_bitmap_data_ptr [y * _disp_w];
			for (int x = xo; x < xe; ++x)
			{
				dst_ptr [x] = c;
			}
		}
	}
}



const IoWindows::SwLoc *  IoWindows::find_sw_from_coord (int x, int y) const
{
	const SwLoc *  loc_ptr = nullptr;

	if (y >= _sw_y && y < _sw_y + _sw_h_tot)
	{
		const int      nbr_sw =
			int (sizeof (_switch_pos_table) / sizeof (_switch_pos_table [0]));
		for (int sw_cnt = 0; sw_cnt < nbr_sw && loc_ptr == nullptr; ++sw_cnt)
		{
			const SwLoc &  loc = _switch_pos_table [sw_cnt];
			int            sw_x;
			int            sw_y;
			compute_sw_coord (sw_x, sw_y, loc._type, loc._x, loc._y);
			if (   x >= sw_x && x < sw_x + _sw_w
			    && y >= sw_y && y < sw_y + _sw_h)
			{
				loc_ptr = &loc;
			}
		}
	}

	return loc_ptr;
}



const IoWindows::SwLoc *	IoWindows::find_sw_from_index (int index) const
{
	const SwLoc *  loc_ptr = nullptr;
	const int      nbr_sw  =
		int (sizeof (_switch_pos_table) / sizeof (_switch_pos_table [0]));
	for (int sw_cnt = 0; sw_cnt < nbr_sw && loc_ptr == nullptr; ++sw_cnt)
	{
		const SwLoc &  loc = _switch_pos_table [sw_cnt];
		if (loc._index == index)
		{
			loc_ptr = &loc;
		}
	}

	return loc_ptr;
}



void	IoWindows::compute_sw_coord (int &x, int &y, SwType type, int col, int row) const
{
	const int      sw_x_base =
		  (type == SwType_CTRL)
		? _sw_w * _nbr_fsw_col + _sw_gap
		: 0;
	x = sw_x_base + col * _sw_w;
	y = _sw_y     + row * _sw_h;
}



IoWindows::Lsc	IoWindows::find_lsc_from_coord (int x, int y) const
{
	Lsc            lsc_found = Lsc_INVALID;
	for (int pos = 0; pos < Lsc_NBR_ELT && lsc_found == Lsc_INVALID; ++pos)
	{
		const Lsc      type = static_cast <Lsc> (pos);
		int            lsc_x;
		int            lsc_y;
		compute_lsc_coord (lsc_x, lsc_y, type);
		if (   x >= lsc_x && x < lsc_x + _lsc_w
		    && y >= lsc_y && y < lsc_y + _lsc_h)
		{
			lsc_found = type;
		}
	}

	return lsc_found;
}



void	IoWindows::compute_lsc_coord (int &x, int &y, Lsc type) const
{
	assert (type >= 0);
	assert (type < Lsc_NBR_ELT);

	x = _lsc_x;
	y = _lsc_y + type * _lsc_h;
}



void	IoWindows::release_mouse_pressed_sw ()
{
	if (_pressed_sw_ptr != nullptr)
	{
		enqueue_sw_msg (_pressed_sw_ptr->_index, false);
		_pressed_sw_ptr = nullptr;
	}
}



void	IoWindows::enqueue_sw_msg (int index, bool on_flag)
{
	update_sw_state (index, on_flag);
	const int64_t  date = get_date ();
	const float    val  = (on_flag) ? 1.f : 0.f;
	enqueue_val (date, ui::UserInputType_SW, index, val);
}



void	IoWindows::update_sw_state (int index, bool on_flag)
{
	const uint64_t mask = uint64_t (1) << index;
	if (on_flag)
	{
		_sw_states |= mask;
	}
	else
	{
		_sw_states &= ~mask;
	}

	const ::WPARAM wparam = index;
	::PostMessageW (_main_win, MsgCustom_SWITCH, wparam, 0);
}



bool	IoWindows::is_sw_pressed (int index) const
{
	const uint64_t mask = uint64_t (1) << index;

	return ((_sw_states & mask) != 0);
}



::LRESULT CALLBACK	IoWindows::winproc_static (::HWND hwnd, ::UINT message, ::WPARAM wparam, ::LPARAM lparam)
{
	::LRESULT      ret_val = 0;
	IoWindows *   obj_ptr =
		reinterpret_cast <IoWindows *> (::GetWindowLongPtrW (hwnd, GWLP_USERDATA));
	if (obj_ptr == nullptr)
	{
		ret_val = ::DefWindowProcW (hwnd, message, wparam, lparam);
	}
	else
	{
		ret_val = obj_ptr->winproc (hwnd, message, wparam, lparam);
	}

	return ret_val;
}



int64_t	IoWindows::get_date ()
{
	::LARGE_INTEGER   d;
	::QueryPerformanceCounter (&d);
	const int64_t     date = d.QuadPart;

	return date;
}



int	IoWindows::select_file (std::string &pathname, bool save_flag, std::string title)
{
	int            ret_val = 0;

	const wchar_t  filter_buf [] = L"All Files\0*.*\0\0";
	const int      max_pathlen = 32767;

	std::basic_string <wchar_t> path_s_utf16;
	ret_val = fstb::txt::Conv::utf8_to_utf16 (path_s_utf16, pathname.c_str ());
	std::vector <wchar_t>   path_utf16 (
		path_s_utf16.begin (), path_s_utf16.end ()
	);
	std::basic_string <wchar_t>   title_utf16;
	if (ret_val == 0)
	{
		path_utf16.resize (max_pathlen, wchar_t (0));

		ret_val = fstb::txt::Conv::utf8_to_utf16 (title_utf16, title.c_str ());
	}

	if (ret_val == 0)
	{
		OPENFILENAMEW  ofn;
		memset (&ofn, 0, sizeof (ofn));
		ofn.lStructSize       = sizeof (ofn);  // Specifies the length, in bytes, of the structure.
		ofn.hwndOwner         = ::GetForegroundWindow (); // Identifies the window that owns the dialog box.
		ofn.hInstance         = nullptr;       // If neither flag is set, this member is ignored.
		ofn.lpstrFilter       = &filter_buf [0]; // Pointer to a buffer containing pairs of null-terminated filter strings.
		ofn.lpstrCustomFilter = nullptr;       // If this member is NULL, the dialog box does not preserve user-defined filter patterns.
		ofn.nMaxCustFilter    = 0;             // This member is ignored if lpstrCustomFilter is NULL
		ofn.nFilterIndex      = 1;             // Specifies the index of the currently selected filter in the File Types control.
		ofn.lpstrFile         = &path_utf16 [0]; // Pointer to a buffer that contains a filename used to initialize the File Name edit control
		ofn.nMaxFile          = max_pathlen;   // Specifies the size, in bytes (ANSI version) or 16-bit characters (Unicode version), of the buffer pointed to by lpstrFile.
		ofn.lpstrFileTitle    = nullptr;       // This member can be NULL.
		ofn.nMaxFileTitle     = 0;             // This member is ignored if lpstrFileTitle is NULL.
		ofn.lpstrInitialDir   = nullptr;       // Pointer to a string that specifies the initial file directory.
		ofn.lpstrTitle        = &title_utf16 [0]; // Pointer to a string to be placed in the title bar of the dialog box.
		ofn.Flags             = OFN_ENABLESIZING | OFN_LONGNAMES;
		ofn.nFileOffset       = 0;
		ofn.nFileExtension    = 0;
		ofn.lpstrDefExt       = nullptr;
		ofn.lCustData         = 0;             // Specifies application-defined data that the system passes to the hook procedure
		ofn.lpfnHook          = nullptr;       // This member is ignored unless the Flags member includes the OFN_ENABLEHOOK flag.
		ofn.lpTemplateName    = nullptr;       // member is ignored unless the OFN_ENABLETEMPLATE flag is set in the Flags member.

		::BOOL         gfn_ret = 0;
		if (save_flag)
		{
			gfn_ret = GetSaveFileNameW (&ofn);
		}
		else
		{
			gfn_ret = GetOpenFileNameW (&ofn);
		}

		// Cancel or error
		if (gfn_ret == 0)
		{
			const int      err_type = ::CommDlgExtendedError ();
			if (err_type == 0)
			{
				ret_val = -1;  // Cancel
			}
			else
			{
				assert (false);
				ret_val = -2;  // Other error
			}
		}

		// Updates file
		else
		{
			ret_val = fstb::txt::Conv::utf16_to_utf8 (pathname, ofn.lpstrFile);
		}
	}

	return ret_val;
}



}  // namespace hw
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
