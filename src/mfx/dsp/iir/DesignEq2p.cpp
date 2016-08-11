/*****************************************************************************

        DesignEq2p.cpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "mfx/dsp/iir/DesignEq2p.h"

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: make_nyq_peak
Description:

Computes a second-order peaking filter behaving at Nyquist frequency as the
analog prototype does: the peak is not compressed on the Nyquist freq.
The peak level cannot be 0.

Implementation details:

The function uses the Orfanidis algorithm. The selectivity q is mapped on the
Ofanidis variables G0, GB and dW the following way:

	G0 = 1
	GB = sqrt (G)
	dW = f0 / q

Then dW is fixed in order to truncate the passband with the Nyiquist
frequency.

Input parameters:
	- q: Filter selectivity. > 0.
	- lvl: Peak level. > 0
	- f0: Peak frequency, Hz. ]0 ; fs/2[
	- fs: Sampling frequency, Hz.
Output parameters:
	- bz: Num. of the equation in the Z plane, index is the power of 1/z.
	- az: Denom. of the equation in the Z plane.
Throws: Nothing
==============================================================================
*/

void	DesignEq2p::make_nyq_peak (float bz [3], float az [3], double q, double lvl, double f0, double fs)
{
	assert (bz != 0);
	assert (az != 0);
	assert (q > 0);
	assert (lvl > 0);
	assert (f0 > 0);
	assert (f0 < fs * 0.5);

	const double	w0 = fstb::PI * f0 / fs;
	const double	gb = sqrt (lvl);

	const double	q_i = 1 / q;
	const double	fx = (sqrt (q_i * q_i + 4) - q_i) * 0.5;
	const double	f1 = f0 * fx;
	const double	f2 = f0 / fx;
	const double	margin = 0.001;
	const double	f1p = std::min (f1, fs * (0.5 - margin * 2));
	const double	f2p = std::min (f2, fs * (0.5 - margin    ));
	const double	dw = fstb::PI * (f2p - f1p) / fs;

	make_nyq_peak (bz, az, 1, lvl, gb, w0, dw);
}



/*
==============================================================================
Name: make_nyq_peak
Description:

Computes a second-order peaking filter behaving at Nyquist frequency as the
analog prototype does: the peak is not compressed on the Nyquist freq.
The peak level cannot be 0.

Bilinear transform is used and provided frequencies are prewarped by the
function.

Algorithm from:

	Sophocles J. Orfanidis, Digital Parametric Equalizer Design With Prescribed
	Nyquist-Frequency Gain, http://www.ece.rutgers.edu/~orfanidi/ece521/peq.pdf

Input parameters:
	- g0: Level at DC, >= 0.
	- g: Level at the peak, > 0.
	- gb: level of the bandwidth, > 0.
	- w0: Peak pulsation, rad/s, ]0 ; pi[.
	- dw: Peak bandwidth, rad/s, ]0 ; pi[. 
Output parameters:
	- bz: Num. of the equation in the Z plane, index is the power of 1/z.
	- az: Denom. of the equation in the Z plane.
Throws: Nothing
==============================================================================
*/

void	DesignEq2p::make_nyq_peak (float bz [3], float az [3], double g0, double g, double gb, double w0, double dw)
{
	assert (bz != 0);
	assert (az != 0);
	assert (g0 >= 0);
	assert (g > 0);
	assert (gb > 0);
	assert (w0 > 0);
	assert (w0 < fstb::PI);
	assert (dw > 0);
	assert (dw < fstb::PI);

	const double	g_2   = g  * g;
	const double	gb_2  = gb * gb;
	const double	g0_2  = g0 * g0;
	const double	w0_2  = w0 * w0;
	const double	dw_2  = dw * dw;
	const double	pi_2  = fstb::PI * fstb::PI;

	const double	f     = std::max (fabs (g_2 - gb_2), 1e-9);
	const double	g00   = std::max (fabs (g_2 - g0_2), 1e-9);
	const double	f00   = fabs (gb_2 - g0_2);
	const double	w0_2_m_pi_2   = w0_2 - pi_2;
	const double	w0_2_m_pi_2_2 = w0_2_m_pi_2 * w0_2_m_pi_2;
	const double	ratio_f00     = f00 * pi_2 * dw_2 / f;
	const double	num   = g0_2 * w0_2_m_pi_2_2 + g_2 * ratio_f00;
	const double	den   =        w0_2_m_pi_2_2 +       ratio_f00;
	const double	g1_2  = num / den;                  // (21)
	const double	g1    = sqrt (g1_2);
	const double	g0_g1 = g0 * g1;
	const double	g01   = fabs (g_2 - g0_g1);
	const double	g11   = fabs (g_2 - g1_2);
	const double	f01   = fabs (gb_2 - g0_g1);
	const double	f11   = std::max (fabs (gb_2 - g1_2), 1e-9);
	const double	tw    = tan (w0 * 0.5);             // (19)
	const double	w2    = sqrt (g11 / g00) * tw * tw; // (16)
	const double	dw_w  = (1 + sqrt (f00 / f11) * w2) * tan (dw * 0.5); // (19)
	const double	c     = f11 * dw_w * dw_w - 2 * w2 * (f01 - sqrt (f00 * f11));
	const double	d     = 2 * w2 * (g01 - sqrt (g00 * g11)); // (17)
	const double	a     = sqrt ((c + d) / f);                // (16)
	const double	b     = sqrt ((g_2 * c + gb_2 * d) / f);   // (16)

	const double	scale = 1 / (1 + w2 + a);
	const double	g0_w2 = g0 * w2;

	bz [0] = float ((g1 + g0_w2 + b ) * scale);
	bz [1] = float ( 2 * (g0_w2 - g1) * scale);
	bz [2] = float ((g1 + g0_w2 - b ) * scale);

	az [0] = 1;
	az [1] = float (2 * (w2 - 1)      * scale);
	az [2] = float ((1 + w2 - a)      * scale);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
