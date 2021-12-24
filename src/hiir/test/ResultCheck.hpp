/*****************************************************************************

        ResultCheck.hpp
        Author: Laurent de Soras, 2005

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (hiir_test_ResultCheck_CODEHEADER_INCLUDED)
#define hiir_test_ResultCheck_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "hiir/test/SweepingSine.h"
#include "hiir/def.h"

#include <algorithm>

#include <cassert>
#include <cmath>
#include <cstdio>



namespace hiir
{
namespace test
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T>
std::vector <T>	ResultCheck <T>::generate_test_signal (const SweepingSine &ss, long len, int nbr_chn)
{
	assert (len > 0);
	assert (nbr_chn > 0);

	std::vector <T>  src (len * nbr_chn);
	printf ("Generating sweeping sine... ");
	fflush (stdout);
	if (nbr_chn == 1)
	{
		ss.generate (src.data ());
	}
	else
	{
		std::vector <T>   src_base (len);
		ss.generate (src_base.data ());
		for (long pos = 0; pos <len; ++pos)
		{
			for (int chn = 0; chn < nbr_chn; ++chn)
			{
				src [pos * nbr_chn + chn] = src_base [pos];
			}
		}
	}
	printf ("Done.\n");

	return src;
}



// We should take group delay into account
template <typename T>
int	ResultCheck <T>::check_halfband (const SweepingSine &ss, double bw, double at, const T out_ptr [], bool hiband_flag, int rate_div)
{
	assert (out_ptr != nullptr);
	assert (bw > 0);
	assert (bw < 0.5);
	assert (at > 0);
	assert (rate_div == 1 || rate_div == 2);

	int            ret_val = 0;

	printf ("Checking... ");
	fflush (stdout);

	// Relax the specs in order to take filter ringing into account
	bw = std::max (bw, 0.01);
	at = std::min (at, 20.0);

	const float    f_nyquist = ss.get_sample_freq () * 0.5f;
	const float    f_lo_end  = f_nyquist * float (0.5 - bw);
	const float    f_hi_beg  = f_nyquist * float (0.5 + bw);
	const long     pos_lo_b  = 0;
	const long     pos_lo_e  = ss.get_sample_pos_for (f_lo_end)  / rate_div;
	assert (pos_lo_b < pos_lo_e);
	const long     pos_hi_b  = ss.get_sample_pos_for (f_hi_beg)  / rate_div;
	const long     pos_hi_e  = ss.get_sample_pos_for (f_nyquist) / rate_div;
	assert (pos_hi_b < pos_hi_e);

	// Measures lower band volume
	double         sum_lo = 0;
	{
		for (long pos = pos_lo_b; pos < pos_lo_e; ++pos)
		{
			const double   val = out_ptr [pos];
			sum_lo += val * val;
		}
	}

	// Measures higher band volume
	double         sum_hi = 0;
	{
		for (long pos = pos_hi_b; pos < pos_hi_e; ++pos)
		{
			const double   val = out_ptr [pos];
			sum_hi += val * val;
		}
	}

	printf ("Done.\n");

	const double   sum_pb = (hiband_flag) ? sum_hi : sum_lo;
	const double   sum_sb = (hiband_flag) ? sum_lo : sum_hi;
	const long     len_pb = (hiband_flag) ? pos_hi_e - pos_hi_b : pos_lo_e - pos_lo_b;
	const long     len_sb = (hiband_flag) ? pos_lo_e - pos_lo_b : pos_hi_e - pos_hi_b;

	// Checks passband volume
	const double   vol_pb_avg = sqrt (sum_pb * 2 / len_pb);
	const double   err_pb_avg = fabs (vol_pb_avg - 1);
	if (err_pb_avg > 0.1)
	{
		assert (vol_pb_avg > 0);
		const double   vol_pb_avg_db = log10 (vol_pb_avg) * 20;
		printf (
			"Error: abnormal average volume in passband (%f dB).\n",
			vol_pb_avg_db
		);
		ret_val = -1;
	}

	// Checks stopband volume
	const double   vol_sb_avg = sqrt (sum_sb * 2 / len_sb);
	const double   vol_sb_th  = pow (10.0, at / -20.0);
	const double   err_sb_avg = vol_sb_avg / vol_sb_th - 1;
	if (err_sb_avg > 0.25)
	{
		assert (vol_sb_avg > 0);
		const double   vol_sb_avg_db = log10 (vol_sb_avg) * 20;
		printf (
			"Error: abnormal average volume in stopband (%f dB).\n",
			vol_sb_avg_db
		);
		ret_val = -2;
	}

	return ret_val;
}



template <typename T>
int	ResultCheck <T>::check_uspl (const SweepingSine &ss, double bw, double at, const T out_ptr [])
{
	assert (out_ptr != nullptr);
	assert (bw > 0);
	assert (bw < 0.5);
	assert (at > 0);

	int            ret_val = 0;

	// Relax the specs in order to take FIR accuracy into account
	bw = std::max (bw, 0.01);
	at = std::min (at, 50.0);

	printf ("Checking... ");
	fflush (stdout);

	// Builds a simple FIR to keep only signal in the stopband.
	const long     fir_len  = 127;   // Must be odd because of the fir_mid case
	assert ((fir_len & 1) != 0);
	const double   f_sb_beg = 0.5 + bw;
	const double   f_shft   = 3.5 / (fir_len + 1);  // Shifts main lobe into the stopband
	const double   f_fir    = f_sb_beg + f_shft;
	std::vector <float>	fir (fir_len);
	for (long fir_pos = 0; fir_pos < fir_len; ++fir_pos)
	{
		const long     fir_mid = (fir_len - 1) / 2;
		if (fir_pos == fir_mid)
		{
			fir [fir_pos] = float (f_fir - 1);
		}
		else
		{
			const double   w_phase = 2 * hiir::PI * (fir_pos + 0.5) / fir_len;
			const double   w       = 0.5 * (1 - cos (w_phase));

			const double   s_phase = f_fir * hiir::PI * (fir_pos - fir_mid);
			const double   s       = f_fir * sin (s_phase) / s_phase;

			fir [fir_pos] = float (w * s);
		}
	}

	const long     len = ss.get_len () * 2;
	const long     len_ana = len - fir_len + 1;
	assert (len_ana > 0);

	// Measures global and stopband volumes
	double         sum = 0;
	double         sum_sb = 0;
	for (long pos = 0; pos < len_ana; ++pos)
	{
		const double   val = out_ptr [pos];
		sum += val * val;

		double         val_fir = 0;
		for (long k = 0; k < fir_len; ++k)
		{
			val_fir += out_ptr [pos + k] * fir [k];
		}
		sum_sb += val_fir * val_fir;
	}

	printf ("Done.\n");

	// Checks global volume
	const double   vol_avg = sqrt (sum * 2 / len_ana);
	const double   err_avg = fabs (vol_avg - 1);
	if (err_avg > 0.1)
	{
		assert (vol_avg > 0);
		const double   vol_avg_db = log10 (vol_avg) * 20;
		printf (
			"Error: abnormal average volume (%f dB).\n",
			vol_avg_db
		);
		ret_val = -1;
	}

	// Checks stopband volume
	const double   vol_sb_avg = sqrt (sum_sb * 2 / len_ana);
	const double   vol_sb_th  = pow (10.0, at / -20.0);
	const double   err_sb_avg = vol_sb_avg / vol_sb_th - 1;
	if (err_sb_avg > 0.25)
	{
		assert (vol_sb_avg > 0);
		const double   vol_sb_avg_db = log10 (vol_sb_avg) * 20;
		printf (
			"Error: abnormal average volume in stopband (%f dB).\n",
			vol_sb_avg_db
		);
		ret_val = -2;
	}

	return ret_val;
}



// We should take group delay into account
template <typename T>
int	ResultCheck <T>::check_phase (const SweepingSine &ss, double bw, const T out_0_ptr [], const T out_1_ptr [])
{
	assert (out_0_ptr != nullptr);
	assert (out_1_ptr != nullptr);
	assert (bw > 0);
	assert (bw < 0.5);

	printf ("Checking... ");
	fflush (stdout);

	int            ret_val = 0;

	const float    f_nyquist = ss.get_sample_freq () * 0.5f;
	const float    f_b       = float (f_nyquist *      bw );
	const float    f_e       = float (f_nyquist * (1 - bw));
	const long     pos_b     = ss.get_sample_pos_for (f_b);
	const long     pos_e     = ss.get_sample_pos_for (f_e);
	assert (pos_b < pos_e);

	double         err_sum = 0;
	double         err_max = 0;
	for (long pos = pos_b; pos < pos_e; ++pos)
	{
		const double   v_0     = out_0_ptr [pos];
		const double   v_1     = out_1_ptr [pos];
		const double   val     = sqrt (v_0 * v_0 + v_1 * v_1);
		const double   err     = val - 1;
		const double   err_pos = (err > 0) ? err : 0;
		const double   err_abs = fabs (err);
		err_max  = std::max (err_max, err_pos);
		err_sum += err_abs;
	}
	const double   err_avg = err_sum / (pos_e - pos_b);

	printf ("Done.\n");

	if (err_max > 0.25)
	{
		printf ("Error: abnormal maximum phase error (%f %%).\n", err_max * 100);
		ret_val = -1;
	}
	if (err_avg > 0.125)
	{
		printf ("Error: abnormal average phase error (%f %%).\n", err_avg * 100);
		ret_val = -2;
	}

	return ret_val;
}



template <typename T>
int	ResultCheck <T>::check_delay (const T out_ptr [], const T ref_ptr [], long nbr_spl, double dly_expect, double f_fs)
{
	assert (out_ptr != nullptr);
	assert (ref_ptr != nullptr);
	assert (nbr_spl > 0);

	constexpr auto max_dly_err = 0.01; // Samples

	int            ret_val = 0;

	const int      skip      = nbr_spl / 8;
	const double   wl_half   = 0.5 / f_fs; // Half a period
	const int      start     = ceil_int (dly_expect - wl_half);
	const double   dly_found =
		find_delay (out_ptr, ref_ptr, nbr_spl, skip, start);
	const double   err       = fabs (dly_found - dly_expect);
	if (err > max_dly_err)
	{
		printf (
			"Error: abnormal delay error (%f spl, expected %f spl).\n",
			dly_found, dly_expect
		);
		ret_val = -1;
	}

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T>
double	ResultCheck <T>::find_delay (const T tst_ptr [], const T ref_ptr [], long len, long skip, int start)
{
	constexpr int  dly_max =  64;
	constexpr int  dly_min = -64;
	constexpr int  extra   = dly_max - dly_min + 1;

	assert (tst_ptr != nullptr);
	assert (ref_ptr != nullptr);
	assert (skip >= 0);
	assert (len > skip + extra);

	double         sel_val = -1e+300;
	int            sel_dly = dly_min - 1;
	start = std::min (std::max (start, dly_min), dly_max);

	std::array <double, extra> dly_arr;
	for (int dly_tst = start; dly_tst <= dly_max; ++dly_tst)
	{
		double         sum = 0;
		for (long pos = skip - dly_min; pos < len - dly_max; ++pos)
		{
			sum += ref_ptr [pos] * tst_ptr [pos + dly_tst];
		}

		dly_arr [dly_tst - dly_min] = sum;
		if (sum <= sel_val)
		{
			if (dly_tst < dly_min + 2)
			{
				return double (sel_dly);
			}
			const double   frac = find_extremum_pos_parabolic (
				dly_arr [dly_tst - 2 - dly_min],
				dly_arr [dly_tst - 1 - dly_min],
				sum
			);
			return double (sel_dly) + frac;
		}

		sel_val = sum;
		sel_dly = dly_tst;
	}

	// Not found
	return double (dly_max + 1);
}



// Finds the x position of the extremum (min or max) in the parabolic-
// interpolated curve passes through (-1, r1), (0, r2) and (+1, r3).
// The curve is implicitely defined by:
// f(x) = ((r3 + r1) / 2 - r2) * x^2 + ((r3 - r1) / 2) * x + r2
// The points must not be aligned so the extremum exists.
// It is not necessariy located between -1 and 1.
template <typename T>
double	ResultCheck <T>::find_extremum_pos_parabolic (double r1, double r2, double r3) noexcept
{
	const auto     den = 2 * r2 - (r3 + r1);
	assert (den != 0);

	const auto     pos = (r3 - r1) * 0.5 / den;

	return pos;
}



}  // namespace test
}  // namespace hiir



#endif   // hiir_test_ResultCheck_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
