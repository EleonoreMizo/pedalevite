/*****************************************************************************

        OscSinCosStableSimd.hpp
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_osc_OscSinCosStableSimd_CODEHEADER_INCLUDED)
#define mfx_dsp_osc_OscSinCosStableSimd_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/ToolsSimd.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace osc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class VD>
void	OscSinCosStableSimd::process_block (float cos_ptr [], float sin_ptr [], int nbr_spl) noexcept
{
	assert (VD::check_ptr (cos_ptr));
	assert (VD::check_ptr (sin_ptr));
	assert (nbr_spl > 0);

	const auto     alpha   = fstb::Vf32::load (&_v_alpha);
	const auto     beta    = fstb::Vf32::load (&_v_beta );
	auto           pos_cos = fstb::Vf32::load (&_pos_cos);
	auto           pos_sin = fstb::Vf32::load (&_pos_sin);

	const int      n4      = nbr_spl & ~(_nbr_units - 1);
	for (int pos = 0; pos < n4; pos += _nbr_units)
	{
		VD::store_f32 (cos_ptr + pos, pos_cos);
		VD::store_f32 (sin_ptr + pos, pos_sin);
		step (pos_cos, pos_sin, alpha, beta);
	}

	const int      rem_len = nbr_spl - n4;
	if (rem_len > 0)
	{
		pos_cos.storeu_part (cos_ptr + n4, rem_len);
		pos_sin.storeu_part (sin_ptr + n4, rem_len);

		const auto     prv_cos = pos_cos;
		const auto     prv_sin = pos_sin;
		step (pos_cos, pos_sin, alpha, beta);

		switch (nbr_spl - n4)
		{
		case 1:
			pos_cos = fstb::Vf32::compose <1> (prv_cos, pos_cos);
			pos_sin = fstb::Vf32::compose <1> (prv_sin, pos_sin);
			break;
		case 2:
			pos_cos = fstb::Vf32::compose <2> (prv_cos, pos_cos);
			pos_sin = fstb::Vf32::compose <2> (prv_sin, pos_sin);
			break;
		case 3:
			pos_cos = fstb::Vf32::compose <3> (prv_cos, pos_cos);
			pos_sin = fstb::Vf32::compose <3> (prv_sin, pos_sin);
			break;
		default:
			assert (false);
			break;
		}
	}

	pos_cos.store (&_pos_cos);
	pos_sin.store (&_pos_sin);
}



fstb::Vf32	OscSinCosStableSimd::get_cos () const noexcept
{
	return fstb::Vf32::load (&_pos_cos);
}



fstb::Vf32	OscSinCosStableSimd::get_sin () const noexcept
{
	return fstb::Vf32::load (&_pos_sin);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	OscSinCosStableSimd::update_future (float alpha, float beta) 
{
	auto           pos_cos = fstb::Vf32::load (&_pos_cos);
	auto           pos_sin = fstb::Vf32::load (&_pos_sin);

	float          cur_cos = pos_cos.template extract <0> ();
	float          cur_sin = pos_sin.template extract <0> ();
	step (cur_cos, cur_sin, alpha, beta);
	pos_cos = pos_cos.template insert <1> (cur_cos);
	pos_sin = pos_sin.template insert <1> (cur_sin);
	step (cur_cos, cur_sin, alpha, beta);
	pos_cos = pos_cos.template insert <2> (cur_cos);
	pos_sin = pos_sin.template insert <2> (cur_sin);
	step (cur_cos, cur_sin, alpha, beta);
	pos_cos = pos_cos.template insert <3> (cur_cos);
	pos_sin = pos_sin.template insert <3> (cur_sin);

	pos_cos.store (&_pos_cos);
	pos_sin.store (&_pos_sin);
}



template <typename T>
void	OscSinCosStableSimd::step (T &pos_cos, T &pos_sin, T alpha, T beta) noexcept
{
	const auto     tmp = pos_cos - alpha * pos_sin;
	pos_sin += beta * tmp;
	pos_cos  = tmp - alpha * pos_sin;
}



template <typename T>
void	OscSinCosStableSimd::compute_step (T &alpha, T &beta, float angle_rad) noexcept
{
   const auto     a = tanf (angle_rad * 0.5f);
   alpha = T (a);

// const auto     b = sin (angle_rad);
	const auto     b = 2 * a / (1 + a * a);
   beta  = T (b);
}



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_osc_OscSinCosStableSimd_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
