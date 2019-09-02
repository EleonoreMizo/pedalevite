/*****************************************************************************

        TestOscWavetable.cpp
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

#include "fstb/def.h"
#include "fstb/fnc.h"
#include "mfx/dsp/osc/WavetableMipMapper.h"
#include "test/FileOp.h"
#include "test/TestOscWavetable.h"
#include "test/TimerAccurate.h"

#include <vector>

#include <cassert>
#include <cstdint>
#include <cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestOscWavetable::perform_test ()
{
	int            ret_val = test_valid ();

	if (ret_val == 0)
	{
		test_speed ();
	}

	return (ret_val);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestOscWavetable::test_valid ()
{
	std::vector <float>  result_m;

	OscType        osc;
	OscType::WavetableDataType wt;
	configure_osc (osc, wt);

	const int      sample_freq = 44100;
	const int      block_len   = 256;
	const int      len         = (sample_freq * 16) & ~(block_len - 1);
	assert (len % block_len == 0);
	std::vector <float>  data (len);

	const int      nbr_oct = 10;
	const float    mult    = float (nbr_oct << OscType::PITCH_FRAC_BITS) / len;
	const int32_t  offset  = BASE_PITCH - (nbr_oct << OscType::PITCH_FRAC_BITS);
	for (int pos = 0; pos < len; pos += block_len)
	{
		const int32_t  pitch = offset + fstb::floor_int (pos * mult);
		osc.set_pitch (pitch);
		osc.process_block (&data [pos], block_len);
	}

	result_m.insert (result_m.end (), data.begin (), data.end ());

	for (int pos = 0; pos < len; pos += block_len)
	{
		const double	freq  =
			1000 * exp (cos (pos * 2 * fstb::PI * 3 / sample_freq) * 1);
		const int32_t  pitch =
			osc.conv_freq_to_pitch (float (freq), float (sample_freq));
		osc.set_pitch (pitch);
		osc.process_block (&data [pos], block_len);
	}

	result_m.insert (result_m.end (), data.begin (), data.end ());

	FileOp::save_wav ("results/oscwavetable0.wav", result_m, 44100, 0.5f);

	return 0;
}



void	TestOscWavetable::test_speed ()
{
	const int      block_len  = 256;
	const int      nbr_blocks = 65536;

	OscType        osc;
	OscType::WavetableDataType	wt;
	configure_osc (osc, wt);
	osc.set_pitch (osc.conv_freq_to_pitch (1000, 44100));

	std::vector <float>   dest (block_len);
	float *        dest_ptr = &dest [0];

	printf ("OscWavetable speed test...\n");

	TimerAccurate  tim;
	double         acc_dummy = 0;

	tim.reset ();
	tim.start ();
	for (int block_cnt = 0; block_cnt < nbr_blocks; ++block_cnt)
	{
		osc.process_block (dest_ptr, block_len);

		// Prevents the optimizer to remove all the loops
		acc_dummy += dest_ptr [block_len - 1];
	}
	tim.stop ();

	double	      spl_per_s = tim.get_best_rate (block_len * nbr_blocks);
	spl_per_s += fstb::limit (acc_dummy, -1e-300, 1e-300); // Anti-optimizer trick
	const double   mega_sps  = spl_per_s / 1000000.0;
	printf ("Speed: %12.3f Mspl/s.\n", mega_sps);
}



void	TestOscWavetable::configure_osc (OscType &osc, OscType::WavetableDataType &wt)
{
	assert (&osc != 0);
	assert (&wt != 0);

	// Wavetable generation
	const int      last_table = wt.get_nbr_tables () - 1;
	const int      table_len  = wt.get_table_len (last_table);
	for (long pos = 0; pos < table_len; ++pos)
	{
#if 1
		// Saw
		wt.set_sample (
			last_table,
			(pos - table_len / 4) & (table_len - 1),
			(float (pos * 2) / table_len) - 1
		);
#else
		// Sine
		wt.set_sample (
			last_table,
			pos,
			cos (pos * (2 * basic::PI) / table_len)
		);
#endif
	}

	// Mipmapping
	mfx::dsp::osc::WavetableMipMapper <OscType::WavetableDataType> mipmapper;
	mipmapper.build_mipmaps (wt);

	osc.set_wavetable (wt);
	osc.set_base_pitch (BASE_PITCH);
}



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
