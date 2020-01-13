/*****************************************************************************

        TimerAccurate.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TimerAccurate_HEADER_INCLUDED)
#define TimerAccurate_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#include <chrono>
#include <ratio>
#include <type_traits>



class TimerAccurate
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	using MaxResClock = std::conditional <
		std::ratio_less <
			std::chrono::system_clock::period,
			std::chrono::steady_clock::period
		>::value,
		std::chrono::system_clock,
		std::chrono::steady_clock
	>::type;

	fstb_FORCEINLINE void
						reset ();
	fstb_FORCEINLINE void
						start ();
	fstb_FORCEINLINE void
						stop ();
	fstb_FORCEINLINE void
						stop_lap ();
	MaxResClock::duration
	               get_best_duration () const;
	double			get_best_rate (long nbr_spl) const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	MaxResClock::time_point
	               _start;
	MaxResClock::time_point
	               _end;
	MaxResClock::duration
	               _best = MaxResClock::duration::max ();



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const TimerAccurate &other) const = delete;
	bool           operator != (const TimerAccurate &other) const = delete;

}; // class TimerAccurate



#include "test/TimerAccurate.hpp"



#endif   // TimerAccurate_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
