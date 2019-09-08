/*****************************************************************************

        TestApprox.cpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/AllocAlign.h"
#include "fstb/Approx.h"
#include "fstb/ToolsSimd.h"
#include "test/BufferFiller.h"
#include "test/TestApprox.h"
#include "test/TimerAccurate.h"

#include <vector>

#include <cassert>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/


int	TestApprox::perform_test ()
{
	int            ret_val = 0;

	constexpr float   pi_f = float (fstb::PI);
	typedef fstb::ToolsSimd::VectF32 VectF32;

	TestFnc <float, 0>::test_op1 (
		[] (float x) { return fstb::Approx::sin_nick (x); },
		"sin_nick (float)", -pi_f, pi_f
	);
	TestFnc <float, 1>::test_op1 (
		[] (float x) { return fstb::Approx::sin_nick (x); },
		"sin_nick (float)", -pi_f, pi_f
	);
	TestFnc <float, 2>::test_op1 (
		[] (float x) { return fstb::Approx::sin_nick (x); },
		"sin_nick (float)", -pi_f, pi_f
	);

	TestFnc <VectF32, 0>::test_op1 (
		[] (VectF32 x) { return fstb::Approx::sin_nick (x); },
		"sin_nick (VectF32)", -pi_f, pi_f
	);
	TestFnc <VectF32, 1>::test_op1 (
		[] (VectF32 x) { return fstb::Approx::sin_nick (x); },
		"sin_nick (VectF32)", -pi_f, pi_f
	);
	TestFnc <VectF32, 2>::test_op1 (
		[] (VectF32 x) { return fstb::Approx::sin_nick (x); },
		"sin_nick (VectF32)", -pi_f, pi_f
	);

	TestFnc <float, 0>::test_op1 (
		[] (float x) { return fstb::Approx::sin_rbj_halfpi (x); },
		"sin_rbj_halfpi (float)", -1.f, 1.f
	);
	TestFnc <float, 1>::test_op1 (
		[] (float x) { return fstb::Approx::sin_rbj_halfpi (x); },
		"sin_rbj_halfpi (float)", -1.f, 1.f
	);
	TestFnc <float, 2>::test_op1 (
		[] (float x) { return fstb::Approx::sin_rbj_halfpi (x); },
		"sin_rbj_halfpi (float)", -1.f, 1.f
	);

	TestFnc <VectF32, 0>::test_op1 (
		[] (VectF32 x) { return fstb::Approx::sin_rbj_halfpi (x); },
		"sin_rbj_halfpi (VectF32)", -1.f, 1.f
	);
	TestFnc <VectF32, 1>::test_op1 (
		[] (VectF32 x) { return fstb::Approx::sin_rbj_halfpi (x); },
		"sin_rbj_halfpi (VectF32)", -1.f, 1.f
	);
	TestFnc <VectF32, 2>::test_op1 (
		[] (VectF32 x) { return fstb::Approx::sin_rbj_halfpi (x); },
		"sin_rbj_halfpi (VectF32)", -1.f, 1.f
	);

	TestFnc <float, 0>::test_op1 (
		[] (float x) { return fstb::Approx::log2 (x); },
		"log2 (float)", 1e-3f, 1e3f
	);
	TestFnc <float, 1>::test_op1 (
		[] (float x) { return fstb::Approx::log2 (x); },
		"log2 (float)", 1e-3f, 1e3f
	);
	TestFnc <float, 2>::test_op1 (
		[] (float x) { return fstb::Approx::log2 (x); },
		"log2 (float)", 1e-3f, 1e3f
	);

	TestFnc <VectF32, 0>::test_op1 (
		[] (VectF32 x) { return fstb::ToolsSimd::log2_approx (x); },
		"log2_approx (VectF32)", 1e-3f, 1e3f
	);
	TestFnc <VectF32, 1>::test_op1 (
		[] (VectF32 x) { return fstb::ToolsSimd::log2_approx (x); },
		"log2_approx (VectF32)", 1e-3f, 1e3f
	);
	TestFnc <VectF32, 2>::test_op1 (
		[] (VectF32 x) { return fstb::ToolsSimd::log2_approx (x); },
		"log2_approx (VectF32)", 1e-3f, 1e3f
	);

	TestFnc <float, 0>::test_op1 (
		[] (float x) { return fstb::Approx::exp2 (x); },
		"exp2 (float)", -20.f, 20.f
	);
	TestFnc <float, 1>::test_op1 (
		[] (float x) { return fstb::Approx::exp2 (x); },
		"exp2 (float)", -20.f, 20.f
	);
	TestFnc <float, 2>::test_op1 (
		[] (float x) { return fstb::Approx::exp2 (x); },
		"exp2 (float)", -20.f, 20.f
	);

	TestFnc <VectF32, 0>::test_op1 (
		[] (VectF32 x) { return fstb::ToolsSimd::exp2_approx (x); },
		"exp2_approx (VectF32)", -20.f, 20.f
	);
	TestFnc <VectF32, 1>::test_op1 (
		[] (VectF32 x) { return fstb::ToolsSimd::exp2_approx (x); },
		"exp2_approx (VectF32)", -20.f, 20.f
	);
	TestFnc <VectF32, 2>::test_op1 (
		[] (VectF32 x) { return fstb::ToolsSimd::exp2_approx (x); },
		"exp2_approx (VectF32)", -20.f, 20.f
	);

	TestFnc <float, 0>::test_op1 (
		[] (float x) { return fstb::Approx::tan_taylor5 (x); },
		"tan_taylor5 (float)", -1.f, 1.f
	);
	TestFnc <float, 1>::test_op1 (
		[] (float x) { return fstb::Approx::tan_taylor5 (x); },
		"tan_taylor5 (float)", -1.f, 1.f
	);
	TestFnc <float, 2>::test_op1 (
		[] (float x) { return fstb::Approx::tan_taylor5 (x); },
		"tan_taylor5 (float)", -1.f, 1.f
	);

	TestFnc <VectF32, 0>::test_op1 (
		[] (VectF32 x) { return fstb::Approx::tan_taylor5 (x); },
		"tan_taylor5 (VectF32)", -1.f, 1.f
	);
	TestFnc <VectF32, 1>::test_op1 (
		[] (VectF32 x) { return fstb::Approx::tan_taylor5 (x); },
		"tan_taylor5 (VectF32)", -1.f, 1.f
	);
	TestFnc <VectF32, 2>::test_op1 (
		[] (VectF32 x) { return fstb::Approx::tan_taylor5 (x); },
		"tan_taylor5 (VectF32)", -1.f, 1.f
	);

	TestFnc <float, 0>::test_op1 (
		[] (float x) { return fstb::Approx::tan_mystran (x); },
		"tan_mystran (float)", -1.f, 1.f
	);
	TestFnc <float, 1>::test_op1 (
		[] (float x) { return fstb::Approx::tan_mystran (x); },
		"tan_mystran (float)", -1.f, 1.f
	);
	TestFnc <float, 2>::test_op1 (
		[] (float x) { return fstb::Approx::tan_mystran (x); },
		"tan_mystran (float)", -1.f, 1.f
	);

	TestFnc <VectF32, 0>::test_op1 (
		[] (VectF32 x) { return fstb::Approx::tan_mystran (x); },
		"tan_mystran (VectF32)", -1.f, 1.f
	);
	TestFnc <VectF32, 1>::test_op1 (
		[] (VectF32 x) { return fstb::Approx::tan_mystran (x); },
		"tan_mystran (VectF32)", -1.f, 1.f
	);
	TestFnc <VectF32, 2>::test_op1 (
		[] (VectF32 x) { return fstb::Approx::tan_mystran (x); },
		"tan_mystran (VectF32)", -1.f, 1.f
	);

	TestFnc <float, 0>::test_op1 (
		[] (float x) { return fstb::Approx::tanh_mystran (x); },
		"tanh_mystran (float)", -1.f, 1.f
	);
	TestFnc <float, 1>::test_op1 (
		[] (float x) { return fstb::Approx::tanh_mystran (x); },
		"tanh_mystran (float)", -1.f, 1.f
	);
	TestFnc <float, 2>::test_op1 (
		[] (float x) { return fstb::Approx::tanh_mystran (x); },
		"tanh_mystran (float)", -1.f, 1.f
	);

	TestFnc <VectF32, 0>::test_op1 (
		[] (VectF32 x) { return fstb::Approx::tanh_mystran (x); },
		"tanh_mystran (VectF32)", -1.f, 1.f
	);
	TestFnc <VectF32, 1>::test_op1 (
		[] (VectF32 x) { return fstb::Approx::tanh_mystran (x); },
		"tanh_mystran (VectF32)", -1.f, 1.f
	);
	TestFnc <VectF32, 2>::test_op1 (
		[] (VectF32 x) { return fstb::Approx::tanh_mystran (x); },
		"tanh_mystran (VectF32)", -1.f, 1.f
	);

	TestFnc <float, 0>::test_op1 (
		[] (float x) { return fstb::Approx::tanh_2dat (x); },
		"tanh_2dat (float)", -10.f, 10.f
	);
	TestFnc <float, 1>::test_op1 (
		[] (float x) { return fstb::Approx::tanh_2dat (x); },
		"tanh_2dat (float)", -10.f, 10.f
	);
	TestFnc <float, 2>::test_op1 (
		[] (float x) { return fstb::Approx::tanh_2dat (x); },
		"tanh_2dat (float)", -10.f, 10.f
	);

	TestFnc <VectF32, 0>::test_op1 (
		[] (VectF32 x) { return fstb::Approx::tanh_2dat (x); },
		"tanh_2dat (VectF32)", -10.f, 10.f
	);
	TestFnc <VectF32, 1>::test_op1 (
		[] (VectF32 x) { return fstb::Approx::tanh_2dat (x); },
		"tanh_2dat (VectF32)", -10.f, 10.f
	);
	TestFnc <VectF32, 2>::test_op1 (
		[] (VectF32 x) { return fstb::Approx::tanh_2dat (x); },
		"tanh_2dat (VectF32)", -10.f, 10.f
	);

	return ret_val;
}


/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/




template <typename T, typename S>
static inline S conv_s_to_t (T x) { return static_cast <S> (x); };



template <typename T, typename S>
static inline T conv_t_to_s (S x) { return static_cast <T> (x); };

template <>
inline float conv_t_to_s (fstb::ToolsSimd::VectF32 x)
{
	return fstb::ToolsSimd::Shift <0>::extract (x);
};


template <typename T, int ILL2>
template <typename OP, typename S>
void	TestApprox::TestFnc <T, ILL2>::test_op1 (const OP &op, const char name_0 [], S min_val, S max_val)
{
	constexpr int  nbr_blocks = 10000;
	constexpr int  block_len_s = 64;
	constexpr int  s_per_t = sizeof (T) / sizeof (S);
	static_assert ((block_len_s % s_per_t) == 0, "");
	constexpr int  block_len  = block_len_s / s_per_t;
	constexpr int  interleave = 1 << ILL2;
	static_assert ((block_len % interleave) == 0, "");

	typedef std::vector <T, fstb::AllocAlign <T, 64> > Buffer;

	Buffer          src_arr (block_len);
	Buffer          dst_arr (block_len);

	BufferFiller::gen_rnd_scaled (
		reinterpret_cast <S *> (&src_arr [0]), block_len_s, min_val, max_val
	);

	printf ("Speed test %s, interleave %d... ", name_0, interleave);
	fflush (stdout);
	TimerAccurate  tim;
	S              dummy_val = S (0);

	tim.start ();
	for (int blk_cnt = 0; blk_cnt < nbr_blocks; ++blk_cnt)
	{
		for (int pos = 0; pos < block_len; pos += interleave)
		{
			T              a [interleave];

			if (true    ) { a [0] = src_arr [pos    ]; }
			if (ILL2 > 0) { a [1] = src_arr [pos + 1]; }
			if (ILL2 > 1) { a [2] = src_arr [pos + 2]; }
			if (ILL2 > 1) { a [3] = src_arr [pos + 3]; }
			if (ILL2 > 2) { a [4] = src_arr [pos + 4]; }
			if (ILL2 > 2) { a [5] = src_arr [pos + 5]; }
			if (ILL2 > 2) { a [6] = src_arr [pos + 6]; }
			if (ILL2 > 2) { a [7] = src_arr [pos + 7]; }
			
			if (true    ) { dst_arr [pos    ] = op (a [0]); }
			if (ILL2 > 0) { dst_arr [pos + 1] = op (a [1]); }
			if (ILL2 > 1) { dst_arr [pos + 2] = op (a [2]); }
			if (ILL2 > 1) { dst_arr [pos + 3] = op (a [3]); }
			if (ILL2 > 2) { dst_arr [pos + 4] = op (a [4]); }
			if (ILL2 > 2) { dst_arr [pos + 5] = op (a [5]); }
			if (ILL2 > 2) { dst_arr [pos + 6] = op (a [6]); }
			if (ILL2 > 2) { dst_arr [pos + 7] = op (a [7]); }
		}
		dummy_val += conv_t_to_s <S> (src_arr [block_len - 1]);
	}
	tim.stop ();

	double	      spl_per_s = tim.get_best_rate (block_len_s * nbr_blocks);
	spl_per_s += fstb::limit (double (dummy_val), -1e-300, 1e-300);
	const double   mega_sps  = spl_per_s / 1000000.0;
	printf ("%12.3f Mspl/s.\n", mega_sps);
}



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
