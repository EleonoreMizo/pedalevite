/*****************************************************************************

        TestPhaseHalfPi.hpp
        Author: Laurent de Soras, 2005

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (hiir_test_TestPhaseHalfPi_CURRENT_CODEHEADER)
	#error Recursive inclusion of TestPhaseHalfPi code header.
#endif
#define hiir_test_TestPhaseHalfPi_CURRENT_CODEHEADER

#if ! defined (hiir_test_TestPhaseHalfPi_CODEHEADER_INCLUDED)
#define hiir_test_TestPhaseHalfPi_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "hiir/test/BlockSplitter.h"
#include "hiir/test/FileOp.h"
#include "hiir/test/ResultCheck.h"
#include "hiir/test/SweepingSine.h"

#include <type_traits>

#include <cassert>
#include <cstdio>



namespace hiir
{
namespace test
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class TO>
int	TestPhaseHalfPi <TO>::perform_test (TO &phaser, const double coef_arr [NBR_COEFS], const SweepingSine &ss, const char *type_0, double transition_bw)
{
	assert (coef_arr != nullptr);
	assert (type_0   != nullptr);
	assert (transition_bw > 0);
	assert (transition_bw < 0.5);

	typedef typename TO::DataType DataType;
	const int      nbr_chn = TO::_nbr_chn;

	const char *   datatype_0 =
		  std::is_same <DataType, double>::value ? "double"
		: std::is_same <DataType, float >::value ? "float"
		:                                          "<unknown type>";
	printf (
		"Test: PhaseHalfPi, %s, %d chn, %s implementation, %d coefficients.\n",
		datatype_0, nbr_chn, type_0, NBR_COEFS
	);

	const long     len = ss.get_len ();
	std::vector <DataType>	src (len * nbr_chn);
	printf ("Generating sweeping sine... ");
	fflush (stdout);
	if (nbr_chn == 1)
	{
		ss.generate (&src [0]);
	}
	else
	{
		std::vector <DataType>	src_base (len);
		ss.generate (&src_base [0]);
		for (long pos = 0; pos <len; ++pos)
		{
			for (int chn = 0; chn < nbr_chn; ++chn)
			{
				src [pos * nbr_chn + chn] = src_base [pos];
			}
		}
	}
	printf ("Done.\n");

	phaser.set_coefs (coef_arr);
	phaser.clear_buffers ();

	std::vector <DataType>	dest_0 (len * nbr_chn, 0);
	std::vector <DataType>	dest_1 (len * nbr_chn, 0);

	printf ("Phasing... ");
	fflush (stdout);
	BlockSplitter	bs (64);
	for (bs.start (len); bs.is_continuing (); bs.set_next_block ())
	{
		const long     b_pos = bs.get_pos ();
		const long     b_len = bs.get_len ();
		const int      idx   = b_pos * nbr_chn;
		phaser.process_block (
			&dest_0 [idx], &dest_1 [idx], &src [idx], b_len
		);
	}
	printf ("Done.\n");

	int            ret_val = 0;
	std::vector <DataType>  dst_chk_0 (len);
	std::vector <DataType>  dst_chk_1 (len);
	for (int chn = 0; chn < nbr_chn && ret_val == 0; ++chn)
	{
		for (long pos = 0; pos < len; ++pos)
		{
			const int      idx = pos * nbr_chn + chn;
			dst_chk_0 [pos] = dest_0 [idx];
			dst_chk_1 [pos] = dest_1 [idx];
		}

		ret_val = ResultCheck <DataType>::check_phase (
			ss,
			transition_bw,
			&dst_chk_0 [0],
			&dst_chk_1 [0]
		);

		char           filename_0 [255+1];
		sprintf (
			filename_0, "phaser_%02d_%s_%dx-%01d.raw",
			TestedType::NBR_COEFS, type_0, nbr_chn, chn
		);
		FileOp <DataType>::save_raw_data_16_stereo (
			filename_0, &dst_chk_0 [0], &dst_chk_1 [0], len, 1
		);
	}

	printf ("\n");

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace test
}  // namespace hiir



#endif   // hiir_test_TestPhaseHalfPi_CODEHEADER_INCLUDED

#undef hiir_test_TestPhaseHalfPi_CURRENT_CODEHEADER



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
