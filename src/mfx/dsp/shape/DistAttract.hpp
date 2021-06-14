/*****************************************************************************

        DistAttract.hpp
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_shape_DistAttract_CODEHEADER_INCLUDED)
#define mfx_dsp_shape_DistAttract_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/Approx.h"

#include <algorithm>

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace shape
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



float	DistAttract::process_sample (float x) noexcept
{
	const float    env_val = _env.process_sample (x) + 1e-5f;   // + small cst to avoid log(0) later

	attract (x, env_val);

	return _cur_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DistAttract::attract (float x, float env_val) noexcept
{
	assert (env_val > 0);

	if (_mad_flag)
	{
		const float    dest = _lvl_b * _sign;
		const float    dist = dest - _cur_val;
		const float    f    = std::copysign (1.0f / 16, dist);

		_speed += f * _ratio_f;
		const float    speed_limit = std::max (
			fstb::Approx::log2 (env_val) * 0.25f - 1,
			_speed_lim_min
		);
		assert (speed_limit > 0);
		_speed = fstb::limit (_speed, -speed_limit, speed_limit);
		_cur_val += _speed * _ratio_f;

		if (_cur_val * _sign > _lvl_b)
		{
			_sign = -_sign;
		}

		if (   fabsf (x) < fabsf (_cur_val)
		    && fabsf (_cur_val) < _lvl_b)
		{
			_mad_flag = false;
		}
	}

	else
	{
		if (fabsf (x) < 1e-3f)
		{
			_cur_val = x;
		}
		else
		{
			const float    amount   = 1 - 1 / (fabsf (x) * 4 + 1);

			const Polarity	pol      = val_to_pol (x);
			const float    attract_point = _lvl_a [pol];
			const float    dist_a   = attract_point - _cur_val;
			const float    comp_a   = dist_a * amount;

			const float    dist_x   = x - _cur_val;
			const float    comp_x   = dist_x * (1 - amount);

			const float    comp_all = comp_x + comp_a;

			const float    step     = comp_all * _ratio_f;

			_cur_val += step;

			if (fabsf (x) >= _lvl_b)
			{
				_mad_flag = true;
				_sign     = (_cur_val < 0) ? 1.0f : -1.0f;
				_speed    = 0;
			}
		}
	}
}



constexpr DistAttract::Polarity	DistAttract::val_to_pol (float val) noexcept
{
	return ((val < 0) ? Polarity_NEG : Polarity_POS);
}



constexpr float	DistAttract::pol_to_val (Polarity polarity) noexcept
{
#if __cplusplus >= 201402L
	assert (polarity >= 0);
	assert (polarity < Polarity_NBR_ELT);
#endif

	return (polarity != Polarity_POS) ? -1.f : 1.f;
}



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_shape_DistAttract_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
