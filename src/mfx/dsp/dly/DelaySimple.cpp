/*****************************************************************************

        DelaySimple.cpp
        Author: Laurent de Soras, 2019

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

#include "fstb/fnc.h"
#include "mfx/dsp/dly/DelaySimple.h"
#include "mfx/dsp/dly/RingBufVectorizer.h"

#include <cassert>
#include <cstring>



namespace mfx
{
namespace dsp
{
namespace dly
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DelaySimple::setup (int max_dly, int max_block_len)
{
	assert (max_dly >= 0);
	assert (max_block_len > 0);

	_max_dly       = max_dly;
	_max_block_len = max_block_len;

	update_buf ();
}



void	DelaySimple::set_delay (int d)
{
	assert (d >= 0);
	assert (d <= _max_dly);

	_dly = d;
}



// Can work in-place
void	DelaySimple::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (dst_ptr != 0);
	assert (src_ptr != 0);
	assert (nbr_spl > 0);
	assert (nbr_spl <= _max_block_len);

	int            pos     = 0;
	float * const  buf_ptr = _buf.data ();
	RingBufVectorizer rbv (_len);

	for (rbv.start (nbr_spl, _pos_w, delay (_pos_w))
	;	rbv.end ()
	;	rbv.next ())
	{
		const int      work_len = rbv.get_seg_len ();
		const int      pos_w    = rbv.get_curs_pos (0);
		const int      pos_r    = rbv.get_curs_pos (1);

		memcpy (buf_ptr + pos_w, src_ptr + pos, work_len * sizeof (*buf_ptr));
		memcpy (dst_ptr + pos, buf_ptr + pos_r, work_len * sizeof (*buf_ptr));

		pos += work_len;
	}

	_pos_w = rbv.get_curs_pos (0);
}



void	DelaySimple::clear_buffers ()
{
	memset (_buf.data (), 0, _buf.size () * sizeof (_buf [0]));
	_pos_w = 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DelaySimple::update_buf ()
{
	const int      len_min = _max_block_len + _max_dly;
	_len    = 1 << fstb::get_next_pow_2 (len_min);
	_mask   = _len - 1;
	_pos_w &= _mask;
	_buf.resize (_len);
}



}  // namespace dly
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
