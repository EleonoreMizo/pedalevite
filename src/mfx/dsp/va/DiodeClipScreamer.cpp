/*****************************************************************************

        DiodeClipScreamer.cpp
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
#include "mfx/dsp/va/DiodeClipScreamer.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace va
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DiodeClipScreamer::set_sample_freq (double sample_freq) noexcept
{
	assert (sample_freq > 0);

	_sample_freq = float (sample_freq);
	_dirty_flag  = true;
	update_internal_coef ();
}



// Distortion potentiometer control
void	DiodeClipScreamer::set_dist (float dist) noexcept
{
	assert (dist >= 0);
	assert (dist <= 1);

	_dist       = dist;
	_dirty_flag = true;
}



// Approximative way to set the saturation level
void	DiodeClipScreamer::set_sat_lvl (float lvl) noexcept
{
	assert (lvl > 0);

	_vt         = float (25.86e-3 * 3.16227766) * lvl;
	_dirty_flag = true;
}



void	DiodeClipScreamer::set_input_hpf_capa (float c) noexcept
{
	assert (c > 0);

	_cn         = c;
	_dirty_flag = true;
}



void	DiodeClipScreamer::set_input_hpf_freq (float f) noexcept
{
	set_input_hpf_capa (1.0f / (float (2 * fstb::PI) * _rn * f));
}



void	DiodeClipScreamer::set_fdbk_hpf_capa (float c) noexcept
{
	assert (c > 0);

	_ci         = c;
	_dirty_flag = true;
}



void	DiodeClipScreamer::set_fdbk_hpf_freq (float f) noexcept
{
	set_fdbk_hpf_capa (1.0f / (float (2 * fstb::PI) * _ri * f));
}



void	DiodeClipScreamer::set_fdbk_lpf_capa (float c) noexcept
{
	assert (c > 0);

	_cf         = c;
	_dirty_flag = true;
}



void	DiodeClipScreamer::set_fdbk_lpf_freq (float f) noexcept
{
	set_fdbk_lpf_capa (1.0f / (float (2 * fstb::PI) * _rfmax * f));
}



// Call this after a parameter change
void	DiodeClipScreamer::update_eq () noexcept
{
	update_internal_coef ();
}



float	DiodeClipScreamer::process_sample (float x) noexcept
{
	assert (! _dirty_flag);

	float         p;

	// Non-inverted input subcircuit
	const float   s  = x + _wcn_s;
	p  = _wvn_kx * s;
	const float   vp = s - p;
	_wcn_s -= p + p;

	// Inverted input subcircuit
	p = _wvi_k * (vp + _wci_s);
	const float   i0 = _ki * p;
	_wci_s -= p;

	// Feedback subcircuit + antiparallel diodes
	p = _wi0_r * i0;
	const float    wd_a = _wpf_kt * (p - _wcf_s) - p;
	const float    tmp  = _wd_k1 + _wd_k2 * fstb::Approx::wright_omega_4 (
		_wd_k3x + _wd_k4 * fabsf (wd_a)
	);
	float          wd_b = wd_a;
	if (wd_a >= 0)
	{
		wd_b += tmp;
	}
	else
	{
		wd_b -= tmp;
	}
	const float    vf = _wsd_krx * (wd_a - wd_b) - wd_a;
	_wcf_s = vf + vf - _wcf_s;

	// Global output
	// vf is the distored signal, vp the HPF dry
	const float    y = vf + vp;

	return y;
}



void	DiodeClipScreamer::clear_buffers () noexcept
{
	_wcn_s = 0;
	_wci_s = 0;
	_wcf_s = 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DiodeClipScreamer::update_internal_coef () noexcept
{
	if (_dirty_flag)
	{
		const float    wcn_r = 1 / (2 * _cn * _sample_freq);
		const float    wci_r = 1 / (2 * _ci * _sample_freq);
		const float    wcf_r = 1 / (2 * _cf * _sample_freq);
		_wvn_kx  = wcn_r / (wcn_r + _rn);
		const float    wci_r_2 = 2 * wci_r;
		_wvi_k   = wci_r_2 / (wci_r + _ri);
		_ki      = 1 / wci_r_2;
		const float    wrs_r = _rs;
		const float    nd_vt = _nd * _vt;
		_wd_k2   = -2 * nd_vt;
		_wd_k4   =  1 / nd_vt;

		_wi0_r   = _rfmax * _dist;
		_wpf_kt  = _wi0_r / (_wi0_r + wcf_r);
		const float    wpf_r = wcf_r * _wpf_kt;
		const float    wrs_r_p_wpf_r = wrs_r + wpf_r;
		_wsd_krx = 0.5f * wpf_r / wrs_r_p_wpf_r;
		const float    wsd_r = wrs_r_p_wpf_r;
		_wd_k1   = 2 * wsd_r * _is;
		const float    wd_k3 = wsd_r * _is * _wd_k4;
		_wd_k3x  = wd_k3 + fstb::Approx::log2 (wd_k3) * float (fstb::LN2);

		_dirty_flag = false;
	}
}



}  // namespace va
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
