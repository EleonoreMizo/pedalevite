/*****************************************************************************

        TestOscSample.cpp
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

#include "mfx/dsp/osc/OscSample.h"
#include "mfx/dsp/rspl/InterpFtor.h"
#include "test/FileOp.h"
#include "test/TestOscSample.h"
#include "test/TestSampleMipMapper.h"
#include "test/TimerAccurate.h"

#include <algorithm>
#include	<vector>

#include <cassert>
#include <cstdint>
#include <cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestOscSample::perform_test ()
{
	using namespace mfx::dsp::osc;
	using namespace mfx::dsp::rspl;

	const int		spl_len   = 10L * 1000L * 1000L;
	const int		block_len = 64;
	const int		dest_len  = 44100 * 12;

	// We use it as an easy sample source.
	TestSampleMipMapper	tsmm (spl_len);

	// Oscillator setup
	typedef	OscSample <
		TestSampleMipMapper::SampleDataType,
		InterpFtor::CubicHermite,
		int32_t
	>					OscType;
	OscType			osc;

	osc.set_sample_data (tsmm.use_sample_data ());
	osc.set_bandlimit (-0x10000);
	osc.set_pitch (0x00000);

	// Processing
	std::vector <OscType::CalcDataType>	dest_int (dest_len);

	const int      pitch_range = 8 * 0x10000;	// 8 octaves
	const int      pitch_step  =  pitch_range / (dest_len / block_len);
	int            pitch       = -pitch_range / 4;
	assert (pitch_step != 0);

	printf ("Testing OscSample...\n");

	TimerAccurate  tim;
	tim.start ();
	for (int block_pos = 0; block_pos < dest_len; block_pos += block_len)
	{
		osc.set_pitch (pitch);
		const int      work_len = std::min (block_len, dest_len - block_pos);
		osc.process_block (&dest_int [block_pos], work_len);
		pitch += pitch_step;
	}

	tim.stop ();

	double	      spl_per_s = tim.get_best_rate (dest_len);
	const double   mega_sps  = spl_per_s / 1000000.0;
	printf ("Speed: %12.3f Mspl/s.\n", mega_sps);

	// Format conversion
	std::vector <float>	dest;
	for (size_t pos = 0; pos < dest_int.size (); ++pos)
	{
		dest.push_back (dest_int [pos] * (1.0f / 0x8000));
	}

	FileOp::save_wav ("results/oscsample0.wav", dest, 44100, 0.5f);

	return 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
