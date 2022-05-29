/*****************************************************************************

        OscSinCosStableSimd.cpp
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

#include "mfx/dsp/osc/OscSinCosStableSimd.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace osc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



OscSinCosStableSimd::OscSinCosStableSimd () noexcept
:	_pos_cos ()
,	_pos_sin ()
,	_v_alpha ()
,	_v_beta ()
{
	fstb::Vf32::zero ().store (&_v_alpha);
	fstb::Vf32::zero ().store (&_v_beta );
	clear_buffers ();
}



void	OscSinCosStableSimd::set_step (float angle_rad) noexcept
{
	fstb::Vf32     alpha4;
	fstb::Vf32     beta4;
	compute_step (_s_alpha, _s_beta, angle_rad             );
	compute_step (  alpha4,   beta4, angle_rad * _nbr_units);
	alpha4.store (&_v_alpha);
	beta4.store (&_v_beta);

	update_future (_s_alpha, _s_beta);
}



void	OscSinCosStableSimd::step () noexcept
{
	auto           alpha   = fstb::Vf32::load (&_v_alpha);
	auto           beta    = fstb::Vf32::load (&_v_beta );
	auto           pos_cos = fstb::Vf32::load (&_pos_cos);
	auto           pos_sin = fstb::Vf32::load (&_pos_sin);

	step (pos_cos, pos_sin, alpha, beta);

	pos_cos.store (&_pos_cos);
	pos_sin.store (&_pos_sin);
}



void	OscSinCosStableSimd::clear_buffers () noexcept
{
	fstb::Vf32 (1)     .store (&_pos_cos);
	fstb::Vf32::zero ().store (&_pos_sin);
	update_future (_s_alpha, _s_beta);
}



void	OscSinCosStableSimd::correct_fast () noexcept
{
	auto           c       = fstb::Vf32::load (&_pos_cos);
	auto           s       = fstb::Vf32::load (&_pos_sin);
	const auto     norm_sq = fstb::fma (s, s, c * c);
	const auto     c1_5    = fstb::Vf32 (1.5f);
	const auto     c0_5    = fstb::Vf32 (0.5f);
	const auto     mult    = c1_5 - norm_sq * c0_5;

	c *= mult;
	s *= mult;

	c.store (&_pos_cos);
	s.store (&_pos_sin);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
