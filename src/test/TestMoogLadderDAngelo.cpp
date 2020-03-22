/*****************************************************************************

        TestMoogLadderDAngelo.cpp
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
#include "mfx/dsp/va/MoogLadderDAngelo.h"
#include "mfx/FileOpWav.h"
#include "test/TestMoogLadderDAngelo.h"
#include "test/TimerAccurate.h"

#include <array>
#include <vector>

#include <cassert>
#include <cmath>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestMoogLadderDAngelo::perform_test ()
{
	int            ret_val = 0;

	if (ret_val == 0)
	{
		ret_val = perform_test_n <4> ();
	}
	if (ret_val == 0)
	{
		ret_val = perform_test_n <8> ();
	}
	if (ret_val == 0)
	{
		ret_val = perform_test_n <3> ();
	}

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int N>
int	TestMoogLadderDAngelo::perform_test_n ()
{
	int            ret_val = 0;

	printf ("Testing dsp::va::MoogLadderDAngelo <%d>...\n", N);

	const int      ovrspl = 4;
	const double   sample_freq = 44100;
	const int      len = fstb::round_int (sample_freq * 10);

	std::vector <float>  src (len);
	std::vector <float>  dst (len);

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

	mfx::dsp::va::MoogLadderDAngelo <N> filter;
	filter.set_sample_freq (sample_freq * ovrspl);
	filter.set_reso_norm (0.75f);

	for (int pos = 0; pos < len; ++pos)
	{
		const float    freq = float (20 * pow (1000, double (pos) / len));
		filter.set_freq_compensated (freq);
		float          x = src [pos];
		std::array <float, ovrspl> tmp;
		updw->_up.process_sample (tmp.data (), x);
		for (int k = 0; k < ovrspl; ++k)
		{
			tmp [k] = filter.process_sample (tmp [k]);
		}
		x = updw->_dw.process_sample (tmp.data ());
		dst [pos] = x;
	}

	mfx::FileOpWav::save (
		(
			  std::string ("results/moogladderdangelo")
			+ char ('0' + N / 10) + char ('0' + N % 10)
			+ "-1.wav"
		).c_str (),
		dst, sample_freq, 0.5f
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
		filter.set_reso_norm (reso);
		filter.set_freq_compensated (1000);
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



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
