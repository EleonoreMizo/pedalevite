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



void	DistSlewRateLim::set_sample_freq (double sample_freq) noexcept
{
	assert (sample_freq > 0);

	_sample_freq = float (sample_freq);
	_inv_fs      = float (1.0 / sample_freq);
	update_rate_p ();
	update_rate_n ();
}



void	DistSlewRateLim::clear_buffers () noexcept
{
	_limiter.clear_buffers ();
}



void	DistSlewRateLim::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept
{
	_limiter.process_block (dst_ptr, src_ptr, nbr_spl);
}



void	DistSlewRateLim::set_rate_limit (float rate_max_s) noexcept
{
	assert (rate_max_s > 0);

	set_rate_limit_pos (rate_max_s);
	set_rate_limit_neg (rate_max_s);
}



void	DistSlewRateLim::set_rate_limit_pos (float rate_max_s) noexcept
{
	_rate_max_p_s = rate_max_s;
	update_rate_p ();
}



void	DistSlewRateLim::set_rate_limit_neg (float rate_max_s) noexcept
{
	_rate_max_n_s = rate_max_s;
	update_rate_n ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DistSlewRateLim::update_rate_p () noexcept
{
	const auto     rate_max = _rate_max_p_s * _inv_fs;
	_limiter.set_max_rate_p (rate_max);
}



void	DistSlewRateLim::update_rate_n () noexcept
{
	const auto     rate_max = _rate_max_n_s * _inv_fs;
	_limiter.set_max_rate_n (rate_max);
}



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
