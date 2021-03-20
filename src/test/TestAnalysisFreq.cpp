/*****************************************************************************

        TestAnalysisFreq.cpp
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

#include "fstb/Approx.h"
#include "fstb/def.h"
#include "mfx/dsp/ana/FreqYin.h"
#include "mfx/dsp/ana/ValSmooth.h"
#include "mfx/FileOpWav.h"
#include "test/TestAnalysisFreq.h"
#include "test/TimerAccurate.h"

#include <algorithm>

#include <cassert>
#include <cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestAnalysisFreq::perform_test ()
{
	printf ("Testing mfx::dsp::ana::FreqYin...\n");

	double         sample_freq = 0;
	std::vector <std::vector <float> >	chn_arr;
	int            ret_val = mfx::FileOpWav::load (
		"../../../src/test/samples/guitar-02.wav", chn_arr, sample_freq
	);

	if (ret_val == 0)
	{
		const int      len = int (chn_arr [0].size ());
		std::vector <float>  dst_pitch (len);
		std::vector <float>  dst_sig (len);

		mfx::dsp::ana::FreqYin <mfx::dsp::ana::ValSmooth <float, 0> >  fdet;
		fdet.set_sample_freq (sample_freq);
		fdet.set_freq_bot (30);
		fdet.set_freq_top (1500);
		fdet.clear_buffers ();

		double         phi  = 0;
		const double   fmul = 2 * fstb::PI / sample_freq;
		float          freq = 0;
		const int      blk_size = 32;

		TimerAccurate  tim;
		tim.reset ();
		tim.start ();

		int            pos = 0;
		do
		{
			const int      work_len = std::min (blk_size, len - pos);
			const float    freq_tst = fdet.process_block (&chn_arr [0] [pos], work_len);

			const float    note =
				fstb::Approx::log2 (std::max (freq_tst, 1.f) * (1.f / 440)) * 12 + 69;

			for (int k = 0; k < work_len; ++k)
			{
				dst_sig [pos]   = fstb::Approx::sin_nick (float (phi)) * 0.25f;
				dst_pitch [pos] = (freq_tst <= 0) ? -1.f : note * (1 / 100.f);

				if (freq_tst > 0)
				{
					freq = freq_tst;
				}
				phi += freq * fmul;
				if (phi > fstb::PI)
				{
					phi -= 2 * fstb::PI;
				}
				++ pos;
			}
		}
		while (pos < len);

		tim.stop ();

		const double   spl_per_s = tim.get_best_rate (len);
		const double   kilo_sps  = spl_per_s / 1e3;
		const double   rt_mul    = spl_per_s / sample_freq;
		printf ("Speed: %12.3f kspl/s (x%.3f real-time).\n", kilo_sps, rt_mul);

		mfx::FileOpWav::save ("results/freqyin1a.wav", dst_sig, sample_freq, 0.5f);
		mfx::FileOpWav::save ("results/freqyin1b.wav", dst_pitch, sample_freq, 1.f);
	}

	printf ("%s\n\n", (ret_val == 0) ? "Done." : "*** Error ***");

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
