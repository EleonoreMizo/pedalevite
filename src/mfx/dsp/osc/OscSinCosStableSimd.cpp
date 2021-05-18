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
,	_alpha ()
,	_beta ()
{
	fstb::ToolsSimd::store_f32 (&_alpha, fstb::ToolsSimd::set_f32_zero ());
	fstb::ToolsSimd::store_f32 (&_beta , fstb::ToolsSimd::set_f32_zero ());
	clear_buffers ();
}



void	OscSinCosStableSimd::set_step (float angle_rad) noexcept
{
	fstb::ToolsSimd::VectF32   alpha;
	fstb::ToolsSimd::VectF32   beta;
	fstb::ToolsSimd::VectF32   alpha4;
	fstb::ToolsSimd::VectF32   beta4;
	compute_step (alpha , beta , angle_rad             );
	compute_step (alpha4, beta4, angle_rad * _nbr_units);
	fstb::ToolsSimd::store_f32 (&_alpha, alpha4);
	fstb::ToolsSimd::store_f32 (&_beta , beta4 );

	auto           pos_cos = fstb::ToolsSimd::load_f32 (&_pos_cos);
	auto           pos_sin = fstb::ToolsSimd::load_f32 (&_pos_sin);
	const float    old_cos = fstb::ToolsSimd::Shift <0>::extract (pos_cos);
	const float    old_sin = fstb::ToolsSimd::Shift <0>::extract (pos_sin);
	for (int i = 1; i < _nbr_units; ++i)
	{
		step (pos_cos, pos_sin, alpha, beta);
		pos_cos = fstb::ToolsSimd::Shift <1>::rotate (pos_cos);
		pos_sin = fstb::ToolsSimd::Shift <1>::rotate (pos_sin);
		pos_cos = fstb::ToolsSimd::Shift <0>::insert (pos_cos, old_cos);
		pos_sin = fstb::ToolsSimd::Shift <0>::insert (pos_sin, old_sin);
	}
	fstb::ToolsSimd::store_f32 (&_pos_cos, pos_cos);
	fstb::ToolsSimd::store_f32 (&_pos_sin, pos_sin);
}



void	OscSinCosStableSimd::step () noexcept
{
	auto           alpha   = fstb::ToolsSimd::load_f32 (&_alpha);
	auto           beta    = fstb::ToolsSimd::load_f32 (&_beta );
	auto           pos_cos = fstb::ToolsSimd::load_f32 (&_pos_cos);
	auto           pos_sin = fstb::ToolsSimd::load_f32 (&_pos_sin);

	step (pos_cos, pos_sin, alpha, beta);

	fstb::ToolsSimd::store_f32 (&_pos_cos, pos_cos);
	fstb::ToolsSimd::store_f32 (&_pos_sin, pos_sin);
}



void	OscSinCosStableSimd::clear_buffers () noexcept
{
	fstb::ToolsSimd::store_f32 (&_pos_cos, fstb::ToolsSimd::set1_f32 (1));
	fstb::ToolsSimd::store_f32 (&_pos_sin, fstb::ToolsSimd::set_f32_zero ());
}



void	OscSinCosStableSimd::correct_fast () noexcept
{
	auto           c       = fstb::ToolsSimd::load_f32 (&_pos_cos);
	auto           s       = fstb::ToolsSimd::load_f32 (&_pos_sin);
	const auto     norm_sq = fstb::ToolsSimd::fmadd (c * c, s, s);
	const auto     c1_5    = fstb::ToolsSimd::set1_f32 (1.5f);
	const auto     c0_5    = fstb::ToolsSimd::set1_f32 (0.5f);
	const auto     mult    = c1_5 - norm_sq * c0_5;

	c *= mult;
	s *= mult;

	fstb::ToolsSimd::store_f32 (&_pos_cos, c);
	fstb::ToolsSimd::store_f32 (&_pos_sin, s);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
