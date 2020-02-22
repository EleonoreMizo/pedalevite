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
#include "fstb/fnc.h"

#include <array>

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

#if 0
/*

Summing version.
Summing issues are even bigger than with the simplified version.

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

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Decomposes the input equation into three basic filters using w0 and q

	// First, normalises the input equation:
	// - Scales the frequencies in order to put Nyquist at s = pi
	// - Unity coefficient for s^2 in the denominator: a2 = 1
	const double   sc_a    = 1 / s_eq_a [2];
	const double   fsc     = f0 / fs;
	const double   fscp    = (2 * fstb::PI) * fsc;
	const double   fscp2   = fscp * fscp;

	std::array <double, 3>  bs {
		s_eq_b [0] * (sc_a * fscp2),
		s_eq_b [1] * (sc_a * fscp ),
		s_eq_b [2] *  sc_a
	};
	std::array <double, 3>  as {
		s_eq_a [0] * (sc_a * fscp2),
		s_eq_a [1] * (sc_a * fscp ),
		1
	};

	// Finds w0 and q
	// Eq. 30, 35 and 37
	// Q = w0 / a1
	const double   w02 = as [0];
	const double   w0  = sqrt (w02);
	const double   q   = w0 / as [1]; // Actually capital Q in the documentation.

	// Eq. 11: q = 1 / (2*Q), called r here in order to avoid confusion with Q.
	// den(H(s)) = w0^2 + 2*r*w0 + s^2
	const double   r   = 0.5f / q;

	// Computes the levels for each component: LP, BP, HP
	// Eq. 30, 35 and 37
	const double   lvl_l = bs [0] / w02;
	const double   lvl_b = bs [1] * q / w0;
	const double   lvl_h = bs [2];

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Poles

	// exp (-r * w0)
//	const double   emrw0 = exp (as [1] * -0.5f);
	const double   emrw0 = exp (-r * w0);

	// Eq. 12
	double         a1z   = -2 * emrw0;
	const double   r2    = r * r;
	if (r <= 1)
	{
		a1z *= cos ( w0 * sqrt (1 - r2));
	}
	else
	{
		a1z *= cosh (w0 * sqrt (r2 - 1));
	}
	const double   a2z   = emrw0 * emrw0;

	z_eq_a [0] = TZ (1);
	z_eq_a [1] = TZ (a1z);
	z_eq_a [2] = TZ (a2z);

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Zeros, common part

	// Eq. 27
	const double   aa0_sqrt = 1 + a1z + a2z;
	const double   aa0      = fstb::sq (aa0_sqrt     );
	const double   aa1      = fstb::sq (1 - a1z + a2z);
	const double   aa2      = -4 * a2z;

	// Eq. 26 at the cutoff frequency
	const double   s2w0h    = fstb::sq (sin (w0 * 0.5f));
	const double   phi0     = 1 - s2w0h;
	const double   phi1     =     s2w0h;
	const double   phi2     = 4 * phi0 * phi1;

	// Shared partial calculations
	const double   aaphisum = aa0 * phi0 + aa1 * phi1 + aa2 * phi2;
	const double   phidif4  = 4 * (phi1 - phi0);

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Zeros, LP part

	// Eq. 34
	const double   bb0l_sqrt = 1 + a1z + a2z;
	const double   bb0l     = bb0l_sqrt * bb0l_sqrt;

	// Eq. 31
	const double   rr1l     = aaphisum * (q * q);

	// Eq. 32
	const double   bb1l     = (rr1l - bb0l * phi0) / phi1;

	// Eq. 33
	const double   bb1l_sqrt = sqrt (bb1l);
	const double   b0zl     = (bb0l_sqrt + bb1l_sqrt) * 0.5f;
	const double   b1zl     =  bb0l_sqrt - b0zl;

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Zeros, HP part

	// Eq. 36
	const double   b0zh     = sqrt (aaphisum) * q / (4 * phi1);
	const double   b1zh     = -2 * b0zh;
	const double   b2zh     =      b0zh;

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Zeros, BP part

	// Eq. 39
	const double   rr1b     = aaphisum;
	const double   rr2b     = aa1 - aa0 - phidif4 * aa2;

	// Eq. 40
	const double   bb2b     = (rr1b - rr2b * phi1) / (4 * phi1 * phi1);
	const double   bb1b     = rr2b + phidif4 * bb2b;

	// Eq. 41
	const double   b1zb     = -0.5 *  sqrt (bb1b);
	const double   b0zb     =  0.5 * (sqrt (bb2b + b1zb * b1zb) - b1zb);
	const double   b2zb     = -b0zb - b1zb;

 #if 0 // Not used actually
	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Zeros, PK part

	const double   pg       = /*** To do ***/;   // Peak level
	const double   pg2      = pg * pg;

	// Eq. 44
	const double   rr1p     = aaphisum * pg2;
	const double   rr2p     = (aa1 - aa0 - phidif4 * aa2) * pg2;

	// Eq. 45
	const double   bb0p     = aa0;
	const double   bb2p     = (rr1p - rr2p * phi1 - bb0p) / (4 * phi1 * phi1);
	const double   bb1p     = rr2p + bb0p + phidif4 * bb2p;

	// Eq. 29
	const double   bb0p_sqrt = sqrt (bb0p);
	const double   bb1p_sqrt = sqrt (bb1p);
	const double   ww       =  0.5f  * (bb0p_sqrt + bb1p_sqrt);
	const double   b0zp     =  0.5f  * (ww + sqrt (ww * ww + bb2p));
	const double   b1zp     =  0.5f  * (bb0p_sqrt - bb1p_sqrt);
	const double   b2zp     = -0.25f * bb2p / b0zp;
 #endif

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Zeros, combination

	z_eq_b [0] = TZ (b0zl * lvl_l + b0zb * lvl_b + b0zh * lvl_h);
	z_eq_b [1] = TZ (b1zl * lvl_l + b1zb * lvl_b + b1zh * lvl_h);
	z_eq_b [2] = TZ (               b2zb * lvl_b + b2zh * lvl_h);

#elif 1
	// Experimental generic version
	// Does not work very well for cutoff frequencies > fs / 4.
	// HPF fails for Q < sqrt(0.5)

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Decomposes the input equation into three basic filters using w0 and q

	// First, normalises the input equation:
	// - Scales the frequencies in order to put Nyquist at s = pi
	// - Unity coefficient for s^2 in the denominator: a2 = 1
	const double   sc_a    = 1 / s_eq_a [2];
	const double   fsc     = f0 / fs;
	const double   fscp    = (2 * fstb::PI) * fsc;
	const double   fscp2   = fscp * fscp;

	std::array <double, 3>  bs {
		s_eq_b [0] * (sc_a * fscp2),
		s_eq_b [1] * (sc_a * fscp ),
		s_eq_b [2] *  sc_a
	};
	std::array <double, 3>  as {
		s_eq_a [0] * (sc_a * fscp2),
		s_eq_a [1] * (sc_a * fscp ),
		1
	};

	// Finds w0 and q
	// Eq. 30, 35 and 37
	// Q = w0 / a1
	const double   w02 = as [0];
	const double   w0  = sqrt (w02);
	const double   q   = w0 / as [1]; // Actually capital Q in the documentation.

	// Eq. 11: q = 1 / (2*Q), called r here in order to avoid confusion with Q.
	// den(H(s)) = w0^2 + 2*r*w0 + s^2
	const double   r   = 0.5f / q;

	// Computes the levels for each component: LP, BP, HP
	// Eq. 30, 35 and 37
	const double   lvl_l = bs [0] / w02;
	const double   lvl_b = bs [1] * q / w0;
	const double   lvl_h = bs [2];

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Poles

	// exp (-r * w0)
//	const double   emrw0 = exp (as [1] * -0.5f);
	const double   emrw0 = exp (-r * w0);

	// Eq. 12
	double         a1z   = -2 * emrw0;
	const double   r2    = r * r;
	if (r <= 1)
	{
		a1z *= cos ( w0 * sqrt (1 - r2));
	}
	else
	{
		a1z *= cosh (w0 * sqrt (r2 - 1));
	}
	const double   a2z   = emrw0 * emrw0;

	z_eq_a [0] = TZ (1);
	z_eq_a [1] = TZ (a1z);
	z_eq_a [2] = TZ (a2z);

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Zeros

	// Eq. 27
	const double   aa0_sqrt = 1 + a1z + a2z;
	const double   aa0      = fstb::sq (aa0_sqrt     );
	const double   aa1      = fstb::sq (1 - a1z + a2z);
	const double   aa2      = -4 * a2z;

	// w = 0:
	// phi0 = 1, phi1 = 0, phi2 = 0
	// |H(0)|^2 = B0 / A0
	// B0 = |H(0)|^2 * A0
	const double   h2s_0    = fstb::sq (bs [0] / as [0]);
	const double   bb0      = h2s_0 * aa0;

 #if 1

	/*
	Complex version using the magnitude and its slope at w0 to find B1 and B2.
	Actually not working much better than the trivial version.

	phi0(x) = 1 - sin(x/2)^2            phi0'(x) = -1/2 * sin(x)
	phi1(x) =     sin(x/2)^2            phi1'(x) =  1/2 * sin(x)
	phi2(x) = 4 * phi0(x) * phi1(x)     phi2'(x) = sin(2*x)
	f(x)  =   (B0 * phi0(x) + B1 * phi1(x) + B2 * phi2(x))
	        / (A0 * phi0(x) + A1 * phi1(x) + A2 * phi2(x))
	f'(x) = 
	        (B0 * phi0'(x) + B1 * phi1'(x) + B2 * phi2'(x))
	   /    (A1 * phi1(x)  + A0 * phi0(x)  + A2 * phi2(x) )
	-    (  (A0 * phi0'(x) + A1 * phi1'(x) + A2 * phi2'(x))
	      * (B1 * phi1(x)  + B0 * phi0(x)  + B2 * phi2(x) ))
	   /    (A1 * phi1(x)  + A0 * phi0(x)  + A2 * phi2(x) )^2

	Equation for |H(w0)|^2 = G*G: f(w0) = G*G

	G*G =   (B0 * phi0(w0) + B1 * phi1(w0) + B2 * phi2(w0))
	      / (A0 * phi0(w0) + A1 * phi1(w0) + A2 * phi2(w0))
	G*G = (B0 * phi0(w0) + B1 * phi1(w0) + B2 * phi2(w0)) / aaphisum

	  B1 * phi1(w0)
	+ B2 * phi2(w0)
	=
	  G*G * aaphisum - B0 * phi0(w0)

	Equation for d/dw(|H(w)|^2)(w=w0) = p: f'(w0) = p

	    p
	  * (A1 * phi1(w0)  + A0 * phi0(w0)  + A2 * phi2(w0))^2
	= 
	    (B0 * phi0'(w0) + B1 * phi1'(w0) + B2 * phi2'(w0))
	  * (A1 * phi1(w0)  + A0 * phi0(w0)  + A2 * phi2(w0))
	-   (A0 * phi0'(w0) + A1 * phi1'(w0) + A2 * phi2'(w0))
	  * (B1 * phi1(w0)  + B0 * phi0(w0)  + B2 * phi2(w0))

	  B1 * (aaphisum * phi1'(w0) - aaphidsum * phi1(w0))
	+ B2 * (aaphisum * phi2'(w0) - aaphidsum * phi2(w0))
	= 
	  p * aaphisum^2 + B0 * (aaphidsum * phi0(w0) - aaphisum * phi0'(w0))

	*/

	// Eq. 26 at the cutoff frequency
	const double   s2w0h    = fstb::sq (sin (w0 * 0.5f));
	const double   phi0     = 1 - s2w0h;
	const double   phi1     =     s2w0h;
	const double   phi2     = 4 * phi0 * phi1;
	const double   aaphisum = aa0 * phi0  + aa1 * phi1  + aa2 * phi2;

	const double   phi1d    = 0.5f * sin (w0);
	const double   phi0d    = -phi1d;
	const double   phi2d    = sin (2 * w0);
	const double   aaphidsum = aa0 * phi0d + aa1 * phi1d + aa2 * phi2d;

	// Finds the gain and slope at w0
	const double   delta    = 1e-6 * w0;
	const double   gp       = compute_h2s (&bs [0], &as [0], w0 - delta);
	const double   gn       = compute_h2s (&bs [0], &as [0], w0 + delta);
	const double   slope    = (gn - gp) / (2 * delta);
	const double   gain_sq  = (gp + gn) * 0.5f;

	// Equation system:
	// B1 * m11 + B2 * m12 = v1
	// B1 * m21 + B2 * m22 = v2
	const double   m11 = phi1;
	const double   m12 = phi2;
	const double   v1  = gain_sq * aaphisum - bb0 * phi0;

	const double   m21 = aaphisum * phi1d - aaphidsum * phi1;
	const double   m22 = aaphisum * phi2d - aaphidsum * phi2;
	const double   v2  =
		  slope * fstb::sq (aaphisum)
		+ bb0 * (aaphidsum * phi0 - aaphisum * phi0d);

	// Solution
	const double   bb1 = (v1 * m22 - v2 * m12) / (m11 * m22 - m12 * m21);
	const double   bb2 = (v1 - bb1 * m11) / m12;

	assert ((phi0 * bb0 + phi1 * bb1 + phi2 * bb2) * aaphisum >= 0);

 #else

	// Trivial version, using magnitudes at Nyquist and w0 to find B1 and B2.
	// Gives bad results in some cases.

	// w = pi:
	// phi0 = 0, phi1 = 1, phi2 = 0
	// |H(pi)|^2 = B1 / A1
	// B1 = |H(pi)|^2 * A1
	const double   h2s_n    = compute_h2s (&bs [0], &as [0], fstb::PI);
	const double   bb1      = h2s_n * aa1;

	// w = w0:
	// |H(w0)|^2 = (B0*phi0 + B1*phi1 + B2*phi2) / (A0*phi0 + A1*phi1 + A2*phi2)
	// B2 = (|H(w0)|^2 * (A0*phi0 + A1*phi1 + A2*phi2) - B0*phi0 - B1*phi1) / phi2

	// Eq. 26 at the cutoff frequency
	const double   s2w0h    = fstb::sq (sin (w0 * 0.5f));
	const double   phi0     = 1 - s2w0h;
	const double   phi1     =     s2w0h;
	const double   phi2     = 4 * phi0 * phi1;

	const double   aaphisum = aa0 * phi0 + aa1 * phi1 + aa2 * phi2;

	const double   h2s_f0   = compute_h2s (&bs [0], &as [0], w0);
	const double   bb2      =
		(h2s_f0 * aaphisum - bb0 * phi0 - bb1 * phi1) / phi2;

 #endif

	// Eq. 29
	// max() added because sometimes B1 or B2 are negative.
	const double   bb0_sqrt = sqrt (bb0);
	const double   bb1_sqrt = sqrt (std::max (bb1, 0.0));
	const double   ww      =  0.5f  * (bb0_sqrt + bb1_sqrt);
	const double   b0z     =  0.5f  * (ww + sqrt (std::max (ww * ww + bb2, 0.0)));
	const double   b1z     =  0.5f  * (bb0_sqrt - bb1_sqrt);
	const double   b2z     = -0.25f * bb2 / b0z;

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



// Computes |H(w)|^2
// w = pulsation (rad)
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
