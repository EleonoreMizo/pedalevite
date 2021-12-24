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
int	TestPhaseHalfPi <TO>::perform_test (TO &phaser, const double coef_arr [_nbr_coefs], const SweepingSine &ss, const char *type_0, double transition_bw)
{
	assert (coef_arr != nullptr);
	assert (type_0   != nullptr);
	assert (transition_bw > 0);
	assert (transition_bw < 0.5);

	const char *   datatype_0 =
		  std::is_same <DataType, double>::value ? "double"
		: std::is_same <DataType, float >::value ? "float"
		:                                          "<unknown type>";
	printf (
		"Test: PhaseHalfPi, %s, %d chn, %s implementation, %d coefficients.\n",
		datatype_0, _nbr_chn, type_0, _nbr_coefs
	);

	const auto     len = ss.get_len ();
	const auto     src = ResultCheck <DataType>::generate_test_signal (
		ss, len, _nbr_chn
	);

	phaser.set_coefs (coef_arr);
	phaser.clear_buffers ();

	std::vector <DataType>	dest_0 (len * _nbr_chn, 0);
	std::vector <DataType>	dest_1 (len * _nbr_chn, 0);

	printf ("Phasing... ");
	fflush (stdout);
	BlockSplitter	bs (64);
	for (bs.start (len); bs.is_continuing (); bs.set_next_block ())
	{
		const long     b_pos = bs.get_pos ();
		const long     b_len = bs.get_len ();
		const int      idx   = b_pos * _nbr_chn;
		phaser.process_block (
			&dest_0 [idx], &dest_1 [idx], &src [idx], b_len
		);
	}
	printf ("Done.\n");

	int            ret_val = 0;
	std::vector <DataType>  dst_chk_0 (len);
	std::vector <DataType>  dst_chk_1 (len);
	for (int chn = 0; chn < _nbr_chn && ret_val == 0; ++chn)
	{
		for (long pos = 0; pos < len; ++pos)
		{
			const int      idx = pos * _nbr_chn + chn;
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
			_nbr_coefs, type_0, _nbr_chn, chn
		);
		FileOp <DataType>::save_raw_data_stereo (
			filename_0, dst_chk_0.data (), dst_chk_1.data (),
			len, hiir_test_file_resol, 1.f
		);
	}

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Delay

	if (ret_val == 0)
	{
		printf ("Checking delay... ");
		fflush (stdout);
		ret_val = TestDelay <TO>::test_phaser (phaser, coef_arr);
		printf ("Done.\n");
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
