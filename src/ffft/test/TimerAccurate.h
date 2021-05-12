/*****************************************************************************

        TimerAccurate.h
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (ffft_test_TimerAccurate_HEADER_INCLUDED)
#define ffft_test_TimerAccurate_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <chrono>
#include <ratio>
#include <type_traits>



namespace ffft
{
namespace test
{



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

	inline void    reset () noexcept;
	inline void    start () noexcept;
	inline void    stop () noexcept;
	inline void    stop_lap () noexcept;
	inline MaxResClock::duration
	               get_best_duration () const noexcept;
	inline double  get_best_rate (long nbr_spl) const noexcept;



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



}  // namespace test
}  // namespace ffft



#include "ffft/test/TimerAccurate.hpp"



#endif   // ffft_test_TimerAccurate_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
