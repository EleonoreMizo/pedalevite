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

#include <algorithm>
#include <vector>

#include <cassert>
#include <cmath>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/


int	TestApprox::perform_test ()
{
	int            ret_val = 0;

	const float    pi_f = float (fstb::PI);
	typedef fstb::ToolsSimd::VectF32 VectF32;

	// sin

	test_op1_all_flt <false> (
		[] (double x) { return sin (x); },
		[] (float x) { return fstb::Approx::sin_nick (x); },
		[] (VectF32 x) { return fstb::Approx::sin_nick (x); },
		"sin_nick", -pi_f, pi_f
	);

	test_op1_all_flt <false> (
		[] (double x) { return sin (x * fstb::PI * 0.5f); },
		[] (float x) { return fstb::Approx::sin_rbj_halfpi (x); },
		[] (VectF32 x) { return fstb::Approx::sin_rbj_halfpi (x); },
		"sin_rbj_halfpi", -1.f, 1.f
	);

	// log2

	test_op1_all_flt_s <false> (
		[] (double x) { return log2 (x); },
		[] (float x) { return fstb::Approx::log2 (x); },
		"log2", 1e-3f, 1e3f
	);
	test_op1_all_flt_v <false> (
		[] (double x) { return log2 (x); },
		[] (VectF32 x) { return fstb::ToolsSimd::log2_approx (x); },
		"log2_approx", 1e-3f, 1e3f
	);
	test_op1_all_flt_v <false> (
		[] (double x) { return log2 (x); },
		[] (VectF32 x) { return fstb::ToolsSimd::log2_approx2 (x); },
		"log2_approx2", 1e-3f, 1e3f
	);

	// exp2

	test_op1_all_flt_s <true> (
		[] (double x) { return exp2 (x); },
		[] (float x) { return fstb::Approx::exp2 (x); },
		"exp2", -20.f, 20.f
	);
	test_op1_all_flt_v <true> (
		[] (double x) { return exp2 (x); },
		[] (VectF32 x) { return fstb::ToolsSimd::exp2_approx (x); },
		"exp2_approx", -20.f, 20.f
	);
	test_op1_all_flt_v <true> (
		[] (double x) { return exp2 (x); },
		[] (VectF32 x) { return fstb::ToolsSimd::exp2_approx2 (x); },
		"exp2_approx2", -20.f, 20.f
	);

	// tan

	test_op1_all_flt <false> (
		[] (double x) { return tan (x); },
		[] (float x) { return fstb::Approx::tan_taylor5 (x); },
		[] (VectF32 x) { return fstb::Approx::tan_taylor5 (x); },
		"tan_taylor5", -1.f, 1.f
	);

	test_op1_all_flt <false> (
		[] (double x) { return tan (x); },
		[] (float x) { return fstb::Approx::tan_mystran (x); },
		[] (VectF32 x) { return fstb::Approx::tan_mystran (x); },
		"tan_mystran", -1.f, 1.f
	);

	// tanh

	test_op1_all_flt <false> (
		[] (double x) { return tanh (x); },
		[] (float x) { return fstb::Approx::tanh_mystran (x); },
		[] (VectF32 x) { return fstb::Approx::tanh_mystran (x); },
		"tanh_mystran", -5.f, 5.f
	);

	test_op1_all_flt <false> (
		[] (double x) { return tanh (x); },
		[] (float x) { return fstb::Approx::tanh_2dat (x); },
		[] (VectF32 x) { return fstb::Approx::tanh_2dat (x); },
		"tanh_2dat", -10.f, 10.f
	);

	test_op1_all_flt <false> (
		[] (double x) { return tanh (x); },
		[] (float x) { return fstb::Approx::tanh_andy (x); },
		[] (VectF32 x) { return fstb::Approx::tanh_andy (x); },
		"tanh_andy", -5.f, 5.f
	);

	test_op1_all_flt <false> (
		[] (double x) { return tanh (x); },
		[] (float x) { return fstb::Approx::tanh_urs (x); },
		[] (VectF32 x) { return fstb::Approx::tanh_urs (x); },
		"tanh_urs", -10.f, 10.f
	);

	// pow

	TestFncLogic <double, true>::test_op2 (
		[] (double x, double y) { return pow (x, y); },
		[] (double x, double y) { return fstb::Approx::pow (x, y); },
		"pow (double)", 0.1f, 10.0f, -10.f, 10.f
	);

	return ret_val;
}


/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/




template <typename T, typename S>
static inline T conv_s_to_t (S x) { return static_cast <T> (x); }

template <>
inline fstb::ToolsSimd::VectF32 conv_s_to_t (float x)
{
	return fstb::ToolsSimd::set1_f32 (x);
}


template <typename S, typename T>
static inline S conv_t_to_s (T x) { return static_cast <S> (x); }

template <>
inline float conv_t_to_s (fstb::ToolsSimd::VectF32 x)
{
	return fstb::ToolsSimd::Shift <0>::extract (x);
}


template <typename T, int ILL2>
template <typename OP, typename S>
void	TestApprox::TestFncSpeed <T, ILL2>::test_op1 (const OP &op, const std::string &name, S min_val, S max_val)
{
	const int      nbr_blocks = 10000;
	const int      block_len_s = 64;
	const int      s_per_t = sizeof (T) / sizeof (S);
	static_assert ((block_len_s % s_per_t) == 0, "");
	const int      block_len  = block_len_s / s_per_t;
	const int      interleave = 1 << ILL2;
	static_assert ((block_len % interleave) == 0, "");

	typedef std::vector <T, fstb::AllocAlign <T, 64> > Buffer;

	Buffer          src_arr (block_len);
	Buffer          dst_arr (block_len);

	BufferFiller::gen_rnd_scaled (
		reinterpret_cast <S *> (&src_arr [0]), block_len_s, min_val, max_val
	);

	printf ("Speed test %s, interleave %d... ", name.c_str (), interleave);
	fflush (stdout);
	TimerAccurate  tim;
	S              dummy_val = S (0);

	tim.start ();
	for (int blk_cnt = 0; blk_cnt < nbr_blocks; ++blk_cnt)
	{
		for (int pos = 0; pos < block_len; pos += interleave)
		{
#if defined (_MSC_VER) && (fstb_WORD_SIZE == 64)
			// VS2017 in 64-bit mode breaks on a C4789 error...
			T              a [8];
#else
			T              a [interleave];
#endif

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



// OPREF is always scalar with double as input and output types.
template <typename T, bool REL_FLAG>
template <typename OPTST, typename OPREF, typename S>
void	TestApprox::TestFncLogic <T, REL_FLAG>::test_op1 (const OPREF &op_ref, const OPTST &op_tst, const std::string &name, S min_val, S max_val)
{
	printf ("Logic test %s... ", name.c_str ());
	fflush (stdout);

	const int      nbr_spl = 234567;
	const double   mul     = double (max_val - min_val) / double (nbr_spl - 1);
	const double   add     = double (min_val);

	double         err_max = 0;
	double         err_tot = 0;
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		const double   val_src = pos * mul + add;
		const T        src_t = conv_s_to_t <T> (S (val_src));

		const double   val_ref = op_ref (val_src);

		const T        dst_t = op_tst (src_t);
		const double   val_tst = double (conv_t_to_s <S> (dst_t));

		double         err_abs = fabs (val_tst - val_ref);
		if (REL_FLAG)
		{
			assert (val_ref != 0);
			err_abs /= fabs (val_ref);
		}
		err_max  = std::max (err_max, err_abs);
		err_tot += err_abs;
	}

	const double   err_avg = err_tot / double (nbr_spl);
	if (REL_FLAG)
	{
		printf ("Err max: %.3f%%, avg: %.3f%%\n", err_max * 100, err_avg * 100);
	}
	else
	{
		printf ("Err max: %.3g, avg: %.3g\n", err_max      , err_avg      );
	}
}



template <typename T, bool REL_FLAG>
template <typename OPTST, typename OPREF, typename S>
void	TestApprox::TestFncLogic <T, REL_FLAG>::test_op2 (const OPREF &op_ref, const OPTST &op_tst, const std::string &name, S min_val1, S max_val1, S min_val2, S max_val2)
{
	printf ("Logic test %s... ", name.c_str ());
	fflush (stdout);

	const int      nbr_spl = 2345;
	const double   mul1    = double (max_val1 - min_val1) / double (nbr_spl - 1);
	const double   add1    = double (min_val1);
	const double   mul2    = double (max_val2 - min_val2) / double (nbr_spl - 1);
	const double   add2    = double (min_val2);

	double         err_max = 0;
	double         err_tot = 0;
	for (int pos1 = 0; pos1 < nbr_spl; ++pos1)
	{
		const double   val_src1 = pos1 * mul1 + add1;
		const T        src_t1   = conv_s_to_t <T> (S (val_src1));

		for (int pos2 = 0; pos2 < nbr_spl; ++pos2)
		{
			const double   val_src2 = pos2 * mul2 + add2;
			const T        src_t2   = conv_s_to_t <T> (S (val_src2));


			const double   val_ref = op_ref (val_src1, val_src2);

			const T        dst_t = op_tst (src_t1, src_t2);
			const double   val_tst = double (conv_t_to_s <S> (dst_t));

			double         err_abs = fabs (val_tst - val_ref);
			if (REL_FLAG)
			{
				if (val_ref != 0)
				{
					err_abs /= fabs (val_ref);
				}
			}
			err_max  = std::max (err_max, err_abs);
			err_tot += err_abs;
		}
	}

	const double   ns2     = double (nbr_spl) * double (nbr_spl);
	const double   err_avg = err_tot / ns2;
	if (REL_FLAG)
	{
		printf ("Err max: %.3f%%, avg: %.3f%%\n", err_max * 100, err_avg * 100);
	}
	else
	{
		printf ("Err max: %.3g, avg: %.3g\n", err_max      , err_avg      );
	}
}



template <bool REL_FLAG, typename OPREF, typename OPTSTS, typename OPTSTV>
void	TestApprox::test_op1_all_flt (const OPREF &op_ref, const OPTSTS &op_s, const OPTSTV &op_v, const std::string &name, float min_val, float max_val)
{
	test_op1_all_flt_s <REL_FLAG> (op_ref, op_s, name, min_val, max_val);
	test_op1_all_flt_v <REL_FLAG> (op_ref, op_v, name, min_val, max_val);
}



template <bool REL_FLAG, typename OPREF, typename OPTSTS>
void	TestApprox::test_op1_all_flt_s (const OPREF &op_ref, const OPTSTS &op_s, const std::string &name, float min_val, float max_val)
{
	const auto     name_s = name + " (float)";
	TestFncLogic <float, REL_FLAG>::test_op1 (
		op_ref, op_s, name_s, min_val, max_val
	);
	TestFncSpeed <float, 0>::test_op1 (
		op_s, name_s, min_val, max_val
	);
	TestFncSpeed <float, 1>::test_op1 (
		op_s, name_s, min_val, max_val
	);
	TestFncSpeed <float, 2>::test_op1 (
		op_s, name_s, min_val, max_val
	);
}



template <bool REL_FLAG, typename OPREF, typename OPTSTV>
void	TestApprox::test_op1_all_flt_v (const OPREF &op_ref, const OPTSTV &op_v, const std::string &name, float min_val, float max_val)
{
	typedef fstb::ToolsSimd::VectF32 VectF32;

	const auto     name_v = name + " (VectF32)";
	TestFncLogic <VectF32, REL_FLAG>::test_op1 (
		op_ref, op_v, name_v, min_val, max_val
	);
	TestFncSpeed <VectF32, 0>::test_op1 (
		op_v, name_v, min_val, max_val
	);
	TestFncSpeed <VectF32, 1>::test_op1 (
		op_v, name_v, min_val, max_val
	);
	TestFncSpeed <VectF32, 2>::test_op1 (
		op_v, name_v, min_val, max_val
	);
}



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
