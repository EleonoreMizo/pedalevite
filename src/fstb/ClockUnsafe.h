/*****************************************************************************

        ClockUnsafe.h
        Author: Laurent de Soras, 2022

Clock based on the CPU cycle counter. It is fast, very fine grained, it
probably should be monotonic but nothing is really guaranteed.

On ARM, it requires enabling user access to the CCR register with a kernel
module. See also:
https://matthewarcus.wordpress.com/2018/01/27/using-the-cycle-counter-registers-on-the-raspberry-pi-3/

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (fstb_ClockUnsafe_HEADER_INCLUDED)
#define fstb_ClockUnsafe_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#if fstb_ARCHI == fstb_ARCHI_X86
# include <intrin.h>
#endif

#include <cstdint>



namespace fstb
{



class ClockUnsafe
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

#if fstb_ARCHI == fstb_ARCHI_ARM
	typedef uint32_t Counter;
#elif fstb_ARCHI == fstb_ARCHI_X86
	typedef uint64_t Counter;
#else
# error Unsupported architecture
#endif

	static fstb_FORCEINLINE Counter
	               read () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               ClockUnsafe ()                               = delete;
	               ClockUnsafe (const ClockUnsafe &other)       = delete;
	               ClockUnsafe (ClockUnsafe &&other)            = delete;
	ClockUnsafe &  operator = (const ClockUnsafe &other)        = delete;
	ClockUnsafe &  operator = (ClockUnsafe &&other)             = delete;
	bool           operator == (const ClockUnsafe &other) const = delete;
	bool           operator != (const ClockUnsafe &other) const = delete;

}; // class ClockUnsafe



}  // namespace fstb



#include "fstb/ClockUnsafe.hpp"



#endif   // fstb_ClockUnsafe_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
