/*****************************************************************************

        TestWindows.cpp
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

#include "mfx/dsp/wnd/Taylor.h"
#include "mfx/dsp/wnd/Ultraspherical.h"
#include "mfx/FileOpWav.h"
#include "test/TestWindows.h"

#include <array>
#include <vector>

#include <cassert>
#include <cmath>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestWindows::perform_test ()
{
	constexpr double  sample_freq = 44100;
	typedef float Flt;
	constexpr int  win_len = 255;
	std::array <Flt, win_len> win_data;
	std::vector <Flt> dst;

	typedef mfx::dsp::wnd::Ultraspherical <Flt> WinUltraS;
	WinUltraS      win_u;

	// Dolph-Chebyshev equiv: mu = 0
	for (double mu = 0; mu <= 4; mu += 0.125)
	{
		const double	r    = 1e-3;	// -60 dB
		const double	x_mu =
			WinUltraS::compute_x_mu_for_prescribed_ripple_ratio (win_len, mu, r);
		printf (
			"Ultraspherical: len = %d, mu = %f, x_mu = %f\n",
			win_len, mu, x_mu
		);
		win_u.set_mu (mu);
		win_u.set_x_mu (x_mu);
		win_u.make_win (win_data.data (), win_len);
		dst.insert (dst.end (), win_data.begin (), win_data.end ());
	}

	mfx::FileOpWav::save (
		"results/win-ultraspherical1.wav", dst, sample_freq, 0.5f
	);

	dst.clear ();
	typedef mfx::dsp::wnd::Taylor <Flt> WinTaylor;
	WinTaylor      win_t;
	for (int db = -40; db >= -100; db -= 2)
	{
		const double   lvl  = pow (10, double (db) / 20);
		win_t.set_side_lobe_lvl (lvl);
		const int      nbar = win_t.compute_nbar_min (lvl);
		win_t.set_nbar (nbar);
		printf (
			"Taylor: len = %d, lvl_db = %-4d, nbar = %3d\n",
			win_len, db, nbar
		);
		win_t.make_win (win_data.data (), win_len);
		dst.insert (dst.end (), win_data.begin (), win_data.end ());
	}

	mfx::FileOpWav::save (
		"results/win-taylor1.wav", dst, sample_freq, 0.5f
	);

	return 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
