/*****************************************************************************

        TimerAccurate.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (TimerAccurate_CODEHEADER_INCLUDED)
#define TimerAccurate_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	TimerAccurate::reset ()
{
	_best = MaxResClock::duration::max ();
}



void	TimerAccurate::start ()
{
	_start = MaxResClock::now ();
}



void	TimerAccurate::stop ()
{
	const auto     now = MaxResClock::now ();
	const auto     dur = now - _start;
	if (dur < _best && dur.count () > 0)
	{
		_best = dur;
	}
}



void	TimerAccurate::stop_lap ()
{
	const auto     now = MaxResClock::now ();
	const auto     dur = now - _start;
	if (dur < _best && dur.count () > 0)
	{
		_best = dur;
	}
	_start = now;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



#endif   // TimerAccurate_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
