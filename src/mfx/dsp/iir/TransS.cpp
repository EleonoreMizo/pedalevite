/*****************************************************************************

        TransS.cpp
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
#include "fstb/fnc.h"
#include "mfx/dsp/iir/TransS.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	TransS::conv_roots_real_to_poly_1 (double poly [2], double k, double root)
{
	assert (poly != 0);

	poly [0] = -root * k;
	poly [1] = k;
}



void	TransS::conv_roots_real_to_poly_2 (double poly [3], double k, double root_1, double root_2)
{
	assert (poly != 0);


	poly [0] = root_1 * root_2 * k;
	poly [1] = (-root_1 -root_2) * k;
	poly [2] = k;
}



void	TransS::conv_roots_cplx_to_poly_2 (double poly [3], double k, const Cplx &root)
{
	assert (poly != 0);
	assert (&root != 0);

	const double   rr = root.real ();
	const double   ri = root.imag ();

	poly [0] = (rr * rr + ri * ri) * k;
	poly [1] = -2 * rr * k;
	poly [2] = k;
}



/*
==============================================================================
Name: conv_lp_pz_to_pb_pz
Description:
	Transform a pole or zero from a lowpass filter into two poles or zeros for
	an equivalent bandpass filter.
	Feeding the function with the conjugate of the pole/zero is not necessary,
	because output poles/zeros may be also directly conjugate.
	The function operates at w = 1 rad/s as center frequency for both LPF and
	BPF.                                 
Input parameters:
	- pz_lp: Pole or zero to transform.
	- bw: Pseudo bandwidth (for w = 1 rad/s). Equivalent to 1/Q. > 0.
Output parameters:
	- pz_bp_1: 1st BP pole/zero.
	- pz_bp_2: 2nd BP pole/zero.
Throws: Nothing
==============================================================================
*/

void	TransS::conv_lp_pz_to_pb_pz (Cplx &pz_bp_1, Cplx &pz_bp_2, const Cplx &pz_lp, double bw)
{
	assert (&pz_bp_1 != 0);
	assert (&pz_bp_2 != 0);
	assert (&pz_lp != 0);
	assert (bw > 0);

	const Cplx     num_1 = pz_lp * bw;
	const Cplx     num_2_sq = num_1 * num_1 - 4.0;

	// Tout ca pour extraire une racine avec un angle entre -pi/2 et pi/2, faudra
	// penser a l'isoler dans une fonction.
	double         rho = sqrt (  num_2_sq.real () * num_2_sq.real ()
	                           + num_2_sq.imag () * num_2_sq.imag ());
	double         theta = 0;
	if (fstb::is_null (num_2_sq.real ()))
	{
		if (num_2_sq.imag () > 0)
		{
			theta = fstb::PI * 0.5;
		}
		else if (num_2_sq.imag () < 0)
		{
			theta = -fstb::PI * 0.5;
		}
	}
	else
	{
		theta = atan (num_2_sq.imag () / num_2_sq.real ());
		if (num_2_sq.real () < 0)
		{
			if (theta < 0)
			{
				theta += fstb::PI;
			}
			else
			{
				theta -= fstb::PI;
			}
		}
	}
	rho = sqrt (rho);
	theta *= 0.5;
	const Cplx		num_2 = Cplx (cos (theta), sin (theta)) * rho;

	pz_bp_1 = (num_1 + num_2) / 2.0;
	pz_bp_2 = (num_1 - num_2) / 2.0;
}



/*
==============================================================================
Name: conv_lp_ap_to_pb_biq
Description:
	Sets 2 biquads as bandpass filters from a pair of conjugate poles of a
	lowpass prototype filter.
	The prototype LPF must be an all-pole form (without zero). For example a
	Butterworth LPF.                
	Only one of the two poles has to be provided, the other one is deduced.
Input parameters:
	- lp_pole: LPF pole to transform, cutoff = 1 rad/s. Should be in the real
		negative plane (stability condition).
	- bw: Pseudo bandwidth (for w = 1 rad/s). Equivalent to 1/Q. > 0.
Output parameters:
	- b_1: Num. of the 1st biquad in the S-plane, index is the power of s.
	- a_1: Denom. of the 1st biquad in the S-plane.
	- b_2: Num. of the 2nd biquad in the S-plane, index is the power of s.
	- a_2: Denom. of the 2nd biquad in the S-plane.
Throws: Nothing
==============================================================================
*/

void	TransS::conv_lp_ap_to_pb_biq (double b_1 [3], double a_1 [3], double b_2 [3], double a_2 [3], const Cplx &lp_pole, double bw)
{
	assert (b_1 != 0);
	assert (a_1 != 0);
	assert (b_2 != 0);
	assert (a_2 != 0);
	assert (&lp_pole != 0);
	assert (lp_pole.real () < 0);
	assert (bw > 0);

	Cplx           pz_bp [2];
	conv_lp_pz_to_pb_pz (pz_bp [0], pz_bp [1], lp_pole, bw);

	double * const b_ptr_arr [2] = { b_1, b_2 };
	double * const a_ptr_arr [2] = { a_1, a_2 };

	for (int biq = 0; biq < 2; ++biq)
	{
		const double   a1 = -2 * pz_bp [biq].real ();
		const double   a2 =   pz_bp [biq].real () * pz_bp [biq].real ()
		                    + pz_bp [biq].imag () * pz_bp [biq].imag ();
		const double   b1 = bw;
		assert (a1 > 0);
		assert (a2 > 0);

		a_ptr_arr [biq] [2] = a2;
		a_ptr_arr [biq] [1] = a1;
		a_ptr_arr [biq] [0] = 1;

		b_ptr_arr [biq] [2] = 0;
		b_ptr_arr [biq] [1] = b1;
		b_ptr_arr [biq] [0] = 0;
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
