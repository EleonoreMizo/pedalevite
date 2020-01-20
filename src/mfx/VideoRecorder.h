/*****************************************************************************

        VideoRecorder.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_VideoRecorder_HEADER_INCLUDED)
#define mfx_VideoRecorder_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/ui/DisplayInterface.h"

#include <mutex>
#include <chrono>
#include <string>
#include <vector>

#include <cstdio>



namespace mfx
{



class VideoRecorder
:	public ui::DisplayInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       VideoRecorder (DisplayInterface &display_main);
	               ~VideoRecorder ();

	ui::DisplayInterface &
	               use_main_disp ();
	const ui::DisplayInterface &
	               use_main_disp () const;

	int            rec_start (std::string pathname);
	int            rec_stop ();
	bool           is_recording () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::ui::DisplayInterface
	int            do_get_width () const final;
	int            do_get_height () const final;
	int            do_get_stride () const final;
	uint8_t *      do_use_screen_buf () final;
	const uint8_t* do_use_screen_buf () const final;

	void           do_refresh (int x, int y, int w, int h) final;
	void           do_force_reset () final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::chrono::steady_clock Clk;

	int            dump_frame (int x, int y, int w, int h, std::chrono::steady_clock::time_point t);

	ui::DisplayInterface &
	               _disp;
	volatile bool  _rec_flag;
	Clk::time_point
	               _t_beg;
	FILE *         _f_ptr;
	std::mutex     _f_mtx;
	int            _w;
	int            _h;
	int            _s;
	std::vector <uint8_t>
	               _buf_raw;
	std::vector <uint8_t>
	               _buf_cmp;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               VideoRecorder ()                               = delete;
	               VideoRecorder (const VideoRecorder &other)     = delete;
	               VideoRecorder (VideoRecorder &&other)          = delete;
	VideoRecorder &
	               operator = (const VideoRecorder &other)        = delete;
	VideoRecorder &
	               operator = (VideoRecorder &&other)             = delete;
	bool           operator == (const VideoRecorder &other) const = delete;
	bool           operator != (const VideoRecorder &other) const = delete;

}; // class VideoRecorder



}  // namespace mfx



//#include "mfx/VideoRecorder.hpp"



#endif   // mfx_VideoRecorder_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

