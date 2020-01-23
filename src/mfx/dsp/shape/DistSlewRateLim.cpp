/*****************************************************************************

        DistSlewRateLim.cpp
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

#include "mfx/dsp/shape/DistSlewRateLim.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace shape
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DistSlewRateLim::set_sample_freq (double sample_freq)
{
	assert (sample_freq > 0);

	_sample_freq = float (sample_freq);
	_inv_fs      = float (1.0 / sample_freq);
	update_rate ();
}



void	DistSlewRateLim::clear_buffers ()
{
	_state = 0;
}



void	DistSlewRateLim::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	float          state = _state;

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		const float    x       = src_ptr [pos];
		const float    val_min = state - _rate_max;
		const float    val_max = state + _rate_max;
		state = fstb::limit (x, val_min, val_max);
		dst_ptr [pos] = state;
	}

	_state = state;
}



void	DistSlewRateLim::set_rate_limit (float rate_max_s)
{
	assert (rate_max_s > 0);

	_rate_max_s = rate_max_s;
	update_rate ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DistSlewRateLim::update_rate ()
{
	_rate_max = _rate_max_s * _inv_fs;
}



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
