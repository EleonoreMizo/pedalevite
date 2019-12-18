/*****************************************************************************

        DManual.cpp
        Author: Laurent de Soras, 2017

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

#include "mfx/adrv/CbInterface.h"
#include "mfx/adrv/DManual.h"

#include <cassert>



namespace mfx
{
namespace adrv
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DManual::get_buffers (float * in_ptr_arr [_nbr_chn], const float * out_ptr_arr [_nbr_chn])
{
	assert (_state >= State_INIT);
	assert (in_ptr_arr != 0);
	assert (out_ptr_arr != 0);

	for (int chn = 0; chn < _nbr_chn; ++chn)
	{
		in_ptr_arr [chn]  = &_chn_buf_arr [Dir_IN ] [chn] [0];
		out_ptr_arr [chn] = &_chn_buf_arr [Dir_OUT] [chn] [0];
	}
}



void	DManual::process_block ()
{
	assert (_state == State_RUN);

	const float *  src_ptr_arr [_nbr_chn];
	float *        dst_ptr_arr [_nbr_chn];
	for (int chn = 0; chn < _nbr_chn; ++chn)
	{
		src_ptr_arr [chn] = &_chn_buf_arr [Dir_IN ] [chn] [0];
		dst_ptr_arr [chn] = &_chn_buf_arr [Dir_OUT] [chn] [0];
	}

	_cb_ptr->process_block (dst_ptr_arr, src_ptr_arr, _max_block_size);

	_sample_index += _max_block_size;
}



size_t	DManual::get_sample_index () const
{
	assert (_state >= State_INIT);

	return _sample_index;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	DManual::do_init (double &sample_freq, int &max_block_size, CbInterface &callback, const char *driver_0, int chn_idx_in, int chn_idx_out)
{
	fstb::unused (driver_0, chn_idx_in, chn_idx_out);
	assert (_state == State_LOADED);

	sample_freq     = 44100;
	max_block_size  = 64;

	_sample_freq    = sample_freq;
	_max_block_size = max_block_size;
	_cb_ptr         = &callback;
	_sample_index   = 0;

	const int      mbs_align = (max_block_size + 3) & -4;
	for (int chn = 0; chn < _nbr_chn; ++chn)
	{
		for (int dir = 0; dir < Dir_NBR_ELT; ++dir)
		{
			_chn_buf_arr [dir] [chn].clear ();
			_chn_buf_arr [dir] [chn].resize (mbs_align, 0);
		}
	}

	_state = State_INIT;

	return 0;
}



int	DManual::do_start ()
{
	assert (_state == State_INIT);

	_state = State_RUN;

	return 0;
}



int	DManual::do_stop ()
{
	assert (_state == State_RUN);

	_state = State_INIT;

	return 0;
}



void	DManual::do_restart ()
{
	assert (_state == State_RUN);
}



std::string	DManual::do_get_last_error () const
{
	return "";
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace adrv
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
