/*****************************************************************************

        DistoDspAttract.hpp
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_pi_dist2_DistoDspAttract_CODEHEADER_INCLUDED)
#define mfx_pi_dist2_DistoDspAttract_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/Approx.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace pi
{
namespace dist2
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



float	DistoDspAttract::process_sample (float x)
{
	const float    env_val = _env.process_sample (x) + 1e-5f;   // + small cst to avoid log(0) later

	attract (x, env_val);

	return _cur_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DistoDspAttract::attract (float x, float env_val)
{
	assert (env_val > 0);

	if (_mad_flag)
	{
		const float    dest = _lvl_b * _sign;
		const float    dist = dest - _cur_val;
		const float    f    = std::copysign (1.0f / 16, dist);

		_speed += f * _ratio_f;
		const float    speed_limit = fstb::limit (
			fstb::Approx::log2 (env_val) * 0.25f - 1,
			1.0f / 256,
			1.0f / 16
		);
		assert (speed_limit > 0);
		_speed = fstb::limit (_speed, -speed_limit, speed_limit);
		_cur_val += _speed * _ratio_f;

		if (_cur_val * _sign > _lvl_b)
		{
			_sign = -_sign;
		}

		if (   fabs (x) < fabs (_cur_val)
		    && fabs (_cur_val) < _lvl_b)
		{
			_mad_flag = false;
		}
	}

	else
	{
		if (fabs (x) < 1e-3f)
		{
			_cur_val = x;
		}
		else
		{
			const float    amount   = 1 - 1 / (fabs (x) * 4 + 1);

			const Polarity	pol      = val_to_pol (x);
			const float    attract_point = _lvl_a [pol];
			const float    dist_a   = attract_point - _cur_val;
			const float    comp_a   = dist_a * amount;

			const float    dist_x   = x - _cur_val;
			const float    comp_x   = dist_x * (1 - amount);

			const float    comp_all = comp_x + comp_a;

			const float    step     = comp_all * _ratio_f;

			_cur_val += step;

			if (fabs (x) >= _lvl_b)
			{
				_mad_flag = true;
				_sign     = (_cur_val < 0) ? 1.0f : -1.0f;
				_speed    = 0;
			}
		}
	}
}



DistoDspAttract::Polarity	DistoDspAttract::val_to_pol (float val)
{
	return ((val < 0) ? Polarity_NEG : Polarity_POS);
}



float	DistoDspAttract::pol_to_val (Polarity polarity)
{
	assert (polarity >= 0);
	assert (polarity < Polarity_NBR_ELT);

	return (polarity != Polarity_POS) ? -1.f : 1.f;
}



}  // namespace dist2
}  // namespace pi
}  // namespace mfx



#endif   // mfx_pi_dist2_DistoDspAttract_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
