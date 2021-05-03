/*****************************************************************************

        RecD2d.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_RecD2d_HEADER_INCLUDED)
#define mfx_RecD2d_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/msg/MsgRet.h"
#include "fstb/msg/QueueRetMgr.h"
#include "mfx/FileOpWav.h"

#include <array>
#include <atomic>
#include <utility>
#include <vector>

#include <cstdio>
#include <cstdint>



namespace mfx
{



class RecD2d
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               RecD2d () = default;
	virtual        ~RecD2d ();

	int            create_file (const char pathname_0 [], int nbr_chn, double sample_freq, size_t max_len = ~size_t (0));
	int            close_file ();
	std::pair <bool, int>
	               process_messages ();
	bool           is_recording () const noexcept;
	int64_t        get_size_frames () const noexcept;

	// RT thread
	void           write_data (const float * const chn_arr [], int nbr_spl) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _buf_size  =  1024; // Buffer sent by the RT thread, in samples
	static const int  _page_size = 1<<18; // Buffer for disk writing, in samples, > _buf_size

	class Buffer
	{
	public:
		std::array <float, _buf_size> // Interleaved data
		               _buf;
		int            _len = 0;      // Sample frames
		void           clear () { /* Nothing */ }
	};

	typedef fstb::msg::MsgRet <Buffer> D2dMsg;
	typedef fstb::msg::QueueRetMgr <D2dMsg> D2dQueue;
	
	int            write_disk_buffer ();

	D2dQueue       _queue_mgr;
	typename D2dQueue::QueueSPtr
	               _queue_sptr     = _queue_mgr.create_new_ret_queue ();
	int            _max_frm_rt     = _buf_size; // Maximum number of frames per buffer
	int            _max_frm_disk   = _page_size;
	int            _buf_cur_len    = 0; // Sample frames
	std::atomic <bool>
	               _write_flag { false };
	FileOpWav      _file_writer;
	std::atomic <typename D2dQueue::CellType *>
	               _cell_ptr { nullptr };
	int            _nbr_chn        = 0;
	std::vector <float>
	               _buf_disk;
	int            _buf_disk_pos   = 0; // Sample frames



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               RecD2d (const RecD2d &other)            = delete;
	               RecD2d (RecD2d &&other)                 = delete;
	RecD2d &       operator = (const RecD2d &other)        = delete;
	RecD2d &       operator = (RecD2d &&other)             = delete;
	bool           operator == (const RecD2d &other) const = delete;
	bool           operator != (const RecD2d &other) const = delete;

}; // class RecD2d



}  // namespace mfx



//#include "mfx/RecD2d.hpp"



#endif   // mfx_RecD2d_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
