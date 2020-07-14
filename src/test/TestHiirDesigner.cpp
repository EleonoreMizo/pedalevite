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

#include "fstb/def.h"
#include "hiir/PolyphaseIir2Designer.h"
#include "test/TestHiirDesigner.h"

#include <vector>

#include <cassert>
#include <cmath>
#include <cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestHiirDesigner::perform_test ()
{
	int            ret_val = 0;

	// This code designs a filter on a group delay constraint

	const double   fs       = 44100.0;   // Sample frequency, base rate (decimated)
	const double   f_tst    =  1000.0;   // Frequency for the group delay test
	const double   tg_gdly  =     4.0;   // Desired group delay (in samples) at oversampled rate
	const int      nbr_coef =     8;     // Number of coefficients
	const double   prec     =  1e-6;     // Precision (in samples) to reach for the group delay at f_tst

	const double   ft_rel   = f_tst / (fs * 2);

	double         rs_attn = 0;
	double         rs_tb   = 0;
	std::vector <double> rs_coef (nbr_coef);
	const hiir::PolyphaseIir2Designer::ResCode   res =
		hiir::PolyphaseIir2Designer::compute_coefs_spec_order_gdly (
			rs_coef.data (), &rs_attn, &rs_tb, nbr_coef, tg_gdly, ft_rel, prec
		);

	if (res != hiir::PolyphaseIir2Designer::ResCode_OK)
	{
		printf ("Design requirements cannot be met.\n");
		ret_val = static_cast <int> (res);
	}

	else
	{
		printf ("Coefficients: %d\n"    , nbr_coef);
		printf ("Attenuation : %g dB\n" , rs_attn);
		printf ("Trans BW    : %g\n"    , rs_tb);
		printf ("Passband    : %g Hz @ %g Hz sampling rate\n",
			(0.5 - rs_tb) * fs, fs);
		printf ("Group delay : %g spl\n", tg_gdly);
		printf ("GD rel freq : %g\n"    , ft_rel);
		printf ("Coefficient list:\n");
		for (auto a : rs_coef)
		{
			printf ("\t%.17g\n", a);
		}
	}

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
