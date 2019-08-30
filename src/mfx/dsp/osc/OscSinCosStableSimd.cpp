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

#include "fstb/DataAlign.h"
#include "mfx/dsp/osc/OscSinCosStableSimd.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace osc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



OscSinCosStableSimd::OscSinCosStableSimd ()
:	_pos_cos ()
,	_pos_sin ()
,	_alpha ()
,	_beta ()
{
	fstb::ToolsSimd::store_f32 (&_alpha, fstb::ToolsSimd::set_f32_zero ());
	fstb::ToolsSimd::store_f32 (&_beta , fstb::ToolsSimd::set_f32_zero ());
	clear_buffers ();
}



void	OscSinCosStableSimd::set_step (float angle_rad)
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



void	OscSinCosStableSimd::step ()
{
	auto           alpha   = fstb::ToolsSimd::load_f32 (&_alpha);
	auto           beta    = fstb::ToolsSimd::load_f32 (&_beta );
	auto           pos_cos = fstb::ToolsSimd::load_f32 (&_pos_cos);
	auto           pos_sin = fstb::ToolsSimd::load_f32 (&_pos_sin);

	step (pos_cos, pos_sin, alpha, beta);

	fstb::ToolsSimd::store_f32 (&_pos_cos, pos_cos);
	fstb::ToolsSimd::store_f32 (&_pos_sin, pos_sin);
}



// nbr_spl = nbr_vec * 4
void	OscSinCosStableSimd::process_block (float cos_ptr [], float sin_ptr [], int nbr_vec)
{
	assert (fstb::DataAlign <true>::check_ptr (cos_ptr));
	assert (fstb::DataAlign <true>::check_ptr (sin_ptr));
	assert (nbr_vec > 0);

	auto           alpha   = fstb::ToolsSimd::load_f32 (&_alpha);
	auto           beta    = fstb::ToolsSimd::load_f32 (&_beta );
	auto           pos_cos = fstb::ToolsSimd::load_f32 (&_pos_cos);
	auto           pos_sin = fstb::ToolsSimd::load_f32 (&_pos_sin);

	const int      nbs_spl = nbr_vec * _nbr_units;
	for (int pos = 0; pos < nbs_spl; pos += _nbr_units)
	{
		fstb::ToolsSimd::store_f32 (cos_ptr + pos, pos_cos);
		fstb::ToolsSimd::store_f32 (sin_ptr + pos, pos_sin);
		step (pos_cos, pos_sin, alpha, beta);
	}

	fstb::ToolsSimd::store_f32 (&_pos_cos, pos_cos);
	fstb::ToolsSimd::store_f32 (&_pos_sin, pos_sin);
}



fstb::ToolsSimd::VectF32	OscSinCosStableSimd::get_cos () const
{
	return (fstb::ToolsSimd::load_f32 (&_pos_cos));
}



fstb::ToolsSimd::VectF32	OscSinCosStableSimd::get_sin () const
{
	return (fstb::ToolsSimd::load_f32 (&_pos_sin));
}



void	OscSinCosStableSimd::clear_buffers ()
{
	fstb::ToolsSimd::store_f32 (&_pos_cos, fstb::ToolsSimd::set1_f32 (1));
	fstb::ToolsSimd::store_f32 (&_pos_sin, fstb::ToolsSimd::set_f32_zero ());
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	OscSinCosStableSimd::step (fstb::ToolsSimd::VectF32 &pos_cos, fstb::ToolsSimd::VectF32 &pos_sin, fstb::ToolsSimd::VectF32 alpha, fstb::ToolsSimd::VectF32 beta)
{
	const auto     old_cos = pos_cos;
	const auto     old_sin = pos_sin;

   pos_cos = old_cos - (alpha * old_cos + beta * old_sin);
   pos_sin = old_sin - (alpha * old_sin - beta * old_cos);
}



void	OscSinCosStableSimd::compute_step (fstb::ToolsSimd::VectF32 &alpha, fstb::ToolsSimd::VectF32 &beta, float angle_rad)
{
   const float    s = float (sin (angle_rad * 0.5f));
   alpha = fstb::ToolsSimd::set1_f32 (s * s * 2);
   beta  = fstb::ToolsSimd::set1_f32 (float (sin (angle_rad)));
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
