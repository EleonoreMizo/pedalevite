/*****************************************************************************

        Biquad.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_iir_Biquad_CODEHEADER_INCLUDED)
#define mfx_dsp_iir_Biquad_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Biquad::neutralise ()
{
	_eq_z_b [0] = 1;
	_eq_z_b [1] = 0;
	_eq_z_b [2] = 0;
	_eq_z_a [1] = 0;
	_eq_z_a [2] = 0;
}



void	Biquad::set_z_eq (const float b [3], const float a [3])
{
	assert (b != 0);
	assert (a != 0);

	_eq_z_b [0] = b [0];
	_eq_z_b [1] = b [1];
	_eq_z_b [2] = b [2];
	_eq_z_a [1] = a [1];
	_eq_z_a [2] = a [2];
}



void	Biquad::get_z_eq (float b [3], float a [3]) const
{
	assert (b != 0);
	assert (a != 0);

	b [0] = _eq_z_b [0];
	b [1] = _eq_z_b [1];
	b [2] = _eq_z_b [2];
	a [1] = _eq_z_a [1];
	a [2] = _eq_z_a [2];
}



float	Biquad::process_sample (float x)
{
	const int      alt_pos = 1 - _mem_pos;
	const float    y =      _eq_z_b [0] *      x
	                   + (  _eq_z_b [1] * _mem_x [_mem_pos]
	                      + _eq_z_b [2] * _mem_x [ alt_pos])
	                   - (  _eq_z_a [1] * _mem_y [_mem_pos]
	                      + _eq_z_a [2] * _mem_y [ alt_pos]);

	_mem_x [alt_pos] = x;
	_mem_y [alt_pos] = y;
	_mem_pos         = alt_pos;

	return y;
}



float	Biquad::process_sample (float x, const float inc_b [3], const float inc_a [3])
{
	assert (inc_b != 0);
	assert (inc_a != 0);

	const float    y = process_sample (x);
	step_z_eq (inc_b, inc_a);

	return y;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Biquad::step_z_eq (const float inc_b [3], const float inc_a [3])
{
	assert (inc_b != 0);
	assert (inc_a != 0);

	_eq_z_b [0] += inc_b [0];
	_eq_z_b [1] += inc_b [1];
	_eq_z_b [2] += inc_b [2];
	_eq_z_a [1] += inc_a [1];
	_eq_z_a [2] += inc_a [2];
}



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_iir_Biquad_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
