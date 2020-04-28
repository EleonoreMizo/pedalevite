/*****************************************************************************

        TestPinkShade.cpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"
#include "test/TestPinkShade.h"
#include "mfx/dsp/nz/PinkShade.h"
#include "mfx/FileOpWav.h"

#include <cassert>
#include <cstdlib>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestPinkShade::perform_test ()
{
	int            ret_val = 0;

	printf ("Testing mfx::dsp::nz::PinkShade...\n");

	const double   sample_freq = 44100;
	const int      len         = fstb::round_int (sample_freq * 10);
	mfx::dsp::nz::PinkShade gen;

	std::vector <float> dst (len);
	gen.process_block (dst.data (), len);

	mfx::FileOpWav::save ("results/pinkshade1.wav", dst, sample_freq, 0.5f);

	printf ("Done.\n");

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
