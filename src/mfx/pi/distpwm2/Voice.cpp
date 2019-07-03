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
#include "mfx/dsp/mix/Generic.h"
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



Voice::Voice ()
:	_lvl (0.125f)
,	_pt (PulseType_RECT)
,	_pw (100)
,	_pw_inv (1 / _pw)
,	_dur_cycle (0)
{
	dsp::mix::Generic::setup ();
}



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
					_dur_cycle = 1e6f;
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
					_dur_cycle = 1e6f;
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
				_dur_cycle = 1e6f;
			}
		}
		break;

	case PulseType_SINE:
		{
			const float    t_scale  = 1.f / _fade_len;
			const float    pos      = _dur_cycle * _pw_inv;
			const float    amp      = std::max (1 - pos * t_scale, 0.f);
			val = gen_wf_multi (pos) * _lvl * amp;
		}
		break;

	case PulseType_SAW:
		{
			const float    t_scale  = 2.f / _fade_len;
			float          pos      = _dur_cycle * _pw_inv * 0.5f;
			const float    amp      = std::max (1 - pos * t_scale, 0.f);
			const int      pos_i    = fstb::floor_int (pos);
			pos -= pos_i;
			val  = (pos * 2 * _lvl - _lvl) * amp;
		}
		break;

	default:
		assert (false);
		val = 0; // Set val to keep the compiler happy
		break;
	}

	_dur_cycle += 1;

	return val;
}



void	Voice::process_block (float dst_ptr [], int nbr_spl)
{
	assert (dst_ptr != 0);
	assert (nbr_spl > 0);

	switch (_pt)
	{
	case PulseType_RECT:  process_block_rect ( dst_ptr, nbr_spl); break;
	case PulseType_RAMP:	 process_block_ramp ( dst_ptr, nbr_spl); break;
	case PulseType_CYCLE: process_block_cycle (dst_ptr, nbr_spl); break;
	case PulseType_SINE:	 process_block_sine ( dst_ptr, nbr_spl); break;
	case PulseType_SAW:	 process_block_saw (  dst_ptr, nbr_spl); break;
	default:
		assert (false);
		break;
	}
}



void	Voice::clear_buffers ()
{
	_dur_cycle = 1e6f;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Voice::process_block_rect (float dst_ptr [], int nbr_spl)
{
	int            pos = 0;

	// Rising edge and possible immediate falling edge in the same sample
	if (_dur_cycle < 1)
	{
		float          val = -_lvl + 2 * _lvl * _dur_cycle;
		const float    dif = _dur_cycle - _pw;
		if (dif >= 0 && dif <= 1)
		{
			val += (-_lvl - val) * dif;
		}
		dst_ptr [pos] = val;
		++ pos;
		_dur_cycle += 1;
	}

	while (pos < nbr_spl)
	{
		const float    dif = _dur_cycle - _pw;

		// Positive part
		if (dif < 0)
		{
			int            work_len = -fstb::floor_int (dif);
			work_len = std::min (work_len, nbr_spl - pos);
			dsp::mix::Generic::fill (dst_ptr + pos, work_len, _lvl);
			pos += work_len;
			_dur_cycle += work_len;
		}

		// Falling edge
		else if (dif <= 1)
		{
			dst_ptr [pos] = _lvl * (1 - 2 * dif);
			++ pos;
			_dur_cycle = 1e6f;
		}

		// Negative part
		else
		{
			const int      work_len = nbr_spl - pos;
			dsp::mix::Generic::fill (dst_ptr + pos, work_len, -_lvl);
			pos += work_len;
			_dur_cycle += work_len;
		}
	}
}



void	Voice::process_block_ramp (float dst_ptr [], int nbr_spl)
{
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		float          val = -_lvl;
		const float    p_c = _dur_cycle * _pw_inv;
		if (p_c < 2)
		{
			val += p_c * _lvl;
		}
		else
		{
			const float    dif = _dur_cycle - _pw;
			if (dif <= 1)
			{
				val = _lvl + (-_lvl - val) * dif;
				_dur_cycle = 1e6f;
			}
		}
		dst_ptr [pos] = val;

		_dur_cycle += 1;
	}
}



void	Voice::process_block_cycle (float dst_ptr [], int nbr_spl)
{
	int            pos = 0;
	do
	{
		int            work_len = nbr_spl - pos;
		const float    dif = _dur_cycle - 2 * _pw;
		if (dif >= 0)
		{
			dsp::mix::Generic::clear (dst_ptr + pos, work_len);
			_dur_cycle = 1e6f;
		}
		else
		{
			const int      rem_cycle = -fstb::floor_int (dif);
			work_len = std::min (work_len, rem_cycle);
			float          p_c      = _dur_cycle * _pw_inv;
			const int      pos_stop = pos + work_len;
			for (int p2 = pos; p2 < pos_stop; ++p2)
			{
				if (p_c >= 2)
				{
					dst_ptr [p2] = 0;
				}
				else
				{
					const float    val = gen_wf (p_c) * _lvl;
					dst_ptr [p2] = val;
					p_c += _pw_inv;
				}
			}
			_dur_cycle += work_len;
		}

		pos += work_len;
	}
	while (pos < nbr_spl);
}



void	Voice::process_block_sine (float dst_ptr [], int nbr_spl)
{
	const float    t_scale  = 1.f / _fade_len;
	float          p_c      = _dur_cycle * _pw_inv;
	float          amp      = _lvl * std::max (1 - p_c * t_scale, 0.f);
	const float    amp_step = _lvl * _pw_inv * t_scale;
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		const float    val = gen_wf_multi (p_c) * amp;
		dst_ptr [pos] = val;
		p_c += _pw_inv;
		amp  = std::max (amp - amp_step, 0.f);
	}
	_dur_cycle += nbr_spl;
}



void	Voice::process_block_saw (float dst_ptr [], int nbr_spl)
{
	const float    t_scale  = 2.f / _fade_len;
	const float    p_c_step = _pw_inv * 0.5f;
	float          p_c      = _dur_cycle * p_c_step;
	float          amp      = _lvl * std::max (1 - p_c * t_scale, 0.f);
	const float    amp_step = _lvl * p_c_step * t_scale;
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		const float    p_c_f = p_c - fstb::floor_int (p_c);
		const float    val   = (p_c_f * 2 - 1) * amp;
		dst_ptr [pos] = val;
		p_c += p_c_step;
		amp  = std::max (amp - amp_step, 0.f);
	}
	_dur_cycle += nbr_spl;
}



// https://www.desmos.com/calculator/sxx6c9eha2
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
