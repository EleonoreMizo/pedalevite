/*****************************************************************************

        TestVelvetConv.cpp
        Author: Laurent de Soras, 2021

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
#include "mfx/dsp/grn/VelvetConv.h"
#include "mfx/dsp/wnd/ProcHann.h"
#include "mfx/FileOpWav.h"
#include "test/TestVelvetConv.h"
#include "test/TimerAccurate.h"

#include <cassert>
#include <cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestVelvetConv::perform_test ()
{
	int            ret_val = 0;

	printf ("Testing mfx::dsp::grn::VelvetConv <float>...\n");

	constexpr double  sample_freq = 44100.0;   // Sample frequency
	const int      len = fstb::round_int (sample_freq * 10);
	std::vector <float>  dst (len);

	constexpr int  grain_size = 2048;
	std::array <float, grain_size> grain;
	constexpr float   lvl = 0.1f;
#if 0
	grain.fill (lvl);
#else
	for (int k = 0; k < grain_size; ++k)
	{
		grain [k] = lvl * sinf (k * float (2 * fstb::PI * 997 / sample_freq));
	}
#endif
	mfx::dsp::wnd::ProcHann <float> hann;
	hann.setup (grain_size);
	hann.process_frame_mul (grain.data ());

	mfx::dsp::grn::VelvetConv <float> velvet_conv;
	velvet_conv.set_granule (grain.data (), int (grain.size ()), false);
	velvet_conv.set_density (30.f);

	TimerAccurate  tim;
	tim.reset ();

	constexpr int     nbr_passes = 16;
	tim.start ();
	for (int p = 0; p < nbr_passes; ++p)
	{
		velvet_conv.process_block (dst.data (), len);
		tim.stop_lap ();
	}

	const double   spl_per_s = tim.get_best_rate (len);
	const double   mega_sps  = spl_per_s / 1e6;
	const double   rt_mul    = spl_per_s / sample_freq;
	printf ("Speed: %12.3f Mspl/s (x%.3f real-time).\n", mega_sps, rt_mul);

	mfx::FileOpWav::save ("results/velvetconv1.wav", dst, sample_freq, 0.5f);

	printf ("Done.\n\n");

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
