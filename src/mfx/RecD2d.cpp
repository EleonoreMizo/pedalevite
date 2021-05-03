/*****************************************************************************

        RecD2d.cpp
        Author: Laurent de Soras, 2019

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

#include "mfx/dsp/mix/Generic.h"
#include "mfx/RecD2d.h"

#include <algorithm>

#include <cassert>



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



RecD2d::~RecD2d ()
{
	if (_file_writer.is_open ())
	{
		close_file ();
	}

	if (_queue_sptr.get () != nullptr)
	{
		typename D2dQueue::CellType * c_ptr = _cell_ptr;
		if (c_ptr != nullptr)
		{
			c_ptr->_val.ret ();
			_cell_ptr = nullptr;
		}
		
		process_messages ();
		_queue_mgr.flush_ret_queue (*_queue_sptr);
		_queue_mgr.kill_ret_queue (_queue_sptr);
	}
	assert (_cell_ptr == nullptr);
}



int	RecD2d::create_file (const char pathname_0 [], int nbr_chn, double sample_freq, size_t max_len)
{
	assert (! _file_writer.is_open ());
	assert (! _write_flag);
	assert (pathname_0 != nullptr);
	assert (nbr_chn > 0);
	assert (sample_freq > 0);

	int            ret_val =
		_file_writer.create_save (pathname_0, nbr_chn, sample_freq, max_len);
	if (ret_val == 0)
	{
		_nbr_chn        = nbr_chn;
		_queue_mgr.use_pool ().expand_to (128);
		_queue_mgr.flush_ret_queue (*_queue_sptr);
		_buf_disk.resize (_page_size);
		_max_frm_rt     = _buf_size / nbr_chn;
		_max_frm_disk   = _page_size / nbr_chn;
		_buf_disk_pos   = 0;
		_write_flag     = true;
	}

	return ret_val;
}



int	RecD2d::close_file ()
{
	int            ret_val = 0;

	if (! _file_writer.is_open ())
	{
		assert (! _write_flag);
		ret_val = -1;
	}
	else
	{
		_write_flag = false;

		write_disk_buffer ();
		ret_val = _file_writer.close_file ();

		_queue_mgr.flush_ret_queue (*_queue_sptr);
	}

	return ret_val;
}



// first  = at least one message processed
// second = error code
std::pair <bool, int>	RecD2d::process_messages ()
{
	bool           proc_flag = false;
	int            ret_val   = 0;

	D2dQueue::CellType * cell_ptr = nullptr;
	do
	{
		cell_ptr = _queue_mgr.dequeue ();
		if (cell_ptr != nullptr)
		{
			proc_flag = true;
			if (_file_writer.is_open ())
			{
				Buffer &       bc = cell_ptr->_val._content;
				int            pos_rt_buf = 0;
				while (pos_rt_buf < bc._len)
				{
					const int      work_len = std::min (
						_max_frm_disk - _buf_disk_pos,
						bc._len - pos_rt_buf
					);

					dsp::mix::Generic::copy_1_1 (
						&_buf_disk [_buf_disk_pos * _nbr_chn],
						&bc._buf [pos_rt_buf * _nbr_chn],
						work_len * _nbr_chn
					);

					pos_rt_buf    += work_len;
					_buf_disk_pos += work_len;

					if (_buf_disk_pos >= _max_frm_disk)
					{
						ret_val = write_disk_buffer ();
					}
				}
			}

			cell_ptr->_val.ret ();
		}
	}
	while (cell_ptr != nullptr);

	return std::make_pair (proc_flag, ret_val);
}



bool	RecD2d::is_recording () const noexcept
{
	return _write_flag;
}



int64_t	RecD2d::get_size_frames () const noexcept
{
	return _file_writer.get_size_frames ();
}



void	RecD2d::write_data (const float * const chn_arr [], int nbr_spl) noexcept
{
	assert (chn_arr != nullptr);
	assert (nbr_spl > 0);

	if (_write_flag)
	{
		int            pos = 0;
		do
		{
			typename D2dQueue::CellType * c_ptr = _cell_ptr;
			if (c_ptr == nullptr)
			{
				// Autogrow flag is set so the call to this function will not be
				// RT-safe in case of cell starvation, but data won't be lost at
				// this level.
				try
				{
					c_ptr = _queue_mgr.use_pool ().take_cell (true);
				}
				catch (...)
				{
					assert (false);
					c_ptr = nullptr;
				}
				if (c_ptr == nullptr)
				{
					assert (false);
				}
				else
				{
					_cell_ptr = c_ptr;
					c_ptr->_val._content._len = 0;
				}
			}
			Buffer &       bc = (*_cell_ptr)._val._content;

			const int      work_len = std::min (
				nbr_spl - pos,
				_max_frm_rt - bc._len
			);

			if (_nbr_chn == 2)
			{
				dsp::mix::Generic::copy_2_2i (
					&bc._buf [bc._len * _nbr_chn],
					chn_arr [0] + pos,
					chn_arr [1] + pos,
					work_len
				);
			}
			else
			{
				for (int chn_cnt = 0; chn_cnt < _nbr_chn; ++chn_cnt)
				{
					assert (chn_arr [chn_cnt] != nullptr);
					dsp::mix::Generic::copy_1_ni1 (
						&bc._buf [bc._len * _nbr_chn + chn_cnt],
						chn_arr [chn_cnt] + pos,
						work_len,
						_nbr_chn
					);
				}
			}

			pos     += work_len;
			bc._len += work_len;

			if (bc._len >= _max_frm_rt)
			{
				_queue_mgr.enqueue (*_cell_ptr, _queue_sptr);
				_cell_ptr = nullptr;
			}
		}
		while (pos < nbr_spl);
	}

	typename D2dQueue::CellType * c_ptr = _cell_ptr;
	if (! _write_flag && c_ptr != nullptr)
	{
		c_ptr->_val._content._len = 0;
		_queue_mgr.enqueue (*c_ptr, _queue_sptr);
		_cell_ptr = nullptr;
	}

	_queue_mgr.flush_ret_queue (*_queue_sptr);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	RecD2d::write_disk_buffer ()
{
	int            ret_val = 0;

	if (_buf_disk_pos > 0)
	{
		if (_file_writer.is_open ())
		{
			ret_val = _file_writer.write_data (&_buf_disk [0], _buf_disk_pos);
		}
		_buf_disk_pos = 0;
	}

	return ret_val;
}



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
