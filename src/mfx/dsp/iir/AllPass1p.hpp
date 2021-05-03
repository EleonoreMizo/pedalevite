/*****************************************************************************

        AllPass1p.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_iir_AllPass1p_CODEHEADER_INCLUDED)
#define mfx_dsp_iir_AllPass1p_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: set_coef
Description:
	Set the coefficient for the first order all pass transfert function:
	                -1
	       b0 +    z
	H(z) = -----------
	                -1
	       1  + b0.z
	The equivalent time equation is:
	y[t] = b0.x[t-0] + x[t-1] - b0.y[t-1]
Input parameters:
	- b0: Coefficient, [-1 ; 1].
Throws: Nothing
==============================================================================
*/

void	AllPass1p::set_coef (float b0) noexcept
{
	_eq_z_b0 = b0;
}



float	AllPass1p::get_coef () const noexcept
{
	return _eq_z_b0;
}



void	AllPass1p::copy_z_eq (const AllPass1p &other) noexcept
{
	_eq_z_b0 = other._eq_z_b0;
}



float	AllPass1p::process_sample (float x) noexcept
{
	const float    y = _eq_z_b0 * (x - _mem_y) + _mem_x;
	_mem_y = y;
	_mem_x = x;

	return y;
}



float	AllPass1p::process_sample (float x, float inc_b0) noexcept
{
	const float    y = process_sample (x);
	step_z_eq (inc_b0);

	return y;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	AllPass1p::step_z_eq (const float inc_b0) noexcept
{
	_eq_z_b0 += inc_b0;
}



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_iir_AllPass1p_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
