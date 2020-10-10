/*****************************************************************************

        TestInterpPhase.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (TestInterpPhase_CODEHEADER_INCLUDED)
#define TestInterpPhase_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/lang/type_name.h"
#include "fstb/def.h"
#include "fstb/fnc.h"
#include "mfx/dsp/rspl/InterpFirPolyphase.h"
#include "test/BufferFiller.h"
#include "test/TimerAccurate.h"

#include <array>

#include <cassert>
#include <cstdint>
#include <cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class T, int NPL2>
int	TestInterpPhase <T, NPL2>::perform_test ()
{
	int            ret_val = 0;

	const std::string class_name =
		fstb::lang::type_name <InterpPhase> ().to_str ();
	printf ("Testing %s...\n", class_name.c_str ());

	if (ret_val == 0)
	{
		ret_val = perform_test_integrity ();
	}
	if (ret_val == 0)
	{
		ret_val = perform_test_performance ();
	}

   if (ret_val == 0)
   {
   	printf ("%s tested successfully.\n", class_name.c_str ());
   }
   else
   {
      printf ("%s test failed (%d).\n", class_name.c_str (), ret_val);
   }
   printf ("\n");

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class T, int NPL2>
int	TestInterpPhase <T, NPL2>::perform_test_integrity ()
{
	int            ret_val = 0;

	/*** To do ***/

	return ret_val;
}



template <class T, int NPL2>
int	TestInterpPhase <T, NPL2>::perform_test_performance ()
{
	int            ret_val = 0;

	typedef mfx::dsp::rspl::InterpFirPolyphase <
		InterpPhase, NBR_PHASES_L2
	> InterpFir;

	InterpFir      interp;

	const int      impulse_len = InterpFir::IMPULSE_LEN;
	const int      nbr_phases  = InterpFir::NBR_PHASES;
	double         impulse [impulse_len];
	for (int pos = 0; pos < impulse_len; ++pos)
	{
		const double   p =
			float (pos - impulse_len / 2) * (1.0f / nbr_phases);
		const double   val = fstb::sinc (p);
		impulse [pos] = val;
	}

	interp.set_impulse (impulse);

	const int      MAX_BUF_LEN  = 1024;
	const int      MAX_CHN      = 4;
	const int      CHN_STRIDE   = 20;
	const int      dest_len     = 64;	// <= MAX_DEST_LEN
#if ! defined (NDEBUG)
	const int      MAX_DEST_LEN = MAX_BUF_LEN - MAX_CHN * CHN_STRIDE;
	assert (dest_len < MAX_DEST_LEN);
#endif // NDEBUG

	const int64_t  read_step (0x89ABCDEFUL);	// Close to 0.5 on a 32-bit scale
	int64_t        read_pos (0);

	typedef std::array <float, MAX_BUF_LEN> ArrayFloat;
	ArrayFloat     buf_in;
	ArrayFloat     buf_out;
	BufferFiller::gen_rnd_non_zero (&buf_in [0], int (buf_in.size ()));
	double         acc_dummy = 0;

	const int      nbr_laps = 100;
	const int      nbr_loops = 1000;
	TimerAccurate  tim;

	// interpolate()
	{
		tim.reset ();
		tim.start ();
		for (int lap_cnt = 0; lap_cnt < nbr_laps; ++lap_cnt)
		{
			for (int loop_cnt = 0; loop_cnt < nbr_loops; ++loop_cnt)
			{
				for (int pos = 0; pos < dest_len; ++pos)
				{
					const int      read_pos_int  = int (read_pos >> 32);
					const uint32_t read_pos_frac = uint32_t (read_pos);

					buf_out [pos] = interp.interpolate (&buf_in [read_pos_int], read_pos_frac);

					read_pos += read_step;
				}
				read_pos &= 0xFFFFFFFFUL;	// Zeroes integer position

				// Prevents the optimizer to remove all the loops
				acc_dummy += buf_out [dest_len - 1];
			}

			tim.stop_lap ();
		}

		double	      spl_per_s = tim.get_best_rate (dest_len * nbr_loops);
		spl_per_s += fstb::limit (acc_dummy, -1e-300, 1e-300); // Anti-optimizer trick
		const double   mega_sps  = spl_per_s / 1000000.0;
		printf (
			"interpolate()          , FIR length %d, %d sample-blocks            : %12.3f Mspl/s.\n",
			PHASE_LEN,
			dest_len,
			mega_sps
		);
	}

	// interpolate_multi_chn()
	for (int nbr_chn = 1; nbr_chn <= MAX_CHN; ++nbr_chn)
	{
		tim.reset ();
		tim.start ();
		for (int lap_cnt = 0; lap_cnt < nbr_laps; ++lap_cnt)
		{
			for (int loop_cnt = 0; loop_cnt < nbr_loops; ++loop_cnt)
			{
				for (int pos = 0; pos < dest_len; ++pos)
				{
					const int      read_pos_int  = int (read_pos >> 32);
					const uint32_t read_pos_frac = uint32_t (read_pos);

					interp.interpolate_multi_chn (
						&buf_out [pos],
						&buf_in [read_pos_int],
						read_pos_frac,
						nbr_chn,
						CHN_STRIDE / 3,
						CHN_STRIDE
					);

					read_pos += read_step;
				}
				read_pos &= 0xFFFFFFFFUL;	// Zeroes integer position

				// Prevents the optimizer to remove all the loops
				acc_dummy += buf_out [dest_len - 1];
			}

			tim.stop_lap ();
		}

		double	      spl_per_s = tim.get_best_rate (dest_len * nbr_loops);
		spl_per_s += fstb::limit (acc_dummy, -1e-300, 1e-300); // Anti-optimizer trick
		const double   mega_sps  = spl_per_s / 1000000.0;
		printf (
			"interpolate_multi_chn(), FIR length %d, %d sample-blocks, %d channels: %12.3f Mspl/s.\n",
			PHASE_LEN,
			dest_len,
			nbr_chn,
			mega_sps
		);
	}

	return ret_val;
}



#endif   // TestInterpPhase_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
