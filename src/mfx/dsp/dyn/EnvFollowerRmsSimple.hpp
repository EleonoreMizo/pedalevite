/*****************************************************************************

        EnvFollowerRmsSimple.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_dyn_EnvFollowerRmsSimple_CODEHEADER_INCLUDED)
#define mfx_dsp_dyn_EnvFollowerRmsSimple_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace dyn
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



float	EnvFollowerRmsSimple::process_sample (float x) noexcept
{
	const float    y = process_sample_no_sqrt (x);

	return (static_cast <float> (sqrt (y)));
}



float	EnvFollowerRmsSimple::process_sample_no_sqrt (float x) noexcept
{
	process_sample_internal (_state, x);

	return (_state);
}



/*
==============================================================================
Name: apply_volume
Description:
	Modify the current envelope state to simulate a gain in the input. It is
	usefull when the input signal gain is changed and envelope needs to be
	updated to reflect the change immediately.
Input parameters:
	- gain: Gain. Should be positive.
Throws: Nothing
==============================================================================
*/

void	EnvFollowerRmsSimple::apply_volume (float gain) noexcept
{
	assert (gain >= 0);

	_state *= gain * gain;
}



float	EnvFollowerRmsSimple::get_state_no_sqrt () const noexcept
{
	return _state;
}



void	EnvFollowerRmsSimple::set_state_raw (float x2) noexcept
{
	assert (x2 >= 0);

	_state = x2;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	EnvFollowerRmsSimple::process_sample_internal (float &state, float x) const noexcept
{
	process_sample_internal_no_sq (state, x * x);
}



void	EnvFollowerRmsSimple::process_sample_internal_no_sq (float &state, float x2) const noexcept
{
	const float    delta = x2 - state;
	state += delta * _coef;

	assert (state >= 0);
}



}  // namespace dyn
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_dyn_EnvFollowerRmsSimple_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
