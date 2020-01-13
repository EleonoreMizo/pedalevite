/*****************************************************************************

        FreqAmGha.cpp
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

#include "fstb/def.h"
#include "fstb/fnc.h"
#include "mfx/dsp/ana/FreqAmGha.h"
#include "mfx/dsp/iir/TransSZBilin.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace ana
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	FreqAmGha::set_sample_freq (double sample_freq)
{
	assert (sample_freq > 0);

	_sample_freq = float (sample_freq);
	_inv_fs      = float (1.0 / sample_freq);
	update_lpf ();
	clear_buffers ();
}



void	FreqAmGha::set_freq_bot (float f)
{
	assert (f > 0);

	_freq_bot = f;
}



void	FreqAmGha::set_freq_top (float f)
{
	assert (f > 0);

	_freq_top = f;
	update_lpf ();
}



void	FreqAmGha::set_smoothing (float responsiveness, float thr)
{
	assert (responsiveness > 0);
	assert (responsiveness <= 1);
	assert (thr >= 0);

	_freq.set_responsiveness (responsiveness);
	_freq.set_threshold (thr);
}



void	FreqAmGha::clear_buffers ()
{
	_lpf_in.clear_buffers ();
	reset ();
}



float	FreqAmGha::process_block (const float spl_ptr [], int nbr_spl)
{
	assert (spl_ptr != nullptr);
	assert (nbr_spl > 0);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		process_sample (spl_ptr [pos]);
	}

	return _freq.get_val ();
}



float	FreqAmGha::process_sample (float x)
{
	_val_old = _val_cur;
	_val_cur = _lpf_in.process_sample (x);

	// Positive zero-crossing
	if (_val_old < 0 && _val_cur > 0)
	{
		_slope_cur = _val_cur - _val_old;

		// Equal slopes
		if (fstb::is_eq (_slope_cur, _slope_max, _slope_tol))
		{
			ZeroCross &    zc = _zc_arr [_index];

			const float    t_frac = compute_zc_time (_val_cur, _val_old);

			// Records new data and reset time
			zc._slope = _slope_cur;
			zc._timer = _time_cur + t_frac;
			_time_cur = -t_frac;

			// Mew max slope, just reset
			if (_index == 0)
			{
				_nbr_match_fail = 0;
				++ _index;
			}

			// If timer duration and slopes match
			else if (
				   fstb::is_eq (_zc_arr [0]._timer, zc._timer , _timer_tol)
			   && fstb::is_eq (_zc_arr [0]._slope, _slope_cur, _slope_tol)
			)
			{
				// Sums timer values
				float          period = 0;
				for (int i = 0; i < _index; ++i)
				{
					period += _zc_arr [i]._timer;
				}

				const float    freq = _sample_freq / period;
				if (freq >= _freq_bot && freq <= _freq_top)
				{
					_freq.proc_val (freq);
				}

				// Resets new zero index values to compare with
				_zc_arr [0]     = zc;
				_index          = 1;
				_nbr_match_fail = 0;
			}

			// Crossing midpoint but no match
			else
			{
				++ _index;
				if (_index >= _data_size)
				{
					reset ();
				}
			}
 		}

		// New slope is much higher than the registered max slope
		else if (_slope_cur > _slope_max)
		{
			const float    t_frac = compute_zc_time (_val_cur, _val_old);
			_slope_max      = _slope_cur;
			_time_cur       = -t_frac;
			_nbr_match_fail = 0;
			_index          = 0;
		}

		// Slope not steep enough
		else
		{
			++ _nbr_match_fail;
			if (_nbr_match_fail >= _max_fails)
			{
				reset ();
			}
		}
	}

	++ _time_cur;

	return _freq.get_val ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	FreqAmGha::reset ()
{
	_index          = 0;
	_nbr_match_fail = 0;
	_slope_max      = 0;
	_freq.reset ();
}



void	FreqAmGha::update_lpf ()
{
	if (_sample_freq > 0)
	{
		const float   b_s [3] = { 1,                   0, 0 };
		const float   a_s [3] = { 1, float (fstb::SQRT2), 1 };
		const float   k =
			iir::TransSZBilin::compute_k_approx (_freq_top * _inv_fs);
		float         a_z [3];
		float         b_z [3];
		iir::TransSZBilin::map_s_to_z_approx (b_z, a_z, b_s, a_s, k);
		_lpf_in.set_z_eq (b_z, a_z);
	}
}



// Estimates the fractional sample position of the zero-crossing.
float	FreqAmGha::compute_zc_time (float cur, float old)
{
	assert (cur >= 0);
	assert (old <= 0);
	assert (old < cur);

	return old / (old - cur);
}



}  // namespace ana
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
