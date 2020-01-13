/*****************************************************************************

        AllPass2p.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_iir_AllPass2p_CODEHEADER_INCLUDED)
#define mfx_dsp_iir_AllPass2p_CODEHEADER_INCLUDED



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
Name: set_coefs
Description:
	Set the coefficient for the second order all pass transfert function:
	                 -1       -2
	        b0 + b1.z   +    z
	H(z) = ----------------------
	                 -1       -2
	        1  + b1.z   + b0.z

	The equivalent time equation is :       
	y[t] = b0.x[t-0] + b1.x[t-1] +    x[t-2]
	                 - b1.y[t-1] - b0.y[t-2]

		  Input parameters:
	- b0: Coefficient.
	- b1: Coefficient.
Throws: Nothing
==============================================================================
*/

void	AllPass2p::set_z_eq (float b0, float b1)
{
	_eq_z_b [0] = b0;
	_eq_z_b [1] = b1;
}



void	AllPass2p::get_z_eq (float &b0, float &b1) const
{
	b0 = _eq_z_b [0];
	b1 = _eq_z_b [1];
}



float	AllPass2p::process_sample (float x)
{
	const int      alt_pos = 1 - _mem_pos;
	const float    y =   _eq_z_b [0] * (x                 - _mem_y [ alt_pos])
	                   + _eq_z_b [1] * (_mem_x [_mem_pos] - _mem_y [_mem_pos])
	                   +                _mem_x [ alt_pos];

	_mem_x [alt_pos] = x;
	_mem_y [alt_pos] = y;
	_mem_pos         = alt_pos;

	return y;
}



float	AllPass2p::process_sample (float x, const float inc_b [2])
{
	assert (inc_b != nullptr);

	const float    y = process_sample (x);
	step_z_eq (inc_b);

	return y;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	AllPass2p::step_z_eq (const float inc_b [2])
{
	assert (inc_b != nullptr);

	_eq_z_b [0] += inc_b [0];
	_eq_z_b [1] += inc_b [1];
}



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_iir_AllPass2p_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
