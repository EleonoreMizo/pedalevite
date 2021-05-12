/*****************************************************************************

        FileOp.hpp
        Author: Laurent de Soras, 2005

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (hiir_test_FileOp_CODEHEADER_INCLUDED)
#define hiir_test_FileOp_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "hiir/test/conf.h"

#include <algorithm>
#include <array>
#include <vector>

#include <cassert>
#include <cstdint>
#include <cstdio>



namespace hiir
{
namespace test
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



#if defined (hiir_test_SAVE_RESULTS)

template <typename T>
inline int	FileOp <T>::save_raw_data_16 (const char *filename_0, const T src_ptr [], long nbr_spl, float scale)
{
	assert (filename_0 != nullptr);
	assert (filename_0 [0] != '\0');
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);
	assert (scale != 0);

	int            ret_val = 0;

	printf ("Saving %s... ", filename_0);
	fflush (stdout);

	const float    mult  = 16384.0f / scale;

	FILE *         f_ptr = fopen (filename_0, "wb");
	if (f_ptr == 0)
	{
		ret_val = -1;
	}
	else
	{
		enum { BLOCK_LEN = 1024 };

		for (long block_pos = 0
		;	block_pos < nbr_spl && ret_val == 0
		;	block_pos += BLOCK_LEN)
		{
			std::array <int16_t, BLOCK_LEN>  buf;
			const long     block_size = std::min (
				nbr_spl - block_pos,
				long (BLOCK_LEN)
			);
			for (long pos = 0; pos < block_size; ++pos)
			{
				float          x = src_ptr [block_pos + pos] * mult;
				x = std::max (std::min (x, 32767.0f), -32768.0f);
				buf [pos] = int16_t (round_int (x));
			}

			if (fwrite (&buf [0], sizeof (buf [0]) * block_size, 1, f_ptr) != 1)
			{
				ret_val = -2;
			}
		}
	}

	fclose (f_ptr);

	printf ("Done.\n");

	return ret_val;
}



template <typename T>
inline int	FileOp <T>::save_raw_data_16_stereo (const char *filename_0, const T src_0_ptr [], const T src_1_ptr [], long nbr_spl, float scale)
{
	assert (filename_0 != nullptr);
	assert (filename_0 [0] != '\0');
	assert (src_0_ptr != nullptr);
	assert (src_1_ptr != nullptr);
	assert (nbr_spl > 0);
	assert (scale != 0);

	int            ret_val = 0;

	std::vector <float>  stereo (nbr_spl * 2);
	for (long pos = 0; pos < nbr_spl; ++pos)
	{
		stereo [pos * 2    ] = src_0_ptr [pos];
		stereo [pos * 2 + 1] = src_1_ptr [pos];
	}

	ret_val = save_raw_data_16 (filename_0, &stereo [0], nbr_spl * 2, scale);

	return ret_val;
}



#else



template <typename T>
inline int	FileOp <T>::save_raw_data_16 (const char * /*filename_0*/, const T /*src_ptr*/ [], long /*nbr_spl*/, float /*scale*/)
{
	return 0;
}

template <typename T>
inline int	FileOp <T>::save_raw_data_16_stereo (const char * /*filename_0*/, const T /*src_0_ptr*/ [], const T /*src_1_ptr*/ [], long /*nbr_spl*/, float /*scale*/)
{
	return 0;
}



#endif   // hiir_test_SAVE_RESULTS



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace test
}  // namespace hiir



#endif   // hiir_test_FileOp_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
