/*****************************************************************************

        TestHardclipBl.cpp
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
#include "fstb/SingleObj.h"
#include "hiir/PolyphaseIir2Designer.h"
#include "mfx/dsp/iir/Downsampler4xSimd.h"
#include "mfx/dsp/iir/Upsampler4xSimd.h"
#include "mfx/dsp/osc/SweepingSin.h"
#include "mfx/dsp/shape/HardclipBl.h"
#include "mfx/FileOpWav.h"
#include "test/TestHardclipBl.h"
#include "test/TimerAccurate.h"

#include <cassert>
#include <cmath>
#include <cstdio>



// 1 or 4
#define TestHardclipBl_OVRSPL (4)



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestHardclipBl::perform_test ()
{
	int            ret_val = 0;

	printf ("Testing dsp::shape::HardclipBl...\n");

	const double   sample_freq = 44100;
	const int      ssin_len = fstb::round_int (sample_freq * 10);

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

	const int      len = int (src.size ());
	std::vector <float>  dst (len);

	const int      nbr_coef_42 = 3;
	const int      nbr_coef_21 = 8;
	struct UpDown
	{
		mfx::dsp::iir::Upsampler4xSimd <nbr_coef_42, nbr_coef_21>   _up;
		mfx::dsp::iir::Downsampler4xSimd <nbr_coef_42, nbr_coef_21> _dw;
	};
	fstb::SingleObj <UpDown> updw;
	double coef_42 [nbr_coef_42];
	double coef_21 [nbr_coef_21];
		hiir::PolyphaseIir2Designer::compute_coefs_spec_order_tbw (
			&coef_42 [0], nbr_coef_42, 1.0 / 5
		);
		hiir::PolyphaseIir2Designer::compute_coefs_spec_order_tbw (
			&coef_21 [0], nbr_coef_21, 1.0 / 100
		);
	updw->_up.set_coefs (coef_42, coef_21);
	updw->_dw.set_coefs (coef_42, coef_21);

	mfx::dsp::shape::HardclipBl   hclip;

	float          gain = 100;

	// Naive clipping (for reference)
	for (int pos = 0; pos < len; ++pos)
	{
		float           x = src [pos] * gain;
#if (TestHardclipBl_OVRSPL == 1) // W/o oversampling
		x = fstb::limit (x, -1.f, 1.f);
#else // With oversampling
		float       tmp [TestHardclipBl_OVRSPL];
		updw->_up.process_sample (tmp, x);
		for (int k = 0; k < TestHardclipBl_OVRSPL; ++k)
		{
			tmp [k] = fstb::limit (tmp [k], -1.f, 1.f);
		}
		x = updw->_dw.process_sample (tmp);
#endif
		dst [pos] = x;
	}

	mfx::FileOpWav::save ("results/hardclipbl0.wav", dst, sample_freq, 0.5f);

	// Antialiased clipping
	for (int pos = 0; pos < len; ++pos)
	{
		float           x = src [pos] * gain;
#if (TestHardclipBl_OVRSPL == 1) // W/o oversampling
		x = hclip.process_sample (x);
#else // With oversampling
		float       tmp [TestHardclipBl_OVRSPL];
		updw->_up.process_sample (tmp, x);
		for (int k = 0; k < TestHardclipBl_OVRSPL; ++k)
		{
			tmp [k] = hclip.process_sample (tmp [k]);
		}
		x = updw->_dw.process_sample (tmp);
#endif
		dst [pos] = x;
	}

	mfx::FileOpWav::save ("results/hardclipbl1.wav", dst, sample_freq, 0.5f);

	// Speed test
	TimerAccurate  chrono;
	const int         nbr_passes = 10;
	for (int g = 2; g <= 100; g += 98)
	{
		hclip.clear_buffers ();
		float          acc_dummy = 0;
		chrono.reset ();
		chrono.start ();
		for (int pass_cnt = 0; pass_cnt < nbr_passes; ++pass_cnt)
		{
			for (int pos = 0; pos < len; ++pos)
			{
				acc_dummy += hclip.process_sample (src [pos] * g);
			}
		}
		chrono.stop ();
		double	      spl_per_s = chrono.get_best_rate (len * nbr_passes);
		spl_per_s += fstb::limit (acc_dummy, -1e-30f, 1e-30f); // Anti-optimizer trick
		const double   mega_sps  = spl_per_s / 1000000.0;
		printf ("Speed (gain = %4d):%9.3f Mspl/s\n", g, mega_sps);
	}

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	TestHardclipBl::gen_saw (std::vector <float> &data, double sample_freq, double freq, int len)
{
	const int      per = fstb::round_int (sample_freq / freq);
	for (int pos = 0; pos < len; ++pos)
	{
		const float    val = (pos % per) * (2.f / per) - 1.f;
		data.push_back (val);
	}
}



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
