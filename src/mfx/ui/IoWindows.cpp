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

#include "fstb/fnc.h"
#include "mfx/ui/IoWindows.h"
#include "mfx/Cst.h"

#include <stdexcept>

#include <cassert>
#include <cmath>
#include <cstring>



namespace mfx
{
namespace ui
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



IoWindows::IoWindows (volatile bool &quit_request_flag)
:	_screen_buf ()
,	_main_win (0)
,	_bitmap (0)
,	_bitmap_data_ptr (0)
,	_led_arr ()
,	_pix_table ()
,	_msg_pool ()
,	_recip_list ()
,	_clock_freq (0)
,	_quit_request_flag (quit_request_flag)
,	_quit_flag ()
,	_msg_loop_thread (&IoWindows::main_loop, this)
{
	for (auto &l : _led_arr)
	{
		l = 0;
	}

	_msg_pool.expand_to (256);
	for (int i = 0; i < UserInputType_NBR_ELT; ++i)
	{
		const int      nbr_dev =
			do_get_nbr_param (static_cast <UserInputType> (i));
		_recip_list [i].resize (nbr_dev, 0);
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

	if (_bitmap != 0)
	{
		::DeleteObject (_bitmap);
		_bitmap = 0;
		_bitmap_data_ptr = 0;
	}
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



int	IoWindows::do_get_nbr_param (UserInputType /*type*/) const
{
	return Cst::_max_input_param;
}



void	IoWindows::do_set_msg_recipient (UserInputType type, int index, MsgQueue *queue_ptr)
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



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



const wchar_t	IoWindows::_window_class_name_0 [] =
	L"pedalevite_mfx_ui_iowindows_main";

const IoWindows::ScanEntry	IoWindows::_scan_table [] =
{
	// Pedals, top row (6) 
	{ true , 0x10      , UserInputType_SW    ,  2,  0 }, // A
	{ true , 0x11      , UserInputType_SW    ,  3,  0 }, // Z
	{ true , 0x12      , UserInputType_SW    ,  4,  0 }, // E
	{ true , 0x13      , UserInputType_SW    ,  5,  0 }, // R
	{ true , 0x14      , UserInputType_SW    ,  6,  0 }, // T
	{ true , 0x15      , UserInputType_SW    ,  7,  0 }, // Y

	// Pedals, bottom row (6)
	{ true , 0x1E      , UserInputType_SW    ,  8,  0 }, // Q
	{ true , 0x1F      , UserInputType_SW    ,  9,  0 }, // S
	{ true , 0x20      , UserInputType_SW    , 14,  0 }, // D
	{ true , 0x21      , UserInputType_SW    , 15,  0 }, // F
	{ true , 0x22      , UserInputType_SW    , 16,  0 }, // G
	{ true , 0x23      , UserInputType_SW    , 17,  0 }, // H

	// Rotary encoders with push button (2)
	{ true , 0x2C      , UserInputType_ROTENC,  5, -1 }, // W
	{ true , 0x2D      , UserInputType_ROTENC,  5, +1 }, // X
	{ true , 0x2E      , UserInputType_SW    , 18,  0 }, // C
	{ true , 0x2F      , UserInputType_ROTENC,  6, -1 }, // V
	{ true , 0x30      , UserInputType_ROTENC,  6, +1 }, // B
	{ true , 0x31      , UserInputType_SW    , 19,  0 }, // N

	// Common rotary encoders
	{ false, 0x31      , UserInputType_ROTENC,  0, -1 }, // 1
	{ false, 0x32      , UserInputType_ROTENC,  0, +1 }, // 2
	{ false, 0x33      , UserInputType_ROTENC,  1, -1 }, // 3
	{ false, 0x34      , UserInputType_ROTENC,  1, +1 }, // 4
	{ false, 0x35      , UserInputType_ROTENC,  2, -1 }, // 5
	{ false, 0x36      , UserInputType_ROTENC,  2, +1 }, // 6
	{ false, 0x37      , UserInputType_ROTENC,  3, -1 }, // 7
	{ false, 0x38      , UserInputType_ROTENC,  3, +1 }, // 8
	{ false, 0x39      , UserInputType_ROTENC,  4, -1 }, // 9
	{ false, 0x30      , UserInputType_ROTENC,  4, +1 }, // 0

	// Panel buttons
	{ false, VK_RETURN , UserInputType_SW    ,  0,  0 }, // Enter (main)
	{ false, VK_ESCAPE , UserInputType_SW    ,  1,  0 }, // Esc
	{ false, VK_UP     , UserInputType_SW    , 10,  0 }, // Up
	{ false, VK_DOWN   , UserInputType_SW    , 11,  0 }, // Down
	{ false, VK_LEFT   , UserInputType_SW    , 12,  0 }, // Left
	{ false, VK_RIGHT  , UserInputType_SW    , 13,  0 }, // Right

	// Expression pedal position
	{ false, VK_NUMPAD0, UserInputType_POT   ,  0,  0 },
	{ false, VK_NUMPAD1, UserInputType_POT   ,  0,  1 },
	{ false, VK_NUMPAD2, UserInputType_POT   ,  0,  2 },
	{ false, VK_NUMPAD3, UserInputType_POT   ,  0,  3 },
	{ false, VK_NUMPAD4, UserInputType_POT   ,  0,  4 },
	{ false, VK_NUMPAD5, UserInputType_POT   ,  0,  5 },
	{ false, VK_NUMPAD6, UserInputType_POT   ,  0,  6 },
	{ false, VK_NUMPAD7, UserInputType_POT   ,  0,  7 },
	{ false, VK_NUMPAD8, UserInputType_POT   ,  0,  8 },
	{ false, VK_NUMPAD9, UserInputType_POT   ,  0,  9 }
};



void	IoWindows::main_loop ()
{
	::HMODULE      module = ::GetModuleHandleW (0);

	::WNDCLASSEXW  wcex;
	memset (&wcex, 0, sizeof (wcex));

	// Create the window class
	wcex.cbSize			= sizeof (WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc	= &winproc_static;
	wcex.cbClsExtra	= 0;
	wcex.cbWndExtra	= 0;
	wcex.hInstance		= module;
	wcex.hIcon			= 0;
	wcex.hCursor		= ::LoadCursor (NULL, IDC_ARROW);
	wcex.hbrBackground= HBRUSH (COLOR_APPWORKSPACE + 1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName= _window_class_name_0;
	wcex.hIconSm		= 0;
	::RegisterClassExW (&wcex);	// No check

	_main_win = ::CreateWindowW (
		_window_class_name_0,
		L"P\x00E9" L"dale Vite",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, 0,
		320, 200,
		0,
		0,
		module,
		0
	);
	if (_main_win == 0)
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
		const ::BOOL   gm_res = ::GetMessageW (&msg, 0, 0, 0);
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
	_main_win = 0;

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

	case MsgCustom_REDRAW:
		def_proc_flag = process_redraw (hwnd, wparam, lparam);
		break;

	case MsgCustom_LED:
		def_proc_flag = process_led (hwnd, wparam, lparam);
		break;
	default:
		def_proc_flag = true;
		break;
	}

	if (def_proc_flag)
	{
		ret_val = ::DefWindowProcW (hwnd, message, wparam, lparam);
	}

	return ret_val;
}



bool	IoWindows::process_redraw (::HWND hwnd, ::WPARAM wparam, ::LPARAM lparam)
{
	::RECT         rect;
	rect.left   =  wparam & 65535;
	rect.top    =  wparam >> 16;
	rect.right  = (lparam & 65535) + rect.left;
	rect.bottom = (lparam >> 16  ) + rect.top;
	::InvalidateRect (_main_win, &rect, FALSE);

	return false;
}



bool	IoWindows::process_led (::HWND hwnd, ::WPARAM wparam, ::LPARAM lparam)
{
	const int      index = wparam;
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



bool	IoWindows::process_paint (::HWND hwnd, ::WPARAM wparam, ::LPARAM lparam)
{
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

	::HDC          context = ::CreateCompatibleDC (0);
	::HGDIOBJ      old_obj = ::SelectObject (context, _bitmap);
	::BitBlt (hdc, x1, y1, w, h, context, x1, y1, SRCCOPY);
	::SelectObject (context, old_obj);

	::DeleteDC (context);

	::EndPaint (hwnd, &paint_data);

	return false;
}



bool	IoWindows::process_key (::HWND hwnd, ::WPARAM wparam, ::LPARAM lparam, bool down_flag)
{
	const int      virt      =    wparam;
	const int      scan      =   (lparam >> 16) & 255;
	const bool     prev_flag = (((lparam >> 30) &   1) != 0);

	const ScanEntry * se_ptr = 0;
	const int      nbr_keys =
		int (sizeof (_scan_table) / sizeof (_scan_table [0]));
	for (int k = 0; k < nbr_keys && se_ptr == 0; ++ k)
	{
		const ScanEntry & se = _scan_table [k];
		if (   (  se._scan_flag && scan == se._code)
			   || (! se._scan_flag && virt == se._code))
		{
			se_ptr = &se;
		}
	}

	if (se_ptr != 0)
	{
		::LARGE_INTEGER   d;
		::QueryPerformanceCounter (&d);
		const int64_t     date = d.QuadPart;

		if (se_ptr->_type == UserInputType_SW)
		{
			if (! (down_flag && prev_flag))
			{
				const float    val = (down_flag) ? 1.f : 0.f;
				enqueue_val (date, se_ptr->_type, se_ptr->_index, val);
			}
		}
		else if (se_ptr->_type == UserInputType_POT)
		{
			if (down_flag && ! prev_flag)
			{
				const float    val = float (se_ptr->_dir) / 9.f;
				enqueue_val (date, se_ptr->_type, se_ptr->_index, val);
			}
		}
		else if (se_ptr->_type == UserInputType_ROTENC)
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



// date in _clock_freq
void	IoWindows::enqueue_val (int64_t date, UserInputType type, int index, float val)
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
	bkg._r = 0;
	bkg._g = 0;
	bkg._b = 255;
	PixArgb        black;
	black._r = 32;
	black._g = 0;
	black._b = 192;
	PixArgb        w_dif;
	w_dif._r = 255 - black._r;
	w_dif._g = 255 - black._g;
	w_dif._b = 255 - black._b;

	if (xd1 < _scr_w && yd1 < _scr_h)
	{
		PixArgb *      dst_ptr = &_bitmap_data_ptr [yd1 * _disp_w * _zoom + xd1 * _zoom];
		const uint8_t* src_ptr = &_screen_buf [yd1 * _scr_s + xd1];
		const int      wd      = std::min (xd2, _scr_w) - xd1;
		const int      hd      = std::min (yd2, _scr_h) - yd1;
		const int      pix_h   = _zoom - 1;
		const int      pix_w   = _zoom - 1;
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
#if 1
	const int      led_w  = _led_h;
	const int      led_x1 = led_cnt * led_w;
	const int      led_x2 = led_x1 + led_w;
#else
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



::LRESULT CALLBACK	IoWindows::winproc_static (::HWND hwnd, ::UINT message, ::WPARAM wparam, ::LPARAM lparam)
{
	::LRESULT      ret_val = 0;
	IoWindows *   obj_ptr =
		reinterpret_cast <IoWindows *> (::GetWindowLongPtrW (hwnd, GWLP_USERDATA));
	if (obj_ptr == 0)
	{
		ret_val = ::DefWindowProcW (hwnd, message, wparam, lparam);
	}
	else
	{
		ret_val = obj_ptr->winproc (hwnd, message, wparam, lparam);
	}

	return ret_val;
}



}  // namespace ui
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
