/*****************************************************************************

        TestMoogLadderMystran.cpp
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
#include "mfx/dsp/shape/WsBitcrush.h"
#include "mfx/dsp/shape/WsFloorOfs.h"
#include "mfx/dsp/shape/WsNegCond.h"
#include "mfx/dsp/shape/WsTruncMod.h"
#include "mfx/dsp/va/MoogLadderMystran.h"
#include "mfx/FileOpWav.h"
#include "test/TestMoogLadderMystran.h"
#include "test/TimerAccurate.h"

#include <array>
#include <vector>

#include <cassert>
#include <cmath>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestMoogLadderMystran::perform_test ()
{
	int            ret_val = 0;

	printf ("Testing dsp::va::MoogLadderMystran...\n");

	const int      ovrspl = 4;
	const double   sample_freq = 44100;
	const int      len = fstb::round_int (sample_freq * 10);

	std::vector <float>  src (len);
	std::vector <float>  dst (len);
	std::vector <float>  dst2 (len);

	// Sawtooth
	const int      per  = fstb::round_int (sample_freq / 55.0);
	const float    gain = 1.0f;
	for (int pos = 0; pos < len; ++pos)
	{
		src [pos] = ((pos % per) * (2.f / per) - 1.f) * gain;
	}

	const int      nbr_coef_42 = 3;
	const int      nbr_coef_21 = 8;
	struct UpDown
	{
		mfx::dsp::iir::Upsampler4xSimd <nbr_coef_42, nbr_coef_21>   _up;
		mfx::dsp::iir::Downsampler4xSimd <nbr_coef_42, nbr_coef_21> _dw;
	};
	fstb::SingleObj <UpDown> updw;
	fstb::SingleObj <UpDown> updw2;
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
	updw2->_up.set_coefs (coef_42, coef_21);
	updw2->_dw.set_coefs (coef_42, coef_21);

	mfx::dsp::va::MoogLadderMystran filter;
	filter.set_sample_freq (sample_freq * ovrspl);
	filter.set_reso (0.75f);

	std::array <float, ovrspl> tmp1;
	std::array <float, ovrspl> tmp2;
	std::array <float, 4> in_arr;
	for (int pos = 0; pos < len; ++pos)
	{
		const float    freq = float (20 * pow (1000, double (pos) / len));
		filter.set_freq (freq);
		float          x = src [pos];
		updw->_up.process_sample (tmp1.data (), x);
		for (int k = 0; k < ovrspl; ++k)
		{
			tmp1 [k] = filter.process_sample (tmp1 [k], in_arr.data ());
			tmp2 [k] =
				      in_arr [0]
				- 4 * in_arr [1]
				+ 6 * in_arr [2]
				- 4 * in_arr [3]
				+     tmp1 [k];
		}
		dst [pos]  = updw->_dw.process_sample (tmp1.data ());
		dst2 [pos] = updw2->_dw.process_sample (tmp2.data ());
	}

	mfx::FileOpWav::save (
		"results/moogladdermystran-1.wav", dst, sample_freq, 0.5f
	);
	mfx::FileOpWav::save (
		"results/moogladdermystran-1hp4.wav", dst2, sample_freq, 0.5f
	);

	filter.clear_buffers ();
	filter.set_reso (1.5f);
	updw->_up.clear_buffers ();
	updw->_dw.clear_buffers ();
	const float    mamp = 0.3f;
	const float    mo1 = float (2 * fstb::PI * 55 * 5 / sample_freq);
	const float    mo2 = float (2 * fstb::PI * 55 * 9 / sample_freq);
	for (int pos = 0; pos < len; ++pos)
	{
		const float    freq = float (20 * pow (1000, double (pos) / len));
		const float    mval = float (tanh ((sin (pos * mo1) + sin (pos * mo2)) * 2) * mamp);
		filter.set_freq (freq);
		float          x = src [pos];
		updw->_up.process_sample (tmp1.data (), x);
		updw2->_up.process_sample (tmp2.data (), mval);
		for (int k = 0; k < ovrspl; ++k)
		{
			tmp1 [k] = filter.process_sample_pitch_mod (tmp1 [k], tmp2 [k]);
		}
		dst [pos] = updw->_dw.process_sample (tmp1.data ());
	}

	mfx::FileOpWav::save (
		"results/moogladdermystran-2.wav", dst, sample_freq, 0.5f
	);

	// Speed test
	static const std::array <float, 2> reso_arr = { 0.0f, 1.0f };
	TimerAccurate  chrono;
#if defined (NDEBUG)
	const int      nbr_passes = 10;
#else
	const int      nbr_passes = 1;
#endif
	for (size_t reso_cnt = 0; reso_cnt < reso_arr.size (); ++reso_cnt)
	{
		float          acc_dummy  = 0;
		const float    reso = reso_arr [reso_cnt];
		filter.set_reso (reso);
		filter.set_freq (1000);
		filter.clear_buffers ();
		chrono.reset ();
		chrono.start ();
		for (int pass_cnt = 0; pass_cnt < nbr_passes; ++pass_cnt)
		{
			for (int pos = 0; pos < len; ++pos)
			{
				dst [pos] = filter.process_sample (src [pos]);
			}
			acc_dummy += dst [len - 1];
		}
		chrono.stop ();
		double	      spl_per_s = chrono.get_best_rate (len * nbr_passes);
		spl_per_s += fstb::limit (acc_dummy, -1e-30f, 1e-30f); // Anti-optimizer trick
		const double   mega_sps  = spl_per_s / 1000000.0;
		printf ("Speed (reso = %4.2f):%9.3f Mspl/s\n", reso, mega_sps);
	}

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
