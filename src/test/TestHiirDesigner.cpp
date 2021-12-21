/*****************************************************************************

        TestHiirDesigner.cpp
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



#undef test_TestHiirDesigner_WITH_EIGEN



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/fnc.h"
#include "fstb/Vf32.h"
#include "mfx/dsp/osc/SweepingSin.h"
#include "mfx/dsp/iir/Downsampler4xSimd.h"
#include "mfx/dsp/iir/Downsampler8xSimd.h"
#include "mfx/dsp/iir/Upsampler4xSimd.h"
#include "mfx/dsp/iir/Upsampler8xSimd.h"
#include "hiir/Downsampler2xFpu.h"
#include "hiir/HalfBandTpl.h"
#include "hiir/PhaseHalfPiTpl.h"
#include "hiir/PolyphaseIir2Designer.h"
#include "hiir/Upsampler2xFpu.h"
#include "test/TestHiirDesigner.h"

#if defined (test_TestHiirDesigner_WITH_EIGEN)
# include <Eigen/Dense>
#endif

#include <algorithm>
#include <complex>

#include <cassert>
#include <cmath>
#include <cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestHiirDesigner::perform_test ()
{
	int            ret_val = 0;

	const double   fs       = 44100.0;   // Sample frequency, base rate (decimated)
	const double   f_tst    =  4000.0;   // Frequency for the group delay test

#if 0

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// 8x

# if 1
	// 3, 4, 10, 1.125, 2.2634, x
	// 3, 5, 10, 1.717, 2.76, x
	const double   total_pd = 6.0;
	const double   pd_84    = 1.717;
	const double   pd_42    = 2.76;
	const double   pd_21    = total_pd - pd_42 / 2 - pd_84 / 4;
	constexpr int  nc_84    = 3;
	constexpr int  nc_42    = 5;
	constexpr int  nc_21    = 10;
	const auto     coef_84  = build_filter (nc_84, fs * 4, f_tst, pd_84);
	const auto     coef_42  = build_filter (nc_42, fs * 2, f_tst, pd_42);
	const auto     coef_21  = build_filter (nc_21, fs    , f_tst, pd_21);
# elif 1
	const double   total_pd = 5.0;
	const double   pd_84    = 1.691;
	const double   pd_42    = 2.208;
	const double   pd_21    = total_pd - pd_42 / 2 - pd_84 / 4;
	constexpr int  nc_84    = 3;
	constexpr int  nc_42    = 4;
	constexpr int  nc_21    = 8;
	const auto     coef_84  = build_filter (nc_84, fs * 4, f_tst, pd_84);
	const auto     coef_42  = build_filter (nc_42, fs * 2, f_tst, pd_42);
	const auto     coef_21  = build_filter (nc_21, fs    , f_tst, pd_21);
# else
	const double   total_pd = 4.0;
	const double   pd_84    = 1.1;
	const double   pd_42    = 1.645;
	const double   pd_21    = total_pd - pd_42 / 2 - pd_84 / 4;
	constexpr int  nc_84    = 2;
	constexpr int  nc_42    = 3;
	constexpr int  nc_21    = 7;
	const auto     coef_84  = build_filter (nc_84, fs * 4, f_tst, pd_84);
	const auto     coef_42  = build_filter (nc_42, fs * 2, f_tst, pd_42);
	const auto     coef_21  = build_filter (nc_21, fs    , f_tst, pd_21);
# endif

	constexpr int  rate = 8;
	if (coef_21.empty () || coef_42.empty () || coef_84.empty ())
	{
		return -1;
	}
	mfx::dsp::iir::Upsampler8xSimd <nc_84, nc_42, nc_21> us;
	mfx::dsp::iir::Downsampler8xSimd <nc_84, nc_42, nc_21> ds;
	us.set_coefs (coef_84.data (), coef_42.data (), coef_21.data ());
	ds.set_coefs (coef_84.data (), coef_42.data (), coef_21.data ());
	measure_phase_delay <rate> (us, ds, fs, f_tst);

#elif 0

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// 4x

# if 1
	const double   total_pd = 6.0;
	double         pd_21    = 4.602;
	double         pd_42    = (total_pd - pd_21) * 2;
	constexpr int  nc_42    = 5;
	constexpr int  nc_21    = 12;
	const auto     coef_42  = build_filter (nc_42, fs * 2, f_tst, pd_42);
	const auto     coef_21  = build_filter (nc_21, fs, f_tst, pd_21);
# elif 1
	const double   total_pd = 5.0;
	double         pd_21    = 3.8838;
	double         pd_42    = (total_pd - pd_21) * 2;
	constexpr int  nc_42    = 4;
	constexpr int  nc_21    = 10;
	const auto     coef_42  = build_filter (nc_42, fs * 2, f_tst, pd_42);
	const auto     coef_21  = build_filter (nc_21, fs, f_tst, pd_21);
# else
	const double   total_pd = 4.0;
	double         pd_21    = 3.1725;
	double         pd_42    = (total_pd - pd_21) * 2;
	constexpr int  nc_42    = 3;
	constexpr int  nc_21    = 8;
	const auto     coef_42  = build_filter (nc_42, fs * 2, f_tst, pd_42);
	const auto     coef_21  = build_filter (nc_21, fs, f_tst, pd_21);
# endif

	constexpr int  rate = 4;
	if (coef_21.empty () || coef_42.empty ())
	{
		return -1;
	}
	mfx::dsp::iir::Upsampler4xSimd <nc_42, nc_21> us;
	mfx::dsp::iir::Downsampler4xSimd <nc_42, nc_21> ds;
	us.set_coefs (coef_42.data (), coef_21.data ());
	ds.set_coefs (coef_42.data (), coef_21.data ());
	measure_phase_delay <rate> (us, ds, fs, f_tst);

#else

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// 2x

# if 1
	const double   tg_pdly  =  5.0; // Desired phase delay (in samples) at oversampled rate
	constexpr int  nbr_coef = 12;   // Number of coefficients
# elif 0
	const double   tg_pdly  =  4.0;
	constexpr int  nbr_coef = 10;
# elif 0
	const double   tg_pdly  =  3.0;
	constexpr int  nbr_coef =  8;
# else
	const double   tg_pdly  =  2.0;
	constexpr int  nbr_coef =  6;
# endif

	constexpr int  rate = 2;

	auto           coef_21  = build_filter (nbr_coef, fs, f_tst, tg_pdly);
	if (coef_21.empty ())
	{
		return -1;
	}

	else
	{
		printf ("Phase delays:\n");
		for (int k = 0; k < 20; ++k)
		{
			const double   f0 = 0.5 * k / (20 * rate);
			const double   d  = hiir::PolyphaseIir2Designer::compute_phase_delay (
				coef_21.data (), nbr_coef, f0
			);
			printf ("%5.0f Hz : %6.2f spl\n", f0 * rate * fs, d);
		}

		hiir::Upsampler2xFpu <nbr_coef> us;
		hiir::Downsampler2xFpu <nbr_coef> ds;
		us.set_coefs (coef_21.data ());
		ds.set_coefs (coef_21.data ());
		measure_phase_delay <rate> (us, ds, fs, f_tst);
	}

#endif

#if 1

	// No real test here, we just make sure everything compiles fine
	constexpr int  nc = 3;
	constexpr std::array <double, nc> coef_arr { 0.3, 0.5, 0.7 };

# if defined (test_TestHiirDesigner_WITH_EIGEN)
	constexpr int vec_size = 8;
	typedef Eigen::Array <float, vec_size, 1> VecTest;
# else
	typedef fstb::Vf32 VecTest;
	constexpr int vec_size = VecTest::_length;
# endif

	hiir::PhaseHalfPiTpl <nc, VecTest, vec_size> ph;
	hiir::Upsampler2xTpl <nc, VecTest, vec_size> us;
	hiir::Downsampler2xTpl <nc, VecTest, vec_size> ds;
	hiir::HalfBandTpl <nc, VecTest, vec_size> hb;
	ph.set_coefs (coef_arr.data ());
	us.set_coefs (coef_arr.data ());
	ds.set_coefs (coef_arr.data ());
	hb.set_coefs (coef_arr.data ());
	ph.clear_buffers ();
	us.clear_buffers ();
	ds.clear_buffers ();
	hb.clear_buffers ();
	constexpr int  len = 10;
	alignas (fstb_SIMD128_ALIGN) std::array <VecTest, len * 2> src_arr {};
	alignas (fstb_SIMD128_ALIGN) std::array <VecTest, len * 2> dst1_arr {};
	alignas (fstb_SIMD128_ALIGN) std::array <VecTest, len * 2> dst2_arr {};
	ph.process_block (dst1_arr.data (), dst2_arr.data (), src_arr.data (), len);
	us.process_block (dst1_arr.data (), src_arr.data (), len);
	ds.process_block (dst1_arr.data (), src_arr.data (), len);
	ds.process_block_split (dst1_arr.data (), dst2_arr.data (), src_arr.data (), len);
	hb.process_block (dst1_arr.data (), src_arr.data (), len);
	hb.process_block_hpf (dst1_arr.data (), src_arr.data (), len);
	hb.process_block_split (dst1_arr.data (), dst2_arr.data (), src_arr.data (), len);

#endif

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// This code designs a filter on a phase delay constraint
// Returns an empty vector if the filter couldn't be designed.
std::vector <double>	TestHiirDesigner::build_filter (int nbr_coef, double fs, double f_tst, double tg_pdly)
{
	assert (nbr_coef > 0);
	assert (fs > 0);
	assert (f_tst * 4 <= fs);
	assert (tg_pdly >= 0);

	const double   ft_rel  = f_tst / (fs * 2);

	// Precision (in samples) to reach for the phase delay at f_tst
	const double   prec    =  1e-6;

	double         rs_attn = 0;
	double         rs_tb   = 0;
	double         pdly_mapped = tg_pdly + 0.5;
	std::vector <double> rs_coef (nbr_coef);
	const hiir::PolyphaseIir2Designer::ResCode   res =
		hiir::PolyphaseIir2Designer::compute_coefs_spec_order_pdly (
			rs_coef.data (), &rs_attn, &rs_tb, nbr_coef, pdly_mapped, ft_rel, prec,
			0.001, 10000, // Attenuation
			0.001, 0.499 // Transition bandwidth
		);

	if (res != hiir::PolyphaseIir2Designer::ResCode_OK)
	{
		printf (
			"Design requirements cannot be met, returned %d.\n",
			 static_cast <int> (res)
		);
		rs_coef.clear ();
	}

	else
	{
		const double   gdly_raw = hiir::PolyphaseIir2Designer::compute_group_delay (
			rs_coef.data (), nbr_coef, f_tst / (fs * 2), false
		);
		const double   gdly = gdly_raw - 0.5;
		printf ("Coefficients: %d\n"    , nbr_coef);
		printf ("Attenuation : %g dB\n" , rs_attn);
		printf ("Trans BW    : %g\n"    , rs_tb);
		printf ("Passband    : %g Hz @ %g Hz sampling rate\n",
			(0.5 - rs_tb) * fs, fs
		);
		printf ("Phase delay : %g spl\n", tg_pdly);
		printf ("Group delay : %g spl\n", gdly);
		printf ("Dly rel freq: %g\n"    , ft_rel);
		printf ("Coefficient list:\n");
		for (auto a : rs_coef)
		{
			printf ("\t%.17g\n", a);
		}
	}

	return rs_coef;
}



template <int R, typename U, typename D>
void	TestHiirDesigner::measure_phase_delay (U &us, D &ds, double fs, double f_tst)
{
	constexpr int  rate = R;
	int            len = fstb::round_int (fs * 10);
	mfx::dsp::osc::SweepingSin ssin (fs, f_tst, f_tst);
	std::vector <float> ref (len);
	std::vector <float> tst (len);
	ssin.generate (ref.data (), len);

	ds.clear_buffers ();
	us.clear_buffers ();
	constexpr int  blk_len = 1024;
	std::array <float, blk_len * rate> buf_tmp;
	int            blk_pos = 0;
	do
	{
		const int      work_len = std::min (len - blk_pos, blk_len);
#if 1
		us.process_block (buf_tmp.data (), &ref [blk_pos], work_len);
		ds.process_block (&tst [blk_pos], buf_tmp.data (), work_len);
#elif 0
		// 1.5 spl
		static_assert (rate == 2, "");
		for (int k = 0; k < work_len; ++k)
		{
			buf_tmp [k * 2    ] = ref [blk_pos + k] * 2;
			buf_tmp [k * 2 + 1] = 0;
		}
		ds.process_block (&tst [blk_pos], buf_tmp.data (), work_len);
#else
		// 2 spl
		static_assert (rate == 2, "");
		us.process_block (buf_tmp.data (), &ref [blk_pos], work_len);
		for (int k = 0; k < work_len; ++k)
		{
			tst [blk_pos + k] = buf_tmp [k * 2];
		}
#endif
		blk_pos += work_len;
	}
	while (blk_pos < len);

	const auto     measured_dly = find_delay (ref, tst, len / 2);
	printf ("Measured dly: %g spl\n", measured_dly);
}



// Finds the average delay between tst and ref by computing the cross-
// correlation and finding the first peak.
// tst is assumed to be late, relative to ref.
// skip indicates how many samples to skip at the beginning (setup time for
// the filter)
double	TestHiirDesigner::find_delay (const std::vector <float> &ref, const std::vector <float> &tst, int skip)
{
	constexpr int  dly_max = 256;
	assert (skip >= 0);
	assert (ref.size () - skip > dly_max);
	assert (tst.size () >= tst.size ());

	const auto     len     = int (ref.size ());
	double         sel_val = -1e+300;
	int            sel_dly = -1;

	std::array <double, dly_max> dly_arr;
	for (int dly_tst = 0; dly_tst < dly_max; ++dly_tst)
	{
		double         sum = 0;
		for (int pos = skip; pos < len - dly_max; ++pos)
		{
			sum += ref [pos] * tst [pos + dly_tst];
		}

		dly_arr [dly_tst] = sum;
		if (sum <= sel_val)
		{
			if (dly_tst < 2)
			{
				return double (sel_dly);
			}
			const double   frac = fstb::find_extremum_pos_parabolic (
				dly_arr [dly_tst - 2], dly_arr [dly_tst - 1], sum
			);
			return double (sel_dly) + frac;
		}

		sel_val = sum;
		sel_dly = dly_tst;
	}

	// Not found
	return double (dly_max);
}



double	TestHiirDesigner::evaluate_mag_lpf (const std::vector <double> &coef_arr, double f_fs)
{
	using Cplx = std::complex <double>;
	const auto     jw  = Cplx (0, 2 * fstb::PI * f_fs);
	const auto     zm1 = exp (-jw);
	const auto     zm2 = zm1 * zm1;
	auto           apf = std::array <Cplx, 2> { Cplx (1), zm1 };
	const auto     nbr_coef = int (coef_arr.size ());
	for (int k = 0; k < nbr_coef; ++k)
	{
		const auto     a = Cplx { coef_arr [k] };
		const auto     u = (a + zm2) / (Cplx (1) + a * zm2);
		apf [k & 1] *= u;
	}
	const auto     h   = (apf [0] + apf [1]) * Cplx (0.5);
	const auto     mag = std::abs (h);

	return mag;
}



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
