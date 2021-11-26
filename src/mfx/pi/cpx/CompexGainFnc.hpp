/*****************************************************************************

        CompexGainFnc.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_pi_cpx_CompexGainFnc_CODEHEADER_INCLUDED)
#define mfx_pi_cpx_CompexGainFnc_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/Approx.h"
#include "fstb/fnc.h"
#include "fstb/ToolsSimd.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace cpx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// env_2l2 = 2 * log2 (linear volume envelope).
// An arbitrary negative value corresponds to zero.
// Returns a linear gain
template <bool store_flag>
float	CompexGainFnc::compute_gain (float env_2l2)
{
	// Additional gain (manual + auto)
	const float    gain_l2 = compute_gain_l2 <store_flag> (env_2l2);

	// Conversion to linear, multiplicative volume
	const float    gain = fstb::Approx::exp2 (gain_l2);

	return gain;
}



// env_2l2 = 2 * log2 (linear volume envelope).
// An arbitrary negative value corresponds to zero.
// Returns log2 (gain)
template <bool store_flag>
float	CompexGainFnc::compute_gain_l2 (float env_2l2)
{
	// Shifts volume in order to have the threshold at 0
	const float    env_l2 = env_2l2 * 0.5f;
	const float    el2    = env_l2 - _vol_offset_pre;

	// Knee polynomial
	const float    c2   = _knee_coef_arr [2];
	const float    c1   = _knee_coef_arr [1];
	const float    c0   = _knee_coef_arr [0];
	const float    poly = fstb::Poly::horner (el2, c0, c1, c2);

	// Linear ratios
	const float    ratio  = (el2 < 0) ? _ratio_lo : _ratio_hi;
	const float    linear = el2 * ratio;

	// Selects result
	const float    vl2 = (fabsf (el2) < _knee_th_abs) ? poly : linear;

	// Computes and limits gain
	float          gain_l2 = fstb::limit (vl2 - el2, _gain_min_l2, _gain_max_l2);
	// Another solution would be mirroring before minimising:
	// gain_top_l2 - abs (gain_l2 - gain_top_l2) with gain_top_l2 > gain_max_l2.
	// Quickest solution but may generate very low signals turning later into denormals.

	// Handles the case where detected volume is close to 0 (-oo dB).
	// Smoothly fades the gain to 0 dB.
	// g = g * limit ((v - v0) / (v1 - v0), 0, 1)
	auto           active_rate   = (env_l2 - _active_thr_l2) * _active_mul;
	active_rate  = fstb::limit (active_rate, 0.f, 1.f);
	gain_l2     *= active_rate;

	// Stores the gain adjustment if requested
	if (store_flag)
	{
		_cur_gain = gain_l2;
	}

	// Additional gain (manual + auto)
	gain_l2 += _vol_offset_post;

	return gain_l2;
}



// env_2l2 = 2 * log2 (linear volume envelope).
// An arbitrary negative value corresponds to zero.
// Returns a linear gain
template <bool store_flag>
fstb::Vf32	CompexGainFnc::compute_gain (const fstb::Vf32 env_2l2)
{
	// Shifts volume in order to have the threshold at 0
	const auto     half = fstb::Vf32 (0.5f);
	const auto     env_l2 = env_2l2 * half;
	const auto     vol_offset_pre = fstb::Vf32 (_vol_offset_pre);
	const auto     el2 = env_l2 - vol_offset_pre;

	// Knee polynomial
	const auto     c0   = fstb::Vf32 (_knee_coef_arr [0]);
	const auto     c1   = fstb::Vf32 (_knee_coef_arr [1]);
	const auto     c2   = fstb::Vf32 (_knee_coef_arr [2]);
	auto           poly = fstb::Poly::horner (el2, c0, c1, c2);

	// Linear ratios
	const auto     tst_pos  = el2.is_lt_0 ();
	auto           ratio_lo = fstb::Vf32 (_ratio_lo);
	auto           ratio_hi = fstb::Vf32 (_ratio_hi);
	const auto     ratio    = fstb::ToolsSimd::select (tst_pos, ratio_lo, ratio_hi);
	auto           linear   = el2 * ratio;

	// Selects result
	const auto     knee_th_abs = fstb::Vf32 (_knee_th_abs);
	const auto     el2_abs     = fstb::abs (el2);
	const auto     tst_knee    = (el2_abs < knee_th_abs);
	const auto     vl2         = fstb::ToolsSimd::select (tst_knee, poly, linear);

	// Computes and limits gain
	auto           gain_l2     = vl2 - el2;
	const auto     gain_min_l2 = fstb::Vf32 (_gain_min_l2);
	const auto     gain_max_l2 = fstb::Vf32 (_gain_max_l2);
	gain_l2 = fstb::limit (gain_l2, gain_min_l2, gain_max_l2);
	// Another solution would be mirroring before minimising:
	// gain_top_l2 - abs (gain_l2 - gain_top_l2) with gain_top_l2 > gain_max_l2.
	// Quickest solution but may generate very low signals turning later into denormals.

	// Handles the case where detected volume is close to 0 (-oo dB).
	// Smoothly fades the gain to 0 dB.
	// g = g * limit ((v - v0) / (v1 - v0), 0, 1)
	const auto     active_thr_l2 = fstb::Vf32 (_active_thr_l2);
	const auto     active_mul    = fstb::Vf32 (_active_mul);
	const auto     one           = fstb::Vf32 (1);
	const auto     zero          = fstb::Vf32::zero ();
	auto           active_rate   = env_l2 - active_thr_l2;
	active_rate *= active_mul;
	active_rate  = fstb::limit (active_rate, zero, one);
	gain_l2     *= active_rate;

	// Stores the gain adjustment if requested
	if (store_flag)
	{
		_cur_gain = fstb::ToolsSimd::Shift <0>::extract (gain_l2);
	}

	// Additional gain (manual + auto)
	const auto     vol_offset_post = fstb::Vf32 (_vol_offset_post);
	gain_l2 += vol_offset_post;

	// Conversion to linear, multiplicative volume
	const auto     gain = fstb::Approx::exp2 (gain_l2);

	return gain;
}



void	CompexGainFnc::clear_buffers ()
{
	_cur_gain = 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace cpx
}  // namespace pi
}  // namespace mfx



#endif   // mfx_pi_cpx_CompexGainFnc_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
