/*****************************************************************************

        TestSvfAntisat.cpp
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
#include "mfx/dsp/va/AntisatAtanh.h"
#include "mfx/dsp/va/AntisatRcp.h"
#include "mfx/dsp/va/AntisatSinh.h"
#include "mfx/dsp/va/AntisatSq.h"
#include "mfx/dsp/va/SvfAntisat.h"
#include "mfx/FileOpWav.h"
#include "test/TestSvfAntisat.h"
#include "test/TimerAccurate.h"

#include <array>
#include <vector>

#include <cassert>
#include <cmath>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestSvfAntisat::perform_test ()
{
	int            ret_val = 0;

	if (ret_val == 0)
	{
		ret_val = perform_test <mfx::dsp::va::AntisatAtanh> (
			"AntisatAtanh", "atanh"
		);
	}
	if (ret_val == 0)
	{
		ret_val = perform_test <mfx::dsp::va::AntisatRcp> (
			"AntisatRcp", "rcp"
		);
	}
	if (ret_val == 0)
	{
		ret_val = perform_test <mfx::dsp::va::AntisatSinh> (
			"AntisatSinh", "sinh"
		);
	}
	if (ret_val == 0)
	{
		ret_val = perform_test <mfx::dsp::va::AntisatSq> (
			"AntisatSq", "sq"
		);
	}

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class T>
int	TestSvfAntisat::perform_test (const char classname_0 [], const char filename_0 [])
{
	int            ret_val = 0;

	printf (
		"Testing dsp::va::SvfAntisat <mfx::dsp::va::%s>...\n", classname_0
	);

	const double   sample_freq = 44100;
	const int      len = fstb::round_int (sample_freq * 10);

	std::vector <float>  src (len);
	std::vector <float>  dst_l (len);
	std::vector <float>  dst_b (len);
	std::vector <float>  dst_h (len);

	// Sawtooth
	const int      per  = fstb::round_int (sample_freq / 55.0);
	const float    gain = 1.0f;
	for (int pos = 0; pos < len; ++pos)
	{
		src [pos] = (float (pos % per) * (2.f / float (per)) - 1.f) * gain;
	}

	mfx::dsp::va::SvfAntisat <T> svf;
	svf.set_sample_freq (sample_freq);
	svf.set_reso (1.5f);

	for (int pos = 0; pos < len; ++pos)
	{
		const float    freq = float (20 * pow (1000, double (pos) / len));
		svf.set_freq (freq);
		svf.process_sample (dst_l [pos], dst_b [pos], dst_h [pos], src [pos]);
	}

	mfx::FileOpWav::save (
		(std::string ("results/svfantisat") + filename_0 + "1l.wav").c_str (),
		dst_l, sample_freq, 0.25f
	);
	mfx::FileOpWav::save (
		(std::string ("results/svfantisat") + filename_0 + "1b.wav").c_str (),
		dst_b, sample_freq, 0.25f
	);
	mfx::FileOpWav::save (
		(std::string ("results/svfantisat") + filename_0 + "1h.wav").c_str (),
		dst_h, sample_freq, 0.25f
	);

	// Speed test
	static const std::array <float, 2> reso_arr = { 0, 1.5f };
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
		svf.set_reso (reso);
		svf.set_freq (1000);
		svf.clear_buffers ();
		chrono.reset ();
		chrono.start ();
		for (int pass_cnt = 0; pass_cnt < nbr_passes; ++pass_cnt)
		{
			for (int pos = 0; pos < len; ++pos)
			{
				svf.process_sample (dst_l [pos], dst_b [pos], dst_h [pos], src [pos]);
			}
			acc_dummy += dst_l [len - 1];
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
