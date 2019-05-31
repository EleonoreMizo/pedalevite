/*****************************************************************************

        Voice.cpp
        Author: Laurent de Soras, 2019

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

#include "fstb/fnc.h"
#include "mfx/pi/distpwm2/Voice.h"

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace pi
{
namespace distpwm2
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Voice::set_pulse_type (PulseType type)
{
	assert (type >= 0);
	assert (type < PulseType_NBR_ELT);

	_pt = type;
}



// pw in samples
void	Voice::set_pulse_width (float pw)
{
	assert (pw > 1);

	_pw     = pw;
	_pw_inv = 1.0f / pw;
}



// age_frac: age of the exact sync position in sample, relative to the
// current processed sample
void	Voice::sync (float age_frac)
{
	assert (age_frac >= 0);
	assert (age_frac < 1);

	_dur_cycle = age_frac;
}



float	Voice::process_sample ()
{
	float          val;

	switch (_pt)
	{
	case PulseType_RECT:
		{
			val = _lvl;
			if (_dur_cycle < 1)
			{
				val = -_lvl + 2 * _lvl * _dur_cycle;
			}
			const float    dif = _dur_cycle - _pw;
			if (dif >= 0)
			{
				if (dif <= 1)
				{
					val += (-_lvl - val) * dif;
					_dur_cycle = 1e10f;
				}
				else
				{
					val = -_lvl;
				}
			}
		}
		break;

	case PulseType_RAMP:
		{
			val = -_lvl;
			const float    pos = _dur_cycle * _pw_inv;
			if (pos < 2)
			{
				val += pos * _lvl;
			}
			else
			{
				const float    dif = _dur_cycle - _pw;
				if (dif <= 1)
				{
					val = _lvl + (-_lvl - val) * dif;
					_dur_cycle = 1e10f;
				}
			}
		}
		break;

	case PulseType_CYCLE:
		{
			val = 0;
			const float    pos = _dur_cycle * _pw_inv;
			if (pos < 2)
			{
				val = gen_wf (pos) * _lvl;
			}
			else
			{
				_dur_cycle = 1e10f;
			}
		}
		break;

	case PulseType_SINE:
		{
			val = 0;
			const float    pos = _dur_cycle * _pw_inv;
			const float    amp = std::max (1 - pos * 0.02f, 0.f);
			val = gen_wf_multi (pos) * _lvl * amp;
		}
		break;

	case PulseType_SAW:
		{
			float          pos   = _dur_cycle * _pw_inv * 0.5f;
			const float    amp = std::max (1 - pos * 0.04f, 0.f);
			const int      pos_i = fstb::floor_int (pos);
			pos -= pos_i;
			val = (pos * 2 * _lvl - _lvl) * amp;
		}
		break;

	default:
		assert (false);
		break;
	}

	_dur_cycle += 1;

	return val;
}



void	Voice::clear_buffers ()
{
	_dur_cycle = 1e10f;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



float	Voice::gen_wf (float pos)
{
	assert (pos >= 0);
	assert (pos <= 2);

	float          val;
	if (pos < 0.5f)
	{
		val = gen_poly (pos * 2);
	}
	else if (pos < 1.5f)
	{
		val = 2 * gen_poly (1.5f - pos) - 1;
	}
	else
	{
		val = gen_poly (pos * 2 - 3) - 1;
	}

	return val;
}



float	Voice::gen_wf_multi (float pos)
{
	assert (pos >= 0);

	float          val;
	if (pos < 0.5f)
	{
		val = gen_poly (pos * 2);
	}
	else
	{
		pos -= 0.5f;
		const int   pos_int = fstb::floor_int (pos);
		pos -= pos_int & ~1;
		if ((pos_int & 1) == 0)
		{
			val = 2 * gen_poly (1 - pos) - 1;
		}
		else
		{
			val = 2 * gen_poly (pos - 1) - 1;
		}
	}

	return val;
}



float	Voice::gen_poly (float x)
{
	float          a  = -15 + 6 * x;
	float          b  =      10 * x;
	const float    x2 = x * x;

	return x2 * (b + x2 * a);
}



}  // namespace distpwm2
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
