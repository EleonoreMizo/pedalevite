/*****************************************************************************

        EnvFollowerPeak.hpp
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_dyn_EnvFollowerPeak_CODEHEADER_INCLUDED)
#define mfx_dsp_dyn_EnvFollowerPeak_CODEHEADER_INCLUDED



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



float	EnvFollowerPeak::process_sample (float x)
{
	process_sample_internal (_state, x);

	return _state;
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

void	EnvFollowerPeak::apply_volume (float gain)
{
	assert (gain >= 0);

	_state *= gain;
}



float	EnvFollowerPeak::get_state () const
{
	return _state;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	EnvFollowerPeak::process_sample_internal (float &state, float x) const
{
	const float    xa    = fabs (x);
	const float    delta = xa - state;
	const float    coef  = (delta >= 0) ? _coef_a : _coef_r;
	state += delta * coef;

	assert (state >= 0);
}



}  // namespace dyn
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_dyn_EnvFollowerPeak_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
