/*****************************************************************************

        SweepingSine.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (hiir_test_SweepingSine_CODEHEADER_INCLUDED)
#define hiir_test_SweepingSine_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "hiir/def.h"

#include <cassert>
#include <cmath>



namespace hiir
{
namespace test
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T>
void	SweepingSine::generate (T dest_ptr []) const
{
	assert (dest_ptr != nullptr);

	const long double freq_mult_l = log (_freq_end / _freq_beg) / _nbr_spl;
	const long double freq_mult   = exp (double (freq_mult_l));
	long double    freq_base = _freq_beg / _sample_freq * (2 * hiir::PI);
	long double    freq      = freq_base;
	long double    phase     = 0;
	long           pos       = 0;
	do
	{
		if ((pos & 0x0FF) == 0)
		{
			freq = freq_base * exp (double (pos) * double (freq_mult_l));
		}

		dest_ptr [pos] = static_cast <T> (sin (double (phase)));
		phase += freq;
		if (phase > hiir::PI)
		{
			phase -= 2 * hiir::PI;
		}
		freq *= freq_mult;
		++ pos;
	}
	while (pos < _nbr_spl);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace test
}  // namespace hiir



#endif   // hiir_test_SweepingSine_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
