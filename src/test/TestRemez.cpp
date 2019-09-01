/*****************************************************************************

        TestRemez.cpp
        Author: Laurent de Soras, 2019

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

#include	"mfx/dsp/fir/Remez.h"
#include "test/TestRemez.h"
#include "test/TimerAccurate.h"

#include <cassert>
#include <cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestRemez::perform_test ()
{
	using namespace mfx::dsp::fir;

	int            ret_val = 0;

	Remez             remez;
	Remez::RemezSpec  spec;
	Remez::CoefList   coefs;

	// (129 ; 0.05) make it output wrong result
#if 0
	const int		nbr_coefs = 129;
	const double	f = 0.5;		// Cutoff freq, fraction of Nyquist frequency
	const double	df = 0.06;	// Transition band, fraction of Nyquist frequency
	const double	ripple_ratio = dsp::Remez::compute_ripple_ratio (0.1, 85);
#elif 1
	const int		nbr_coefs = 81;
	const double	f = 0.5;
	const double	df = 0.1;
	const double	ripple_ratio = 100;
#else
	const int		m = 64;
	const int		nbr_coefs = m * 16 + 1;
	const double	fc = 1.0 / m;
	const double	df = 1.5 * fc - 0.9 * fc;
	const double	f = 0.9 * fc + df / 2;
	const double	ripple_ratio = 1;//dsp::Remez::compute_ripple_ratio (0.1, 85);
#endif
	spec.push_back (RemezPoint (0.00      , f/2 - df/4, 1, 1));
	spec.push_back (RemezPoint (f/2 + df/4, 0.50      , 0, ripple_ratio));

	printf ("TestRemez, half-band LPF...\n");

	TimerAccurate  chrono;
	chrono.start ();
	const int      remez_result =
		remez.compute_coefs (coefs, nbr_coefs, spec, Remez::Type_BANDPASS);
	chrono.stop ();
	double         duration = double (chrono.get_best_duration ().count ());

	printf ("Return value: %d\n", remez_result);
	printf ("Duration    : %12.6f ms\n", duration * 1e-6);
	if (remez_result == 0)
	{
		printf ("Cofficients :\nb = [");
		for (int pos = 0; pos < nbr_coefs; ++pos)
		{
			printf ("%+.8f ", coefs [pos]);
			if (((pos+1) & 3) == 0)
			{
				printf ("...\n     ");
			}
		}
		printf ("]; freqz (b);\n\n");
	}

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
