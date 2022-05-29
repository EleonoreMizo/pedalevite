/*****************************************************************************

        TestOscSinCosStableSimd.cpp
        Author: Laurent de Soras, 2022

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law.You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"
#include "mfx/dsp/osc/OscSinCosStableSimd.h"
#include "test/TestOscSinCosStableSimd.h"
#include "test/TimerAccurate.h"

#include <array>

#include <cassert>
#include <cmath>
#include <cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestOscSinCosStableSimd::perform_test ()
{
	int            ret_val = 0;

	printf ("Testing mfx::dsp::osc::OscSinCosStableSimd...\n");

	if (ret_val == 0)
	{
		ret_val = test_block_size ();
	}
	if (ret_val == 0)
	{
		ret_val = test_stability_speed ();
	}

	printf ("Done.\n\n");

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestOscSinCosStableSimd::test_block_size () 
{
	constexpr int     len         = 1'000'000;
	constexpr int     buf_len_max = 256;
	constexpr int     buf_margin  = 4;
	// We have to allow some significant error because there is a slight
	// mismatch between the reference frequency and the actual oscillator
	// frequency caused by the limited numerical accuracy (phase accumulator
	// and tan() result in set_step()).
	constexpr auto    max_err     = 1e-3f;

	int            ret_val = 0;

	const float    angle = 0.1f;

	mfx::dsp::osc::OscSinCosStableSimd osc;
	osc.set_step (angle);
	osc.clear_buffers ();

	float          phase_ref = 0;
	int            pos_blk   = 0;
	int            buf_len   = 1;
	alignas (16) std::array <float, buf_len_max + buf_margin> buf_cos;
	alignas (16) std::array <float, buf_len_max + buf_margin> buf_sin;
	constexpr float   chk_val = 999;
	do
	{
		buf_cos.fill (chk_val);
		buf_sin.fill (chk_val);

		osc.process_block (buf_cos.data (), buf_sin.data (), buf_len);
		for (int k = 0; k < buf_len; ++k)
		{
			const auto c_tst = buf_cos [k];
			const auto s_tst = buf_sin [k];
			const auto a_ref = double (angle) * double (pos_blk + k);
			const auto c_ref = float (cos (a_ref));
			const auto s_ref = float (sin (a_ref));
			if (   ! fstb::is_eq (c_tst, c_ref, max_err)
			    || ! fstb::is_eq (s_tst, s_ref, max_err))
			{
				printf (
					"*** Error: pos=%d, buf_len=%d, iter=%d, "
					"cos_tst=%f, cos_ref=%f, sin_tst=%f, sin_ref=%f\n",
					k, buf_len, pos_blk + k,
					c_tst, c_ref, s_tst, s_ref
				);
				ret_val = -1;
				break;
			}
		}
		for (int k = 0; k < buf_margin; ++k)
		{
			if (   buf_cos [buf_len + k] != chk_val
			    || buf_sin [buf_len + k] != chk_val)
			{
				printf (
					"*** Error: buffer overrun at pos=%d, buf_len=%d, iter=%d\n",
					buf_len + k, buf_len, pos_blk + buf_len
				);
				ret_val = -1;
				break;
			}
		}

		phase_ref += angle * buf_len;
		pos_blk += buf_len;
		osc.correct_fast ();

		++ buf_len;
		if (buf_len > buf_len_max)
		{
			buf_len = 1;
		}
	}
	while (pos_blk < len && ret_val == 0);

	return ret_val;
}



int	TestOscSinCosStableSimd::test_stability_speed () 
{
	constexpr double  sample_freq = 44100; // Hz
	constexpr int     len         = 100'000'000;
	constexpr int     nbr_units   = mfx::dsp::osc::OscSinCosStableSimd::_nbr_units;
	static_assert (len % nbr_units == 0, "");

	int            ret_val = 0;

	// Tested angles, rad
	std::array <float, 4> angle_arr {{ 1e-6f, 1e-3f, 0.1f, 2.f }};

	TimerAccurate  tim;

	double         spl_per_s = 0;
	for (auto angle : angle_arr)
	{
		printf ("Angle: %10g rad/spl... ", double (angle));
		fflush (stdout);

		mfx::dsp::osc::OscSinCosStableSimd osc;
		osc.set_step (angle);
		osc.clear_buffers ();

		tim.reset ();
		tim.start ();

		for (int pos = 0; pos < len; pos += nbr_units)
		{
			osc.step ();
		}

		tim.stop ();
		if (spl_per_s == 0)
		{
			spl_per_s = tim.get_best_rate (len);
		}

		const float    s = osc.get_sin ().template extract <0> ();
		const float    c = osc.get_cos ().template extract <0> ();
		const float    m = sqrtf (s * s + c * c);
		printf ("module = %8f after %d iterations.\n", m, len);
	}

	const double   mega_sps  = spl_per_s / 1e6;
	const double   rt_mul    = spl_per_s / sample_freq;
	printf ("Speed: %12.3f Mspl/s (x%.3f real-time).\n", mega_sps, rt_mul);

	return ret_val;
}



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
