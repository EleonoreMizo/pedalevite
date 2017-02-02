/*****************************************************************************

        TransSZMatched.hpp
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_iir_TransSZMatched_CODEHEADER_INCLUDED)
#define mfx_dsp_iir_TransSZMatched_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class TS, class TZ>
void	TransSZMatched::map_s_to_z (TZ z_eq_b [3], TZ z_eq_a [3], const TS s_eq_b [3], const TS s_eq_a [3], double f0, double fs)
{
	assert (z_eq_b != 0);
	assert (z_eq_a != 0);
	assert (s_eq_b != 0);
	assert (s_eq_a != 0);
	assert (f0 > 0);
	assert (f0 < fs * 0.5f);
	assert (fs > 0);

// Complete version.
// Summing issues are even bigger than with the simplified version.
#if 0

/*

Something to try:

Summing all the bands does not work well, but there is a formula for working
peak that we could use to hopefully reduce the error.
If b0 != 0 && b1 != 0 && b2 != 0, decompose the s-biquad into a peak part
and a low+band+high part:

b2*s^2 + b1*s + b0       a2*s^2 + k*b1*s + a0   (b2-g*a2)*s^2 + b1*(1-g*k)*s + (b0-g*a0)
------------------ = g * -------------------- + ---------------------------------------- 
a2*s^2 + a1*s + a0       a2*s^2 +   a1*s + a0          a2*s^2 + a1*s + a0               

With:
g = min (b2/a2, b0/a0)
k = (1 - 1/b1) / g

*/

	const double   sc_a  = 1 / s_eq_a [2];
	const double   fsc   = f0 / fs;
	const double   fscp  = (2 * fstb::PI) * fsc;

	// Poles
	const double   w02   = s_eq_a [0] * sc_a;
	const double   drw0  = s_eq_a [1] * sc_a;
	const double   w0    = sqrt (w02);
	const double   r     = drw0 / (2 * w0);
	const double   emrw0 = exp (drw0 * -0.5f * fscp);
	double         a1z   = -2 * emrw0;
	if (r <= 1)
	{
		a1z *= cos ( fscp * w0 * sqrt (1 - r * r));
	}
	else
	{
		a1z *= cosh (fscp * w0 * sqrt (r * r - 1));
	}
	const double   a2z   = emrw0 * emrw0;

	z_eq_a [0] = TZ (1);
	z_eq_a [1] = TZ (a1z);
	z_eq_a [2] = TZ (a2z);

	// Zeros
	const double   q        = w0 / drw0;
	const double   swo2     = sin (w0 * fsc * fstb::PI);
	const double   phi1     = swo2 * swo2;
	const double   phi0     = 1 - phi1;
	const double   phi2     = 4 * phi0 * phi1;

	const double   aa0_sqrt = 1 + a1z + a2z;
	const double   aa0      = aa0_sqrt * aa0_sqrt;
	const double   aa1_sqrt = 1 - a1z + a2z;
	const double   aa1      = aa1_sqrt * aa1_sqrt;
	const double   aa2      = -4 * a2z;

	const double   phi1_inv = 1 / phi1;
	const double   h2z_den  = aa0 * phi0 + aa1 * phi1 + aa2 * phi2;

	// Low-pass part
	const double   lp_bb0_sqrt = aa0_sqrt;
	const double   lp_bb0      = lp_bb0_sqrt * lp_bb0_sqrt;
	const double   lp_r1       = h2z_den * q * q;
	const double   lp_bb1      = (lp_r1 - lp_bb0 * phi0) * phi1_inv;
	const double   lp_b0       = 0.5 * (lp_bb0_sqrt + sqrt (lp_bb1));
	const double   lp_b1       = lp_bb0_sqrt - lp_b0;

	// High-pass part
	const double   hp_b0       = 0.25 * sqrt (h2z_den) * q * phi1_inv;
	const double   hp_b1       = -2 * hp_b0;
	const double   hp_b2       = hp_b0;

	// Band-pass part
	const double   phi0m14     = 4 * (phi0 - phi1);
	const double   bp_r1       = h2z_den;
	const double   bp_r2       = aa1 - aa0 + phi0m14 * aa2;
	const double   bp_bb2      = 0.5 * (bp_r1 - bp_r2 * phi1) / (phi1 * phi1);
	const double   bp_bb1      = bp_r2 - phi0m14 * bp_bb2;
	const double   bp_b1       = -0.5 *  sqrt (bp_bb1);
	const double   bp_b0       =  0.5 * (sqrt (bp_bb2 + bp_b1 * bp_b1) - bp_b1);
	const double   bp_b2       = -bp_b0 - bp_b1;

	// Scaling for canonical L/B/HPF
	const double   mb0s = s_eq_b [0] / s_eq_a [0];
	const double   mb1s = s_eq_b [1] / s_eq_a [1];
	const double   mb2s = s_eq_b [2] * sc_a;

	// Matrix
	const double   b0z = mb0s * lp_b0 + mb1s * bp_b0 + mb2s * hp_b0;
	const double   b1z = mb0s * lp_b1 + mb1s * bp_b1 + mb2s * hp_b1;
	const double   b2z =                mb1s * bp_b2 + mb2s * hp_b2;

	z_eq_b [0] = TZ (b0z);
	z_eq_b [1] = TZ (b1z);
	z_eq_b [2] = TZ (b2z);

// Experimental version
// Does not work correctly for shelving filter with Q > 0.7
#elif 0

	if (s_eq_a [2] != 1)
	{
		const TS sa [3] =
		{
			s_eq_a [0] / s_eq_a [2],
			s_eq_a [1] / s_eq_a [2],
			1,
		};
		const TS sb [3] =
		{
			s_eq_b [0] / s_eq_a [2],
			s_eq_b [1] / s_eq_a [2],
			s_eq_b [2] / s_eq_a [2],
		};
		map_s_to_z (z_eq_b, z_eq_a, sb, sa, f0, fs);

		return;
	}

	const double   fsc   = f0 / fs;
	const double   fscp  = (2 * fstb::PI) * fsc;

	// Poles
	const double   w02   = s_eq_a [0];
	const double   drw0  = s_eq_a [1];
	const double   w0    = sqrt (w02);
	const double   r     = drw0 / (2 * w0);
	const double   emrw0 = exp (drw0 * -0.5f * fscp);
	double         a1z   = -2 * emrw0;
	if (r <= 1)
	{
		a1z *= cos ( fscp * w0 * sqrt (1 - r * r));
	}
	else
	{
		a1z *= cosh (fscp * w0 * sqrt (r * r - 1));
	}
	const double   a2z   = emrw0 * emrw0;

	z_eq_a [0] = TZ (1);
	z_eq_a [1] = TZ (a1z);
	z_eq_a [2] = TZ (a2z);

	// Zeros
	const double   h2s_0  = (s_eq_b [0] * s_eq_b [0]) / (s_eq_a [0] * s_eq_a [0]);
	const double   h2s_n  = compute_h2s (s_eq_b, s_eq_a, 2 * fstb::PI / fsc);
	const double   h2s_f0 = compute_h2s (s_eq_b, s_eq_a, 1);

	// Eq. 27
	const double   aa0_sqrt = (1 + a1z + a2z);
	const double   aa1_sqrt = (1 - a1z + a2z);
	const double   aa0 = aa0_sqrt * aa0_sqrt; // A0
	const double   aa1 = aa1_sqrt * aa1_sqrt; // A1
	const double   aa2 = -4 * a2z;            // A2

	// Eq. 25

	// 0:
	// H^2(z) = B0 / A0
	const double   bb0  = h2s_0 * aa0;

	// Nyquist:
	// H^2(z) = B1 / A1
	const double   bb1  = h2s_n * aa1;

	// f0:
	// H^2(z) = (B0*phi0 + B1*phi1 + B2*phi2) / (A0*phi0 + A1*phi1 + A2*phi2)
	// B2 = (H^2(z) * (A0*phi0 + A1*phi1 + A2*phi2) - B0*phi0 - B1*phi1) / phi2
	const double   swo2 = sin (w0 * fsc * fstb::PI);
	const double   phi1 = swo2 * swo2;
	const double   phi0 = 1 - phi1;
	const double   phi2 = 4 * phi0 * phi1;
	const double   bb2  =
		(h2s_f0 * (aa0*phi0 + aa1*phi1 + aa2*phi2) - bb0*phi0 - bb1*phi1) / phi2;

	// Eq. 29
	const double   bb0_sqrt = sqrt (bb0);
	const double   bb1_sqrt = sqrt (bb1);
	const double   ww   =  0.5  * (bb0_sqrt + bb1_sqrt);
	const double   b0z  =  0.5  * (ww + sqrt (ww * ww + bb2));
	const double   b1z  =  0.5  * (bb0_sqrt - bb1_sqrt);
	const double   b2z  = -0.25 * bb2 / b0z;

	z_eq_b [0] = TZ (b0z);
	z_eq_b [1] = TZ (b1z);
	z_eq_b [2] = TZ (b2z);

// Simplified version
#else

	const double   sc_a  = 1 / s_eq_a [2];
	const double   fsc   = f0 / fs;
	const double   fscp  = (2 * fstb::PI) * fsc;

	// Poles
	const double   w02   = s_eq_a [0] * sc_a;
	const double   drw0  = s_eq_a [1] * sc_a;
	const double   w0    = sqrt (w02);
	const double   r     = drw0 / (2 * w0);
	const double   emrw0 = exp (drw0 * -0.5f * fscp);
	double         a1z   = -2 * emrw0;
	if (r <= 1)
	{
		a1z *= cos ( fscp * w0 * sqrt (1 - r * r));
	}
	else
	{
		a1z *= cosh (fscp * w0 * sqrt (r * r - 1));
	}
	const double   a2z   = emrw0 * emrw0;

	z_eq_a [0] = TZ (1);
	z_eq_a [1] = TZ (a1z);
	z_eq_a [2] = TZ (a2z);

	// Zeros
	const double   q     = w0 / drw0;
	const double   fz    = w0 * fsc;
	const double   fzoq  = fz / q;
	const double   omfz2 = 1 - fz * fz;
	const double   den_i = 1 / sqrt (omfz2 * omfz2 + fzoq * fzoq);

	// High-pass part
	const double   hp_r1 = (1 - a1z + a2z) * den_i;
	const double   hp_b0 = hp_r1 *  0.25;
	const double   hp_b1 = hp_r1 * -0.5;
	const double   hp_b2 = hp_b0;

	// Low-pass part
	const double   lp_r0 = 1 + a1z + a2z;
	const double   lp_r1 = hp_r1 * fz * fz;
	const double   lp_b0 = (lp_r0 + lp_r1) * 0.5;
	const double   lp_b1 = lp_r0 - lp_b0;

	// Band-pass part
	const double   bp_r0 = lp_r0 / ((2 * fstb::PI) * fz * q);
	const double   bp_r1 = hp_r1 * fzoq;
	const double   bp_b1 = bp_r1           * -0.5;
	const double   bp_b0 = (bp_r0 - bp_b1) *  0.5;
	const double   bp_b2 = -bp_b0 - bp_b1;

	// Scaling for canonical L/B/HPF
	const double   mb0s = s_eq_b [0] / s_eq_a [0];
	const double   mb1s = s_eq_b [1] / s_eq_a [1];
	const double   mb2s = s_eq_b [2] * sc_a;

	// Matrix
	const double   b0z = mb0s * lp_b0 + mb1s * bp_b0 + mb2s * hp_b0;
	const double   b1z = mb0s * lp_b1 + mb1s * bp_b1 + mb2s * hp_b1;
	const double   b2z =                mb1s * bp_b2 + mb2s * hp_b2;

	z_eq_b [0] = TZ (b0z);
	z_eq_b [1] = TZ (b1z);
	z_eq_b [2] = TZ (b2z);

#endif
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// w = pulsation
template <class TS>
double	TransSZMatched::compute_h2s (const TS s_eq_b [3], const TS s_eq_a [3], double w)
{
	const double   b0 = s_eq_b [0];
	const double   b1 = s_eq_b [1];
	const double   b2 = s_eq_b [2];
	const double   a0 = s_eq_a [0];
	const double   a1 = s_eq_a [1];
	const double   a2 = s_eq_a [2];

	const double   w2  = w * w;
	const double   num = (b2 * b2 * w2 + b1 * b1 - 2 * b0 * b2) * w2 + b0 * b0;
	const double   den = (a2 * a2 * w2 + a1 * a1 - 2 * a0 * a2) * w2 + a0 * a0;

	assert (den != 0);
	const double   h2s = num / den;

	return h2s;
}



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_iir_TransSZMatched_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
