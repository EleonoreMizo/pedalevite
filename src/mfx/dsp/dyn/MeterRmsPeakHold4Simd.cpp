/*****************************************************************************

        MeterRmsPeakHold4Simd.cpp
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

#include "fstb/DataAlign.h"
#include "fstb/fnc.h"
#include "fstb/ToolsSimd.h"
#include "mfx/dsp/dyn/EnvHelper.h"
#include "mfx/dsp/dyn/MeterRmsPeakHold4Simd.h"

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



MeterRmsPeakHold4Simd::MeterRmsPeakHold4Simd () noexcept
:	_hold_time_s (2.0)
,	_attack_time_s (0.025)
,	_release_time_s (0.250)
,	_sample_freq (0)
,	_peak_max ()
,	_peak_hold ()
,	_hold_counter ()
,	_rms_sq ()
,	_hold_time (0)
,	_coef_r (0)
,	_coef_r4x (0)
,	_coef_r2 (0)
,	_coef_a2 (0)
{
	set_sample_freq (44100);
	clear_buffers ();
}



void	MeterRmsPeakHold4Simd::set_sample_freq (double freq) noexcept
{
	assert (freq > 0);

	_sample_freq = freq;
	update_times ();
}



void	MeterRmsPeakHold4Simd::set_hold_time_s (double t) noexcept
{
	assert (t > 0);

	_hold_time_s = t;
	update_times ();
}



void	MeterRmsPeakHold4Simd::set_attack_time_s (double t) noexcept
{
	assert (t > 0);

	_attack_time_s = t;
	update_times ();
}



void	MeterRmsPeakHold4Simd::set_release_time_s (double t) noexcept
{
	assert (t > 0);

	_release_time_s = t;
	update_times ();
}



void	MeterRmsPeakHold4Simd::clear_buffers () noexcept
{
	fstb::Vf32::zero ().store (&_peak_max    );
	fstb::Vf32::zero ().store (&_peak_hold   );
	fstb::Vf32::zero ().store (&_hold_counter);
	fstb::Vf32::zero ().store (&_rms_sq      );
}



void	MeterRmsPeakHold4Simd::process_block (const float * const data_ptr [4], int nbr_spl) noexcept
{
	assert (data_ptr != nullptr);
	assert (fstb::DataAlign <true>::check_ptr (data_ptr [0]));
	assert (fstb::DataAlign <true>::check_ptr (data_ptr [1]));
	assert (fstb::DataAlign <true>::check_ptr (data_ptr [2]));
	assert (fstb::DataAlign <true>::check_ptr (data_ptr [3]));
	assert (nbr_spl > 0);

	auto           peak_max  = fstb::Vf32::load (&_peak_max);
	auto           peak_hold = fstb::Vf32::load (&_peak_hold);
	auto           hold_cnt  = fstb::Vf32::load (&_hold_counter);
	auto           rms_sq    = fstb::Vf32::load (&_rms_sq);

	const int      n4 = nbr_spl & -4;
	const int      n3 = nbr_spl &  3;
	for (int pos = 0; pos < n4; pos += 4)
	{
		auto           v0 = fstb::Vf32::loadu (data_ptr [0] + pos);
		auto           v1 = fstb::Vf32::loadu (data_ptr [1] + pos);
		auto           v2 = fstb::Vf32::loadu (data_ptr [2] + pos);
		auto           v3 = fstb::Vf32::loadu (data_ptr [3] + pos);
		fstb::ToolsSimd::transpose_f32 (v0, v1, v2, v3);

		auto           vm = fstb::max (
			fstb::max (fstb::abs (v0), fstb::abs (v1)),
			fstb::max (fstb::abs (v2), fstb::abs (v3))
		);
		process_sample_peak (vm, peak_max, peak_hold, hold_cnt, _coef_r4x, 4);

		process_sample_rms (v0, rms_sq);
		process_sample_rms (v1, rms_sq);
		process_sample_rms (v2, rms_sq);
		process_sample_rms (v3, rms_sq);
	}
	if (n3 > 0)
	{
		auto           v0 = fstb::Vf32::loadu (data_ptr [0] + n4);
		auto           v1 = fstb::Vf32::loadu (data_ptr [1] + n4);
		auto           v2 = fstb::Vf32::loadu (data_ptr [2] + n4);
		auto           v3 = fstb::Vf32::loadu (data_ptr [3] + n4);
		fstb::ToolsSimd::transpose_f32 (v0, v1, v2, v3);

		process_sample_peak (
			fstb::abs (v0), peak_max, peak_hold, hold_cnt, _coef_r, 1
		);
		process_sample_rms (v0, rms_sq);
		if (n3 > 1)
		{
			process_sample_peak (
				fstb::abs (v1), peak_max, peak_hold, hold_cnt, _coef_r, 1
			);
			process_sample_rms (v1, rms_sq);
		}
		if (n3 > 2)
		{
			process_sample_peak (
				fstb::abs (v2), peak_max, peak_hold, hold_cnt, _coef_r, 1
			);
			process_sample_rms (v2, rms_sq);
		}
	}

	peak_max .store (&_peak_max    );
	peak_hold.store (&_peak_hold   );
	hold_cnt .store (&_hold_counter);
	rms_sq   .store (&_rms_sq      );
}



void	MeterRmsPeakHold4Simd::process_sample (fstb::Vf32 x) noexcept
{
	auto           peak_max  = fstb::Vf32::load (&_peak_max);
	auto           peak_hold = fstb::Vf32::load (&_peak_hold);
	auto           hold_cnt  = fstb::Vf32::load (&_hold_counter);
	auto           rms_sq    = fstb::Vf32::load (&_rms_sq);

	const auto     x_a       = fstb::abs (x);
	process_sample_peak (x_a, peak_max, peak_hold, hold_cnt, _coef_r, 1);
	process_sample_rms (x, rms_sq);

	peak_max .store (&_peak_max    );
	peak_hold.store (&_peak_hold   );
	hold_cnt .store (&_hold_counter);
	rms_sq   .store (&_rms_sq      );
}



fstb::Vf32	MeterRmsPeakHold4Simd::get_peak () const noexcept
{
	return fstb::Vf32::load (&_peak_max);
}



fstb::Vf32	MeterRmsPeakHold4Simd::get_peak_hold () const noexcept
{
	return fstb::Vf32::load (&_peak_hold);
}



fstb::Vf32	MeterRmsPeakHold4Simd::get_rms () const noexcept
{
	const auto     rms_sq = fstb::Vf32::load (&_rms_sq);

	return rms_sq.sqrt_approx ();
}



void	MeterRmsPeakHold4Simd::clear_peak () noexcept
{
	 fstb::Vf32::zero ().store (&_peak_max);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	MeterRmsPeakHold4Simd::update_times () noexcept
{
	_hold_time = fstb::ceil_int (_sample_freq * _hold_time_s);
	_coef_r    = float (
		EnvHelper::compute_env_coef_simple (_release_time_s, _sample_freq)
	);
	_coef_r4x  = float (
		EnvHelper::compute_env_coef_simple (_release_time_s, _sample_freq * 0.25)
	);
	_coef_r2   = float (
		EnvHelper::compute_env_coef_simple (_release_time_s, _sample_freq * 0.5)
	);
	_coef_a2   = float (
		EnvHelper::compute_env_coef_simple (_attack_time_s, _sample_freq * 0.5)
	);
}



void	MeterRmsPeakHold4Simd::process_sample_peak (fstb::Vf32 x_a, fstb::Vf32 &peak_max, fstb::Vf32 &peak_hold, fstb::Vf32 &hold_cnt, float coef_r_flt, int step_int) const noexcept
{
	const auto     hold_time = fstb::Vf32 (float (_hold_time));
	const auto     coef_r    = fstb::Vf32 (coef_r_flt);
	const auto     zero      = fstb::Vf32::zero ();
	const auto     one       = fstb::Vf32 (1);
	const auto     step      = fstb::Vf32 (float (step_int));

	// Peak
	peak_max  = fstb::max (x_a, peak_max);

	const auto     xa_gt_ph = (x_a > peak_hold);
	const auto     hc_gt_0  = (hold_cnt > zero);
	const auto     ph_cond  = xa_gt_ph | hc_gt_0;

	const auto     hcm1     = fstb::select (hc_gt_0, hold_cnt - step, hold_cnt);
	hold_cnt  = fstb::select (xa_gt_ph, hold_time, hcm1);

	const auto     coef     = fstb::select (ph_cond, zero, coef_r);
	peak_hold = fstb::max (x_a, peak_hold);
	peak_hold *= one - coef;
}



void	MeterRmsPeakHold4Simd::process_sample_rms (fstb::Vf32 x, fstb::Vf32 &rms_sq) const noexcept
{
	const auto     coef_r2   = fstb::Vf32 (_coef_r2);
	const auto     coef_a2   = fstb::Vf32 (_coef_a2);

	// RMS
	const auto     x_2      = x * x;
	const auto     delta    = x_2 - _rms_sq;
	const auto     del_lt_0 = delta.is_lt_0 ();
	const auto     coef     = fstb::select (del_lt_0, coef_r2, coef_a2);
	rms_sq.mac (delta, coef);
}



}  // namespace dyn
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
