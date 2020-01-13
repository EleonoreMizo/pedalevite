/*****************************************************************************

        DelayAllPassSimd.cpp
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
#include "mfx/pi/fv/DelayAllPassSimd.h"

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace pi
{
namespace fv
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DelayAllPassSimd::set_delay (int len)
{
	_delay_line.set_delay (len);
}



void	DelayAllPassSimd::set_feedback (float coef)
{
	assert (coef > -1);
	assert (coef < 1);

	_fdbk = coef;
}



void	DelayAllPassSimd::clear_buffers ()
{
	_delay_line.clear_buffers ();
}



void	DelayAllPassSimd::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	const auto     fdbk = fstb::ToolsSimd::set1_f32 (_fdbk);

	int            pos = 0;
	do
	{
		const int      max_work_len = _delay_line.get_max_rw_len ();
		const int      rem_len      = nbr_spl - pos;
		const int      work_len     = std::min (rem_len, max_work_len);

		const float *  r_ptr        = _delay_line.use_read_data ();
		float *        w_ptr        = _delay_line.use_write_data ();

		const int      n_4          = work_len & -4;

		for (int p2 = 0; p2 < n_4; p2 += 4)
		{
			const auto    buf = fstb::ToolsSimd::loadu_f32 (r_ptr   + p2);
			const auto    src = fstb::ToolsSimd::loadu_f32 (src_ptr + p2);
			const auto    dst = buf - src;
			const auto    dly = src + buf * fdbk;
			fstb::ToolsSimd::storeu_f32 (dst_ptr + p2, dst);
			fstb::ToolsSimd::storeu_f32 (w_ptr   + p2, dly);
		}

		for (int p2 = n_4; p2 < work_len; ++p2)
		{
			const auto    buf = r_ptr [p2];
			const auto    src = src_ptr [p2];
			const auto    dst = buf - src;
			const auto    dly = src + buf * _fdbk;
			dst_ptr [p2] = dst;
			w_ptr [p2] = dly;
		}

		_delay_line.step (work_len);
		pos     += work_len;
		src_ptr += work_len;
		dst_ptr += work_len;
	}
	while (pos < nbr_spl);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace fv
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
