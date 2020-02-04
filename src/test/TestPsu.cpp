/*****************************************************************************

        TestPsu.cpp
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

#include "fstb/Approx.h"
#include "fstb/fnc.h"
#include "mfx/dsp/osc/SweepingSin.h"
#include "mfx/pi/dist3/Psu.h"
#include "mfx/FileOpWav.h"
#include "test/TestPsu.h"

#include	<vector>

#include <cassert>
#include <cstdint>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestPsu::perform_test ()
{
	const double   sample_freq = 44100;

	mfx::pi::dist3::Psu  psu;
	psu.set_sample_freq (sample_freq);
	psu.set_half_cycle_relative_lvl (0.25f);
	psu.set_ac_freq (50);
	psu.set_ac_lvl (1);

	const int      len = fstb::round_int (sample_freq * 10);
	std::vector <float>  src (len);
	std::vector <float>  dst (len);

	// Sweeping square wave
	{
		uint32_t       phase   = 0;
		const float    per_beg = float ((1ULL << 32) / (sample_freq * 0.5));
		const double   rate    = 8 / double (len);
		for (int pos = 0; pos < len; ++pos)
		{
			src [pos] = 1 - float (phase >> 31) * 2;
			const uint32_t step = uint32_t (per_beg * exp2 (pos * rate));
			phase += step;
		}
	}

	psu.process_block (dst.data (), src.data (), len);

	mfx::FileOpWav::save ("results/psu0.wav", dst, sample_freq, 0.5f);

	// Modulated sweeping sine
	{
		mfx::dsp::osc::SweepingSin ssin (sample_freq, 20.0, 20000.0);
		ssin.generate (src.data (), len);
		int32_t        phase = 0;
		const float    smul  = float (1ULL << 32);
		const float    pmul  = 1.f / smul;
		const int32_t  step  = fstb::round_int (0.25f / sample_freq * smul);
		for (int pos = 0; pos < len; ++pos)
		{
			src [pos] *=
				  fstb::Approx::sin_nick_2pi (float (phase) * pmul) * 2.f;
			phase += step;
		}
	}

	psu.process_block (dst.data (), src.data (), len);

	mfx::FileOpWav::save ("results/psu1.wav", dst, sample_freq, 0.5f);

	return 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
