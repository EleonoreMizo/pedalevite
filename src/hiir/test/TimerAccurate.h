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
#if ! defined (hiir_test_TimerAccurate_HEADER_INCLUDED)
#define hiir_test_TimerAccurate_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "hiir/def.h"

#include <chrono>
#include <ratio>
#include <type_traits>



namespace hiir
{
namespace test
{



class TimerAccurate
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	using MaxResClock = typename std::conditional <
		std::ratio_less <
			std::chrono::system_clock::period,
			std::chrono::steady_clock::period
		>::value,
		std::chrono::system_clock,
		std::chrono::steady_clock
	>::type;

	               TimerAccurate ()  = default;
	virtual        ~TimerAccurate () = default;

	hiir_FORCEINLINE void
	               start () noexcept;
	hiir_FORCEINLINE void
	               stop () noexcept;
	MaxResClock::duration
	inline         get_best_duration () const noexcept;
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

	               TimerAccurate (const TimerAccurate &other)     = delete;
	               TimerAccurate (TimerAccurate &&other)          = delete;
	TimerAccurate &
	               operator = (const TimerAccurate &other)        = delete;
	TimerAccurate &
	               operator = (TimerAccurate &&other)             = delete;
	bool           operator == (const TimerAccurate &other) const = delete;
	bool           operator != (const TimerAccurate &other) const = delete;

}; // class TimerAccurate



}  // namespace test
}  // namespace hiir



#include "hiir/test/TimerAccurate.hpp"



#endif   // hiir_test_TimerAccurate_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
