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

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace osc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// cos_ptr and sin_ptr must point to buffers aligned in position AND length.
// Extra-data between nbr_spl and the next aligned position will be overwritten.
template <class VD>
void	OscSinCosStableSimd::process_block (float cos_ptr [], float sin_ptr [], int nbr_spl) noexcept
{
	assert (VD::check_ptr (cos_ptr));
	assert (VD::check_ptr (sin_ptr));
	assert (nbr_spl > 0);

	const auto     alpha   = fstb::ToolsSimd::load_f32 (&_alpha);
	const auto     beta    = fstb::ToolsSimd::load_f32 (&_beta );
	auto           pos_cos = fstb::ToolsSimd::load_f32 (&_pos_cos);
	auto           pos_sin = fstb::ToolsSimd::load_f32 (&_pos_sin);

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
		fstb::ToolsSimd::storeu_f32_part (cos_ptr + n4, pos_cos, rem_len);
		fstb::ToolsSimd::storeu_f32_part (sin_ptr + n4, pos_sin, rem_len);

		const auto     prv_cos = pos_cos;
		const auto     prv_sin = pos_sin;
		step (pos_cos, pos_sin, alpha, beta);

		switch (nbr_spl - n4)
		{
		case 1:
			pos_cos = fstb::ToolsSimd::Shift <1>::compose (prv_cos, pos_cos);
			pos_sin = fstb::ToolsSimd::Shift <1>::compose (prv_sin, pos_sin);
			break;
		case 2:
			pos_cos = fstb::ToolsSimd::Shift <2>::compose (prv_cos, pos_cos);
			pos_sin = fstb::ToolsSimd::Shift <2>::compose (prv_sin, pos_sin);
			break;
		case 3:
			pos_cos = fstb::ToolsSimd::Shift <3>::compose (prv_cos, pos_cos);
			pos_sin = fstb::ToolsSimd::Shift <3>::compose (prv_sin, pos_sin);
			break;
		default:
			assert (false);
			break;
		}
	}

	fstb::ToolsSimd::store_f32 (&_pos_cos, pos_cos);
	fstb::ToolsSimd::store_f32 (&_pos_sin, pos_sin);
}



fstb::ToolsSimd::VectF32	OscSinCosStableSimd::get_cos () const noexcept
{
	return fstb::ToolsSimd::load_f32 (&_pos_cos);
}



fstb::ToolsSimd::VectF32	OscSinCosStableSimd::get_sin () const noexcept
{
	return fstb::ToolsSimd::load_f32 (&_pos_sin);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	OscSinCosStableSimd::step (fstb::ToolsSimd::VectF32 &pos_cos, fstb::ToolsSimd::VectF32 &pos_sin, fstb::ToolsSimd::VectF32 alpha, fstb::ToolsSimd::VectF32 beta) noexcept
{
	const auto     tmp = pos_cos - alpha * pos_sin;
	pos_sin += beta * tmp;
	pos_cos  = tmp - alpha * pos_sin;
}



void	OscSinCosStableSimd::compute_step (fstb::ToolsSimd::VectF32 &alpha, fstb::ToolsSimd::VectF32 &beta, float angle_rad) noexcept
{
   const double   a = tan (angle_rad * 0.5f);
   alpha = fstb::ToolsSimd::set1_f32 (float (a));

// const double   b = sin (angle_rad);
	const double   b = 2 * a / (1 + a * a);
   beta  = fstb::ToolsSimd::set1_f32 (float (b));
}



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_osc_OscSinCosStableSimd_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
