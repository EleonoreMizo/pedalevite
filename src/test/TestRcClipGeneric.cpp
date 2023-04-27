/*****************************************************************************

        TestRcClipGeneric.cpp
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



// 1 or 4
#define TestRcClipGeneric_OVRSPL (4)



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/Approx.h"
#include "fstb/fnc.h"
#include "fstb/SingleObj.h"
#include "hiir/PolyphaseIir2Designer.h"
#include "mfx/dsp/iir/Downsampler4xSimd.h"
#include "mfx/dsp/iir/Upsampler4xSimd.h"
#include "mfx/dsp/osc/SweepingSin.h"
#include "mfx/dsp/va/RcClipGeneric.h"
#include "mfx/dsp/va/IvDiodeAntipar.h"
#include "mfx/dsp/va/IvPoly.h"
#include "mfx/FileOpWav.h"
#include "test/TestRcClipGeneric.h"
#include "test/TimerAccurate.h"

#include <cassert>
#include <cmath>
#include <cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestRcClipGeneric::perform_test ()
{
	int            ret_val = 0;

	if (ret_val == 0)
	{
		ret_val = perform_test <mfx::dsp::va::IvDiodeAntipar> (
			"IvDiodeAntipar", "diodeap"
		);
	}
	if (ret_val == 0)
	{
		ret_val = perform_test <mfx::dsp::va::IvPoly <17, 5> > (
			"IvPoly <17, 5>", "poly17o5o-"
		);
	}
#if 1
	if (ret_val == 0)
	{
		ret_val = perform_test <mfx::dsp::va::IvPoly <9, 2> > (
			"IvPoly <9, 2>", "poly9o2o-"
		);
	}
#endif

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class T>
int	TestRcClipGeneric::perform_test (const char classname_0 [], const char filename_0 [])
{
	int            ret_val = 0;

	printf (
		"Testing dsp::va::RcClipGeneric + mfx::dsp::va::%s...\n", classname_0
	);

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

	const int      nbr_coef_42 = 4;
	const int      nbr_coef_21 = 8;
	struct UpDown
	{
		mfx::dsp::iir::Upsampler4xSimd <nbr_coef_42, nbr_coef_21>   _up;
		mfx::dsp::iir::Downsampler4xSimd <nbr_coef_42, nbr_coef_21> _dw;
	};
	fstb::SingleObj <UpDown> updw;
	double coef_42 [nbr_coef_42];
	double coef_21 [nbr_coef_21];
	// Rejection        : 103.4 dB
	// Bandwidth        :  20.0 kHz @ 44.1 kHz
	// Total group delay:   5.0 spl @ 1 kHz
	hiir::PolyphaseIir2Designer::compute_coefs_spec_order_tbw (
		&coef_42 [0], nbr_coef_42, 0.216404
	);
	hiir::PolyphaseIir2Designer::compute_coefs_spec_order_tbw (
		&coef_21 [0], nbr_coef_21, 0.0455352
	);
	updw->_up.set_coefs (coef_42, coef_21);
	updw->_dw.set_coefs (coef_42, coef_21);

	mfx::dsp::va::RcClipGeneric <T>  dclip;
	dclip.set_sample_freq (sample_freq * TestRcClipGeneric_OVRSPL);

#if defined (mfx_dsp_va_RcClipGeneric_STAT)
	dclip.reset_stat ();
#endif // mfx_dsp_va_RcClipGeneric_STAT

	float          gain = 1;
	for (int pos = 0; pos < len; ++pos)
	{
		float           x = src [pos] * gain;
#if (TestRcClipGeneric_OVRSPL == 1) // W/o oversampling
		x = dclip.process_sample (x);
#else // With oversampling
		float       tmp [TestRcClipGeneric_OVRSPL];
		updw->_up.process_sample (tmp, x);
		for (int k = 0; k < TestRcClipGeneric_OVRSPL; ++k)
		{
			tmp [k] = dclip.process_sample (tmp [k]);
		}
		x = updw->_dw.process_sample (tmp);
#endif
		dst [pos] = x;
	}

#if defined (mfx_dsp_va_RcClipGeneric_STAT)
	print_stats (dclip, "Standard");
	dclip.reset_stat ();
#endif // mfx_dsp_va_RcClipGeneric_STAT

	mfx::FileOpWav::save (
		(std::string ("results/rclipgen") + filename_0 + "1.wav").c_str (),
		dst, sample_freq, 0.5f
	);

	// High gain (+40 dB)
	dclip.clear_buffers ();
	dclip.set_cutoff_freq (float (sample_freq * 0.49));
	gain = 100;
	for (int pos = 0; pos < len; ++pos)
	{
		float           x = src [pos] * gain;
#if (TestRcClipGeneric_OVRSPL == 1) // W/o oversampling
		x = dclip.process_sample (x);
#else // With oversampling
		float       tmp [TestRcClipGeneric_OVRSPL];
		updw->_up.process_sample (tmp, x);
		for (int k = 0; k < TestRcClipGeneric_OVRSPL; ++k)
		{
			tmp [k] = dclip.process_sample (tmp [k]);
		}
		x = updw->_dw.process_sample (tmp);
#endif
		dst [pos] = x;
	}

#if defined (mfx_dsp_va_RcClipGeneric_STAT)
	print_stats (dclip, "High gain");
	dclip.reset_stat ();
#endif // mfx_dsp_va_RcClipGeneric_STAT

	mfx::FileOpWav::save (
		(std::string ("results/rclipgen") + filename_0 + "3.wav").c_str (),
		dst, sample_freq, 0.5f
	);

	// Now with variable gain with fixed GBP (gain-bandwidth product)
	dclip.clear_buffers ();
	const double per_spl = 2 * sample_freq;
	for (int pos = 0; pos < len; ++pos)
	{
		double          a = fmod (pos / per_spl, 1.0);
		a = 1 - fabs (a * 2 - 1);
		gain = fstb::Approx::exp2 (float (a * a) * 8 - 1);
		dclip.set_capa (gain * 10e-9f);
		float           x = src [pos] * gain;
#if (TestRcClipGeneric_OVRSPL == 1) // W/o oversampling
		x = dclip.process_sample (x);
#else // With oversampling
		float       tmp [TestRcClipGeneric_OVRSPL];
		updw->_up.process_sample (tmp, x);
		for (int k = 0; k < TestRcClipGeneric_OVRSPL; ++k)
		{
			tmp [k] = dclip.process_sample (tmp [k]);
		}
		x = updw->_dw.process_sample (tmp);
#endif
		dst [pos] = x;
	}

#if defined (mfx_dsp_va_RcClipGeneric_STAT)
	print_stats (dclip, "Variable gain, fixed GBP");
	dclip.reset_stat ();
#endif // mfx_dsp_va_RcClipGeneric_STAT

	mfx::FileOpWav::save (
		(std::string ("results/rclipgen") + filename_0 + "2.wav").c_str (),
		dst, sample_freq, 0.5f
	);

	// Speed test
	TimerAccurate  chrono;
	dclip.set_capa (10e-9f);
	const int         nbr_passes = 10;
	for (int g = 1; g <= 100; g *= 100)
	{
		float          acc_dummy = 0;
		chrono.reset ();
		chrono.start ();
		for (int pass_cnt = 0; pass_cnt < nbr_passes; ++pass_cnt)
		{
			for (int pos = 0; pos < len; ++pos)
			{
				acc_dummy += dclip.process_sample (src [pos] * float (g));
			}
		}
		chrono.stop ();
		double	      spl_per_s = chrono.get_best_rate (len * nbr_passes);
		spl_per_s += fstb::limit (acc_dummy, -1e-30f, 1e-30f); // Anti-optimizer trick
		const double   mega_sps  = spl_per_s / 1'000'000.0;
		printf ("Speed (gain = %4d):%9.3f Mspl/s\n", g, mega_sps);
	}

	return ret_val;
}



void	TestRcClipGeneric::gen_saw (std::vector <float> &data, double sample_freq, double freq, int len)
{
	const int      per = fstb::round_int (sample_freq / freq);
	for (int pos = 0; pos < len; ++pos)
	{
		const float    val = float (pos % per) * (2.f / float (per)) - 1.f;
		data.push_back (val);
	}
}



#if defined (mfx_dsp_va_RcClipGeneric_STAT)

template <class T>
void	TestRcClipGeneric::print_stats (T &dclip, const char msg_0 [])
{
	printf ("%s\n", msg_0);
	
	typename T::Stat  stat;
	dclip.get_stats (stat);

	printf ("=== Iterations ===\n");
	print_histo (
		stat._hist_it.data (),
		int (stat._hist_it.size ()),
		stat._nbr_spl_proc
	);

	printf ("\n");
}



void	TestRcClipGeneric::print_histo (int hist_arr [], int nbr_bars, int nbr_spl)
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
	const double   total_inv   = 1.0 / (double (total) + 1e-12);
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

#endif // mfx_dsp_va_RcClipGeneric_STAT



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
