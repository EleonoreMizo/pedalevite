/*****************************************************************************

        PerfClockCount.h
        Author: Laurent de Soras, 2022

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_PerfClockCount_HEADER_INCLUDED)
#define mfx_PerfClockCount_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



// Define this symbol to use the highest resolution timer, based on clock
// cycles. However it seems worsen the performances on ARM.
#undef mfx_PerfClockCount_USE_UNSAFE_CLOCK



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#if defined (mfx_PerfClockCount_USE_UNSAFE_CLOCK)
# include "fstb/ClockUnsafe.h"
#else
# include <chrono>
#endif



namespace mfx
{



#if defined (mfx_PerfClockCount_USE_UNSAFE_CLOCK)
	typedef fstb::ClockUnsafe::Counter PerfClockCount;
	typedef fstb::ClockUnsafe::Counter PerfClockCountVal;
	fstb_FORCEINLINE PerfClockCountVal get_clock_val (PerfClockCount c) noexcept
	{
		return c;
	}
#else
	typedef std::chrono::microseconds PerfClockCount;
	typedef PerfClockCount::rep PerfClockCountVal;
	fstb_FORCEINLINE PerfClockCountVal get_clock_val (PerfClockCount c) noexcept
	{
		return c.count ();
	}
#endif



}  // namespace mfx



//#include "mfx/PerfClockCount.hpp"



#endif   // mfx_PerfClockCount_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
