/*****************************************************************************

        RemezPoint.cpp
        Author: Laurent de Soras, 2019

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

#include "mfx/dsp/fir/RemezPoint.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace fir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



RemezPoint::RemezPoint (double lower_freq, double upper_freq, double amp, double weight) noexcept
:	_lower_freq (lower_freq)
,	_upper_freq (upper_freq)
,	_amp (amp)
,	_weight (weight)
{
	assert (lower_freq >= 0);
	assert (lower_freq <= upper_freq);
	assert (upper_freq <= 0.5);
	assert (amp >= 0);
	assert (weight > 0);
}



void	RemezPoint::set_lower_freq (double freq) noexcept
{
	assert (freq >= 0);
	assert (freq <= 0.5);

	_lower_freq = freq;
}



double	RemezPoint::get_lower_freq () const noexcept
{
	return _lower_freq;
}



void	RemezPoint::set_upper_freq (double freq) noexcept
{
	assert (freq >= 0);
	assert (freq <= 0.5);

	_upper_freq = freq;
}



double	RemezPoint::get_upper_freq () const noexcept
{
	return _upper_freq;
}



void	RemezPoint::set_amp (double amp) noexcept
{
	assert (amp >= 0);

	_amp = amp;
}



double	RemezPoint::get_amp () const noexcept
{
	return _amp;
}



void	RemezPoint::set_weight (double weight) noexcept
{
	assert (weight > 0);

	_weight = weight;
}



double	RemezPoint::get_weight () const noexcept
{
	return _weight;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace fir
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
