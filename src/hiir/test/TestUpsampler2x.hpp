/*****************************************************************************

        TestUpsampler2x.hpp
        Author: Laurent de Soras, 2005

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (hiir_TestUpsampler2x_CURRENT_CODEHEADER)
	#error Recursive inclusion of TestUpsampler2x code header.
#endif
#define hiir_TestUpsampler2x_CURRENT_CODEHEADER

#if ! defined (hiir_TestUpsampler2x_CODEHEADER_INCLUDED)
#define hiir_TestUpsampler2x_CODEHEADER_INCLUDED



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
constexpr int	TestUpsampler2x <TO>::_nbr_chn;
template <class TO>
constexpr int	TestUpsampler2x <TO>::_nbr_coefs;



template <class TO>
int	TestUpsampler2x <TO>::perform_test (TO &uspl, const double coef_arr [_nbr_coefs], const SweepingSine &ss, const char *type_0, double transition_bw, double stopband_at)
{
	assert (coef_arr != nullptr);
	assert (type_0   != nullptr);
	assert (transition_bw > 0);
	assert (transition_bw < 0.5);
	assert (stopband_at > 0);

	const char *   datatype_0 =
		  std::is_same <DataType, double>::value ? "double"
		: std::is_same <DataType, float >::value ? "float"
		:                                          "<unknown type>";
	printf (
		"Test: Upsampler2x, %s, %d chn, %s implementation, %d coefficients.\n",
		datatype_0, _nbr_chn, type_0, _nbr_coefs
	);

	const auto     len = ss.get_len ();
	const auto     src = ResultCheck <DataType>::generate_test_signal (
		ss, len, _nbr_chn
	);

	uspl.set_coefs (coef_arr);
	uspl.clear_buffers ();

	const long     len_save = len * 2;
	std::vector <DataType>  dest (len_save * _nbr_chn, 0);

	printf ("Upsampling... ");
	fflush (stdout);
	BlockSplitter	bs (64);
	for (bs.start (len); bs.is_continuing (); bs.set_next_block ())
	{
		const long     b_pos = bs.get_pos ();
		const long     b_len = bs.get_len ();
		uspl.process_block (
			&dest [b_pos * 2 * _nbr_chn], &src [b_pos * _nbr_chn], b_len
		);
	}
	printf ("Done.\n");

	int            ret_val = 0;
	std::vector <DataType>  dst_chk (len_save);
	for (int chn = 0; chn < _nbr_chn && ret_val == 0; ++chn)
	{
		for (long pos = 0; pos < len_save; ++pos)
		{
			dst_chk [pos] = dest [pos * _nbr_chn + chn];
		}
		ret_val = ResultCheck <DataType>::check_uspl (
			ss,
			transition_bw,
			stopband_at,
			&dst_chk [0]
		);

		char         filename_0 [255+1];
		sprintf (
			filename_0, "uspl_%02d_%s_%dx-%01d.raw",
			_nbr_coefs, type_0, _nbr_chn, chn
		);
		FileOp <DataType>::save_raw_data (
			filename_0, dst_chk.data (), len_save, hiir_test_file_resol, 1.f
		);
	}

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Delay

	if (ret_val == 0)
	{
		printf ("Checking delay... ");
		fflush (stdout);
		ret_val = TestDelay <TO>::test_upsampler (uspl, coef_arr);
		printf ("Done.\n");
	}

	printf ("\n");

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace test
}  // namespace hiir



#endif   // hiir_TestUpsampler2x_CODEHEADER_INCLUDED

#undef hiir_TestUpsampler2x_CURRENT_CODEHEADER



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
