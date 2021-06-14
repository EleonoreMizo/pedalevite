/*****************************************************************************

        MeterRmsPeakHold.cpp
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

#include "fstb/fnc.h"
#include "mfx/dsp/dyn/EnvHelper.h"
#include "mfx/dsp/dyn/MeterRmsPeakHold.h"

#include <algorithm>

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace dyn
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



MeterRmsPeakHold::MeterRmsPeakHold () noexcept
:	_hold_time_s (2.0)
,	_attack_time_s (0.025)
,	_release_time_s (0.250)
,	_sample_freq (0)
,	_peak_max (0)
,	_peak_hold (0)
,	_hold_counter (0)
,	_rms_sq (0)
,	_hold_time (0)
,	_coef_r (0)
,	_coef_r2 (0)
,	_coef_a2 (0)
{
	set_sample_freq (44100);
}



void	MeterRmsPeakHold::set_sample_freq (double freq) noexcept
{
	assert (freq > 0);

	_sample_freq = freq;
	update_times ();
}



void	MeterRmsPeakHold::set_hold_time_s (double t) noexcept
{
	assert (t > 0);

	_hold_time_s = t;
	update_times ();
}



void	MeterRmsPeakHold::set_attack_time_s (double t) noexcept
{
	assert (t > 0);

	_attack_time_s = t;
	update_times ();
}



void	MeterRmsPeakHold::set_release_time_s (double t) noexcept
{
	assert (t > 0);

	_release_time_s = t;
	update_times ();
}



void	MeterRmsPeakHold::clear_buffers () noexcept
{
	_peak_max     = 0;
	_peak_hold    = 0;
	_hold_counter = 0;
	_rms_sq       = 0;
}



void	MeterRmsPeakHold::process_block (const float data_ptr [], int nbr_spl) noexcept
{
	assert (data_ptr != nullptr);
	assert (nbr_spl > 0);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		const float    x   = data_ptr [pos];
		process_sample_internal (x);
	}

	fix_tiny_values ();
}



void	MeterRmsPeakHold::process_sample (float x) noexcept
{
	process_sample_internal (x);
	fix_tiny_values ();
}



void	MeterRmsPeakHold::skip_block (int nbr_spl) noexcept
{
	assert (nbr_spl > 0);

	int            rem_len  = nbr_spl;
	const int      hold_len = std::min (rem_len, _hold_counter);
	if (hold_len > 0)
	{
		_hold_counter -= hold_len;
		rem_len       -= hold_len;
	}
	if (_hold_counter <= 0 && rem_len > 0)
	{
		const double   cumulated_coef = pow (1 - _coef_r, rem_len);
		_peak_hold *= cumulated_coef;
	}

	const double   cumulated_coef = pow (1 - _coef_r2, nbr_spl);
	_rms_sq *= cumulated_coef;

	fix_tiny_values ();
}



double	MeterRmsPeakHold::get_peak () const noexcept
{
	return _peak_max;
}



double	MeterRmsPeakHold::get_peak_hold () const noexcept
{
	return _peak_hold;
}



double	MeterRmsPeakHold::get_rms () const noexcept
{
	return sqrt (_rms_sq);
}



void	MeterRmsPeakHold::clear_peak () noexcept
{
	_peak_max = 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	MeterRmsPeakHold::update_times () noexcept
{
	_hold_time  = fstb::ceil_int (_sample_freq * _hold_time_s);
	_coef_r     =
		EnvHelper::compute_env_coef_simple (_release_time_s, _sample_freq);
	_coef_r2   =
		EnvHelper::compute_env_coef_simple (_release_time_s, _sample_freq * 0.5);
	_coef_a2    =
		EnvHelper::compute_env_coef_simple (_attack_time_s, _sample_freq * 0.5);
}



void	MeterRmsPeakHold::process_sample_internal (float x) noexcept
{
	const float    x_a = fabsf (x);
	const float    x_2 = x * x;

	// Peak
	if (x_a > _peak_max)
	{
		_peak_max     = x_a;
	}

	if (x_a > _peak_hold)
	{
		_peak_hold    = x_a;
		_hold_counter = _hold_time;
	}
	else
	{
		if (_hold_counter > 0)
		{
			-- _hold_counter;
		}
		else
		{
			_peak_hold *= 1 - _coef_r;
		}
	}

	// RMS
	const double   delta = x_2 - _rms_sq;
	const double   coef  = (delta > 0) ? _coef_a2 : _coef_r2;
	_rms_sq += delta * coef;
}



// Avoids entering the denormal zone with silent input.
void	MeterRmsPeakHold::fix_tiny_values () noexcept
{
	const double   threshold = 1e-10;   // About -200 dB

	if (_rms_sq < threshold * threshold)
	{
		_rms_sq = 0;
	}

	if (_hold_counter == 0 && _peak_hold < threshold)
	{
		_peak_hold = 0;
	}
}



}  // namespace dyn
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
