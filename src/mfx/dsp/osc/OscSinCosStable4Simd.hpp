/*****************************************************************************

        OscSinCosStable4Simd.hpp
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_osc_OscSinCosStable4Simd_CODEHEADER_INCLUDED)
#define mfx_dsp_osc_OscSinCosStable4Simd_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/Approx.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace osc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class VP>
void	OscSinCosStable4Simd <VP>::set_phase (fstb::Vf32 angle_rad) noexcept
{
   assert (angle_rad >= fstb::Vf32 (-3 * fstb::PI));
   assert (angle_rad <= fstb::Vf32 (+3 * fstb::PI));

   fstb::Vf32     c;
   fstb::Vf32     s;
   fstb::Approx::cos_sin_rbj (c, s, angle_rad);

   VP::store_f32 (&_pos_cos, c);
   VP::store_f32 (&_pos_sin, s);
}



template <class VP>
void	OscSinCosStable4Simd <VP>::set_step (fstb::Vf32 angle_rad) noexcept
{
   const auto     ab = compute_step (angle_rad);
   VP::store_f32 (&_alpha, std::get <0> (ab));
   VP::store_f32 (&_beta , std::get <1> (ab));
}



// Returns cos, sin
template <class VP>
std::tuple <fstb::Vf32, fstb::Vf32>	OscSinCosStable4Simd <VP>::step () noexcept
{
   const auto     a = VP::load_f32 (&_alpha);
   const auto     b = VP::load_f32 (&_beta);

   return step (a, b);
}



// Returns cos, sin
template <class VP>
std::tuple <fstb::Vf32, fstb::Vf32>	OscSinCosStable4Simd <VP>::step (fstb::Vf32 angle_rad) noexcept
{
   const auto     ab = compute_step (angle_rad);
   return step (std::get <0> (ab), std::get <1> (ab));
}



template <class VP>
fstb::Vf32	OscSinCosStable4Simd <VP>::get_cos () const noexcept
{
   return VP::load_f32 (&_pos_cos);
}



template <class VP>
fstb::Vf32	OscSinCosStable4Simd <VP>::get_sin () const noexcept
{
   return VP::load_f32 (&_pos_sin);
}



template <class VP>
void	OscSinCosStable4Simd <VP>::clear_buffers () noexcept
{
   VP::store_f32 (&_pos_cos, fstb::Vf32 (1));
   VP::store_f32 (&_pos_sin, fstb::Vf32 (0));
}



// Uses a single-step Newton-Raphson approximation of 1 / sqrt (1 + r ^ 2)
// with 1 as initial guess for the corrective term.
template <class VP>
void	OscSinCosStable4Simd <VP>::correct_fast () noexcept
{
   auto           c = VP::load_f32 (&_pos_cos);
   auto           s = VP::load_f32 (&_pos_sin);
	const auto     norm_sq = c * c + s * s;
	const auto     mult    = (fstb::Vf32 (3.0) - norm_sq) * fstb::Vf32 (0.5);
	// 2nd order approx: mult = (3/8) * norm_sq^2 - (10/8) * norm_sq + (15/8)

   c *= mult;
   s *= mult;
   VP::store_f32 (&_pos_cos, c);
   VP::store_f32 (&_pos_sin, s);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class VP>
std::tuple <fstb::Vf32, fstb::Vf32>	OscSinCosStable4Simd <VP>::step (fstb::Vf32 alpha, fstb::Vf32 beta) noexcept
{
   auto           c  = VP::load_f32 (&_pos_cos);
   auto           s  = VP::load_f32 (&_pos_sin);
   const auto     cs = std::make_tuple (c, s);

	const auto     tmp = c - alpha * s;
	s += beta * tmp;
	c  = tmp - alpha * s;
   VP::store_f32 (&_pos_cos, c);
   VP::store_f32 (&_pos_sin, s);

   return cs;
}



// Returns alpha, beta
template <class VP>
std::tuple <fstb::Vf32, fstb::Vf32>	OscSinCosStable4Simd <VP>::compute_step (fstb::Vf32 angle_rad) noexcept
{
	const auto     alpha = fstb::Approx::tan_pade55 (angle_rad * fstb::Vf32 (0.5f));
	const auto     beta  = fstb::Approx::sin_rbj (angle_rad);

   return std::make_tuple (alpha, beta);
}



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_osc_OscSinCosStable4Simd_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
