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



#if ! defined (hiir_test_TimerAccurate_CODEHEADER_INCLUDED)
#define hiir_test_TimerAccurate_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace hiir
{
namespace test
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	TimerAccurate::start () noexcept
{
	_start = MaxResClock::now ();
}



void	TimerAccurate::stop () noexcept
{
	const auto     now = MaxResClock::now ();
	const auto     dur = now - _start;
	if (dur < _best && dur.count () > 0)
	{
		_best = dur;
	}
}



TimerAccurate::MaxResClock::duration	TimerAccurate::get_best_duration () const noexcept
{
	assert (_best != MaxResClock::duration::max ());

	return _best;
}



double	TimerAccurate::get_best_rate (long nbr_spl) const noexcept
{
	static const double  per =
		  double (MaxResClock::duration::period::num)
		/ double (MaxResClock::duration::period::den);

	const double   dur  = double (get_best_duration ().count ()) * per;
	const double   rate = double (nbr_spl) / dur;

	return rate;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace test
}  // namespace hiir



#endif   // hiir_test_TimerAccurate_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
