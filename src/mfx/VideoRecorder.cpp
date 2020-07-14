/*****************************************************************************

        VideoRecorder.cpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"
#include "mfx/CompressSimple.h"
#include "mfx/VideoRecorder.h"
#include "mfx/VidRecFmt.h"

#include <ratio>

#include <cassert>
#include <cstring>



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



VideoRecorder::VideoRecorder (DisplayInterface &display_main)
:	_disp (display_main)
,	_rec_flag (false)
,	_t_beg ()
,	_f_ptr (nullptr)
,	_f_mtx ()
,	_w (0)
,	_h (0)
,	_s (0)
,	_buf_raw ()
,	_buf_cmp ()
,	_buf_limit (0)
,	_buf_pos (0)
{
	// Nothing
}



VideoRecorder::~VideoRecorder ()
{
	if (_rec_flag)
	{
		rec_stop ();
	}
}



ui::DisplayInterface &	VideoRecorder::use_main_disp ()
{
	return _disp;
}



const ui::DisplayInterface &	VideoRecorder::use_main_disp () const
{
	return _disp;
}



int	VideoRecorder::rec_start (std::string pathname)
{
	assert (! pathname.empty ());

	if (_rec_flag)
	{
		rec_stop ();
	}

	int            ret_val = 0;
	std::lock_guard <std::mutex>  lock (_f_mtx);

	_f_ptr = fstb::fopen_utf8 (pathname.c_str (), "wb");
	if (_f_ptr == nullptr)
	{
		ret_val = -1;
	}

	if (ret_val == 0)
	{
		_w = _disp.get_width ();
		_h = _disp.get_height ();
		_s = _disp.get_stride ();
		const size_t   len = size_t (_w) * size_t (_h);
		_buf_raw.resize (len);

		// Should be slightly bigger than the raw data buffer to take the
		// worst case into account.
		_buf_limit = std::max (len * 5 / 4 + 16, size_t (1) << 18);

		// * 2 because we have to be prepared to get a big frame when _buf_pos
		// is already close to _buf_limit.
		_buf_cmp.resize (_buf_limit * 2);
		_buf_pos = 0;

		VidRecFmt::HeaderFile   header;
		header._version  = uint16_t (VidRecFmt::_fmt_version);
		header._width    = uint16_t (_w);
		header._height   = uint16_t (_h);
		header._pix_code = uint16_t (VidRecFmt::PixCode_GREY8);
		memcpy (&_buf_cmp [_buf_pos], &header, sizeof (header));
		_buf_pos += sizeof (header);
	}

	if (ret_val == 0)
	{
		_t_beg  = Clk::now ();
		ret_val = dump_frame (0, 0, _w, _h, _t_beg);
	}

	if (ret_val == 0)
	{
		_rec_flag = true;
	}
	else if (_f_ptr != nullptr)
	{
		fclose (_f_ptr);
		_f_ptr = nullptr;
	}

	return ret_val;
}



int	VideoRecorder::rec_stop ()
{
	assert (_rec_flag);

	std::lock_guard <std::mutex>  lock (_f_mtx);

	const auto     t { Clk::now () };
	dump_frame (0, 0, 0, 0, t);

	fclose (_f_ptr);
	_f_ptr    = nullptr;
	_rec_flag = false;

	return 0;
}



bool	VideoRecorder::is_recording () const
{
	return _rec_flag;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	VideoRecorder::do_get_width () const
{
	return _disp.get_width ();
}



int	VideoRecorder::do_get_height () const
{
	return _disp.get_height ();
}



int	VideoRecorder::do_get_stride () const
{
	return _disp.get_stride ();
}



uint8_t *	VideoRecorder::do_use_screen_buf ()
{
	return _disp.use_screen_buf ();
}



const uint8_t*	VideoRecorder::do_use_screen_buf () const
{
	return _disp.use_screen_buf ();
}



void	VideoRecorder::do_refresh (int x, int y, int w, int h)
{
	if (_rec_flag)
	{
		std::lock_guard <std::mutex>  lock (_f_mtx);
		const auto     t { Clk::now () };
		dump_frame (x, y, w, h, t);
	}
	_disp.refresh (x, y, w, h);
}



void	VideoRecorder::do_force_reset ()
{
	_disp.force_reset ();
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	VideoRecorder::dump_frame (int x, int y, int w, int h, std::chrono::steady_clock::time_point t)
{
	int            ret_val = 0;

	// Builds frame header
	const Clk::duration  dur { t - _t_beg };
	typedef std::ratio_divide <
		Clk::period::type,
		VidRecFmt::TimestampUnit::period::type
	> Conv;
	const uint64_t timestamp_us = uint64_t (
		dur.count () * Conv::num / Conv::den
	);

	VidRecFmt::HeaderFrame  header;
	header._timestamp = timestamp_us;
	header._x         = uint16_t (x);
	header._y         = uint16_t (y);
	header._w         = uint16_t (w);
	header._h         = uint16_t (h);
	memcpy (&_buf_cmp [_buf_pos], &header, sizeof (header));
	_buf_pos += sizeof (header);
	assert (_buf_pos <= _buf_cmp.size ());

	// Gets picture data
	size_t         pos = 0;
	const uint8_t* scr_ptr = _disp.use_screen_buf ();
	scr_ptr += y * _s + x;
	for (int dy = 0; dy < h; ++dy)
	{
		memcpy (&_buf_raw [pos], scr_ptr, w);
		scr_ptr += _s;
		pos     += w;
	}
	const size_t   len_raw = pos;

	// Compress the picture data
	const size_t   len_cmp = CompressSimple::compress_frame (
		&_buf_cmp [_buf_pos], _buf_raw.data (), len_raw
	);
	_buf_pos += len_cmp;
	assert (_buf_pos <= _buf_cmp.size ());

	// Writes on disk if required
	if (_buf_pos >= _buf_limit || len_raw == 0)
	{
		if (fwrite (_buf_cmp.data (), 1, _buf_pos, _f_ptr) != _buf_pos)
		{
			ret_val = -1;
		}
		_buf_pos = 0;
	}

	return ret_val;
}



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
