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
#include <type_traits>
#include <vector>

#include <cassert>
#include <cmath>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/


int	TestApprox::perform_test ()
{
	int            ret_val = 0;

	typedef fstb::Vf32 Vf32;

	// sin

	test_op1_all_flt <false> (ret_val,
		[] (double x) { return sin (x); },
		[] (float x) { return fstb::Approx::sin_nick (x); },
		[] (Vf32 x) { return fstb::Approx::sin_nick (x); },
		"sin_nick", -fstb::PI, fstb::PI, 0.001
	);

	test_op1_all_flt <false> (ret_val,
		[] (double x) { return sin (x * fstb::PI * 0.5f); },
		[] (float x) { return fstb::Approx::sin_rbj_halfpi (x); },
		[] (Vf32 x) { return fstb::Approx::sin_rbj_halfpi (x); },
		"sin_rbj_halfpi", -1.0, 1.0, 2e-07
	);

	// log2

	test_op1_all_flt <false> (ret_val,
		[] (double x) { return log2 (x); },
		[] (float x) { return fstb::Approx::log2_crude (x); },
		[] (Vf32 x) { return fstb::Approx::log2_crude (x); },
		"log2_crude", 1e-3, 1e3, 0.05
	);

	test_op1_all_flt <false> (ret_val,
		[] (double x) { return log2 (x); },
		[] (float x) { return fstb::Approx::log2 (x); },
		[] (Vf32 x) { return fstb::Approx::log2 (x); },
		"log2", 1e-3, 1e3, 0.01
	);

	test_op1_all_flt <false> (ret_val,
		[] (double x) { return log2 (x); },
		[] (float x) { return fstb::Approx::log2_5th (x); },
		[] (Vf32 x) { return fstb::Approx::log2_5th (x); },
		"log2_5th", 1e-3, 1e3, 0.0002
	);

	test_op1_all_flt <false> (ret_val,
		[] (double x) { return log2 (x); },
		[] (float x) { return fstb::Approx::log2_7th (x); },
		[] (Vf32 x) { return fstb::Approx::log2_7th (x); },
		"log2_7th", 1e-3, 1e3, 7e-06
	);

	test_op1_all_flt_v <false> (ret_val,
		[] (double x) { return log2 (x); },
		[] (Vf32 x) { return fstb::log2 (x); },
		"log2", 1e-3, 1e3, 1e-06
	);

	// exp2

	test_op1_all_flt <true> (ret_val,
		[] (double x) { return exp2 (x); },
		[] (float x) { return fstb::Approx::exp2_crude (x); },
		[] (Vf32 x) { return fstb::Approx::exp2_crude (x); },
		"exp2_crude", -20.0, 20.0, 0.035
	);

	test_op1_all_flt <true> (ret_val,
		[] (double x) { return exp2 (x); },
		[] (float x) { return fstb::Approx::exp2 (x); },
		[] (Vf32 x) { return fstb::Approx::exp2 (x); },
		"exp2", -20.0, 20.0, 0.0035
	);

	test_op1_all_flt <true> (ret_val,
		[] (double x) { return exp2 (x); },
		[] (float x) { return fstb::Approx::exp2_5th (x); },
		[] (Vf32 x) { return fstb::Approx::exp2_5th (x); },
		"exp2_5th", -20.0, 20.0, 1e-6
	);

	test_op1_all_flt <true> (ret_val,
		[] (double x) { return exp2 (x); },
		[] (float x) { return fstb::Approx::exp2_7th (x); },
		[] (Vf32 x) { return fstb::Approx::exp2_7th (x); },
		"exp2_7th", -20.0, 20.0, 1e-6
	);

	test_op1_all_flt_v <true> (ret_val,
		[] (double x) { return exp2 (x); },
		[] (Vf32 x) { return fstb::exp2 (x); },
		"exp2", -20.0, 20.0, 1e-6
	);

	// tan

	test_op1_all_flt <false> (ret_val,
		[] (double x) { return tan (x); },
		[] (float x) { return fstb::Approx::tan_taylor5 (x); },
		[] (Vf32 x) { return fstb::Approx::tan_taylor5 (x); },
		"tan_taylor5", -1.0, 1.0, 0.1
	);

	test_op1_all_flt <false> (ret_val,
		[] (double x) { return tan (x); },
		[] (float x) { return fstb::Approx::tan_mystran (x); },
		[] (Vf32 x) { return fstb::Approx::tan_mystran (x); },
		"tan_mystran", -1.0, 1.0, 2e-05
	);

	// tanh

	test_op1_all_flt <false> (ret_val,
		[] (double x) { return tanh (x); },
		[] (float x) { return fstb::Approx::tanh_mystran (x); },
		[] (Vf32 x) { return fstb::Approx::tanh_mystran (x); },
		"tanh_mystran", -5.0, 5.0, 0.004
	);

	test_op1_all_flt <false> (ret_val,
		[] (double x) { return tanh (x); },
		[] (float x) { return fstb::Approx::tanh_2dat (x); },
		[] (Vf32 x) { return fstb::Approx::tanh_2dat (x); },
		"tanh_2dat", -10.0, 10.0, 1e-06
	);

	test_op1_all_flt <false> (ret_val,
		[] (double x) { return tanh (x); },
		[] (float x) { return fstb::Approx::tanh_andy (x); },
		[] (Vf32 x) { return fstb::Approx::tanh_andy (x); },
		"tanh_andy", -5.0, 5.0, 0.0007
	);

	test_op1_all_flt <false> (ret_val,
		[] (double x) { return tanh (x); },
		[] (float x) { return fstb::Approx::tanh_urs (x); },
		[] (Vf32 x) { return fstb::Approx::tanh_urs (x); },
		"tanh_urs", -10.0, 10.0, 2e-05
	);

	// pow

	TestFncLogic <double, true>::test_op2 (ret_val,
		[] (double x, double y) { return pow (x, y); },
		[] (double x, double y) { return fstb::Approx::pow_crude (x, y); },
		"pow_crude (double)", 0.1, 10.0, -10.0, 10.0, 0.71
	);

	// rsqrt

	test_op1_all_s <float, true> (ret_val,
		[] (double x) { return 1 / sqrt (x); },
		[] (float x) { return fstb::Approx::rsqrt <0> (x); },
		"rsqrt <0>", 1e-9, 4.0, 0.035
	);
	test_op1_all_flt <true> (ret_val,
		[] (double x) { return 1 / sqrt (x); },
		[] (float x) { return fstb::Approx::rsqrt <1> (x); },
		[] (Vf32 x) { return fstb::Approx::rsqrt <1> (x); },
		"rsqrt <1>", 1e-9, 4.0, 0.001
	);
	test_op1_all_flt <true> (ret_val,
		[] (double x) { return 1 / sqrt (x); },
		[] (float x) { return fstb::Approx::rsqrt <2> (x); },
		[] (Vf32 x) { return fstb::Approx::rsqrt <2> (x); },
		"rsqrt <2>", 1e-9, 4.0, 5e-06
	);
	test_op1_all_flt <true> (ret_val,
		[] (double x) { return 1 / sqrt (x); },
		[] (float x) { return fstb::Approx::rsqrt <3> (x); },
		[] (Vf32 x) { return fstb::Approx::rsqrt <3> (x); },
		"rsqrt <3>", 1e-9, 4.0, 2e-07
	);

	test_op1_all_flt_v <true> (ret_val,
		[] (double x) { return 1 / sqrt (x); },
		[] (Vf32 x) { return x.rsqrt (); },
		"rsqrt", 1e-9, 4.0, 1e-6
	);

	test_op1_all_s <double, true> (ret_val,
		[] (double x) { return 1 / sqrt (x); },
		[] (double x) { return fstb::Approx::rsqrt <2> (x); },
		"rsqrt <2>", 1e-9, 4.0, 5e-6
	);
	test_op1_all_s <double, true> (ret_val,
		[] (double x) { return 1 / sqrt (x); },
		[] (double x) { return fstb::Approx::rsqrt <3> (x); },
		"rsqrt <3>", 1e-9, 4.0, 5e-11
	);
	test_op1_all_s <double, true> (ret_val,
		[] (double x) { return 1 / sqrt (x); },
		[] (double x) { return fstb::Approx::rsqrt <4> (x); },
		"rsqrt <4>", 1e-9, 4.0, 5e-16
	);

	// atan2
	// Make sure we do not test (0, 0) as the angle is not defined and
	// the results between the approx and the reference may differ.

	TestFncLogic <float, false>::test_op2 (ret_val,
		[] (double x, double y) { return atan2 (y, x); },
		[] (float x, float y) { return fstb::Approx::atan2_3th (y, x); },
		"atan2_3th (float)", -10.001, 10.0, -10.0, 10.0, 0.007
	);
	TestFncLogic <float, false>::test_op2 (ret_val,
		[] (double x, double y) { return atan2 (y, x); },
		[] (float x, float y) { return fstb::Approx::atan2_7th (y, x); },
		"atan2_7th (float)", -10.001, 10.0, -10.0, 10.0, 0.00012
	);

	return ret_val;
}


/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/




template <typename T, typename S>
static inline T conv_s_to_t (S x) { return static_cast <T> (x); }

template <>
inline fstb::Vf32 conv_s_to_t (float x)
{
	return fstb::Vf32 (x);
}



template <typename S, typename T>
static inline S conv_t_to_s (T x) { return static_cast <S> (x); }

template <>
inline float conv_t_to_s (fstb::Vf32 x)
{
	return x.template extract <0> ();
}



template <typename T>
static inline bool check_all_elt_same (T x) { fstb::unused (x); return true; }

template <>
inline bool check_all_elt_same (fstb::Vf32 x)
{
	return bool (x.template spread <0> () == x);
}



template <typename T>
struct ToScalar {	typedef T Type; };
template <>
struct ToScalar <fstb::Vf32> { typedef fstb::Vf32::Scalar Type; };



template <typename T, int ILL2>
template <typename OP>
void	TestApprox::TestFncSpeed <T, ILL2>::test_op1 (const OP &op, const std::string &name, double min_val, double max_val)
{
	typedef typename ToScalar <T>::Type S;

	constexpr int  nbr_blocks  = 1000;
	constexpr int  nbr_tests   = 100;
	constexpr int  block_len_s = 64;
	constexpr int  s_per_t     = sizeof (T) / sizeof (S);
	static_assert ((block_len_s % s_per_t) == 0, "");
	constexpr int  block_len   = block_len_s / s_per_t;
	constexpr int  interleave  = 1 << ILL2;
	static_assert ((block_len % interleave) == 0, "");

	typedef std::vector <T, fstb::AllocAlign <T, 64> > Buffer;

	Buffer          src_arr (block_len);
	Buffer          dst_arr (block_len);

	BufferFiller::gen_rnd_scaled (
		reinterpret_cast <S *> (&src_arr [0]), block_len_s,
		S (min_val), S (max_val)
	);

	printf ("Speed test %s, interleave %d... ", name.c_str (), interleave);
	fflush (stdout);
	TimerAccurate  tim;
	S              dummy_val = S (0);

	tim.start ();
	for (int test_cnt = 0; test_cnt < nbr_tests; ++test_cnt)
	{
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
		tim.stop_lap ();
	}

	double	      spl_per_s = tim.get_best_rate (block_len_s * nbr_blocks);
	spl_per_s += fstb::limit (double (dummy_val), -1e-300, 1e-300);
	const double   mega_sps  = spl_per_s / 1000000.0;
	printf ("%12.3f Mspl/s.\n", mega_sps);
}



// OPREF is always scalar with double as input and output types.
template <typename T, bool REL_FLAG>
template <typename OPTST, typename OPREF>
void	TestApprox::TestFncLogic <T, REL_FLAG>::test_op1 (int &ret_val, const OPREF &op_ref, const OPTST &op_tst, const std::string &name, double min_val, double max_val, double err_allowed)
{
	typedef typename ToScalar <T>::Type S;

	if (ret_val != 0)
	{
		return;
	}

	printf ("Logic test %s... ", name.c_str ());
	fflush (stdout);

	const int      nbr_spl = 234567;
	const double   mul     = double (max_val - min_val) / double (nbr_spl - 1);
	const double   add     = double (min_val);

	double         err_max = 0;
	double         err_tot = 0;
	int            count   = 0;
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		const double   val_src = pos * mul + add;
		const T        src_t = conv_s_to_t <T> (S (val_src));

		const double   val_ref = op_ref (val_src);

		const T        dst_t = op_tst (src_t);
		if (! check_all_elt_same (dst_t))
		{
			printf ("\n*** Error: different vector elements.\n");
			ret_val = -1;
			return;
		}
		const double   val_tst = double (conv_t_to_s <S> (dst_t));

		double         err_abs = fabs (val_tst - val_ref);
		bool           check_flag = true;
		if (REL_FLAG)
		{
			if (val_ref == 0)
			{
				check_flag = false;
			}
			else
			{
				err_abs /= fabs (val_ref);
			}
		}

		if (check_flag)
		{
			if (err_allowed > 0 && err_abs > err_allowed)
			{
				printf (
					"\n*** Error: too large error %.3g. "
					"Input: %.9g, expected: %.9g, output: %.9g.\n",
					err_abs, val_src, val_ref, val_tst
				);
				ret_val = -1;
				return;
			}

			err_max  = std::max (err_max, err_abs);
			err_tot += err_abs;
			++ count;
		}
	}

	const double   err_avg = err_tot / double (count);
	printf (
		"Err max%s: %.3g, avg: %.3g\n",
		(REL_FLAG) ? " (relative)" : "",
		err_max, err_avg
	);
}



template <typename T, bool REL_FLAG>
template <typename OPTST, typename OPREF>
void	TestApprox::TestFncLogic <T, REL_FLAG>::test_op2 (int &ret_val, const OPREF &op_ref, const OPTST &op_tst, const std::string &name, double min_val1, double max_val1, double min_val2, double max_val2, double err_allowed)
{
	typedef typename ToScalar <T>::Type S;

	if (ret_val != 0)
	{
		return;
	}

	printf ("Logic test %s... ", name.c_str ());
	fflush (stdout);

	const int      nbr_spl = 2345;
	const double   mul1    = double (max_val1 - min_val1) / double (nbr_spl - 1);
	const double   add1    = double (min_val1);
	const double   mul2    = double (max_val2 - min_val2) / double (nbr_spl - 1);
	const double   add2    = double (min_val2);

	double         err_max = 0;
	double         err_tot = 0;
	long long      count   = 0;
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
			if (! check_all_elt_same (dst_t))
			{
				printf ("\n*** Error: different vector elements.\n");
				ret_val = -1;
				return;
			}
			const double   val_tst = double (conv_t_to_s <S> (dst_t));

			double         err_abs = fabs (val_tst - val_ref);
			bool           check_flag = true;
			if (REL_FLAG)
			{
				if (val_ref == 0)
				{
					check_flag = false;
				}
				else
				{
					err_abs /= fabs (val_ref);
				}
			}

			if (check_flag)
			{
				if (err_allowed > 0 && err_abs > err_allowed)
				{
					printf (
						"\n*** Error: too large error %.3g. "
						"Input: (%.9g, %.9g), expected: %.9g, output: %.9g.\n",
						err_abs, val_src1, val_src2, val_ref, val_tst
					);
					ret_val = -1;
					return;
				}

				err_max  = std::max (err_max, err_abs);
				err_tot += err_abs;
				++ count;
			}
		}
	}

	const double   err_avg = err_tot / double (count);
	printf (
		"Err max%s: %.3g, avg: %.3g\n",
		(REL_FLAG) ? " (relative)" : "",
		err_max, err_avg
	);
}



template <bool REL_FLAG, typename OPREF, typename OPTSTS, typename OPTSTV>
void	TestApprox::test_op1_all_flt (int &ret_val, const OPREF &op_ref, const OPTSTS &op_s, const OPTSTV &op_v, const std::string &name, double min_val, double max_val, double err_allowed)
{
	test_op1_all_s <float, REL_FLAG> (
		ret_val, op_ref, op_s, name, min_val, max_val, err_allowed
	);
	test_op1_all_flt_v <REL_FLAG> (
		ret_val, op_ref, op_v, name, min_val, max_val, err_allowed
	);
}



template <typename T, bool REL_FLAG, typename OPREF, typename OPTSTS>
void	TestApprox::test_op1_all_s (int &ret_val, const OPREF &op_ref, const OPTSTS &op_s, const std::string &name, double min_val, double max_val, double err_allowed)
{
	auto           name_s = name + " (";
	name_s +=
		  std::is_same <T, float >::value ? "float"
		: std::is_same <T, double>::value ? "double"
		:                                   "\?\?\?";
	name_s += ")";
	TestFncLogic <T, REL_FLAG>::test_op1 (
		ret_val, op_ref, op_s, name_s, min_val, max_val, err_allowed
	);
	if (ret_val == 0)
	{
		TestFncSpeed <T, 0>::test_op1 (
			op_s, name_s, min_val, max_val
		);
		TestFncSpeed <T, 1>::test_op1 (
			op_s, name_s, min_val, max_val
		);
		TestFncSpeed <T, 2>::test_op1 (
			op_s, name_s, min_val, max_val
		);
	}
}



template <bool REL_FLAG, typename OPREF, typename OPTSTV>
void	TestApprox::test_op1_all_flt_v (int &ret_val, const OPREF &op_ref, const OPTSTV &op_v, const std::string &name, double min_val, double max_val, double err_allowed)
{
	typedef fstb::Vf32 Vf32;

	const auto     name_v = name + " (Vf32)";
	TestFncLogic <Vf32, REL_FLAG>::test_op1 (
		ret_val, op_ref, op_v, name_v, min_val, max_val, err_allowed
	);
	if (ret_val == 0)
	{
		TestFncSpeed <Vf32, 0>::test_op1 (
			op_v, name_v, min_val, max_val
		);
		TestFncSpeed <Vf32, 1>::test_op1 (
			op_v, name_v, min_val, max_val
		);
		TestFncSpeed <Vf32, 2>::test_op1 (
			op_v, name_v, min_val, max_val
		);
	}
}



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
