/*****************************************************************************

        RampModule.cpp
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

#include "fstb/Approx.h"
#include "fstb/fnc.h"
#include "mfx/pi/ramp/RampModule.h"

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace pi
{
namespace ramp
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



RampModule::RampModule ()
:	_sample_freq (-1)
,	_delay (0)
,	_period (1)
,	_inv_flag (false)
,	_snh_ratio (0)
,	_smooth (0)
,	_type (CurveType_LINEAR)
,	_pos_delay (0)
,	_delay_spl (0)
,	_pos (0)
,	_step (0)
,	_delay_flag (true)
,	_snh_flag (false)
,	_smooth_flag (false)
,	_snh_pos (0)
,	_snh_step (0)
,	_snh_state (0)
,	_smooth_state (0)
,	_smooth_spl_coef (0)
{
	set_sample_freq (44100);

	set_snh (_snh_ratio);
	set_smooth (_smooth);
	set_phase (0);
}



void	RampModule::set_sample_freq (double sample_freq)
{
	_sample_freq = sample_freq;
	update_step ();
	update_delay ();
	update_snh ();
	update_smooth ();
}



void	RampModule::set_initial_delay (double t)
{
	assert (t >= 0);

	_delay = t;
	update_delay ();
}



void	RampModule::set_period (double per)
{
	assert (per > 0);

	_period = per;
	update_step ();
	update_smooth ();
	update_snh ();
}



void	RampModule::set_phase (double phase)
{
	assert (phase >= 0);
	assert (phase < 1);

	_delay_flag = (phase <= 0);
	_pos_delay  = 0;
	_pos        = phase;
	_snh_pos    = phase;
	_snh_state  = get_raw_val ();
}



void	RampModule::set_sign (bool inv_flag)
{
	_inv_flag = inv_flag;
}



void	RampModule::set_type (CurveType type)
{
	assert (type >= 0);
	assert (type < CurveType_NBR_ELT);

	_type = type;
}



void	RampModule::set_snh (double ratio)
{
	assert (ratio >= 0);

	_snh_ratio = ratio;
	update_snh ();
}



void	RampModule::set_smooth (double ratio)
{
	assert (ratio >= 0);

	_smooth = ratio;
	update_smooth ();
}



void	RampModule::tick (int nbr_spl)
{
	if (_delay_flag)
	{
		const int      work_len = std::min (_delay_spl - _pos_delay, nbr_spl);
		if (work_len > 0)
		{
			nbr_spl -= work_len;
			_pos_delay += work_len;
		}
		if (_pos_delay >= _delay_spl)
		{
			_delay_flag = false;
		}
	}

	if (nbr_spl > 0)
	{
		int            work_len = nbr_spl;
		if (_snh_flag)
		{
			// Compute the S&H position at the end of this block
			const double   end_snh_pos     = _snh_pos + _snh_step * nbr_spl;
			const int      end_snh_pos_int = fstb::floor_int (end_snh_pos);

			// If it is triggered at least once during the block
			if (end_snh_pos_int > 0)
			{
				const int      spl_before_last_snh =
					fstb::ceil_int ((end_snh_pos_int - _snh_pos) / _snh_step);
				work_len = spl_before_last_snh;
			}
		}
		tick_sub (work_len);

		const int      rem_len = nbr_spl - work_len;
		if (rem_len > 0)
		{
			tick_sub (rem_len);
		}
	}
}



double	RampModule::get_val () const
{
	// Source: smooth, S&H or direct
	double         val =
		  (_smooth_flag) ? _smooth_state
		: (_snh_flag)    ? _snh_state
		:                  get_raw_val ();

	if (_inv_flag)
	{
		val = 1 - val;
	}

	return val;
}



double	RampModule::get_phase () const
{
	return _pos;
}



void	RampModule::clear_buffers ()
{
	const float    raw_val = get_raw_val ();
	_snh_state    = raw_val;
	_smooth_state = raw_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	RampModule::update_step ()
{
	_step = 1.0 / (_period * _sample_freq);
}



void	RampModule::update_delay ()
{
	_delay_spl = fstb::round_int (_delay * _sample_freq);
}



void	RampModule::update_snh ()
{
	if (_snh_ratio <= 1e-6)
	{
		_snh_step = 0;
		_snh_flag = false;
	}

	else
	{
		// Turn it on : initialize the state
		if (! _snh_flag)
		{
			_snh_state = get_raw_val ();
		}

		_snh_step = 1.0 / (_period * _snh_ratio * _sample_freq);
		_snh_flag = true;
	}
}



void	RampModule::update_smooth ()
{
	if (_smooth <= 0)
	{
		_smooth_spl_coef = 0;
		_smooth_flag = false;
	}

	else
	{
		// Turn it on : initialize the state
		if (! _smooth_flag)
		{
			_smooth_state = _snh_flag ? _snh_state : get_raw_val ();
		}

		// exp (-4) ~= 0.018, this means we reach 98 % of the final amplitude
		// after t_spl.
		const float		t_spl = float (_period * _smooth * _sample_freq);
//		_smooth_spl_coef = exp (-4.0f / t_spl);
      // +0.1f to avoid too low numbers at the input of fast_exp2
		_smooth_spl_coef = fstb::Approx::exp2 (-5.75f / (t_spl + 0.1f));
		_smooth_flag = true;
	}
}



void	RampModule::tick_sub (int nbr_spl)
{
	assert (nbr_spl > 0);

	if (_pos < 1)
	{
		_pos = std::min (_pos + _step * nbr_spl, 1.0);
	}

	// We process smoothing before S&H because any S&H event is supposed
	// to occur at the very end of the block. Therefore we operate with
	// the previous block value. For non-S&H source timing may be erroneous
	// but we can't do much better without sample per sample processing.
	if (_smooth_flag)
	{
		const double   src = _snh_flag ? _snh_state : get_raw_val ();
		const double   k   = fstb::ipowp (_smooth_spl_coef, nbr_spl);
		const double   a   = 1 - k;
		_smooth_state += a * (src - _smooth_state);
	}

	if (_snh_flag)
	{
		_snh_pos += _snh_step * nbr_spl;
		if (_snh_pos >= 1)
		{
			_snh_pos   -= fstb::floor_int (_snh_pos);
			_snh_state  = get_raw_val ();
		}
	}
}



float	RampModule::get_raw_val () const
{
	float          v = float (_pos);

	switch (_type)
	{
	case CurveType_LINEAR:
		// Nothing
		break;
	case CurveType_ACC_1:
		v = accelerate <4, 2> (v);
		break;
	case CurveType_ACC_2:
		v = v * v;
		break;
	case CurveType_ACC_3:
		v = accelerate <4, 8> (v);
		break;
	case CurveType_ACC_4:
		v = accelerate <8, 8> (v);
		break;
	case CurveType_SAT_1:
		v = 1 - accelerate <4, 2> (1 - v);
		break;
	case CurveType_SAT_2:
		v = (2 - v) * v;
		break;
	case CurveType_SAT_3:
		v = 1 - accelerate <4, 8> (1 - v);
		break;
	case CurveType_SAT_4:
		v = 1 - accelerate <8, 8> (1 - v);
		break;
	case CurveType_FAST_1:
		v = fast (v);
		break;
	case CurveType_FAST_2:
		v = v * 0.125f + 0.875f * fast (fast (v));
		break;
	case CurveType_SLOW_1:
		v = slow (v);
		break;
	case CurveType_SLOW_2:
		v = slow (slow (v));
		break;
	default:
		assert (false);
		break;
	}

	return v;
}



}  // namespace ramp
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
