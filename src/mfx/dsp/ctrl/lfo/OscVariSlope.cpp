/*****************************************************************************

        OscVariSlope.cpp
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

#include "mfx/dsp/ctrl/lfo/OscVariSlope.h"

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace ctrl
{
namespace lfo
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	OscVariSlope::do_set_sample_freq (double sample_freq) noexcept
{
	_phase_gen.set_sample_freq (sample_freq);
}



void	OscVariSlope::do_set_period (double per) noexcept
{
	_phase_gen.set_period (per);
}



void	OscVariSlope::do_set_phase (double phase) noexcept
{
	_phase_gen.set_phase (phase);
}



void	OscVariSlope::do_set_chaos (double chaos) noexcept
{
	_phase_gen.set_chaos (chaos);
}



void	OscVariSlope::do_set_phase_dist (double dist) noexcept
{
	_phase_dist.set_phase_dist (dist);
}



void	OscVariSlope::do_set_phase_dist_offset (double ofs) noexcept
{
	_phase_dist.set_phase_dist_offset (ofs);
}



void	OscVariSlope::do_set_sign (bool inv_flag) noexcept
{
	_inv_flag = inv_flag;
}



void	OscVariSlope::do_set_polarity (bool unipolar_flag) noexcept
{
	_unipolar_flag = unipolar_flag;
}



void	OscVariSlope::do_set_variation (int param, double val) noexcept
{
	switch (param)
	{
	case	Variation_TIME:
		_var_time = val;
		break;
	case	Variation_SHAPE:
		_var_shape = val;
		break;
	default:
		// Nothing
		break;
	}

	update_slopes ();
}



bool	OscVariSlope::do_is_using_variation (int param) const noexcept
{
	return (   param == Variation_TIME
	        || param == Variation_SHAPE);
}



void	OscVariSlope::do_tick (int nbr_spl) noexcept
{
	_phase_gen.tick (nbr_spl);
}



double	OscVariSlope::do_get_val () const noexcept
{
	const double	t   = _phase_dist.process_phase (_phase_gen.get_phase ());
	double			val =
		  (t < _var_time)
		? 1 - _slope_0 * t
		: _slope_1 * t + _base_1;

	if (_inv_flag)
	{
		val = -val;
	}
	if (_unipolar_flag)
	{
		val = (val + 1) * 0.5f;
	}

	return (val);
}



double	OscVariSlope::do_get_phase () const noexcept
{
	return _phase_gen.get_phase ();
}



void	OscVariSlope::do_clear_buffers () noexcept
{
	_phase_gen.clear_buffers ();
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	OscVariSlope::update_slopes () noexcept
{
	const double   height = 2 * (1 - _var_shape);
	const double   time_0 = std::max (1e-9,     _var_time);
	const double   time_1 = std::max (1e-9, 1 - _var_time);
	_slope_0 = height / time_0;
	_slope_1 = height / time_1;
	_base_1  = -1 - _slope_1 * _var_time;
}



}  // namespace lfo
}  // namespace ctrl
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
