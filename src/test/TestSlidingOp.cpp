/*****************************************************************************

        TestSlidingOp.cpp
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

#include "mfx/dsp/fir/SlidingOp.h"
#include "test/TestSlidingOp.h"

#include <algorithm>
#include <functional>
#include <random>

#include <cassert>
#include <cstdio>



template <typename T>
struct MaxFtor
{
	inline T operator () (const T &a, const T &b) { return std::max (a, b); }
};



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestSlidingOp::perform_test ()
{
	int            ret_val = 0;

	if (ret_val == 0)
	{
		ret_val = perform_test_single <
			mfx::dsp::fir::SlidingOp <int32_t, MaxFtor <int32_t> >
		> ();
	}

	if (ret_val == 0)
	{
		ret_val = perform_test_single <
			mfx::dsp::fir::SlidingOp <int32_t, std::plus <int32_t> >
		> ();
	}

	if (ret_val == 0)
	{
		ret_val = perform_test_single <
			mfx::dsp::fir::SlidingOp <float, MaxFtor <float> >
		> ();
	}

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class SO>
int	TestSlidingOp::perform_test_single ()
{
	int            ret_val = 0;

	typedef typename SO::DataType DataType;
	typedef typename SO::Operator Operator;
	SO             so_proc;
	Operator       op;

	const int      win_len  =    27;
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
		const DataType ref = compute_naive_combination (
			op, &src_arr [pos_beg], work_len
		);
		const DataType tst = so_proc.process_sample (x);
		if (tst != ref)
		{
			assert (false);
			printf ("TestSlidingOp: error.\n");
			ret_val = -1;
		}
	}

	return ret_val;
}



template <typename T, typename OP>
T	TestSlidingOp::compute_naive_combination (OP &op, const T src_ptr [], int len)
{
	assert (len > 0);

	T              x = src_ptr [0];
	for (int pos = 1; pos < len; ++pos)
	{
		x = op (x, src_ptr [pos]);
	}

	return x;
}



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
