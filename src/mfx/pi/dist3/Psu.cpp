/*****************************************************************************

        Psu.cpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/Approx.h"
#include "fstb/fnc.h"
#include "mfx/dsp/dyn/EnvHelper.h"
#include "mfx/pi/dist3/Psu.h"

#include <algorithm>

#include <cassert>
#include <cmath>



namespace mfx
{
namespace pi
{
namespace dist3
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Psu::set_sample_freq (double sample_freq)
{
	assert (sample_freq > 0);

	_sample_freq = float (sample_freq);
	_voltage_p.set_sample_freq (sample_freq);
	_voltage_n.set_sample_freq (sample_freq);
	update_constants ();
}



void	Psu::set_half_cycle_relative_lvl (float lvl)
{
	assert (lvl > 0);
	assert (lvl < 1);

	_ratio = lvl;
	update_constants ();
}



void	Psu::set_ac_freq (float freq)
{
	assert (freq > 0);

	_ac_freq = freq;
	update_constants ();
}



void	Psu::set_ac_lvl (float lvl)
{
	assert (lvl > 0);

	_ac_lvl = lvl;
}



// Can work in-place
void	Psu::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (_sample_freq > 0);
	assert (_ratio > 0);
	assert (_ac_freq > 0);
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		// AC generation
		const float    ph_mul = 1.f / float (1ULL << 32);
		const float    ph_nrm = _phase * ph_mul;
		const float    ac_val = fstb::Approx::sin_nick_2pi (ph_nrm) * _ac_lvl;
		const float    ac_abs = fabs (ac_val);
		_phase += _step;

		float          x = src_ptr [pos];
		process_polarity (_voltage_p, _charge_p, x, ac_abs);
		x = -x;
		process_polarity (_voltage_n, _charge_n, x, ac_abs);
		x = -x;
		dst_ptr [pos] = x;
	}
}



void	Psu::clear_buffers ()
{
	_voltage_p.clear_buffers ();
	_voltage_n.clear_buffers ();
	_charge_p  = 0;
	_charge_n  = 0;
	_phase     = 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Psu::Smoother::set_sample_freq (double sample_freq)
{
	if (_order > 0)
	{
		double         t1 = 100.0e-6; // 100 us
		const double   tn = dsp::dyn::EnvHelper::compensate_order (t1, _order);
		_coef = float (
			dsp::dyn::EnvHelper::compute_env_coef_simple (tn, sample_freq)
		);
	}
}



float	Psu::Smoother::process_sample (float x)
{
	if (x < _state_arr [_order])
	{
		_state_arr.fill (x);
	}
	else
	{
		_state_arr [0] = x;
		for (int ord = 0; ord < _order; ++ord)
		{
			float          y = _state_arr [ord + 1];
			y += (x - y) * _coef;
			_state_arr [ord + 1] = y;
			x = y;
		}
	}

	return x;
}



float	Psu::Smoother::get_val () const
{
	return _state_arr [_order];
}



void	Psu::Smoother::clear_buffers ()
{
	_state_arr.fill (0);
}



/*
Equations:

charge [n+1] = charge[n] - voltage[n]
with:
voltage [n] = charge[n] / C

charge [n+1] = charge[n] - charge[n] / C
charge [n+1] = charge[n] * (1 - 1 / C)
charge [n+1] / charge[n] = 1 - 1 / C

charge [n] = charge [0] * (1 - 1 / C) ^ n

_capa is unknown here:
_ratio = (1 - 1 / _capa) ^ h_cycle_len
_ratio = exp (ln (1 - 1 / _capa) * h_cycle_len)
ln (_ratio) = ln (1 - 1 / _capa) * h_cycle_len
ln (_ratio) / h_cycle_len = ln (1 - 1 / _capa)
exp (ln (_ratio) / h_cycle_len) = 1 - 1 / _capa
1 / _capa = 1 - exp (ln (_ratio) / h_cycle_len)
_capa = 1 / (1 - exp (ln (_ratio) / h_cycle_len))
*/

void	Psu::update_constants ()
{
	if (_sample_freq > 0 && _ratio > 0 && _ac_freq > 0)
	{
		const float    hlen   = 0.5f * _sample_freq / _ac_freq;
		_capa_inv = 1 - exp (log (_ratio) / hlen);
		_capa     = 1 / _capa_inv;

		// Oscillator step for the AC voltage
		const float    ph_mul = float (1ULL << 32);
		_step = int32_t (fstb::round_int (ph_mul * _ac_freq / _sample_freq));
	}
}



// Works only on positive values of x
void	Psu::process_polarity (Smoother &voltage, float &charge, float &x, float ac)
{
	assert (voltage.get_val () >= 0);
	assert (charge >= 0);
	assert (ac >= 0);

	float          v = voltage.get_val ();

	// Capacitor charge
	if (ac > v)
	{
		// Assumes unlimited current
		v = voltage.process_sample (ac);
		charge = v * _capa;

		// Voltage limiting (equivalent to current limiting)
		x = clip (x, v);
	}

	else
	{
		// Voltage limiting (equivalent to current limiting)
		x = clip (x, v);

		// Capacitor discharge
		if (x > 0)
		{
			charge -= x;
			voltage.process_sample (charge * _capa_inv);
		}
	}
}



constexpr float	Psu::clip (float x, float vmax)
{
#if 0
	x = std::min (x, vmax);
#else
	// Soft clipping, reduces the introduction of the highest harmonics.
	const float    knee = 1.f / 4;
	const float    thr0 = vmax * (1 - knee);
	if (x > thr0)
	{
		const float    thr1 = vmax * (1 + knee);
		if (x >= thr1)
		{
			x = vmax;
		}
		else
		{
			const float    kv = knee * vmax;
			x -= thr0;
			x /= kv;
			x = 1 - x * 0.5f;
			x *= x;
			x = 1 - x;
			x *= kv;
			x += thr0;
		}
	}
#endif

	return x;
}



}  // namespace dist3
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
