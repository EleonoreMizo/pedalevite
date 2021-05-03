/*****************************************************************************

        InertiaRc.cpp
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

#include "mfx/dsp/ctrl/InertiaRc.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace ctrl
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



InertiaRc::InertiaRc () noexcept
:	_sample_freq (44100)
,	_inertia_time (0.100f)
,	_start_val (0)
,	_final_val (0)
,	_threshold (0.010f)
,	_rc ()
{
	update_inertia_time ();
	force_val (0);
}



void	InertiaRc::set_sample_freq (float sample_freq) noexcept
{
	assert (sample_freq > 0);

	_sample_freq = sample_freq;
	update_inertia_time ();
}



void	InertiaRc::set_inertia_time (float t) noexcept
{
	assert (t >= 0);

	_inertia_time = t;
	update_inertia_time ();
}



float	InertiaRc::get_inertia_time () const noexcept
{
	return _inertia_time;
}



void	InertiaRc::set_threshold (float threshold) noexcept
{
	assert (threshold > 0);

	_threshold = threshold;
	update_inertia_time ();
}



void	InertiaRc::set_val (float val) noexcept
{
	_start_val = get_val ();
	_final_val = val;
	_rc.set_val (0);
}



void	InertiaRc::force_val (float val) noexcept
{
	_start_val = val;
	_final_val = val;
	_rc.set_val (1);
}



float	InertiaRc::get_val () const noexcept
{
	const float    lerp = _rc.get_val ();

	return _start_val + (_final_val - _start_val) * lerp;
}



float	InertiaRc::get_target_val () const noexcept
{
	return _final_val;
}



void	InertiaRc::tick (long nbr_spl) noexcept
{
	_rc.skip_block (nbr_spl);
}



bool	InertiaRc::is_ramping () const noexcept
{
	return (_start_val != _final_val && ! _rc.is_finished ());
}



void	InertiaRc::stop () noexcept
{
	force_val (get_val ());
}



void	InertiaRc::clear_buffers () noexcept
{
	force_val (_final_val);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	InertiaRc::update_inertia_time () noexcept
{
	float          mult = 0;
	if (_inertia_time >  0)
	{
		const double   nbr_spl = double (_inertia_time) * _sample_freq;
		mult = float (exp (log (_threshold) / nbr_spl));
	}

	_rc.setup (1, mult, _threshold);
}



}  // namespace ctrl
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
