/*****************************************************************************

        DiodeClipJcm.cpp
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
#include "mfx/dsp/va/DiodeClipJcm.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace va
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DiodeClipJcm::set_sample_freq (double sample_freq) noexcept
{
	assert (sample_freq > 0);

	_sample_freq = float (sample_freq);
	_dirty_flag  = true;
	update_internal_coef ();
}



// Distortion potentiometer control. 0 = silent input
void	DiodeClipJcm::set_dist (float dist) noexcept
{
	assert (dist >= 0);
	assert (dist <= 1);

	_dist       = dist;
	_dirty_flag = true;
}



// Approximative way to set the saturation level
void	DiodeClipJcm::set_sat_lvl (float lvl) noexcept
{
	assert (lvl > 0);

	_vt         = float (25.86e-3 * 0.5) * lvl;
	_dirty_flag = true;
}



void	DiodeClipJcm::set_input_hpf_capa (float c) noexcept
{
	assert (c > 0);

	_c1         = c;
	_dirty_flag = true;
}



void	DiodeClipJcm::set_input_hpf_freq (float f) noexcept
{
	set_input_hpf_capa (1.0f / (float (2 * fstb::PI) * _r1 * f));
}



void	DiodeClipJcm::set_fdbk_lpf_capa (float c) noexcept
{
	assert (c > 0);

	_cf         = c;
	_dirty_flag = true;
}



void	DiodeClipJcm::set_fdbk_lpf_freq (float f) noexcept
{
	set_fdbk_lpf_capa (1.0f / (float (2 * fstb::PI) * _rp * f));
}



void	DiodeClipJcm::update_eq () noexcept
{
	update_internal_coef ();
}



float	DiodeClipJcm::process_sample (float x) noexcept
{
	assert (! _dirty_flag);

	// Inverted input subcircuit
	const float    c1x   = _wc1_s - x;
	const float    i0    = _kic1x  * c1x  + _kic2  * _wc2_s;
	const float    wc1_a = _kc1x   * x    + _kc1c2 * _wc2_s + _kc1c1 * _wc1_s;
	_wc2_s               = _kc2c1x * c1x  + _kc2c2 * _wc2_s;
	_wc1_s = wc1_a;

	// Feedback subcircuit and antiparallel diodes
	const float    p     = _wi0_r * i0;
	const float    wd_a  = _wpf_kt * (p - _wcf_s) - p;
	const float    tmp   = _wd_k1 + _wd_k2 * fstb::Approx::wright_omega_4 (
		_wd_k3x + _wd_k4 * fabsf (wd_a)
	);
	float          wd_b  = wd_a;
	if (wd_a >= 0)
	{
		wd_b += tmp;
	}
	else
	{
		wd_b -= tmp;
	}
	const float    vf    = _wsd_krx * (wd_a - wd_b) - wd_a;
	_wcf_s = vf + vf - _wcf_s;

	return vf;
}



void	DiodeClipJcm::clear_buffers () noexcept
{
	_wc1_s = 0;
	_wc2_s = 0;
	_wcf_s = 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DiodeClipJcm::update_internal_coef () noexcept
{
	if (_dirty_flag)
	{
		_wc1_r  = 1 / (2 * _c1 * _sample_freq);
		_wc2_r  = 1 / (2 * _c2 * _sample_freq);
		_wcf_r  = 1 / (2 * _cf * _sample_freq);
		_wrs_r  = _rs;
		const float    nd_vt   = _nd * _vt;
		_wd_k2  = -2 * nd_vt;
		_wd_k4  =  1 / nd_vt;
		_ki     = 1 / (2 * _r2);

		const float    rp_dist = _dist * _rp;
		_wrp_r  = _rp - rp_dist;
		_wp2_kt = _wc2_r / (_wrp_r + _wc2_r);
		_wp2_r  = _wrp_r * _wp2_kt;
		_ws2_r  = _r2 + _wp2_r;
		_wp1_kt = _r1 / (_ws2_r + _r1);
		_wp1_r  = _ws2_r * _wp1_kt;
		_ws1_kl = _wp1_r / (_wp1_r + _wc1_r);
		_ws2_kl = _r2 / _ws2_r;

		_wi0_r   = rp_dist;
		_wpf_kt  = _wi0_r / (_wi0_r + _wcf_r);
		_wpf_r   = _wcf_r * _wpf_kt;
		_wsd_krx = 0.5f * _wpf_r / (_wrs_r + _wpf_r);
		_wsd_r   = _wpf_r + _wrs_r;
		_wd_k1   = 2 * _wsd_r * _is;
		const float    wd_k3   = _wsd_r * _is * _wd_k4;
		_wd_k3x  = wd_k3 + fstb::Approx::log2 (wd_k3) * float (fstb::LN2);

		const float    one_m_ws1_kl = 1 - _ws1_kl;
		const float    one_m_ws2_kl = 1 - _ws2_kl;
		const float    one_m_wp2_kt = 1 - _wp2_kt;
		const float    wp1_kt_one_m_ws1_kl = _wp1_kt * one_m_ws1_kl;
		_kic1x  =  2 * _ki * _ws1_kl * _ws2_kl;
		_kic2   = -2 * _ki * _ws2_kl * one_m_wp2_kt * wp1_kt_one_m_ws1_kl;
		_kc1x   =  2 * one_m_ws1_kl;
		_kc1c1  =  2 * _ws1_kl - 1;
		_kc1c2  =  2 * _wp1_kt * one_m_wp2_kt * one_m_ws1_kl;
		_kc2c1x =  2 * _ws1_kl * one_m_ws2_kl;
		_kc2c2  =  2 * one_m_wp2_kt * (_ws2_kl + wp1_kt_one_m_ws1_kl * one_m_ws2_kl) - 1;

		_dirty_flag = false;
	}
}



}  // namespace va
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
