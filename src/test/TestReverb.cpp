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

#include "mfx/dsp/spat/fv/FreeverbCore.h"
#include "mfx/dsp/spat/ltc/LatticeReverb.h"
#include "mfx/dsp/spat/DelayAllPass.h"
#include "mfx/dsp/spat/DelayFrac.h"
#include "mfx/dsp/spat/ReverbDattorro.h"
#include "mfx/dsp/spat/ReverbSC.h"
#include "mfx/FileOpWav.h"
#include "test/TestReverb.h"
#include "test/TimerAccurate.h"

#include <cassert>
#include <cmath>
#include <cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestReverb::perform_test ()
{
	int            ret_val = 0;

	printf ("Testing reverberators...\n");

	if (ret_val == 0)
	{
		ret_val = test_delay_frac ();
	}
	if (ret_val == 0)
	{
		ret_val = test_delay_apf ();
	}
	if (ret_val == 0)
	{
		ret_val = test_freeverb ();
	}
	if (ret_val == 0)
	{
		ret_val = test_reverbsc ();
	}
	if (ret_val == 0)
	{
		ret_val = test_latticereverb ();
	}
	if (ret_val == 0)
	{
		ret_val = test_reverbdattorro ();
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



static void	TestReverb_process_reverbsc (mfx::dsp::spat::ReverbSC <float> &reverb, std::vector <std::vector <float> > &dst_arr, const std::vector <std::vector <float> > &src_arr, int len)
{
	for (int pos = 0; pos < len; ++pos)
	{
		reverb.process_sample (
			&dst_arr [0] [pos], &dst_arr [1] [pos],
			src_arr [0] [pos], src_arr [1] [pos]
		);
	}
}



int	TestReverb::test_reverbsc ()
{
	int            ret_val = 0;

	printf ("ReverbSC\n");

	const double   sample_freq = 44100;

	mfx::dsp::spat::ReverbSC <float> reverb;

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

	TestReverb_process_reverbsc (reverb, dst_arr, src_arr, len);

	mfx::FileOpWav::save ("results/reverbsc0.wav", dst_arr, sample_freq, 0.5f);

	// Speed test
	TimerAccurate  tim;
	tim.reset ();
	tim.start ();
	for (int count = 0; count < 32; ++count)
	{
		TestReverb_process_reverbsc (reverb, dst_arr, src_arr, len);
		tim.stop_lap ();
	}

	const double   spl_per_s = tim.get_best_rate (len);
	const double   kilo_sps  = spl_per_s / 1e3;
	const double   rt_mul    = spl_per_s / sample_freq;
	printf ("Speed: %12.3f kspl/s (x%.3f real-time).\n", kilo_sps, rt_mul);

	return ret_val;
}



static void	TestReverb_process_reverbdattorro (mfx::dsp::spat::ReverbDattorro &reverb, std::vector <std::vector <float> > &dst_arr, const std::vector <std::vector <float> > &src_arr, int len)
{
	for (int pos = 0; pos < len; ++pos)
	{
		std::tie (dst_arr [0] [pos], dst_arr [1] [pos]) = 
			reverb.process_sample (src_arr [0] [pos], src_arr [1] [pos]);
	}
}



int	TestReverb::test_reverbdattorro ()
{
	int            ret_val = 0;

	printf ("ReverbDattorro\n");

	const double   sample_freq = 44100;

	mfx::dsp::spat::ReverbDattorro reverb;

	reverb.set_sample_freq (sample_freq);
	reverb.set_decay (0.9f);
	reverb.set_room_size (1.f);
	reverb.set_shimmer_pitch (+100);
//	reverb.set_diffusion_input (0);
//	reverb.set_diffusion_tank (0);
//	reverb.set_filter_input_bp (1000, float (sample_freq * 0.499));
//	reverb.set_filter_tank_bp (1000, float (sample_freq * 0.499));

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
	for (int pos = 0; pos < 5000; ++ pos)
	{
		src_arr [0] [imp_pos + pos] = 0.25f * float (sin (pos * 2 * fstb::PI * 440 / sample_freq));
	}

	TestReverb_process_reverbdattorro (reverb, dst_arr, src_arr, len);

	mfx::FileOpWav::save ("results/reverbdattorro0.wav", dst_arr, sample_freq, 0.5f);

	// Speed test
	TimerAccurate  tim;
	tim.reset ();
	tim.start ();
	for (int count = 0; count < 8; ++count)
	{
		TestReverb_process_reverbdattorro (reverb, dst_arr, src_arr, len);
		tim.stop_lap ();
	}

	const double   spl_per_s = tim.get_best_rate (len);
	const double   kilo_sps  = spl_per_s / 1e3;
	const double   rt_mul    = spl_per_s / sample_freq;
	printf ("Speed: %12.3f kspl/s (x%.3f real-time).\n", kilo_sps, rt_mul);

	return ret_val;
}



static void	TestReverb_process_latticereverb (mfx::dsp::spat::ltc::LatticeReverb <float, 16> &reverb, std::vector <std::vector <float> > &dst_arr, const std::vector <std::vector <float> > &src_arr, int len)
{
	for (int pos = 0; pos < len; ++pos)
	{
		std::tie (dst_arr [0] [pos], dst_arr [1] [pos]) =
			reverb.process_sample (src_arr [0] [pos], src_arr [1] [pos]);
	}
}



int	TestReverb::test_latticereverb ()
{
	int            ret_val = 0;

	printf ("Lattice reverb\n");

	const double   sample_freq = 44100;

	mfx::dsp::spat::ltc::LatticeReverb <float, 16> reverb;

	reverb.set_sample_freq (sample_freq);
	reverb.clear_buffers ();

	constexpr float   vol  = 1.f;
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

	TestReverb_process_latticereverb (reverb, dst_arr, src_arr, len);

	mfx::FileOpWav::save ("results/latticereverb0.wav", dst_arr, sample_freq, 0.5f);

	// Speed test
	TimerAccurate  tim;
	tim.reset ();
	tim.start ();
	for (int count = 0; count < 8; ++count)
	{
		TestReverb_process_latticereverb (reverb, dst_arr, src_arr, len);
		tim.stop_lap ();
	}

	const double   spl_per_s = tim.get_best_rate (len);
	const double   kilo_sps  = spl_per_s / 1e3;
	const double   rt_mul    = spl_per_s / sample_freq;
	printf ("Speed: %12.3f kspl/s (x%.3f real-time).\n", kilo_sps, rt_mul);

	return ret_val;
}



int	TestReverb::test_delay_frac ()
{
	int            ret_val = 0;

	printf ("DelayFrac\n");

	constexpr double  sample_freq = 44100;

	const int      len = fstb::round_int (sample_freq * 10);
	std::vector <float>  src (len);
	std::vector <float>  dst (len);
	constexpr int     p0 = 100;
	constexpr double  p1 = sample_freq / 15000.f;
	for (int pos = 0; pos < len; ++pos)
	{
		if (pos < len / 2)
		{
			src [pos] = ((pos % p0) < (p0 / 2)) ? 1.f : -1.f;
		}
		else if (pos < len * 3 / 4)
		{
			src [pos] = float (sin (pos * (2 * fstb::PI / double (p0))));
		}
		else
		{
			src [pos] = float (sin (pos * (2 * fstb::PI / p1)));
		}
	}

	typedef mfx::dsp::spat::DelayFrac <float, 6> Delay;
	Delay          delay;
	constexpr int  max_dly_spl = 50;
	delay.set_max_len (max_dly_spl);
	int            dly_fix = fstb::round_int (1.875f * Delay::_nbr_phases);
	delay.set_delay_fix (dly_fix);
	int            dir = 1;
	int            pos = 0;
	while (pos < len)
	{
		int            work_len   = 1;
		const int      max_blk_len = delay.get_max_block_len ();
		if (pos < len * 5 / 8)
		{
			dst [pos] = delay.process_sample (src [pos]);
		}
		else
		{
			if (max_blk_len <= 0)
			{
				work_len = 0;
			}
			else
			{
				work_len = std::min (len - pos, max_blk_len);
				delay.read_block (&dst [pos], work_len);
				delay.write_block (&src [pos], work_len);
				delay.step_block (work_len);
			}
		}
		if (pos >= 1000)
		{
			if (   dly_fix <= Delay::_delay_min * Delay::_nbr_phases
			    || dly_fix >= max_dly_spl       * Delay::_nbr_phases)
			{
				dir = -dir;
			}
			dly_fix += dir;
			delay.set_delay_fix (dly_fix);
		}

		pos += work_len;
	}

	mfx::FileOpWav::save ("results/delayfrac0.wav", src, sample_freq, 0.5f);
	mfx::FileOpWav::save ("results/delayfrac1.wav", dst, sample_freq, 0.5f);

	// Speed test
	delay.set_max_len (256);
	constexpr int  len_block = 87;
	delay.set_delay_flt (len_block + 16);
	TimerAccurate  tim;
	{
		tim.reset ();
		tim.start ();
		for (int count = 0; count < 256; ++count)
		{
			for (pos = 0; pos < len; ++pos)
			{
				dst [pos] = delay.process_sample (src [pos]);
			}
			tim.stop_lap ();
		}

		const double   spl_per_s = tim.get_best_rate (len);
		const double   kilo_sps  = spl_per_s / 1e3;
		const double   rt_mul    = spl_per_s / sample_freq;
		printf ("Speed (sample): %12.3f kspl/s (x%.3f real-time).\n", kilo_sps, rt_mul);
	}
	{
		tim.reset ();
		tim.start ();
		for (int count = 0; count < 32; ++count)
		{
			pos = 0;
			do
			{
				const int      work_len = std::min (len - pos, len_block);
				delay.read_block (&dst [pos], work_len);
				delay.write_block (&src [pos], work_len);
				delay.step_block (work_len);
				pos += work_len;
			}
			while (pos < len);
			tim.stop_lap ();
		}

		const double   spl_per_s = tim.get_best_rate (len);
		const double   kilo_sps  = spl_per_s / 1e3;
		const double   rt_mul    = spl_per_s / sample_freq;
		printf ("Speed (block): %12.3f kspl/s (x%.3f real-time).\n", kilo_sps, rt_mul);
	}

	return ret_val;
}



int	TestReverb::test_delay_apf ()
{
	int            ret_val = 0;

	printf ("DelayAllPass\n");

	constexpr double  sample_freq = 44100;

	const int      len = fstb::round_int (sample_freq * 10);
	std::vector <float>  src (len);
	std::vector <float>  dst (len);
	constexpr int     p0 = 100;
	constexpr double  p1 = sample_freq / 15000.f;
	for (int pos = 0; pos < len; ++pos)
	{
		if (pos < len / 2)
		{
			src [pos] = ((pos % p0) < (p0 / 2)) ? 1.f : -1.f;
		}
		else if (pos < len * 3 / 4)
		{
			src [pos] = float (sin (pos * (2 * fstb::PI / double (p0))));
		}
		else
		{
			src [pos] = float (sin (pos * (2 * fstb::PI / p1)));
		}
	}

	typedef mfx::dsp::spat::DelayAllPass <float, 6> Delay;
	Delay          delay;
	delay.set_coef (0.5f);
	constexpr int  max_dly_spl = 50;
	delay.set_max_len (max_dly_spl);
	int            dly_fix = fstb::round_int (2.875f * Delay::_nbr_phases);
	delay.set_delay_fix (dly_fix);
	int            dir = 1;
	int            pos = 0;
	while (pos < len)
	{
		int            work_len   = 1;
		const int      max_blk_len = delay.get_max_block_len ();
		if (pos < len * 5 / 8)
		{
			dst [pos] = delay.process_sample (src [pos]);
		}
		else
		{
			if (max_blk_len <= 0)
			{
				work_len = 0;
			}
			else
			{
				work_len = std::min (len - pos, max_blk_len);
				delay.process_block (&dst [pos], &src [pos], work_len);
			}
		}
		if (pos >= 1000)
		{
			if (   dly_fix <= Delay::_delay_min * Delay::_nbr_phases
			    || dly_fix >= max_dly_spl       * Delay::_nbr_phases)
			{
				dir = -dir;
			}
			dly_fix += dir;
			delay.set_delay_fix (dly_fix);
		}

		pos += work_len;
	}

	mfx::FileOpWav::save ("results/delayapf0.wav", dst, sample_freq, 0.5f);

	// Speed test
	delay.set_max_len (256);
	constexpr int  len_block = 87;
	delay.set_delay_flt (len_block + 16);
	TimerAccurate  tim;
	{
		tim.reset ();
		tim.start ();
		for (int count = 0; count < 256; ++count)
		{
			for (pos = 0; pos < len; ++pos)
			{
				dst [pos] = delay.process_sample (src [pos]);
			}
			tim.stop_lap ();
		}

		const double   spl_per_s = tim.get_best_rate (len);
		const double   kilo_sps  = spl_per_s / 1e3;
		const double   rt_mul    = spl_per_s / sample_freq;
		printf ("Speed (sample): %12.3f kspl/s (x%.3f real-time).\n", kilo_sps, rt_mul);
	}
	{
		tim.reset ();
		tim.start ();
		for (int count = 0; count < 32; ++count)
		{
			pos = 0;
			do
			{
				const int      work_len = std::min (len - pos, len_block);
				delay.process_block (&dst [pos], &src [pos], work_len);
				pos += work_len;
			}
			while (pos < len);
			tim.stop_lap ();
		}

		const double   spl_per_s = tim.get_best_rate (len);
		const double   kilo_sps  = spl_per_s / 1e3;
		const double   rt_mul    = spl_per_s / sample_freq;
		printf ("Speed (block): %12.3f kspl/s (x%.3f real-time).\n", kilo_sps, rt_mul);
	}

	return ret_val;
}



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
