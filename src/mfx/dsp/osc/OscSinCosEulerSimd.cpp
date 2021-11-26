/*****************************************************************************

        OscSinCosEulerSimd.cpp
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
#include "fstb/ToolsSimd.h"
#include "mfx/dsp/osc/OscSinCosEulerSimd.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace osc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



OscSinCosEulerSimd::OscSinCosEulerSimd () noexcept
:	_pos_cos ()
,	_pos_sin ()
,	_step_cosn (1)
,	_step_sinn (0)
,	_step_cos1 (1)
,	_step_sin1 (0)
{
	fstb::ToolsSimd::store_f32 (&_pos_cos , fstb::Vf32 (1));
	fstb::ToolsSimd::store_f32 (&_pos_sin , fstb::Vf32::zero ());
}



// phase in radian
void	OscSinCosEulerSimd::set_phase (float phase) noexcept
{
	const float    c = cosf (phase);
	const float    s = sinf (phase);

	resync (c, s);
}



// step in radian/sample
void	OscSinCosEulerSimd::set_step (float stp) noexcept
{
	_step_cos1 = cosf (stp);
	_step_sin1 = sinf (stp);
	_step_cosn = cosf (stp * _nbr_units);
	_step_sinn = sinf (stp * _nbr_units);

	const float    c0 = fstb::ToolsSimd::Shift <0>::extract (
		fstb::ToolsSimd::load_f32 (&_pos_cos)
	);
	const float    s0 = fstb::ToolsSimd::Shift <0>::extract (
		fstb::ToolsSimd::load_f32 (&_pos_sin)
	);
	resync (c0, s0);
}



void	OscSinCosEulerSimd::step () noexcept
{
	const auto     c_old = fstb::ToolsSimd::load_f32 (&_pos_cos);
	const auto     s_old = fstb::ToolsSimd::load_f32 (&_pos_sin);
	const auto     c_stp = fstb::Vf32 (_step_cosn);
	const auto     s_stp = fstb::Vf32 (_step_sinn);
	const auto     c_new = c_old * c_stp - s_old * s_stp;
	const auto     s_new = c_old * s_stp + s_old * c_stp;
	fstb::ToolsSimd::store_f32 (&_pos_cos, c_new);
	fstb::ToolsSimd::store_f32 (&_pos_sin, s_new);
}



fstb::Vf32	OscSinCosEulerSimd::get_cos () const noexcept
{
	return fstb::ToolsSimd::load_f32 (&_pos_cos);
}



fstb::Vf32	OscSinCosEulerSimd::get_sin () const noexcept
{
	return fstb::ToolsSimd::load_f32 (&_pos_sin);
}



void	OscSinCosEulerSimd::process_block (float cos_ptr [], float sin_ptr [], int nbr_vec) noexcept
{
	assert (fstb::DataAlign <true>::check_ptr (cos_ptr));
	assert (fstb::DataAlign <true>::check_ptr (sin_ptr));
	assert (nbr_vec > 0);

	auto           c_cur = fstb::ToolsSimd::load_f32 (&_pos_cos);
	auto           s_cur = fstb::ToolsSimd::load_f32 (&_pos_sin);
	const auto     c_stp = fstb::Vf32 (_step_cosn);
	const auto     s_stp = fstb::Vf32 (_step_sinn);

	const int      nbs_spl = nbr_vec * _nbr_units;
	for (int pos = 0; pos < nbs_spl; pos += _nbr_units)
	{
		fstb::ToolsSimd::store_f32 (cos_ptr + pos, c_cur);
		fstb::ToolsSimd::store_f32 (sin_ptr + pos, s_cur);
		const auto     c_new = c_cur * c_stp - s_cur * s_stp;
		const auto     s_new = c_cur * s_stp + s_cur * c_stp;
		c_cur = c_new;
		s_cur = s_new;
	}

	fstb::ToolsSimd::store_f32 (&_pos_cos, c_cur);
	fstb::ToolsSimd::store_f32 (&_pos_sin, s_cur);
}



void	OscSinCosEulerSimd::correct () noexcept
{
	auto           c  = fstb::ToolsSimd::load_f32 (&_pos_cos);
	auto           s  = fstb::ToolsSimd::load_f32 (&_pos_sin);
	const auto     n  = fstb::ToolsSimd::sqrt (c * c + s * s);
	const auto     ni = n.rcp_approx2 ();
	c *= ni;
	s *= ni;
	fstb::ToolsSimd::store_f32 (&_pos_cos, c);
	fstb::ToolsSimd::store_f32 (&_pos_sin, s);
}



// Uses a single-step Newton-Raphson approximation of 1 / sqrt (1 + r ^ 2)
// with 1 as initial guess for the corrective term.
void	OscSinCosEulerSimd::correct_fast () noexcept
{
	auto           c  = fstb::ToolsSimd::load_f32 (&_pos_cos);
	auto           s  = fstb::ToolsSimd::load_f32 (&_pos_sin);
	const auto     r2 = c * c + s * s;
	const auto     a3 = fstb::Vf32 (3.f);
	const auto     ah = fstb::Vf32 (0.5f);
	const auto     ni = (a3 - r2) * ah;
	c *= ni;
	s *= ni;
	fstb::ToolsSimd::store_f32 (&_pos_cos, c);
	fstb::ToolsSimd::store_f32 (&_pos_sin, s);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	OscSinCosEulerSimd::resync (float c0, float s0) noexcept
{
	auto           c = fstb::Vf32 (c0);
	auto           s = fstb::Vf32 (s0);

	for (int k = 1; k < _nbr_units; ++k)
	{
		const float    c_new = c0 * _step_cos1 - s0 * _step_sin1;
		const float    s_new = c0 * _step_sin1 + s0 * _step_cos1;
		c = fstb::ToolsSimd::Shift <0>::insert (c, c_new);
		s = fstb::ToolsSimd::Shift <0>::insert (s, s_new);
		c = fstb::ToolsSimd::Shift <-1>::rotate (c);
		s = fstb::ToolsSimd::Shift <-1>::rotate (s);
		c0 = c_new;
		s0 = s_new;
	}

	fstb::ToolsSimd::store_f32 (&_pos_cos, c);
	fstb::ToolsSimd::store_f32 (&_pos_sin, s);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
