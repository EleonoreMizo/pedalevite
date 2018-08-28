/*****************************************************************************

        DistoDspBounce.hpp
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_pi_dist2_DistoDspBounce_CODEHEADER_INCLUDED)
#define mfx_pi_dist2_DistoDspBounce_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <algorithm>



namespace mfx
{
namespace pi
{
namespace dist2
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



float	DistoDspBounce::process_sample (float x)
{
	// Updates the current ball position according ot its speed
	_pos += _speed;

	float          speed_max_loc = _speed_max;

	// If the ball falls under the floor, makes it bounce
	if (_pos < x)
	{
		_pos = x;
		bounce (x, speed_max_loc);
	}

	// Also makes it bounce if it goes over the ceiling
	else
	{
		const float		tunnel_top = x + _tunnel_height;
		if (_pos > tunnel_top)
		{
			_pos = tunnel_top;
			bounce (x, speed_max_loc);
		}
	}

	// Speed is updated one sample too late with regard to the gravity.
	_speed -= _grav;  // Updates the ball speed according to the gravity
	_speed  = std::min (_speed, speed_max_loc);
	
	_prev_val = x;

	return _pos;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DistoDspBounce::bounce (float &val, float &speed_max_loc)
{
	// Computes the ball speed according to the bounce angle on the curve and
	// bounce rate.
	const float    slope = val - _prev_val;
	_speed = slope + (slope - _speed) * _bounce_rate;

	// Prevents ball to elevate with an exagerated velocity, which would get
	// it down very slowly (and would cut audio by saturation)
	speed_max_loc = std::max (slope, _speed_max);
}



}  // namespace dist2
}  // namespace pi
}  // namespace mfx



#endif   // mfx_pi_dist2_DistoDspBounce_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
