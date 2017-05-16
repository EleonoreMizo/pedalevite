/*****************************************************************************

        Compander.cpp
        Author: Laurent de Soras, 2017

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

#include "mfx/dsp/dyn/Compander.h"
#include "mfx/dsp/dyn/EnvHelper.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace dyn
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Compander::set_sample_freq (double sample_freq)
{
	assert (sample_freq > 0);

	_sample_freq = sample_freq;
	update_coef ();
}



void	Compander::set_type (Type type)
{
	assert (type >= 0);
	assert (type < Type_NBR_ELT);

	_type = type;
}



void	Compander::set_time (float t)
{
	assert (t > 0);

	_avg_time = t;
	update_coef ();
}



void	Compander::set_threshold_lvl (float thr)
{
	assert (thr > 0);

	_lvl_thr = thr;
}



void	Compander::clear_buffers ()
{
	_state = 0;
}



float	Compander::process_sample (float x)
{
	assert (_sample_freq > 0);

	float          y = x;

	if (_state == Type_EXP)
	{
		if (_state >= _lvl_thr)
		{
			y /= _state;
		}
		set_new_state (y);
	}

	else
	{
		set_new_state (x);
		y *= _state;
	}

	return y;
}



void	Compander::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (_sample_freq > 0);
	assert (dst_ptr != 0);
	assert (src_ptr != 0);
	assert (nbr_spl > 0);

	if (_state == Type_EXP)
	{
		for (int pos = 0; pos < nbr_spl; ++pos)
		{
			float          y = src_ptr [pos];
			if (_state >= _lvl_thr)
			{
				y /= _state;
			}
			dst_ptr [pos] = y;
			set_new_state (y);
		}
	}

	else
	{
		for (int pos = 0; pos < nbr_spl; ++pos)
		{
			const float    x = src_ptr [pos];
			float          y = x;
			set_new_state (x);
			y *= _state;
			dst_ptr [pos] = y;
		}
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Compander::update_coef ()
{
	if (_sample_freq > 0)
	{
		_coef = float (EnvHelper::compute_env_coef_simple (
			_avg_time,
			_sample_freq
		));
	}
}



void	Compander::set_new_state (float x)
{
	const float    xa = fabs (x);
	if (xa < _lvl_thr)
	{
		_state = 0;
	}
	else
	{
		_state += (xa - _state) * _coef;
	}
}



}  // namespace dyn
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
