/*****************************************************************************

        TestDesignPhaseMin.cpp
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
#include "mfx/dsp/fir/DesignPhaseMin.h"
#include "mfx/dsp/osc/SweepingSin.h"
#include "mfx/FileOpWav.h"
#include "test/TestDesignPhaseMin.h"

#include <cassert>
#include <cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestDesignPhaseMin::perform_test ()
{
	int            ret_val = 0;

	printf ("Testing mfx::dsp::fir::DesignPhaseMin...\n");

	// Design
	mfx::dsp::fir::DesignPhaseMin designer;
	const int      opt_fft_len = designer.compute_optimal_fft_length (0.25, 7, 1e-3);
	printf ("Optimal FFT length: %d\n", opt_fft_len);
	designer.set_fft_len (opt_fft_len);

	const int      fir_len   = 21;
	const int      fir_len_h = (fir_len + 1) / 2;
	const float    linear [fir_len] = 
	{
		+0.004274756894f, +0.013786530730f, +0.014075832686f, +0.001396230179f,
		-0.026088970251f, -0.046594164063f, -0.030938759181f, +0.038911015147f,
		+0.147489163123f, +0.248148585340f, +0.289153790256f, +0.248148585340f,
		+0.147489163123f, +0.038911015147f, -0.030938759181f, -0.046594164063f,
		-0.026088970251f, +0.001396230179f, +0.014075832686f, +0.013786530730f,
		+0.004274756894f
	};
	const float    min_ref [fir_len_h] =
	{
		+0.146496327920f, +0.244555927329f, +0.310187106952f, +0.275577863910f,
		+0.148610393777f, +0.006819135370f, -0.084289504670f, -0.071903711683f,
		-0.041298574516f, +0.045192401396f, +0.029048971869f
	};
	float          min_test [fir_len_h];
	designer.minimize_phase (min_test, linear, fir_len);

   for (int pos = 0; pos < fir_len_h; ++pos)
   {
		printf ("%f", min_test [pos]);
      if (! fstb::is_eq_rel (min_test [pos], min_ref [pos], 0.001f))
      {
         printf (" Incorrect value (ref: %f)", min_ref [pos]);
         ret_val = -1;
      }
      printf ("\n");
   }

	designer.release_buffers ();

	// Filtering
	constexpr double  sample_freq = 44100;
	const int         len         = fstb::round_int (sample_freq * 10);

	std::vector <float>  src (len);
	mfx::dsp::osc::SweepingSin ssin (sample_freq, 20.0, 20000.0);
	ssin.generate (src.data (), len);

	std::vector <float>  dst_lin (len);
	std::vector <float>  dst_min (len);
	for (int pos = 0; pos < len; ++pos)
	{
		dst_lin [pos] = fir_process_sample (src, pos, linear, fir_len);
		dst_min [pos] = fir_process_sample (src, pos, min_test, fir_len_h);
	}

	mfx::FileOpWav::save ("results/designphasemin_lin.wav", dst_lin, sample_freq, 0.5f);
	mfx::FileOpWav::save ("results/designphasemin_min.wav", dst_min, sample_freq, 0.5f);

	printf ("Done.\n\n");

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



float	TestDesignPhaseMin::fir_process_sample (const std::vector <float> &src, int pos, const float fir_ptr [], int fir_len)
{
	assert (pos >= 0);
	assert (pos < int (src.size ()));
	assert (fir_ptr != nullptr);
	assert (fir_len > 0);

	float          sum = src [pos] * fir_ptr [0];
	for (int k = 1; k < fir_len; ++k)
	{
		const int      pos_k = pos - k;
		if (pos_k > 0)
		{
			sum += src [pos_k] * fir_ptr [k];
		}
	}

	return sum;
}



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
