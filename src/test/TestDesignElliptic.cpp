/*****************************************************************************

        TestDesignElliptic.cpp
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
#include "mfx/dsp/iir/Biquad.h"
#include "mfx/dsp/iir/DesignElliptic.h"
#include "mfx/dsp/iir/OnePole.h"
#include "mfx/dsp/iir/TransS.h"
#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/dsp/iir/Zpk.h"
#include "mfx/dsp/osc/SweepingSin.h"
#include "mfx/FileOpWav.h"
#include "test/TestDesignElliptic.h"

#include <vector>

#include <cassert>
#include <cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestDesignElliptic::perform_test ()
{
	int            ret_val = 0;

	printf ("Testing mfx::dsp::iir::DesignElliptic...\n");

	constexpr double  sample_freq = 44100;
	constexpr int     buf_size    = 64;
	const int         len         = fstb::round_int (sample_freq * 10);

	std::vector <float>  src (len);
	mfx::dsp::osc::SweepingSin ssin (sample_freq, 20.0, 20000.0);
	ssin.generate (src.data (), len);

	mfx::dsp::iir::DesignElliptic ellip;
	const double   pb_r =  0.25;  // dB
	const double   pb_f =  0.75; // relative frequency
	const double   sb_r = 80.0;  // dB
	const double   sb_f =  1.25; // relative frequency
	printf ("Passband: freq = %5.1f %%, ripple = %6.2f dB\n", pb_f * 100, pb_r);
	printf ("Stopband: freq = %5.1f %%, ripple = %6.2f dB\n", sb_f * 100, sb_r);
	ellip.set_spec (pb_r, sb_r, pb_f, sb_f);
	const int      order = ellip.compute_min_order ();
	printf ("Order: %d\n", order);
	mfx::dsp::iir::Zpk   zpk;
	ret_val = ellip.compute_coefs (zpk);
	const int      nbr_poles = int (zpk.use_poles ().size ());
	const int      nbr_zeros = int (zpk.use_zeros ().size ());
	if (ret_val != 0)
	{
		printf ("*** Error: coefficient calculation failed.\n");
	}
	else if (   nbr_poles != order
	         || nbr_zeros >  order)
	{
		printf ("*** Error: coefficient order doesn\'t match.\n");
		ret_val = -1;
	}

	const int      nbr_biq = order / 2;
	std::vector <mfx::dsp::iir::Biquad>  p2_arr (nbr_biq);
	std::vector <mfx::dsp::iir::OnePole> p1_arr (order & 1);

	constexpr double  freq = 1000; // Hz
	double         k = zpk.get_gain ();

	// Biquad sections
	for (int biq_cnt = 0; ret_val == 0 && biq_cnt < nbr_biq; ++ biq_cnt)
	{
		double         bs [3];
		double         as [3];
		auto           z = zpk.get_zero (biq_cnt * 2);
		auto           p = zpk.get_pole (biq_cnt * 2);
		z = mfx::dsp::iir::TransSZBilin::prewarp_root_rel (z, freq, sample_freq);
		p = mfx::dsp::iir::TransSZBilin::prewarp_root_rel (p, freq, sample_freq);
		mfx::dsp::iir::TransS::conv_roots_cplx_to_poly_2 (bs, k, z);
		mfx::dsp::iir::TransS::conv_roots_cplx_to_poly_2 (as, 1, p);
		k = 1;

		float          bz [3];
		float          az [3];
		mfx::dsp::iir::TransSZBilin::map_s_to_z (
			bz, az, bs, as, freq, sample_freq
		);
		p2_arr [biq_cnt].set_z_eq (bz, az);
	}

	// One pole sections
	if (ret_val == 0 && (order & 1) != 0)
	{
		double         bs [2] = { 1, 0 };
		double         as [2];
		if (nbr_zeros == nbr_poles)
		{
			auto           z = zpk.get_zero (nbr_zeros - 1);
			z = mfx::dsp::iir::TransSZBilin::prewarp_root_rel (z, freq, sample_freq);
			mfx::dsp::iir::TransS::conv_roots_real_to_poly_1 (bs, k, z.real ());
			k = 1;
		}

		auto           p = zpk.get_pole (nbr_poles - 1);
		p = mfx::dsp::iir::TransSZBilin::prewarp_root_rel (p, freq, sample_freq);
		mfx::dsp::iir::TransS::conv_roots_real_to_poly_1 (as, 1 / k, p.real ());
		k = 1;

		float          bz [2];
		float          az [2];
		mfx::dsp::iir::TransSZBilin::map_s_to_z_one_pole (
			bz, az, bs, as, freq, sample_freq
		);
		p1_arr.front ().set_z_eq (bz, az);
	}

	// Filtering
	if (ret_val == 0)
	{
		std::vector <float>  dst (len);
		for (int pos = 0; pos < len; ++pos)
		{
			auto           x = src [pos];
			for (auto &p2 : p2_arr)
			{
				x = p2.process_sample (x);
			}
			for (auto &p1 : p1_arr)
			{
				x = p1.process_sample (x);
			}
			dst [pos] = x;
		}
		mfx::FileOpWav::save ("results/designelliptic0.wav", dst, sample_freq, 0.5f);
	}

	printf ("Done.\n\n");

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
