/*****************************************************************************

        TestBigMuffPi.cpp
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
#include "test/TestBigMuffPi.h"
#include "test/TimerAccurate.h"
#include "mfx/dsp/osc/SweepingSin.h"
#include "mfx/dsp/va/BigMuffPi.h"
#include "mfx/FileOpWav.h"

#include <memory>

#include <cassert>
#include <cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestBigMuffPi::perform_test ()
{
	int            ret_val = 0;

	printf ("Testing mfx::dsp::va::BigMuffPi...\n");

	const int      ovrspl      = 1;
	const double   sample_freq = 44100 * ovrspl;
	const int      ssin_len    = fstb::round_int (sample_freq * 10);

	std::vector <float>  src (ssin_len);

	// Sweeping sine
	mfx::dsp::osc::SweepingSin ssin (sample_freq, 20.0, 20000.0);
	ssin.generate (src.data (), ssin_len);

	// Sawtooth
	const int      saw_len = fstb::round_int (sample_freq * 1);
	for (int o = -3; o < 7; ++o)
	{
		const double   freq = 220 * pow (2.0, o + 3 / 12.0);
		gen_saw (src, sample_freq, freq, saw_len);
	}

	float          gain = 1.0f;

	const int      len = int (src.size ());
	std::vector <float>  dst (len);

	mfx::dsp::va::BigMuffPi muff;
	muff.set_sample_freq (sample_freq);
#if defined (mfx_dsp_va_dkm_Simulator_STATS)
	muff.reset_stats ();
#endif // mfx_dsp_va_dkm_Simulator_STATS

	printf ("Simulating...\n");
	fflush (stdout);

	TimerAccurate  tim;
	tim.reset ();
	tim.start ();

	for (int pos = 0; pos < len; ++pos)
	{
		float           x = src [pos] * gain;
#if 0 // Audio-rate modulation
		const int per = 337;
		muff.set_pot (
			mfx::dsp::va::BigMuffPi::Pot_TONE,
			0.5f + 0.5f * fstb::Approx::sin_nick_2pi (
				float (pos % per) * (1.f / float (per)) - 0.5f
			)
		);
#endif
		x = muff.process_sample (x);
		dst [pos] = x;
	}

	tim.stop ();
	const double      spl_per_s = tim.get_best_rate (len);

	mfx::FileOpWav::save ("results/bigmuffpi1.wav", dst, sample_freq, 0.5f);

	const double   kilo_sps  = spl_per_s / 1e3;
	const double   rt_mul    = spl_per_s / sample_freq;
	printf ("Speed: %12.3f kspl/s (x%.3f real-time).\n", kilo_sps, rt_mul);

#if defined (mfx_dsp_va_dkm_Simulator_STATS)
	print_stats (muff);
	muff.reset_stats ();
#endif // mfx_dsp_va_dkm_Simulator_STATS

	printf ("Done.\n\n");

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	TestBigMuffPi::gen_saw (std::vector <float> &data, double sample_freq, double freq, int len)
{
	const int      per = fstb::round_int (sample_freq / freq);
	for (int pos = 0; pos < len; ++pos)
	{
		const float    val = (pos % per) * (2.f / per) - 1.f;
		data.push_back (val);
	}
}



#if defined (mfx_dsp_va_dkm_Simulator_STATS)

void	TestBigMuffPi::print_stats (const mfx::dsp::va::BigMuffPi &muff)
{
	mfx::dsp::va::dkm::Simulator::Stats  st;
	muff.get_stats (st);

	print_stats (st, "global");
}



void	TestBigMuffPi::print_stats (mfx::dsp::va::dkm::Simulator::Stats &stats, const char *title_0)
{
	printf ("=== Iterations: %s ===\n", title_0);
	print_histo (
		stats._hist_it.data (),
		int (stats._hist_it.size ()),
		stats._nbr_spl_proc
	);

	printf ("\n");
}



void	TestBigMuffPi::print_histo (const int hist_arr [], int nbr_bars, int nbr_spl)
{
	int            bar_max = 0;
	int            total   = 0;
	for (int k = 0; k < nbr_bars; ++k)
	{
		const int       val = hist_arr [k];
		bar_max = std::max (bar_max, val);
		total  += val * k;
	}

	const int      bar_size = 64;
	char           bar_0 [bar_size+1];
	for (int k = 0; k < bar_size; ++k)
	{
		bar_0 [k] = '#';
	}
	bar_0 [bar_size] = '\0';

	const double   nbr_spl_inv = 1.0 / double (nbr_spl);
	const double   bar_scale   = double (bar_size) / double (bar_max);
	printf ("Average: %.2f\n", double (total) * nbr_spl_inv);
	for (int k = 0; k < nbr_bars; ++k)
	{
		const int      val = hist_arr [k];
		if (val > 0)
		{
			const double   prop = double (val) * nbr_spl_inv;
			printf ("%3d: %10d, %5.1f %% ", k, val, prop * 100);
			const int      bar_len = fstb::round_int (val * bar_scale);
			printf ("%s\n", bar_0 + bar_size - bar_len);
		}
	}
}

#endif // mfx_dsp_va_dkm_Simulator_STATS



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
