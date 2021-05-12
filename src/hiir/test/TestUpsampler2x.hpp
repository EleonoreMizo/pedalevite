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
int	TestUpsampler2x <TO>::perform_test (TO &uspl, const double coef_arr [NBR_COEFS], const SweepingSine &ss, const char *type_0, double transition_bw, double stopband_at)
{
	assert (coef_arr != nullptr);
	assert (type_0   != nullptr);
	assert (transition_bw > 0);
	assert (transition_bw < 0.5);
	assert (stopband_at > 0);

	typedef typename TO::DataType DataType;
	const int      nbr_chn = TO::_nbr_chn;

	const char *   datatype_0 =
		  std::is_same <DataType, double>::value ? "double"
		: std::is_same <DataType, float >::value ? "float"
		:                                          "<unknown type>";
	printf (
		"Test: Upsampler2x, %s, %d chn, %s implementation, %d coefficients.\n",
		datatype_0, nbr_chn, type_0, NBR_COEFS
	);

	const long     len = ss.get_len ();
	std::vector <DataType>  src (len * nbr_chn);
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

	uspl.set_coefs (coef_arr);
	uspl.clear_buffers ();

	const long     len_save = len * 2;
	std::vector <DataType>  dest (len_save * nbr_chn, 0);

	printf ("Upsampling... ");
	fflush (stdout);
	BlockSplitter	bs (64);
	for (bs.start (len); bs.is_continuing (); bs.set_next_block ())
	{
		const long     b_pos = bs.get_pos ();
		const long     b_len = bs.get_len ();
		uspl.process_block (
			&dest [b_pos * 2 * nbr_chn], &src [b_pos * nbr_chn], b_len
		);
	}
	printf ("Done.\n");

	int            ret_val = 0;
	std::vector <DataType>  dst_chk (len_save);
	for (int chn = 0; chn < nbr_chn && ret_val == 0; ++chn)
	{
		for (long pos = 0; pos < len_save; ++pos)
		{
			dst_chk [pos] = dest [pos * nbr_chn + chn];
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
			TestedType::NBR_COEFS, type_0, nbr_chn, chn
		);
		FileOp <DataType>::save_raw_data_16 (
			filename_0, &dst_chk [0], len_save, 1
		);
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
