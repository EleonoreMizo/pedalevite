/*****************************************************************************

        TestSvf.cpp
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

#include "fstb/fnc.h"
#include "mfx/dsp/iir/Svf2p.h"
#include "mfx/dsp/iir/SvfCore.h"
#include "mfx/dsp/iir/SvfMixerDefault.h"
#include "mfx/FileOpWav.h"
#include "test/TestSvf.h"

#include <vector>

#include <cassert>
#include <cmath>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestSvf::perform_test ()
{
	int            ret_val = 0;

	const double   sample_freq = 44100;
	const int      len = fstb::round_int (sample_freq * 10);

	std::vector <float>  src (len);
	std::vector <float>  dst (len);

	// Sawtooth
	const int      per = fstb::round_int (sample_freq / 55.0);
	for (int pos = 0; pos < len; ++pos)
	{
		src [pos] = (pos % per) * (2.f / per) - 1.f;
	}

	mfx::dsp::iir::Svf2p       design;
	mfx::dsp::iir::SvfCore <>  svf;
	design.set_sample_freq (float (sample_freq));
	design.set_type (mfx::dsp::iir::Svf2p::Type_LOWPASS);
	design.set_q (4);
	design.set_reso (1);

	for (int pos = 0; pos < len; ++pos)
	{
		const float    freq = float (20 * pow (1000, double (pos) / len));
		design.set_freq (freq);
		design.update_eq ();
		svf.set_coefs (design.get_g0 (), design.get_g1 (), design.get_g2 ());
		svf.set_mix (design.get_v0m (), design.get_v1m (), design.get_v2m ());
		dst [pos] = svf.process_sample (src [pos]);
	}

	mfx::FileOpWav::save ("results/svf1.wav", dst, sample_freq, 0.25f);

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
