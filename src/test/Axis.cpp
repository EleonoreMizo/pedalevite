/*****************************************************************************

        Axis.cpp
        Author: Laurent de Soras, 2003

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130) // "'operator' : logical operation on address of string constant"
	#pragma warning (1 : 4223) // "nonstandard extension used : non-lvalue array converted to pointer"
	#pragma warning (1 : 4705) // "statement has no effect"
	#pragma warning (1 : 4706) // "assignment within conditional expression"
	#pragma warning (4 : 4786) // "identifier was truncated to '255' characters in the debug information"
	#pragma warning (4 : 4800) // "forcing value to bool 'true' or 'false' (performance warning)"
	#pragma warning (4 : 4355) // "'this' : used in base member initializer list"
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "Axis.h"

#include <cassert>
#include <cmath>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: ctor
Throws: std::string related exception
==============================================================================
*/

Axis::Axis ()
:	_min_val (0)
,	_max_val (10)
,	_tick_org (0)
,	_tick_dist (1)
,	_name ()
,	_log_flag (false)
,	_tick_flag (true)
{
	// Nothing
}



/*
==============================================================================
Name: set_scale
Description:
	Sets the minimum and maximum of the axis.
	Axis is kept coherent, meaning that in log scale mode, minimum and maximum
	bounds are constrained to strictly positive values.
Input parameters:
	- min_val: Minimum value, < max_val
	- max_val: Maximum value, > min_val
Throws: Nothing
==============================================================================
*/

void	Axis::set_scale (double min_val, double max_val)
{
	assert (min_val < max_val);

	_min_val = min_val;
	_max_val = max_val;

	keep_consistent ();
}



/*
==============================================================================
Name: set_log_scale
Description:
	Sets the type of the scale (log or linear).
	Axis is kept coherent, meaning that in log scale mode, minimum and maximum
	bounds are constrained to strictly positive values.
Input parameters:
	- log_flag: true for a logarithmic scale, false for a linear one.
Throws: Nothing
==============================================================================
*/

void	Axis::set_log_scale (bool log_flag)
{
	_log_flag = log_flag;

	keep_consistent ();
}



/*
==============================================================================
Name: activate_tick
Description:
	Enable or disable intermediate grading of the axis.
Input parameters:
	- flag: True for showing ticks, false to hide them.
Throws: Nothing
==============================================================================
*/

void	Axis::activate_tick (bool flag)
{
	_tick_flag = flag;
}



/*
==============================================================================
Name: set_tick_org
Description:
	Sets the origin of the ticks, which are regularly spaced.
	Axis is kept coeherent, meaning that in log scale mode, tick origin is
	constrained to a strictly positive value.
Input parameters:
	- org: Value of a tick.
Throws: Nothing
==============================================================================
*/

void	Axis::set_tick_org (double org)
{
	_tick_org = org;

	keep_consistent ();
}



/*
==============================================================================
Name: set_tick_dist
Description:
	Only for linear display. Sets the distance between two ticks.
Input parameters:
	- dist: distance, > 0.
Throws: Nothing
==============================================================================
*/

void	Axis::set_tick_dist (double dist)
{
	assert (dist > 0);

	_tick_dist = dist;

	keep_consistent ();
}



/*
==============================================================================
Name: set_name
Description:
	Sets the name of the axis.
Input parameters:
	- name: The name.
Throws: std::string related exceptions.
==============================================================================
*/

void	Axis::set_name (const std::string &name)
{
	_name = name;
}



/*
==============================================================================
Name: get_scale
Description:
	Returns the minimum and maximum bounds of the axis.
Output parameters:
	- min_val: Minimum bound
	- max_val: Maximum bound
Throws: Nothing
==============================================================================
*/

void	Axis::get_scale (double &min_val, double &max_val) const
{
	min_val = _min_val;
	max_val = _max_val;
}



/*
==============================================================================
Name: is_log_scale
Description:
	Indicates if axis scale is logarithmic or linear.
Returns: true if log scale, false for linear.
Throws: Nothing
==============================================================================
*/

bool	Axis::is_log_scale () const
{
	return _log_flag;
}



/*
==============================================================================
Name: is_tick_active
Description:
	Indicates if tick marks are activated.
Returns: true if tick marks are activated.
Throws: Nothing
==============================================================================
*/

bool	Axis::is_tick_active () const
{
	return _tick_flag;
}



/*
==============================================================================
Name: get_tick_org
Description:
	Returns the value set as origin of the ticks.
Returns: The tick origin
Throws: Nothing
==============================================================================
*/

double	Axis::get_tick_org () const
{
	return _tick_org;
}



/*
==============================================================================
Name: get_tick_dist
Description:
	Returns the distance between two ticks in linear mode.
Returns: The distance, > 0
Throws: Nothing
==============================================================================
*/

double	Axis::get_tick_dist () const
{
	return _tick_dist;
}



/*
==============================================================================
Name: get_name
Description:
	Returns the axis name.
Returns: The axis name.
Throws: Nothing
==============================================================================
*/

std::string	Axis::get_name () const
{
	return _name;
}



/*
==============================================================================
Name: conv_val_to_pos
Description:
	Converts a value in the axis scale into a position in the graph. Graph
	border positions are [0 ; 1]
Input parameters:
	- val: The value. If log scale is activated, must be >= 0. 0 is assimilated
		to a very low value.
Returns: the position. May be out of the graph (out of [0 ; 1]).
Throws: Nothing
==============================================================================
*/

double	Axis::conv_val_to_pos (double val) const
{
	double			pos = 0;

	if (_log_flag)
	{
		assert (_max_val > 0);
		assert (_min_val > 0);
		assert (val >= 0);
		if (val == 0)
		{
			val = 1e-300;
		}
		const double   len_log = log10 (_max_val / _min_val);
		const double   val_log = log10 (val / _min_val);
		pos = val_log / len_log;
	}

	else
	{
		pos = (val - _min_val) / (_max_val - _min_val);
	}

	return pos;
}




/*
==============================================================================
Name: tick_begin
Description:
	Gets the first tick index.
Returns: First tick index, relative to the tick origin.
Throws: Nothing
==============================================================================
*/

long	Axis::tick_begin () const
{
	long           it = 0;

	if (_log_flag)
	{
		assert (_tick_org > 0);
		const double	ratio = _min_val / _tick_org;
		const double	min_log = floor (log10 (ratio));
		const double	min_pos = ratio / pow (10, min_log);
		it = static_cast <long> (min_log * 9 + ceil (min_pos) - 1);
	}

	else
	{
		const double	min_pos = (_min_val - _tick_org) / _tick_dist;
		it = static_cast <long> (ceil (min_pos));
	}

	return it;
}



/*
==============================================================================
Name: tick_end
Description:
	Gets the tick index past the last tick shown
Returns: The last tick index + 1
Throws: Nothing
==============================================================================
*/

long	Axis::tick_end () const
{
	long           last_tick = 0;

	if (_log_flag)
	{
		assert (_tick_org > 0);
		const double	ratio = _max_val / _tick_org;
		const double	max_log = floor (log10 (ratio));
		const double	max_pos = ratio / pow (10, max_log);
		last_tick = static_cast <long> (max_log * 9 + floor (max_pos) - 1);
	}

	else
	{
		const double	max_pos = (_max_val - _tick_org) / _tick_dist;
		last_tick = static_cast <long> (floor (max_pos));
	}

	return last_tick + 1;
}



/*
==============================================================================
Name: get_tick_val
Description:
	Returns the value correspounding to a given tick index
Input parameters:
	- it: tick index. May be shown or not.
Returns: Tick value
Throws: Nothing
==============================================================================
*/

double	Axis::get_tick_val (long it) const
{
	double          tick_val = 0;

	if (_log_flag)
	{
		const double	tens = floor (static_cast <double> (it) / 9);
		const double	units = static_cast <double> (it) - tens * 9 + 1;
		tick_val = units * pow (10, tens) * _tick_org;
	}

	else
	{
		tick_val = _tick_org + _tick_dist * it;
	}

	return tick_val;
}



/*
==============================================================================
Name: get_tick_pos
Description:
	Same as get_tick_val(), but the value is a position in the graph.
Input parameters:
	- it: tick index. May be shown or not.
Returns: tick position.
Throws: Nothing
==============================================================================
*/

double	Axis::get_tick_pos (long it) const
{
	return conv_val_to_pos (get_tick_val (it));
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Axis::keep_consistent ()
{
	if (_log_flag)
	{
		if (_max_val <= 0)
		{
			_min_val = 1;
			_max_val = 10;
		}
		else if (_min_val <= 0)
		{
			_min_val = _max_val / 10;
		}

		if (_tick_org <= 0)
		{
			_tick_org = 1;
		}
	}
}



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
