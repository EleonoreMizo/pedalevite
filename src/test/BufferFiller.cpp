/*****************************************************************************

        BufferFiller.cpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "BufferFiller.h"

#include <cassert>
#include <cstdlib>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// Generates random numbers in ]-1; -0.5[ U ]0.5; 1[
void	BufferFiller::gen_rnd_non_zero (float buf_ptr [], int nbr_spl)
{
	assert (buf_ptr != 0);
	assert (nbr_spl > 0);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		const int      rnd = rand ();
		const float    mag = 0.5f + (float (rnd & 0xFFF) + 0.5f) / 0x1000;
		const int      sgn = ((rnd >> 12) & 2) - 1;
		const float    val = mag * sgn;
		buf_ptr [pos] = val;
	}
}



// Generates random numbers in [0; 1[
void	BufferFiller::gen_rnd_positive (float buf_ptr [], int nbr_spl)
{
	assert (buf_ptr != 0);
	assert (nbr_spl > 0);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		const int      rnd = rand ();
		buf_ptr [pos] = float (rnd & 0xFFFF) / 0x10000;
	}
}



void	BufferFiller::gen_rnd_scaled (float buf_ptr [], int nbr_spl, float min_val, float max_val)
{
	assert (buf_ptr != 0);
	assert (nbr_spl > 0);
	assert (min_val <= max_val);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		const int      rnd = rand ();
		buf_ptr [pos] =
			min_val + float (rnd & 0xFFFF) * (max_val - min_val) / 0x10000;
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
