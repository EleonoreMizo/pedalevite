/*****************************************************************************

        TestOscSampleSyncFade.cpp
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

#include "mfx/FileOpWav.h"
#include "test/TestOscSampleSyncFade.h"
#include "test/TimerAccurate.h"

#include <algorithm>

#include <cassert>
#include <cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestOscSampleSyncFade::perform_test ()
{
	using namespace mfx::dsp;

	const int		block_len = 16;
	const int		dest_len  = 44100 * 12;

	std::vector <float>  result_m;

	// We use it as an easy sample source.
	const int		spl_len = 10 * 1000 * 1000;
	TestSampleMipMapper	tsmm (spl_len);

	// Oscillator setup
	OscType			osc;

	wnd::Generic <double, wnd::CoefGenBHMinLobe> win_gen;

	osc.set_sample_data (tsmm.use_sample_data ());
	osc.set_bandlimit (-0x10000);
	osc.set_base_pitch (16 * 0x10000);

	printf ("Testing OscSampleSyncFade...\n");

	// Simple slave pitch sweep
	perform_test_internal (
		osc, result_m, dest_len, block_len,
		0, 0,                // Sync pos
		+0x72345, +0x72345,  // Master pitch
		-0x20000, +0x60000   // Slave pitch
	);

	// Master pitch sweep
	perform_test_internal (
		osc, result_m, dest_len, block_len,
		130, 0,              // Sync pos
		+0x80000, +0xFFFF0,  // Master pitch
		-0x00000, +0x00000   // Slave pitch
	);

	// Master pitch sweep from rythm range to audio range
	perform_test_internal (
		osc, result_m, dest_len, block_len,
		0, 0,                // Sync pos
		+0x10000, +0xB2345,  // Master pitch
		-0x00000, +0x00000   // Slave pitch
	);

	perform_test_internal (
		osc, result_m, 44100*2, block_len,
		0, 1.0f / 16,        // Sync pos
		+0xB2345, +0xB2345,  // Master pitch
		-0x00000, +0x00000   // Slave pitch
	);
	perform_test_internal (
		osc, result_m, 44100*4, block_len,
		0, 0.5,              // Sync pos
		+0xB2345, +0xB2345,  // Master pitch
		-0x00000, +0x00000   // Slave pitch
	);
	perform_test_internal (
		osc, result_m, 44100*4, block_len,
		0, 0.25,             // Sync pos
		+0xB2345, +0xB2345,  // Master pitch
		-0x00000, +0x00000   // Slave pitch
	);
	perform_test_internal (
		osc, result_m, 44100*2, block_len,
		44100*2, -1,         // Sync pos
		+0xB2345, +0xB2345,  // Master pitch
		-0x00000, +0x00000   // Slave pitch
	);

	// Position sweep
	for (int	ratio_l2 = 3; ratio_l2 <= 8; ratio_l2 += 5)
	{
		for (int slave_pitch = -0x20000; slave_pitch <= 0x60000; slave_pitch += 0x10000)
		{
			perform_test_internal (
				osc, result_m, dest_len, block_len,
				0, 1.0f / float (1 << ratio_l2), // Sync pos
				+0x92345, +0x92345,              // Master pitch
				slave_pitch, slave_pitch         // Slave pitch
			);
		}
	}

	mfx::FileOpWav::save (
		"results/oscsamplesyncfade0.wav", result_m, 44100, 0.5f
	);

	printf ("done.\n");

	const int		speed_test_block_len  = 1024;
	const int		speed_test_nbr_blocks = 100L * 1000L;

	std::vector <OscType::CalcDataType>	dummy_result (speed_test_block_len);

	printf ("Evaluating performances, master pitch ~= 350 Hz...");
	TimerAccurate  tim;

	tim.start ();
	for (int block = 0; block < speed_test_nbr_blocks; ++block)
	{
		perform_test_internal_2 (
			osc, dummy_result, speed_test_block_len, speed_test_block_len,
			0, 0,                // Sync pos
			+0xA0000, +0xA0000,  // Master pitch
			+0x00000, +0x00000   // Slave pitch
		);
	}
	tim.stop ();

	double	      spl_per_s = tim.get_best_rate (
		speed_test_block_len * speed_test_nbr_blocks
	);
	const double   mega_sps  = spl_per_s / 1000000.0;
	printf ("%12.3f Mspl/s.\n", mega_sps);

	return 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	TestOscSampleSyncFade::perform_test_internal (OscType &osc, std::vector <float> &result_m, int dest_len, int block_len, int sync_pos_start, float sync_pos_speed, int master_pitch_start, int master_pitch_end, int slave_pitch_start, int slave_pitch_end)
{
	std::vector <OscType::CalcDataType> dest;

	perform_test_internal_2 (
		osc, dest, dest_len, block_len,
		sync_pos_start, sync_pos_speed,
		master_pitch_start, master_pitch_end,
		slave_pitch_start, slave_pitch_end
	);

	add_to_dest (result_m, dest);
}



void	TestOscSampleSyncFade::perform_test_internal_2 (OscType &osc, std::vector <OscType::CalcDataType> &dest, int dest_len, int block_len, int sync_pos_start, float sync_pos_speed, int master_pitch_start, int master_pitch_end, int slave_pitch_start, int slave_pitch_end)
{
	assert (dest_len > 0);
	assert (block_len > 0);
	assert (block_len <= dest_len);

	dest.resize (dest_len);

	const int      nbr_blocks = dest_len / block_len;
	const int      src_len    = osc.use_sample_data ().get_table_len (0);

	fstb::FixedPoint  sync_pos (sync_pos_start);
	fstb::FixedPoint  sync_step (sync_pos_speed * float (block_len));

	fstb::FixedPoint  master_pitch (master_pitch_start);
	fstb::FixedPoint  master_pitch_step (double (master_pitch_end - master_pitch_start) / nbr_blocks);

	fstb::FixedPoint  slave_pitch (slave_pitch_start);
	fstb::FixedPoint  slave_pitch_step (double (slave_pitch_end - slave_pitch_start) / nbr_blocks);

	for (int block_pos = 0; block_pos < dest_len; block_pos += block_len)
	{
		osc.set_master_pitch (master_pitch.get_int_val ());
		osc.set_slave_pitch (slave_pitch.get_int_val ());

		int            sync_pos_int = sync_pos.get_int_val ();
		int            wave_length  = osc.get_wave_length () * 2; // * 2 because of the fadeout
		if (sync_pos_int + wave_length >= src_len)
		{
			assert (wave_length < src_len);
			sync_pos_int = std::max (src_len - wave_length, 0);
			sync_pos.set_val (sync_pos_int, 0);
		}
		else if (sync_pos_int < 0)
		{
			sync_pos.set_val (0, 0);
		}
		osc.set_sync_pos (sync_pos);

		const int      work_len = std::min (block_len, dest_len - block_pos);
		osc.process_block (&dest [block_pos], work_len);

		master_pitch += master_pitch_step;
		slave_pitch  += slave_pitch_step;
		sync_pos     += sync_step;
	}
}



void	TestOscSampleSyncFade::add_to_dest (std::vector <float> &result_m, const std::vector <OscType::CalcDataType> &dest_int)
{
	// Format conversion
	for (size_t pos = 0; pos < dest_int.size (); ++pos)
	{
		result_m.push_back (float (dest_int [pos]) * (1.0f / 0x8000));
	}
}



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
