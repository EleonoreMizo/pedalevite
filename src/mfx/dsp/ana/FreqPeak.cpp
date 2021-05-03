/*****************************************************************************

        FreqPeak.cpp
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
#include "mfx/dsp/ana/FreqPeak.h"
#include "mfx/dsp/iir/TransSZBilin.h"

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace ana
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	FreqPeak::set_sample_freq (double sample_freq)
{
	assert (sample_freq > 0);

	_sample_freq = float (sample_freq);
	_inv_fs      = float (1.0 / sample_freq);
	for (auto &pol : _pol_arr)
	{
		pol._env.set_sample_freq (sample_freq);
		pol._env.set_times (0, 5e-3f);
	}
	update_input_filter ();
	clear_buffers ();
}



void	FreqPeak::set_freq_bot (float f) noexcept
{
	assert (f > 0);

	_freq_bot = f;
	for (auto &freq : _freq_raw_arr)
	{
		freq = std::max (freq, f);
	}
	update_input_filter ();
}



void	FreqPeak::set_freq_top (float f) noexcept
{
	assert (f > 0);

	_freq_top = f;
	for (auto &freq : _freq_raw_arr)
	{
		freq = std::min (freq, f);
	}
	update_input_filter ();
}



void	FreqPeak::set_smoothing (float responsiveness, float thr) noexcept
{
	assert (responsiveness > 0);
	assert (responsiveness <= 1);
	assert (thr >= 0);

	_freq_smooth.set_responsiveness (responsiveness);
	_freq_smooth.set_threshold (thr);
}



void	FreqPeak::set_threshold (float thr) noexcept
{
	assert (thr >= 0);

	_threshold = thr;
}



void	FreqPeak::clear_buffers () noexcept
{
	_hpf_in.clear_buffers ();
	_lpf_in.clear_buffers ();
	_freq_smooth.clear_buffers ();
	for (auto &pol : _pol_arr)
	{
		pol._env.clear_buffers ();
		pol._mem_x.fill (0);
		pol._mem_y.fill (0);
		pol._peak_age = 1e9;
	}
	_freq_raw_arr.fill ((_freq_bot + _freq_top) * 0.5f);
	_freq_buf_pos = 0;
	_polarity     = 0;
}



float	FreqPeak::process_block (const float spl_ptr [], int nbr_spl) noexcept
{
	assert (spl_ptr != nullptr);
	assert (nbr_spl > 0);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		process_sample (spl_ptr [pos]);
	}

	return _freq_smooth.get_val ();
}



float	FreqPeak::process_sample (float x) noexcept
{
	x = _hpf_in.process_sample (x);
	x = _lpf_in.process_sample (x);

	// Bipolar envelope detection
	const std::array <float, 2>   x_pol = {{
		std::max (+x, 0.0f),
		std::max (-x, 0.0f)
	}};
	for (int pol_cnt = 0; pol_cnt < 2; ++pol_cnt)
	{
		Polarity &     pol = _pol_arr [pol_cnt];

		pol._mem_x [0] = pol._mem_x [1];
		pol._mem_x [1] = pol._mem_x [2];
		pol._mem_x [2] = x_pol [pol_cnt];

		pol._mem_y [0] = pol._mem_y [1];
		pol._mem_y [1] = pol._mem_y [2];
		pol._mem_y [2] = pol._env.process_sample (x_pol [pol_cnt]);

		pol._peak_age += 1;
	}

	// A peak is detected in the expected polarity
	Polarity &     pol_det = _pol_arr [_polarity];
	if (   pol_det._mem_y [1] > pol_det._mem_y [0]
	    && pol_det._mem_y [1] > pol_det._mem_y [2]
	    && pol_det._mem_y [1] > _threshold)
	{
		const float    r1 = pol_det._mem_x [0];
		const float    r2 = pol_det._mem_x [1];
		const float    r3 = pol_det._mem_x [2];

		// The peak is located between r1 and r3
		// Parabolic interpolation
		// frac is relative to the r2 position.
		const float    frac = (r1 - r3) * 0.5f / (r1 + r3 - 2 * r2);
		const float    dist = pol_det._peak_age + frac;
		assert (dist > 0);
		const float    freq = _sample_freq / dist;
		if (freq >= _freq_bot && freq <= _freq_top)
		{
			_freq_buf_pos = (_freq_buf_pos + 1) & _freq_buf_mask;
			_freq_raw_arr [_freq_buf_pos] = freq;

			// Use first a median filter to discard single dropouts or octave
			// issues
			const int      p  = _freq_buf_pos;
			const int      m  = _freq_buf_mask;
			const float    a0 = _freq_raw_arr [ p         ];
			const float    a1 = _freq_raw_arr [(p - 1) & m];
			const float    a2 = _freq_raw_arr [(p - 2) & m];
			const float    a3 = _freq_raw_arr [(p - 3) & m];
			const float    a4 = _freq_raw_arr [(p - 4) & m];
			const float    freq_med = compute_median5 (a0, a1, a2, a3, a4);

			// Frequency candidate accepted
			_freq_smooth.proc_val (freq_med);
		}

		pol_det._peak_age = -frac;
		_polarity         = 1 - _polarity;
	}

	// If the previous peak was detected too long ago, invalidates the detected
	// frequency.
	else if (pol_det._peak_age > 10 * _sample_freq / _freq_bot)
	{
		_freq_smooth.reset (0);
	}

	return _freq_smooth.get_val ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	FreqPeak::update_input_filter () noexcept
{
	if (_sample_freq > 0)
	{
		{
			// High-pass filter
			const float   b_s [2] = { 0, 1 };
			const float   a_s [2] = { 1, 1 };
			const float   k =
				iir::TransSZBilin::compute_k_approx (_freq_bot * 0.125f * _inv_fs);
			float         a_z [2];
			float         b_z [2];
			iir::TransSZBilin::map_s_to_z_one_pole_approx (b_z, a_z, b_s, a_s, k);
			_hpf_in.set_z_eq (b_z, a_z);
		}

		{
			// Low-pass filter
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
}



float	FreqPeak::compute_median3 (float a0, float a1, float a2) noexcept
{
	const float    mi = std::min (a0, a1);
	const float    ma = std::max (a0, a1);

	return std::max (mi, std::min (ma, a2));
}



float	FreqPeak::compute_median5 (float a0, float a1, float a2, float a3, float a4) noexcept
{
	sort_pair (a0, a1);
	sort_pair (a2, a3);
	if (a2 < a0)
	{
		std::swap (a1, a3);
		a2 = a0;
	}

	a0 = a4;
	sort_pair (a0, a1);

	if (a0 < a2)
	{
		std::swap (a1, a3);
		a0 = a2;
	}

	return std::min (a0, a3);
}



void	FreqPeak::sort_pair (float &a, float &b) noexcept
{
	if (b < a)
	{
		std::swap (a, b);
	}
}



}  // namespace ana
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
