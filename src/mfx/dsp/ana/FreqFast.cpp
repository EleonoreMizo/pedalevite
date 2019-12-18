/*****************************************************************************

        FreqFast.cpp
        Author: Laurent de Soras, 2018

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

#include "fstb/ToolsSimd.h"
#include "mfx/dsp/ana/FreqFast.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace ana
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	FreqFast::set_sample_freq (double sample_freq)
{
	assert (sample_freq > 0);

	_sample_freq = float (      sample_freq);
	_inv_fs      = float (1.0 / sample_freq);

	_dc_killer.set_sample_freq (sample_freq);
	_env_autogain.set_sample_freq (sample_freq);
	_env_peak_pos.set_sample_freq (sample_freq);
	_env_peak_neg.set_sample_freq (sample_freq);

	_env_autogain.set_times (1.000e-3f, 0.100f);
	_env_peak_pos.set_times (0.010e-3f, 0.001f);
	_env_peak_neg.set_times (0.010e-3f, 0.001f);

	for (auto &buf : _buf_arr)
	{
		buf.resize (_max_block_size);
	}
}



void	FreqFast::set_freq_bot (float f)
{
	assert (f > 0);

	_freq_bot = f;
}



void	FreqFast::set_freq_top (float f)
{
	assert (f > 0);

	_freq_top = f;
}



void	FreqFast::clear_buffers ()
{
	_freq = 0;
	_dc_killer.clear_buffers ();
	_env_autogain.clear_buffers ();
	_env_peak_pos.clear_buffers ();
	_env_peak_neg.clear_buffers ();
	_pp_flag  = false;
	_last_pp  = 0;
	_last_pn  = 0;
	_time_ref = 0;
	_time_p1  = 0;
	_per_prev = 0;
	_per_avg  = 0;
}



float	FreqFast::process_block (const float spl_ptr [], int nbr_spl)
{
	assert (spl_ptr != 0);
	assert (nbr_spl > 0);

	int            pos_blk = 0;
	do
	{
		const int      work_len = std::min (
			int (_max_block_size),
			nbr_spl - pos_blk
		);

		// DC murdering
		_dc_killer.process_block (
			&_buf_arr [0] [0],
			spl_ptr + pos_blk,
			work_len
		);

		// Autogain + noise gate
		proc_autogain (work_len);

		// Twin peak detection
		proc_peaks (work_len);

		// Finds the frequency
		find_freq (work_len);

		pos_blk += work_len;
		_time_ref += work_len;
	}
	while (pos_blk < nbr_spl);

	return _freq;
}



float	FreqFast::process_sample (float x)
{
	// DC murdering
	x = _dc_killer.process_sample (x);

	// Autogain + noise gate
	const float    env  = _env_autogain.process_sample (x);
	const float    a2   = _lvl_gate * _lvl_gate * _lvl_gate * 0.5f;
	const float    env2 = env * env;
	const float    gain = env2 / (env2 * env + a2);
	x *= gain;

	// Twin peak detection
	const float    xp = (x > +_deadzone) ? x : 0;
	const float    xn = (x < -_deadzone) ? x : 0;
	const float    ep = _env_peak_pos.process_sample (xp);
	const float    en = _env_peak_pos.process_sample (xn);
	const float    yp = (xp >= ep *  _peak_thr) ? 1.f : 0;
	const float    yn = (xn <= en * -_peak_thr) ? 1.f : 0;

	// Finds the frequency
	if (_pp_flag)
	{
		if (yn > 0 && _last_pn <= 0)
		{
			_pp_flag = false;
		}
	}
	else
	{
		if (yp > 0 && _last_pp <= 0)
		{
			_pp_flag = true;
			process_freq (0);
		}
	}

	_last_pp = yp;
	_last_pn = yn;

	++ _time_ref;

	return _freq;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// Autogain + gate formula:
// g = (env^n) / ((env^(n+1)) + a^n)
// with n >= 1. Works well with n = 2
// Gain returns to about 1 at a, but the real gate level is higher.
// Derivative: (env^(n-1) * (n * a^n - env^(n+1))) / ((env^(n+1) + a^n)^2)
// Gain is maximum at: (n * a^n) ^ (1 / (n+1))
// Gate parameter:
// a^n = (gate_level ^ (n+1)) / n
void	FreqFast::proc_autogain (int nbr_spl)
{
	assert (nbr_spl > 0);
	assert (nbr_spl <= _max_block_size);

	_env_autogain.process_block (
		&_buf_arr [1] [0],
		&_buf_arr [0] [0],
		nbr_spl
	);

	const float    a2 = _lvl_gate * _lvl_gate * _lvl_gate * 0.5f;

#if 1
	const auto     a2_v = fstb::ToolsSimd::set1_f32 (a2);
	for (int pos = 0; pos < nbr_spl; pos += 4)
	{
		auto           val  = fstb::ToolsSimd::load_f32 (&_buf_arr [0] [pos]);
		const auto     env  = fstb::ToolsSimd::load_f32 (&_buf_arr [1] [pos]);
		const auto     env2 = env * env;
		const auto     den  = env2 * env + a2_v;
		const auto     gain = env2 * fstb::ToolsSimd::rcp_approx2 (den);
		val *= gain;
		fstb::ToolsSimd::store_f32 (&_buf_arr [2] [pos], val);
	}
#else
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		const float    env  = _buf_arr [1] [pos];
		const float    env2 = env * env;
		const float    gain = env2 / (env2 * env + a2);
		_buf_arr [2] [pos] = _buf_arr [0] [pos] * gain;
	}
#endif
}



void	FreqFast::proc_peaks (int nbr_spl)
{
	assert (nbr_spl > 0);
	assert (nbr_spl <= _max_block_size);

	// Separates positive and negative peaks
#if 1
	const auto     dz_p = fstb::ToolsSimd::set1_f32 (+_deadzone);
	const auto     dz_n = fstb::ToolsSimd::set1_f32 (-_deadzone);
	for (int pos = 0; pos < nbr_spl; pos += 4)
	{
		const auto     x        = fstb::ToolsSimd::load_f32 (&_buf_arr [2] [pos]);
		const auto     x_gt_dzp = fstb::ToolsSimd::cmp_gt_f32 (x, dz_p);
		const auto     x_lt_dzn = fstb::ToolsSimd::cmp_lt_f32 (x, dz_n);
		const auto     xp       = fstb::ToolsSimd::and_f32 (x, x_gt_dzp);
		const auto     xn       = fstb::ToolsSimd::and_f32 (x, x_lt_dzn);
		fstb::ToolsSimd::store_f32 (&_buf_arr [0] [pos], xp);
		fstb::ToolsSimd::store_f32 (&_buf_arr [1] [pos], xn);
	}
#else
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		const float    x  = _buf_arr [2] [pos];
		const float    xp = (x > +_deadzone) ? x : 0;
		const float    xn = (x < -_deadzone) ? x : 0;
		_buf_arr [0] [pos] = xp;
		_buf_arr [1] [pos] = xn;
	}
#endif

	// Envelopes
	_env_peak_pos.process_block (&_buf_arr [2] [0], &_buf_arr [0] [0], nbr_spl);
	_env_peak_pos.process_block (&_buf_arr [3] [0], &_buf_arr [1] [0], nbr_spl);

	// Peak detection
#if 1
	const auto     thr_p = fstb::ToolsSimd::set1_f32 (+_peak_thr);
	const auto     thr_n = fstb::ToolsSimd::set1_f32 (-_peak_thr);
	const auto     one   = fstb::ToolsSimd::set1_f32 (1.0f);
	for (int pos = 0; pos < nbr_spl; pos += 4)
	{
		const auto     xp = fstb::ToolsSimd::load_f32 (&_buf_arr [0] [pos]);
		const auto     xn = fstb::ToolsSimd::load_f32 (&_buf_arr [1] [pos]);
		auto           ep = fstb::ToolsSimd::load_f32 (&_buf_arr [2] [pos]);
		auto           en = fstb::ToolsSimd::load_f32 (&_buf_arr [3] [pos]);
		ep *= thr_p;
		en *= thr_n;
		const auto     xp_gt_ep = fstb::ToolsSimd::cmp_gt_f32 (xp, ep);
		const auto     xn_lt_en = fstb::ToolsSimd::cmp_lt_f32 (xn, en);
		const auto     yp       = fstb::ToolsSimd::and_f32 (one, xp_gt_ep);
		const auto     yn       = fstb::ToolsSimd::and_f32 (one, xn_lt_en);
		fstb::ToolsSimd::store_f32 (&_buf_arr [2] [pos], yp);
		fstb::ToolsSimd::store_f32 (&_buf_arr [3] [pos], yn);
	}
#else
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		const float    xp = _buf_arr [0] [pos];
		const float    xn = _buf_arr [1] [pos];
		const float    ep = _buf_arr [2] [pos];
		const float    en = _buf_arr [3] [pos];
		const float    yp = (xp >= ep *  _peak_thr) ? 1.f : 0;
		const float    yn = (xn <= en * -_peak_thr) ? 1.f : 0;

		_buf_arr [2] [pos] = yp;
		_buf_arr [3] [pos] = yn;
	}
#endif
}



void	FreqFast::find_freq (int nbr_spl)
{
	assert (nbr_spl > 0);
	assert (nbr_spl <= _max_block_size);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		const float    xp = _buf_arr [2] [pos];
		const float    xn = _buf_arr [3] [pos];

		// Positive peak passed, check the next negative peak
		if (_pp_flag)
		{
			if (xn > 0 && _last_pn <= 0)
			{
				_pp_flag = false;

				// Frequency processing
				process_freq (pos);
			}
		}

		// Negative peak passed, check the next positive peak
		else
		{
			if (xp > 0 && _last_pp <= 0)
			{
				_pp_flag = true;
			}
		}

		_last_pp = xp;
		_last_pn = xn;
	}
}



void	FreqFast::process_freq (int ts_rel)
{
	const int32_t  time_p0 = _time_ref + ts_rel;
	const float    per     = float (time_p0 - _time_p1);

	if (_per_avg <= 0 || is_period_similar (per, _per_avg))
	{
		validate_period (per);
	}
	else if (_per_prev <= 0)
	{
		_per_prev = per;
	}
	else
	{
		const float    per_2x = per + _per_prev;
		if (is_period_similar (per_2x, _per_avg))
		{
			validate_period (per_2x);
		}
		else
		{
			validate_period (per_2x * 0.5f);
		}
	}

	_time_p1 = time_p0;
	fix_timestamps ();
}



void	FreqFast::validate_period (float per)
{
	assert (per > 0);

	if (_per_avg <= 0)
	{
		_per_avg = per;
	}
	else
	{
		_per_avg += _smoothing * (per - _per_avg);
	}

	if (   _per_avg * _freq_bot * _inv_fs > 1
	    || _per_avg * _freq_top * _inv_fs < 1)
	{
		_freq = 0;
	}
	else
	{
		_freq = _sample_freq / _per_avg;
	}

	_per_prev = 0;
}



void	FreqFast::fix_timestamps ()
{
	const int32_t  lim = INT32_MAX / 2;
	if (_time_p1 > lim)
	{
		_time_ref -= lim;
		_time_p1  -= lim;
	}
}



bool	FreqFast::is_period_similar (float p1, float p2)
{
	assert (p1 >= 0);
	assert (p2 >= 0);

	const float    thr = 0.06f;    // 6% = 1/2 tone

	return (fabs (p1 - p2) < (thr * 0.5f) * (p1 + p2));
}



}  // namespace ana
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
