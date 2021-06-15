/*****************************************************************************

        TestSmooth.cpp
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



// Define this to allow random sampling instead of increasing-length sampling
// for the variable-sampling test
#undef test_TestSmooth_RANDOM_SAMPLING

/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/fnc.h"
#include "mfx/dsp/ctrl/Smooth.h"
#include "mfx/FileOpWav.h"
#include "test/TestSmooth.h"

#include <algorithm>
#include <functional>
#include <random>
#include <vector>

#include <cassert>
#include <cmath>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestSmooth::perform_test ()
{
	int            ret_val = 0;

	const double   sample_freq = 44100;
	const int      dt          = 1024; // Time discretisation, samples
	const int      nbr_steps   = 20;   // Value quantification, for the sawtooth

	// Builds a test signal:
	// - 4x sawtooth with increasing noise
	// - 4x sine
	const int      nbr_saw = 4;
	const int      nbr_sin = 4;
	const int      len_per = dt * nbr_steps;
	const int      len_saw = nbr_saw * len_per;
	const int      len_sin = nbr_sin * len_per;
	std::vector <float>  src (len_saw + len_sin);

	// Sawtooth
	const float    max_noise_amp = 1.f / nbr_steps;
	std::default_random_engine rnd_gen;
	std::uniform_real_distribution <float> dist_f (-max_noise_amp, max_noise_amp);
	auto           dice_f = std::bind (dist_f, rnd_gen);
	for (int per_cnt = 0; per_cnt < nbr_saw; ++per_cnt)
	{
		for (int step_cnt = 0; step_cnt < nbr_steps; ++step_cnt)
		{
			const int      stp_idx = per_cnt * nbr_steps + step_cnt;
			const int      ofs = stp_idx * dt;
			const float    val = float (step_cnt) / float (nbr_steps);
			const float    nza = float (stp_idx ) / float (nbr_saw * nbr_steps);
			for (int pos = 0; pos < dt; ++pos)
			{
				src [ofs + pos] =
					fstb::limit (val + dice_f () * nza, 0.0f, 1.0f) - 0.5f;
			}
		}
	}

	// Sine
	const float    sin_mul = float (2 * fstb::PI / len_per);
	const int      ofs_sin = len_saw;
	for (int pos = 0; pos < len_sin; ++pos)
	{
		src [ofs_sin + pos] = sinf (float (pos) * sin_mul) * 0.5f;
	}

	// Test
	std::vector <float>  dst (src.size ());
	mfx::dsp::ctrl::Smooth smth;
	smth.set_sample_freq (sample_freq);
	smth.set_base_freq (2.0f);
	smth.set_sensitivity (0.1f);

	smth.process_block (dst.data (), src.data (), len_saw);

#if defined (test_TestSmooth_RANDOM_SAMPLING)
	const int      block_len = 16;
	const int      max_update_time = 16;
	std::uniform_int_distribution <int> dist_i (1, max_update_time);
	auto           dice_i = std::bind (dist_i, rnd_gen);
#endif

	int            pos = 0;
	do
	{
#if defined (test_TestSmooth_RANDOM_SAMPLING)
		int            work_len = dice_i () * block_len;
#else
		int            work_len = std::max (1024 * pos / len_sin, 1);
#endif
		work_len = std::min (work_len, len_sin - pos);

		const float    val = smth.process_block (src [ofs_sin + pos], work_len);
		for (int i = 0; i < work_len; ++i)
		{
			dst [ofs_sin + pos + i] = val;
		}

		pos += work_len;
	}
	while (pos < len_sin);

	const float * const chn_arr [2] = { src.data (), dst.data () };
	mfx::FileOpWav::save (
		"results/smooth1.wav", chn_arr, int (dst.size ()), 2, sample_freq, 1.0f
	);

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
