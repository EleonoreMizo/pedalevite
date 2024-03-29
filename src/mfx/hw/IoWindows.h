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
#if ! defined (mfx_hw_IoWindows_HEADER_INCLUDED)
#define mfx_hw_IoWindows_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



#if ! defined (PV_VERSION)
	#error PV_VERSION should be defined.
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "conc/CellPool.h"
#include "fstb/msg/MsgRet.h"
#include "mfx/ui/DisplayInterface.h"
#include "mfx/ui/LedInterface.h"
#include "mfx/ui/UserInputInterface.h"
#include "mfx/ModelMsgCmdAsync.h"
#include "mfx/ModelMsgCmdCbInterface.h"
#include "mfx/Stop.h"

#include <Windows.h>

#include <array>
#include <atomic>
#include <thread>
#include <vector>

#include <cstdint>



namespace mfx
{

class Model;

namespace hw
{



class IoWindows final
:	public ui::DisplayInterface
,	public ui::LedInterface
,	public ui::UserInputInterface
,	public ModelMsgCmdCbInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       IoWindows (Stop &stop);
	               ~IoWindows ();

	void           set_model (Model &model);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// ui::DisplayInterface
	int            do_get_width () const final;
	int            do_get_height () const final;
	int            do_get_stride () const final;
	uint8_t *      do_use_screen_buf () final;
	const uint8_t *
	               do_use_screen_buf () const final;

	void           do_refresh (int x, int y, int w, int h) final;
	void           do_force_reset () final;

	// ui::LedInterface
	int            do_get_nbr_led () const final;
	void           do_set_led (int index, float val) final;

	// ui::UserInputInterface
	int            do_get_nbr_param (ui::UserInputType type) const final;
	void           do_set_msg_recipient (ui::UserInputType type, int index, MsgQueue *queue_ptr) final;
	void           do_return_cell (MsgCell &cell) final;
	std::chrono::microseconds
	               do_get_cur_date () const final;

	// ModelMsgCmdCbInterface
	void           do_notify_model_error_code (int ret_val) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum MsgCustom
	{
		MsgCustom_REDRAW = WM_APP,
		MsgCustom_LED,
		MsgCustom_SWITCH
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
		ui::UserInputType
		               _type;
		int            _index;
		int            _dir;
	};

	enum SwType
	{
		SwType_FOOT = 0,
		SwType_CTRL,

		SwType_NBR_ELT
	};

	class SwLoc
	{
	public:
		int            _index;
		SwType         _type;
		int            _x;
		int            _y;
	};

	// Load/Save config
	enum Lsc
	{
		Lsc_INVALID = -1,

		Lsc_LOAD = 0,
		Lsc_SAVE,

		Lsc_NBR_ELT
	};

#if (PV_VERSION == 2)
	static constexpr int _scr_w     = 800;
	static constexpr int _scr_h     = 480;
	static constexpr int _col_bkg   = 0x000000; // 0xRRGGBB
	static constexpr int _col_min   = 0x000000;
	static constexpr int _col_max   = 0xFFFFFF;
	static constexpr int _zoom      = 1;
#else // PV_VERSION
	static constexpr int _scr_w     = 128;
	static constexpr int _scr_h     =  64;
	static constexpr int _col_bkg   = 0x0000FF; // 0xRRGGBB
	static constexpr int _col_min   = 0x2000C0;
	static constexpr int _col_max   = 0xFFFFFF;
	static constexpr int _zoom      = 4;
#endif // PV_VERSION
	static constexpr int _scr_s     = _scr_w;
	static constexpr int _nbr_led   = 3;
	static constexpr int _nbr_fsw_row = 2; // Two rows of 6 footswitches
	static constexpr int _nbr_fsw_col = 6;
	static constexpr int _nbr_but_row = 2; // Two rows of 3 buttons for the UI
	static constexpr int _nbr_but_col = 3;
	static constexpr int _nbr_lsc_row = 2; // Two rows for load/save config

	static constexpr int _led_h_max = 32;
	static constexpr int _led_h_tmp = _scr_h * _zoom / 2;
	static constexpr int _led_h     = (_led_h_tmp < _led_h_max) ? _led_h_tmp : _led_h_max;
	static constexpr int _led_y     = _scr_h * _zoom;

	static constexpr int _sw_gap    = 16;  // Between the two groups of switches
	static constexpr int _sw_l_max  = 32;
	static constexpr int _sw_l_tmp  =
		(_scr_w * _zoom - _sw_gap * 2) / (_nbr_fsw_col + _nbr_but_col + _nbr_lsc_row);
	static constexpr int _sw_l      = (_sw_l_tmp < _sw_l_max) ? _sw_l_tmp : _sw_l_max;
	static constexpr int _sw_w      = _sw_l;
	static constexpr int _sw_h      = _sw_l;
	static constexpr int _sw_r_max  = (_nbr_fsw_row < _nbr_but_row) ? _nbr_but_row : _nbr_fsw_row;
	static constexpr int _sw_h_tot  = _sw_r_max * _sw_h;
	static constexpr int _sw_y      = _led_y + _led_h;

	static constexpr int _lsc_x     = _sw_w * (_nbr_fsw_col + _nbr_but_col) + _sw_gap * 2;
	static constexpr int _lsc_y     = _sw_y;
	static constexpr int _lsc_w     = _sw_w;
	static constexpr int _lsc_h     = _sw_h;

	static constexpr int _disp_w    = _scr_w * _zoom;
	static constexpr int _disp_h    = _scr_h * _zoom + _led_h + _sw_h_tot;

	typedef std::array <uint8_t, _scr_s * _scr_h> ScreenBuffer;

	typedef conc::CellPool <ui::UserInputMsg> MsgPool;

	typedef std::vector <MsgQueue *> QueueArray;
	typedef std::array <QueueArray, ui::UserInputType_NBR_ELT> RecipientList;

	typedef typename fstb::msg::MsgRet <ModelMsgCmdAsync>::QueueSPtr RetQueueSPtr;

	void           main_loop ();
	void           resize_win (int w, int h);
	void           init_bitmap (int w, int h);
	::LRESULT      winproc (::HWND hwnd, ::UINT message, ::WPARAM wparam, ::LPARAM lparam);
	bool           process_redraw (::HWND hwnd, ::WPARAM wparam, ::LPARAM lparam);
	bool           process_led (::HWND hwnd, ::WPARAM wparam, ::LPARAM lparam);
	bool           process_switch (::HWND hwnd, ::WPARAM wparam);
	bool           process_paint (::HWND hwnd, ::WPARAM wparam, ::LPARAM lparam);
	bool           process_key (::HWND hwnd, ::WPARAM wparam, ::LPARAM lparam, bool down_flag);
	bool           process_lbuttondown (::HWND hwnd, ::WPARAM wparam, ::LPARAM lparam);
	bool           process_lbuttonup (::HWND hwnd, ::WPARAM wparam, ::LPARAM lparam);
	void           enqueue_val (int64_t date, ui::UserInputType type, int index, float val);
	void           redraw_main_screen (int x1, int y1, int x2, int y2);
	void           redraw_led (int x1, int y1, int x2, int y2, int led_cnt);
	void           redraw_sw_all (int x1, int y1, int x2, int y2);
	void           redraw_lsc_all (int x1, int y1, int x2, int y2);
	void           redraw_button (int x1, int y1, int x2, int y2, int sw_x, int sw_y, bool on_flag);
	void           draw_line_h (int xo, int yo, int l, const PixArgb & c, int x1, int y1, int x2, int y2);
	void           draw_line_v (int xo, int yo, int l, const PixArgb & c, int x1, int y1, int x2, int y2);
	void           fill_block (int xo, int yo, int w, int h, const PixArgb & c, int x1, int y1, int x2, int y2);
	const SwLoc *  find_sw_from_coord (int x, int y) const;
	const SwLoc *  find_sw_from_index (int index) const;
	void           compute_sw_coord (int &x, int &y, SwType type, int col, int row) const;
	Lsc            find_lsc_from_coord (int x, int y) const;
	void           compute_lsc_coord (int &x, int &y, Lsc type) const;
	void           release_mouse_pressed_sw ();
	void           enqueue_sw_msg (int index, bool on_flag);
	void           update_sw_state (int index, bool on_flag);
	bool           is_sw_pressed (int index) const;

	static ::LRESULT CALLBACK
	               winproc_static (::HWND hwnd, ::UINT message, ::WPARAM wparam, ::LPARAM lparam);
	static int64_t get_date ();
	static int     select_file (std::string &pathname, bool save_flag, std::string title);

	Model *        _model_ptr;         // Should be set
	RetQueueSPtr   _ret_queue_sptr;

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
	int64_t        _clock_freq;      // Hz
	const SwLoc *  _pressed_sw_ptr;  // Current switch pressed with the mouse. 0 when no switch is pressed.
	uint64_t       _sw_states;       // Each bit = current switch state (mouse or keyboard)

	Stop &         _stop;            // Application is requested to quit

	std::atomic <bool>               // Thread should stop
	               _quit_flag;
	std::thread    _msg_loop_thread;

	static const wchar_t
	               _window_class_name_0 [];
	static const ScanEntry
	               _scan_table [];
	static const SwLoc
	               _switch_pos_table [];



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               IoWindows ()                               = delete;
	               IoWindows (const IoWindows &other)         = delete;
	               IoWindows (IoWindows &&other)              = delete;
	IoWindows &    operator = (const IoWindows &other)        = delete;
	IoWindows &    operator = (IoWindows &&other)             = delete;
	bool           operator == (const IoWindows &other) const = delete;
	bool           operator != (const IoWindows &other) const = delete;

}; // class IoWindows



}  // namespace hw
}  // namespace mfx



//#include "mfx/hw/IoWindows.hpp"



#endif   // mfx_hw_IoWindows_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
