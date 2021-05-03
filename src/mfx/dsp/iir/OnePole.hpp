/*****************************************************************************

        OnePole.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_iir_OnePole_CODEHEADER_INCLUDED)
#define mfx_dsp_iir_OnePole_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	OnePole::neutralise () noexcept
{
	_eq_z_b [0] = 1;
	_eq_z_b [1] = 0;
	_eq_z_a [1] = 0;
}



void	OnePole::set_z_eq (const float b [2], const float a [2]) noexcept
{
	assert (b != nullptr);
	assert (a != nullptr);

	_eq_z_b [0] = b [0];
	_eq_z_b [1] = b [1];
	_eq_z_a [1] = a [1];
}



void	OnePole::get_z_eq (float b [2], float a [2]) const noexcept
{
	assert (b != nullptr);
	assert (a != nullptr);

	b [0] = _eq_z_b [0];
	b [1] = _eq_z_b [1];
	a [1] = _eq_z_a [1];
}



void	OnePole::copy_z_eq (const OnePole &other) noexcept
{
	_eq_z_b [0] = other._eq_z_b [0];
	_eq_z_b [1] = other._eq_z_b [1];
	_eq_z_a [1] = other._eq_z_a [1];
}



float	OnePole::process_sample (float x) noexcept
{
	const float    y =   _eq_z_b [0] *      x
	                   + _eq_z_b [1] * _mem_x
	                   - _eq_z_a [1] * _mem_y;
	_mem_y = y;
	_mem_x = x;

	return y;
}



float	OnePole::process_sample (float x, const float inc_b [2], const float inc_a [2]) noexcept
{
	assert (inc_b != nullptr);
	assert (inc_a != nullptr);

	const float    y = process_sample (x);
	step_z_eq (inc_b, inc_a);

	return y;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	OnePole::step_z_eq (const float inc_b [2], const float inc_a [2]) noexcept
{
	assert (inc_b != nullptr);
	assert (inc_a != nullptr);

	_eq_z_b [0] += inc_b [0];
	_eq_z_b [1] += inc_b [1];
	_eq_z_a [1] += inc_a [1];
}



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_iir_OnePole_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
