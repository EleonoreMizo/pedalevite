/*****************************************************************************

        TestSlidingMax.cpp
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

#include "mfx/dsp/fir/SlidingMax.h"
#include "test/TestSlidingMax.h"

#include <algorithm>
#include <array>
#include <functional>
#include <random>

#include <cassert>
#include <cstdint>
#include <cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestSlidingMax::perform_test ()
{
	int            ret_val = 0;

	if (ret_val == 0)
	{
		ret_val = perform_test_single <
			mfx::dsp::fir::SlidingMax <int32_t>
		> ();
	}

	if (ret_val == 0)
	{
		ret_val = perform_test_single <
			mfx::dsp::fir::SlidingMax <float>
		> ();
	}

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class SO>
int	TestSlidingMax::perform_test_single ()
{
	int            ret_val = 0;

	typedef typename SO::DataType DataType;
	SO             so_proc;

	static const std::array <int, 8> win_len_arr {{
		1, 2, 3, 4, 27, 16, 53, 2345
	}};
	for (int win_len : win_len_arr)
	{
		const int      data_len = 10000;

		std::vector <DataType>  src_arr (data_len);

		std::default_random_engine rnd_gen;
		std::uniform_int_distribution <int32_t> dist (-(1 << 30), 1 << 30);
		auto           dice = std::bind (dist, rnd_gen);

		so_proc.set_length (win_len);

		for (int pos = 0; pos < data_len && ret_val == 0; ++pos)
		{
			DataType       x (static_cast <DataType> (dice ()));
			src_arr [pos] = x;

			const int      pos_beg  = std::max (pos - win_len + 1, 0);
			const int      work_len = pos - pos_beg + 1;
			const DataType ref = compute_naive_max (&src_arr [pos_beg], work_len);
			const DataType tst = so_proc.process_sample (x);
			if (tst != ref)
			{
				assert (false);
				printf ("TestSlidingMax: error.\n");
				ret_val = -1;
			}
		}

		if (ret_val != 0)
		{
			break;
		}
	}

	return ret_val;
}



template <typename T>
T	TestSlidingMax::compute_naive_max (const T src_ptr [], int len)
{
	assert (len > 0);

	T              x = src_ptr [0];
	for (int pos = 1; pos < len; ++pos)
	{
		x = std::max (x, src_ptr [pos]);
	}

	return x;
}



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
