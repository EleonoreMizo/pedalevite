/*****************************************************************************

        DelayComb.cpp
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

#include "fstb/ToolsSimd.h"
#include "mfx/pi/fv/DelayComb.h"

#include <algorithm>

#include <cassert>
#include <cstring>



namespace mfx
{
namespace pi
{
namespace fv
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DelayComb::set_delay (int len)
{
	_delay_line.set_delay (len);
}



void	DelayComb::set_feedback (float coef)
{
	assert (coef >= -1);
	assert (coef <= 1);

	_fdbk = coef;
	update_fdbkdamp ();
}



void	DelayComb::set_damp (float damp)
{
	assert (damp >= 0);
	assert (damp < 1);

	_damp = damp;
	update_fdbkdamp ();
}



void	DelayComb::clear_buffers ()
{
	_delay_line.clear_buffers ();
	_mem_y = 0;
}



// Can work in-place
void	DelayComb::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	float          mem_y = _mem_y;

	int            pos = 0;
	do
	{
		const int      max_work_len = _delay_line.get_max_rw_len ();
		const int      rem_len      = nbr_spl - pos;
		const int      work_len     = std::min (rem_len, max_work_len);

		const float *  r_ptr        = _delay_line.use_read_data ();
		float *        w_ptr        = _delay_line.use_write_data ();

		for (int p2 = 0; p2 < work_len; ++p2)
		{
			const float   buf = r_ptr [p2];
			const float   src = src_ptr [p2];
			mem_y = buf * _fdbkdamp + mem_y * _damp;
			dst_ptr [p2] = buf;
			w_ptr [p2]   = mem_y + src;
		}

		_delay_line.step (work_len);
		pos     += work_len;
		src_ptr += work_len;
		dst_ptr += work_len;
	}
	while (pos < nbr_spl);

	_mem_y = mem_y;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DelayComb::update_fdbkdamp ()
{
	_fdbkdamp = (1 - _damp) * _fdbk;
}



}  // namespace fv
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
