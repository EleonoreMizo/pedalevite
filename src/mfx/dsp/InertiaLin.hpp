/*****************************************************************************

        InertiaLin.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_InertiaLin_CODEHEADER_INCLUDED)
#define mfx_dsp_InertiaLin_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



InertiaLin::InertiaLin (double val)
:	_old_val (val)
,	_new_val (val)
,	_cur_val (val)
{
	// Nothing
}



/*
==============================================================================
Name: set_sample_freq
Description:
	Sets the sampling rate.
Input parameters:
	- fs: Sample frequency, Hz, > 0.
Throws: Nothing
==============================================================================
*/

void	InertiaLin::set_sample_freq (double fs)
{
	assert (fs > 0);

	_step     *= _fs / fs;
	_fs        = fs;
	_step_mul  = 1.0 / (_inertia_time * _fs);
}



/*
==============================================================================
Name: set_inertia_time
Description:
	Sets the inertia time, the time to reach a new value.
	The current ramp is not affected.
Input parameters:
	- inertia_time: time in seconds. >= 0.
Throws: Nothing.
==============================================================================
*/

void	InertiaLin::set_inertia_time (double inertia_time)
{
	assert (inertia_time >= 0);

	_inertia_time = inertia_time;
	_step_mul     = 1.0 / (_inertia_time * _fs);
}



/*
==============================================================================
Name: update_inertia_time
Description:
	Sets the inertia time, the time to reach a new value.
	The current ramp is updated. If the inertia time is zero, the new value is
	reached immediately.
Input parameters:
	- inertia_time: time in seconds. >= 0.
Throws: Nothing.
==============================================================================
*/

void	InertiaLin::update_inertia_time (double inertia_time)
{
	assert (inertia_time >= 0);

	if (is_ramping ())
	{
		if (inertia_time > 0)
		{
			_step *= _inertia_time / inertia_time;
		}
		else
		{
			force_val (_new_val);
		}
	}

	set_inertia_time (inertia_time);
}



/*
==============================================================================
Name: get_inertia_time
Description:
Returns: inertia time, in seconds, >= 0.
Throws: Nothing.
==============================================================================
*/

double	InertiaLin::get_inertia_time () const
{
	return _inertia_time;
}



/*
==============================================================================
Name: set_val
Description:
	Sets a new value. The output value will linearly ramp from the current
	value to reach this new value after the inertia time.
Input parameters:
	- val: The new value.
Throws: Nothing.
==============================================================================
*/

void	InertiaLin::set_val (double val)
{
	if (_inertia_time == 0)
	{
		force_val (val);
	}

	else
	{
		double         new_step = (val - _cur_val) * _step_mul;

		// If we accumulate multiple set_val() during a ramp tending toward the
		// same direction, keep the quickest gliding speed.
		if (_new_val != _old_val && new_step * _step > 0)
		{
			if (fabs (new_step) > fabs (_step))
			{
				_step = new_step;
			}
		}
		else
		{
			_step = new_step;
		}

		_old_val = _cur_val;
		_new_val = val;
	}
}



/*
==============================================================================
Name: force_val
Description:
	Sets a new value immediately (no inertia).
Input parameters:
	- val: The new value.
Throws: Nothing.
==============================================================================
*/

void	InertiaLin::force_val (double val)
{
	_old_val = val;
	_new_val = val;
	_cur_val = val;
	_step    = 0;
}



/*
==============================================================================
Name: get_val
Description:
	Outputs the current value, processed with the inertia.
Returns: The value
Throws: Nothing.
==============================================================================
*/

double	InertiaLin::get_val () const
{
	return _cur_val;
}



/*
==============================================================================
Name: get_target_val
Description:
	Gets the value to reach, as set with set_val() or force_val(). If there
	is no active ramp, this is the current, static value.
Returns: The target value.
Throws: Nothing.
==============================================================================
*/

double	InertiaLin::get_target_val () const
{
	return _new_val;
}



/*
==============================================================================
Name: tick
Description:
	Time elapses.
Input parameters:
	- nbr_spl: Number of samples to wait for. >= 0
Throws: Nothing.
==============================================================================
*/

void	InertiaLin::tick (int nbr_spl)
{
	assert (nbr_spl >= 0);

	if (_step != 0)
	{
		_cur_val += _step * nbr_spl;

		// Reached ?
		if (_step * (_cur_val - _new_val) >= 0)
		{
			_cur_val = _new_val;
			_old_val = _new_val;
			_step    = 0;
		}
	}
}



/*
==============================================================================
Name: is_ramping
Description:
	Indicates if the current value is ramping.
Returns: true if a ramp is running.
Throws: Nothing.
==============================================================================
*/

bool	InertiaLin::is_ramping () const
{
	return (_step != 0);
}



double	InertiaLin::get_step () const
{
	return _step;
}



/*
==============================================================================
Name: stop
Description:
	Stops a ramp at the current state. The current value becomes the final
	value.
Throws: Nothing.
==============================================================================
*/

void	InertiaLin::stop ()
{
	force_val (_cur_val);
}



/*
==============================================================================
Name: clear_buffers
Description:
	Simulates an infinite tick(). Ramps are finished.
Throws: Nothing.
==============================================================================
*/

void	InertiaLin::clear_buffers ()
{
	force_val (_new_val);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_InertiaLin_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
