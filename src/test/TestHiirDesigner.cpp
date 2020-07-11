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



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "hiir/PolyphaseIir2Designer.h"
#include "test/TestHiirDesigner.h"

#include <vector>

#include <cassert>
#include <cmath>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestHiirDesigner::perform_test ()
{
	int            ret_val = 0;

	// This code designs a filter on a group delay constraint

	const double   fs       = 44100.0;   // Sample frequency, base rate
	const double   f_tst    =  1000.0;   // Frequency for the group delay test
	const double   tg_gdly  =     3.675; // Desired group delay (in samples) at oversampled rate
	const int      nbr_coef =     8;     // Number of coefficients
	const double   prec     =  1e-6;     // Precision (in samples) to reach for the group delay at f_tst

	const double   ft_rel   = f_tst / (fs * 2);

	// Simple bisection method
	double         lb_tb    = 0.5e-3;    // Lower bound for the transition bandwidth
	double         ub_tb    = 0.5 - lb_tb; // Upper bound for the transition bandwidth (exprimed as a fraction of the sample rate at full rate (2*fs))
	const double   ub_attn  = hiir::PolyphaseIir2Designer::compute_atten_from_order_tbw (
		nbr_coef, ub_tb
	);
	const double   lb_attn  = hiir::PolyphaseIir2Designer::compute_atten_from_order_tbw (
		nbr_coef, lb_tb
	);
	std::vector <double> ub_coef (nbr_coef);
	std::vector <double> lb_coef (nbr_coef);
	hiir::PolyphaseIir2Designer::compute_coefs_spec_order_tbw (
		ub_coef.data (), nbr_coef, ub_tb
	);
	hiir::PolyphaseIir2Designer::compute_coefs_spec_order_tbw (
		lb_coef.data (), nbr_coef, lb_tb
	);
	double         ub_gdly  = hiir::PolyphaseIir2Designer::compute_group_delay (
		ub_coef.data (), nbr_coef, ft_rel, false
	);
	double         lb_gdly  = hiir::PolyphaseIir2Designer::compute_group_delay (
		lb_coef.data (), nbr_coef, ft_rel, false
	);
	assert ((tg_gdly - ub_gdly) * (tg_gdly - lb_gdly) < 0);

	std::vector <double> rs_coef (nbr_coef);
	double         rs_attn = 0;
	double         rs_gdly = 0;
	double         rs_tb   = 0;
	do
	{
		rs_tb = (ub_tb + lb_tb) * 0.5;
		rs_attn = hiir::PolyphaseIir2Designer::compute_atten_from_order_tbw (
			nbr_coef, rs_tb
		);
		hiir::PolyphaseIir2Designer::compute_coefs_spec_order_tbw (
			rs_coef.data (), nbr_coef, rs_tb
		);
		rs_gdly  = hiir::PolyphaseIir2Designer::compute_group_delay (
			rs_coef.data (), nbr_coef, ft_rel, false
		);

		if ((tg_gdly - lb_gdly) * (tg_gdly - rs_gdly) < 0)
		{
			ub_tb   = rs_tb;
			ub_gdly = rs_gdly;
		}
		else
		{
			lb_tb   = rs_tb;
			lb_gdly = rs_gdly;
		}
	}
	while (fabs (rs_gdly - tg_gdly) > prec);

	printf ("Coefficients: %d\n", nbr_coef);
	printf ("Attenuation : %g dB\n", rs_attn);
	printf ("Trans BW:     %g\n", rs_tb);
	printf ("Group delay:  %g spl\n", rs_gdly);
	printf ("GD rel freq:  %g\n", ft_rel);
	printf ("Coefficient list:\n");
	for (auto a : rs_coef)
	{
		printf ("\t%.17g\n", a);
	}

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
