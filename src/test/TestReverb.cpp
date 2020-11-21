/*****************************************************************************

        TestReverb.cpp
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

#include "mfx/dsp/spat/Bigverb.h"
#include "mfx/dsp/spat/fv/FreeverbCore.h"
#include "mfx/FileOpWav.h"
#include "test/TestReverb.h"
#include "test/TimerAccurate.h"

#include <cassert>
#include <cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestReverb::perform_test ()
{
	int            ret_val = 0;

	printf ("Testing reverberators...\n");

	if (ret_val == 0)
	{
		ret_val = test_freeverb ();
	}
	if (ret_val == 0)
	{
		ret_val = test_bigverb ();
	}

	printf ("Done.\n");

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



static void	TestReverb_process_freeverb (mfx::dsp::spat::fv::FreeverbCore &reverb, std::vector <std::vector <float> > &dst_arr, const std::vector <std::vector <float> > &src_arr, int len)
{
	for (int pos = 0; pos < len; )
	{
		const int      block_len = std::min (len - pos, TestReverb::_max_block_len);
		for (int chn = 0; chn < TestReverb::_nbr_chn; ++chn)
		{
			reverb.process_block (
				&dst_arr [chn] [pos], &src_arr [chn] [pos], block_len, chn
			);
		}
		pos += block_len;
	}
}



int	TestReverb::test_freeverb ()
{
	int            ret_val = 0;

	printf ("Freeverb\n");

	const double   sample_freq = 44100;

	mfx::dsp::spat::fv::FreeverbCore reverb;

	reverb.reset (sample_freq, _max_block_len);
	reverb.set_damp (0.10f);
	reverb.set_reflectivity (0.975f);

	constexpr float   vol  = 1.f / 5.62f; // -15 dB
	const int      len     = fstb::round_int (sample_freq * 10);
	const int      imp_pos = fstb::round_int (sample_freq * 1);
	std::vector <std::vector <float> >  src_arr (_nbr_chn);
	std::vector <std::vector <float> >  dst_arr (_nbr_chn);
	for (int chn = 0; chn < _nbr_chn; ++chn)
	{
		src_arr [chn].assign (len, 0);
		dst_arr [chn].resize (len);

		src_arr [chn] [imp_pos] = vol;
	}
	src_arr [0] [len - imp_pos] = vol;

	TestReverb_process_freeverb (reverb, dst_arr, src_arr, len);

	mfx::FileOpWav::save ("results/freeverb0.wav", dst_arr, sample_freq, 0.5f);

	// Speed test
	TimerAccurate  tim;
	tim.reset ();
	tim.start ();
	for (int count = 0; count < 32; ++count)
	{
		TestReverb_process_freeverb (reverb, dst_arr, src_arr, len);
		tim.stop_lap ();
	}

	const double   spl_per_s = tim.get_best_rate (len);
	const double   kilo_sps  = spl_per_s / 1e3;
	const double   rt_mul    = spl_per_s / sample_freq;
	printf ("Speed: %12.3f kspl/s (x%.3f real-time).\n", kilo_sps, rt_mul);

	return ret_val;
}



static void	TestReverb_process_bigverb (mfx::dsp::spat::Bigverb <float> &reverb, std::vector <std::vector <float> > &dst_arr, const std::vector <std::vector <float> > &src_arr, int len)
{
	for (int pos = 0; pos < len; ++pos)
	{
		reverb.process_sample (
			&dst_arr [0] [pos], &dst_arr [1] [pos],
			src_arr [0] [pos], src_arr [1] [pos]
		);
	}
}



int	TestReverb::test_bigverb ()
{
	int            ret_val = 0;

	printf ("Bigverb\n");

	const double   sample_freq = 44100;

	mfx::dsp::spat::Bigverb <float> reverb;

	reverb.set_sample_freq (sample_freq);
	reverb.set_cutoff (float (sample_freq * 0.499));
	reverb.set_size (0.95f);

	constexpr float   vol  = 5.62f; // +15 dB
	const int      len     = fstb::round_int (sample_freq * 10);
	const int      imp_pos = fstb::round_int (sample_freq * 1);
	std::vector <std::vector <float> >  src_arr (_nbr_chn);
	std::vector <std::vector <float> >  dst_arr (_nbr_chn);
	for (int chn = 0; chn < _nbr_chn; ++chn)
	{
		src_arr [chn].assign (len, 0);
		dst_arr [chn].resize (len);

		src_arr [chn] [imp_pos] = vol;
	}
	src_arr [0] [len - imp_pos] = vol;

	TestReverb_process_bigverb (reverb, dst_arr, src_arr, len);

	mfx::FileOpWav::save ("results/bigverb0.wav", dst_arr, sample_freq, 0.5f);

	// Speed test
	TimerAccurate  tim;
	tim.reset ();
	tim.start ();
	for (int count = 0; count < 32; ++count)
	{
		TestReverb_process_bigverb (reverb, dst_arr, src_arr, len);
		tim.stop_lap ();
	}

	const double   spl_per_s = tim.get_best_rate (len);
	const double   kilo_sps  = spl_per_s / 1e3;
	const double   rt_mul    = spl_per_s / sample_freq;
	printf ("Speed: %12.3f kspl/s (x%.3f real-time).\n", kilo_sps, rt_mul);

	return ret_val;
}



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
