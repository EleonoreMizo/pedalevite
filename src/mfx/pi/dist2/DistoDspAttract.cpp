/*****************************************************************************

        DistoDspAttract.cpp
        Author: Laurent de Soras, 2018

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

#include "mfx/pi/dist2/DistoDspAttract.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace dist2
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



DistoDspAttract::DistoDspAttract ()
:	_sample_freq (44100)
,	_lvl_a ({{ 1, -1 }})
,	_lvl_b (2.f)
,	_center_a (0)
,	_mad_flag (false)
,	_sign (1)
,	_speed (0)
,	_ratio_f (1)
,	_cur_val (0)
,	_env ()
{
	_env.set_time (1.f);
}



void	DistoDspAttract::set_sample_freq (double sample_freq)
{
	assert (sample_freq > 0);

	_sample_freq = float (sample_freq);
	_env.set_sample_freq (sample_freq);
	_ratio_f = float (44100.f / sample_freq);
}



void	DistoDspAttract::clear_buffers ()
{
	_mad_flag = false;
	_cur_val = 0;
	_env.clear_buffers ();
}



void	DistoDspAttract::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (dst_ptr != 0);
	assert (src_ptr != 0);
	assert (nbr_spl > 0);

	static const int  max_block_size = 256;
	std::array <float, max_block_size>  buf_env;

	int            block_pos = 0;
	do
	{
		const int      block_len =
			std::min (nbr_spl - block_pos, int (max_block_size));

		_env.process_block (&buf_env [0], &src_ptr [block_pos], block_len);

		for (int pos = 0; pos < block_len; ++pos)
		{
			const float    x       = src_ptr [block_pos + pos];
			const float    env_val = buf_env [pos] + 1e-5f;   // + small cst to avoid log(0) later
			attract (x, env_val);
			dst_ptr [block_pos + pos] = _cur_val;
		}

		block_pos += block_len;
	}
	while (block_pos < nbr_spl);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace dist2
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
