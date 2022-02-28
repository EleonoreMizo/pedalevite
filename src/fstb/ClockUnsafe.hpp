/*****************************************************************************

        ClockUnsafe.hpp
        Author: Laurent de Soras, 2022

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_ClockUnsafe_CODEHEADER_INCLUDED)
#define fstb_ClockUnsafe_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace fstb
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ClockUnsafe::Counter	ClockUnsafe::read () noexcept
{
#if fstb_ARCHI == fstb_ARCHI_ARM

	Counter        x {};
# if fstb_WORD_SIZE == 64
	__asm__ volatile ("mrs %0, pmccntr_el0\t\n":"=r" (x));
# else
	__asm__ volatile ("mrc p15, 0, %0, c9, c13, 0":"=r" (x));
# endif // fstb_WORD_SIZE

#else // fstb_ARCHI_X86

	const auto     x = static_cast <Counter> (__rdtsc ());

#endif // fstb_ARCHI

	return x;
}

/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace fstb



#endif   // fstb_ClockUnsafe_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
