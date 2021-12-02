/*****************************************************************************

        fnc.hpp
        Author: Laurent de Soras, 2010

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_fnc_CODEHEADER_INCLUDED)
#define	fstb_fnc_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#include <algorithm>
#include <type_traits>

#if (fstb_ARCHI == fstb_ARCHI_X86)
	#if defined (fstb_HAS_SIMD) || fstb_WORD_SIZE == 64
		#include <emmintrin.h>
	#endif
#endif

#if defined (_MSC_VER)
	#if (fstb_ARCHI == fstb_ARCHI_X86)
		#include <intrin.h>
		#if ((_MSC_VER / 100) >= 14)
			#pragma intrinsic (_BitScanReverse)
		#endif
	#endif
#endif

#include <cassert>
#include <climits>
#include <cmath>
#include <cstdint>
#include <cstring>



namespace fstb
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class T>
constexpr int	sgn (T x) noexcept
{
	return x < T (0) ? -1 : x > T (0) ? 1 : 0;
}



template <class T>
constexpr T	limit (T x, T mi, T ma) noexcept
{
	return (x < mi) ? mi : ((x > ma) ? ma : x);
}



template <class T>
constexpr void	sort_2_elt (T &mi, T &ma, T a, T b) noexcept
{
	if (a < b)
	{
		mi = a;
		ma = b;
	}
	else
	{
		mi = b;
		ma = a;
	}
}



template <class T>
constexpr bool	is_pow_2 (T x) noexcept
{
	return ((x & -x) == x);
}



double	round (double x) noexcept
{
	return floor (x + 0.5f);
}



float	round (float x) noexcept
{
	return floorf (x + 0.5f);
}



int	round_int (float x) noexcept
{
	assert (x <= double (INT_MAX));
	assert (x >= static_cast <double> (INT_MIN));

#if (fstb_ARCHI == fstb_ARCHI_X86)

 #if defined (fstb_HAS_SIMD) || fstb_WORD_SIZE == 64

	return _mm_cvtss_si32 (_mm_set_ss (x));

 #elif defined (_MSC_VER)

	int            i;
	__asm
	{
		fld            x
		fistp          i
	}
	return i;

 #else

	return int (floorf (x + 0.5f));

 #endif  // Compiler

#else // fstb_ARCHI_X86

	// Slow
	return int (floorf (x + 0.5f));

#endif // fstb_ARCHI_X86
}



int	round_int (double x) noexcept
{
	return round_int (float (x));
}



int	round_int_accurate (double x) noexcept
{
	assert (x <= double (INT_MAX));
	assert (x >= static_cast <double> (INT_MIN));

#if (fstb_ARCHI == fstb_ARCHI_X86)

 #if defined (fstb_HAS_SIMD) || fstb_WORD_SIZE == 64

	constexpr float   round_to_nearest = 0.5f;
	const double	xx     = x + x + round_to_nearest;
	const __m128d	x_128d = _mm_set_sd (xx);
	const int      i      = _mm_cvtsd_si32 (x_128d) >> 1;
	return i;

 #elif defined (_MSC_VER)

	assert (x <= double (INT_MAX/2));
	assert (x >= double (INT_MIN/2));

	static const float	round_to_nearest = 0.5f;
	int				i;

	__asm
	{
		fld            x
		fadd           st, st (0)
		fadd           round_to_nearest
		fistp          i
		sar            i, 1
	}

	assert (i == int (floor (x + 0.5)));

	return i;

 #elif defined (__GNUC__)

	assert (x <= double (INT_MAX/2));
	assert (x >= double (INT_MIN/2));

	int				i;
	static const float	round_to_nearest = 0.5f;
	asm (
		"fldl				%[x]				\n"
		"fadd				%%st (0), %%st	\n"
		"fadds			(%[r])				\n"
		"fistpl			%[i]				\n"
		"sarl				$1, %[i]			\n"
	:	[i]	"=m"	(i)
 	:	[r]	"r"	(&round_to_nearest)
 	,	[x]	"m"	(x)
	:	"st"
	);

	assert (i == int (floor (x + 0.5)));

	return i;

 #else

	// Slow
	return int (floor (x + 0.5));

 #endif  // Compiler

#else // fstb_ARCHI_X86

	// Slow
	return int (floor (x + 0.5));

#endif // fstb_ARCHI_X86
}



int64_t round_int64 (double x) noexcept
{
	return int64_t (round (x));
}



// May not give the right result for very small negative values.
int	floor_int (float x) noexcept
{
	assert (x <= double (INT_MAX));
	assert (x >= static_cast <double> (INT_MIN));

#if (fstb_ARCHI == fstb_ARCHI_X86)

 #if defined (fstb_HAS_SIMD) || fstb_WORD_SIZE == 64

	constexpr float   round_toward_m_i = -0.5f;
	const float    xx     = x + x + round_toward_m_i;
	const __m128   x_128  = _mm_set_ss (xx);
	const int      i      = _mm_cvt_ss2si (x_128) >> 1;
	return i;

 #elif defined (_MSC_VER)

	assert (x <= double (INT_MAX/2));
	assert (x >= double (INT_MIN/2));

	int            i;
	constexpr float   round_toward_m_i = -0.5f;
	__asm
	{
		fld            x
		fadd           round_toward_m_i
		fistp          i
	}
	return i;

 #elif defined (__GNUC__)

	assert (x <= double (INT_MAX/2));
	assert (x >= double (INT_MIN/2));
	int				i;
	static const float	round_toward_m_i = -0.5f;
	asm (
		"fldl				%[x]				\n"
		"fadds			(%[rm])				\n"
		"fistpl			%[i]				\n"
	:	[i]	"=m"	(i)
 	:	[rm]	"r"	(&round_toward_m_i)
	,	[x]	"m"	(x)
	:	//"st"
	);
	return i;

 #else

	return int (floorf (x));

 #endif // Compiler

#else  // fstb_ARCHI_X86

	return int (floorf (x));

#endif // fstb_ARCHI_X86
}



int	floor_int (double x) noexcept
{
	return floor_int (float (x));
}



// May not give the right result for very small negative values.
int	floor_int_accurate (double x) noexcept
{
	assert (x <= double (INT_MAX));
	assert (x >= static_cast <double> (INT_MIN));

#if (fstb_ARCHI == fstb_ARCHI_X86)

 #if defined (fstb_HAS_SIMD) || fstb_WORD_SIZE == 64

	constexpr float   round_toward_m_i = -0.5f;
	const double   xx     = x + x + round_toward_m_i;
	const __m128d  x_128d = _mm_set_sd (xx);
	const int      i      = _mm_cvtsd_si32 (x_128d) >> 1;
	return i;

 #elif defined (_MSC_VER)

	assert (x <= double (INT_MAX/2));
	assert (x >= double (INT_MIN/2));
	int            i;
	static const float   round_toward_m_i = -0.5f;
	__asm
	{
		fld            x
		fadd           st, st (0)
		fadd           round_toward_m_i
		fistp          i
		sar            i, 1
	}
	assert (i == int (floor (x)) || fabs (i - x) < 1e-10);
	return i;

 #elif defined (__GNUC__)

	assert (x <= double (INT_MAX/2));
	assert (x >= double (INT_MIN/2));
	int				i;	
	static const float	round_toward_m_i = -0.5f;
	asm (
		"fldl				%[x]				\n"
		"fadd				%%st (0), %%st	\n"
		"fadds			(%[rm])				\n"
		"fistpl			%[i]				\n"
		"sarl				$1, %[i]			\n"
	:	[i]	"=m"	(i)
 	:	[rm]	"r"	(&round_toward_m_i)
	,	[x]	"m"	(x)
	:	//"st"
	);
	
	assert (i == int (floor (x)) || fabs (i - x) < 1e-10);

	return i;

 #else

	// Slow
	return int (floor (x));

 #endif // Compiler

#else  // fstb_ARCHI_X86

	// Slow
	return int (floor (x));

#endif // fstb_ARCHI_X86
}



int64_t	floor_int64 (double x) noexcept
{
	return int64_t (floor (x));
}



// May not give the right result for very small positive values.
int	ceil_int (double x) noexcept
{
	assert (x <= double (INT_MAX));
	assert (x >= static_cast <double> (INT_MIN));

#if (fstb_ARCHI == fstb_ARCHI_X86)

 #if defined (fstb_HAS_SIMD) || fstb_WORD_SIZE == 64

	constexpr float   round_toward_p_i = -0.5f;
	const double   xx     = round_toward_p_i - (x + x);
	const __m128d  x_128d = _mm_set_sd (xx);
	const int      i      = _mm_cvtsd_si32 (x_128d) >> 1;
	return -i;

 #elif (defined (_MSC_VER))

	assert (x <= double (INT_MAX/2));
	assert (x >= double (INT_MIN/2));

	int            i;
	static const float   round_toward_p_i = -0.5f;
	__asm
	{
		fld            x
		fadd           st, st (0)
		fsubr          round_toward_p_i
		fistp          i
		sar            i, 1
	}
	assert (-i == int (ceil (x)) || fabs (-i - x) < 1e-10);
	return -i;

 #elif defined (__GNUC__)

	assert (x <= double (INT_MAX/2));
	assert (x >= double (INT_MIN/2));
	int				i;
	static const float	round_toward_p_i = -0.5f;
	asm (
		"fldl				%[x]				\n"
		"fadd				%%st (0), %%st	\n"
		"fsubrs			(%[r])			\n"
		"fistpl			%[i]				\n"
		"sarl				$1, %[i]			\n"
	:	[i]	"=m"	(i)
	:	[r]	"r"	(&round_toward_p_i)
	,	[x]	"m"	(x)
	:	//"st"
	);
	assert (-i == int (ceil (x)) || fabs (-i - x) < 1e-10);
	return -i;

 #else

	// Slow
	return int (ceil (x));

 #endif

#else

	// Slow
	return int (ceil (x));

#endif
}



template <class T>
int	trunc_int (T x) noexcept
{
#if (fstb_ARCHI == fstb_ARCHI_X86) && (defined (fstb_HAS_SIMD) || fstb_WORD_SIZE == 64)
	return _mm_cvtt_ss2si (_mm_set1_ps (float (x)));
#else
	return int (x);
#endif
}



template <class T>
int	conv_int_fast (T x) noexcept
{
	static_assert (std::is_floating_point <T>::value, "T must be floating point");

	int            p;

#if (fstb_ARCHI == fstb_ARCHI_X86)

 #if defined (_MSC_VER)

  #if defined (_WIN64) || defined (_M_X64)

	const __m128d  x_128d = _mm_set_sd (x);
	p = _mm_cvtsd_si32 (x_128d);

  #else

	__asm
	{
		fld            x;
		fistp          p;
	}

  #endif

 #elif defined (__GNUC__)

	asm (
		"flds				%[x]		\n"
		"fistpl			%[v]		\n"
	:	[v]	"=m"	(p)
	:	[x]	"m"	(x)
	:	
	);

 #else

	// Slow...
	p = int (x);

 #endif

#else

	// Slow...
	p = int (x);

#endif

	return p;
}



template <class T>
constexpr bool	is_null (T val, T eps) noexcept
{
	static_assert (std::is_floating_point <T>::value, "T must be floating point");
	assert (eps >= 0);

	return (fabs (val) <= eps);
}



template <class T>
constexpr bool	is_eq (T v1, T v2, T eps) noexcept
{
	static_assert (std::is_floating_point <T>::value, "T must be floating point");
	assert (eps >= 0);

	return is_null (v2 - v1, eps);
}



template <class T>
constexpr bool	is_eq_rel (T v1, T v2, T tol) noexcept
{
	static_assert (std::is_floating_point <T>::value, "T must be floating point");
	assert (tol >= 0);

	const T        v1a = T (fabs (v1));
	const T        v2a = T (fabs (v2));
	const T        eps = std::max (v1a, v2a) * tol;

	return is_eq (v1, v2, eps);
}



/*
==============================================================================
Name: get_prev_pow2
Description:
	Computes the exponent of the power of two equal to or immediately lower
	than the parameter. It is the base-2 log rounded toward minus infinity.
Input parameters:
	- x: Number which we want to compute the base-2 log.
Returns: The exponent
Throws: Nothing
==============================================================================
*/

int	get_prev_pow_2 (uint32_t x) noexcept
{
	assert (x > 0);

#if (fstb_ARCHI == fstb_ARCHI_X86) && defined (_MSC_VER)

  #if ((_MSC_VER / 100) < 14)

	int            p;
	__asm
	{
		xor            eax, eax
		bsr            eax, x
		mov            p, eax
	}

  #else

	unsigned long	p;
	_BitScanReverse (&p, x);

  #endif

#else

	int            p = -1;

	while ((x & ~(uint32_t (0xFFFF))) != 0)
	{
		p += 16;
		x >>= 16;
	}
	while ((x & ~(uint32_t (0xF))) != 0)
	{
		p += 4;
		x >>= 4;
	}
	while (x > 0)
	{
		++p;
		x >>= 1;
	}

#endif

	return int (p);
}



/*
==============================================================================
Name: get_next_pow2
Description:
	Computes the exponent of the power of two equal to or immediately greater
	than the parameter. It is the base-2 log rounded toward plus infinity.
Input parameters:
	- x: Number which we want to compute the base-2 log.
Returns: The exponent
Throws: Nothing
==============================================================================
*/

int	get_next_pow_2 (uint32_t x) noexcept
{
	assert (x > 0);

#if (fstb_ARCHI == fstb_ARCHI_X86) && defined (_MSC_VER)

  #if ((_MSC_VER / 100) < 14)

	-- x;
	int				p;

	if (x == 0)
	{
		p = 0;
	}
	else
	{
		__asm
		{
			xor				eax, eax
			bsr				eax, x
			inc				eax
			mov				p, eax
		}
	}

  #else

	unsigned long	p;
	if (_BitScanReverse (&p, x - 1) == 0)
	{
		p = 0;
	}
	else
	{
		++ p;
	}

  #endif

#else

	--x;
	int				p = 0;

	while ((x & ~(uint32_t (0xFFFFL))) != 0)
	{
		p += 16;
		x >>= 16;
	}
	while ((x & ~(uint32_t (0xFL))) != 0)
	{
		p += 4;
		x >>= 4;
	}
	while (x > 0)
	{
		++p;
		x >>= 1;
	}

#endif

	return int (p);
}



constexpr double	sinc (double x) noexcept
{
	if (x == 0)
	{
		return 1;
	}

	const double   xp = x * PI;

	return sin (xp) / xp;
}



double  pseudo_exp (double x, double c) noexcept
{
	assert (x >= 0);
	assert (c > 0);

	const double   num = exp (c * x) - 1;
	const double   den = exp (c    ) - 1;

	return num / den;
}



double  pseudo_log (double y, double c) noexcept
{
	assert (y >= 0);
	assert (c > 0);

	const double   num = log (y * (exp (c) - 1) + 1);

	return num / c;
}



template <class T, int S, bool L>
class fnc_ShiftGeneric
{
public:
	static_assert (S < int (sizeof (T) * CHAR_BIT), "Shift too large");
	static constexpr T sh (T x) noexcept { return x << S; }
};
template <class T, int S>
class fnc_ShiftGeneric <T, S, false>
{
public:
	static_assert (S < int (sizeof (T) * CHAR_BIT), "Shift too large");
	static constexpr T sh (T x) noexcept { return x >> S; }
};

template <class T, int S>
constexpr T	sshift_l (T x) noexcept
{
	static_assert (std::is_integral <T>::value, "T must be integer");
	return fnc_ShiftGeneric <T, (S < 0) ? -S : S, (S > 0)>::sh (x);
}

template <class T, int S>
constexpr T	sshift_r (T x) noexcept
{
	static_assert (std::is_integral <T>::value, "T must be integer");
	return fnc_ShiftGeneric <T, (S < 0) ? -S : S, (S < 0)>::sh (x);
}



template <typename T>
constexpr T	sra_ceil (T x, int s) noexcept
{
	assert (s >= 0);

	const auto     div = T (1) << s;
	const auto     ofs = div - T (1);

	return (x + ofs) >> s;
}



template <typename T>
constexpr T	div_ceil (T num, T den) noexcept
{
	assert (den > T (0));

	const auto     ofs = den - T (1);
	const auto     n_o = num + ofs;
	assert (n_o > T (0));

	return n_o / den;
}



template <class T>
constexpr T	sq (T x) noexcept
{
	return x * x;
}



template <class T>
constexpr T	cube (T x) noexcept
{
	return x * x * x;
}



// U must be a signed integer type
template <class T, class U>
constexpr T	ipow (T x, U n) noexcept
{
	const U			abs_n = std::abs (n);
	const T			z (ipowp (x, abs_n));

	return (n < U (0)) ? T (1) / z : z;
}



// U must be an integer type (signed or not)
template <class T, class U>
constexpr T	ipowp (T x, U n) noexcept
{
	assert (! (n < U (0)));

#if 1
	T					z (1);

	while (n != U (0))
	{
		if ((n & U (1)) != U (0))
		{
			z *= x;
		}
		n >>= 1;
		x *= x;
	}
#else
	T              z (((n & U (1)) != U (0)) ? x : 1);
	n >>= 1;
	if (n > U (0))
	{
		x *= x;
		z *= ipowp (x, n);
	}
#endif

	return z;
}



// Result looks optimal with all optimisations enabled
template <int N, class T>
constexpr T	ipowpc (T x) noexcept
{
	static_assert (N >= 0, "N must be positive or null.");

#if (__cplusplus >= 201402L)
	if (N == 0)
	{
		return T (1);
	}
	else if (N > 1)
	{
		T              y = ipowpc <N / 2> (x);
		y *= y;
		if ((N & 1) != 0)
		{
			y *= x;
		}
		return y;
	}

	return x;
#else
	return
		  (N == 0) ? 1
		: (N >  1) ? (sq (ipowpc <N / 2> (x)) * (((N & 1) != 0) ? x : 1))
		: x;
#endif
}



template <class T>
constexpr T	rcp_uint (int x) noexcept
{
	static_assert (std::is_floating_point <T>::value, "T must be floating point");

	constexpr int  table_len           = 256;
	constexpr T    rcp_arr [table_len] =
	{
		T (0.00000000000000E+00), T (1.00000000000000E+00), T (5.00000000000000E-01), T (3.33333333333333E-01),
		T (2.50000000000000E-01), T (2.00000000000000E-01), T (1.66666666666667E-01), T (1.42857142857143E-01),
		T (1.25000000000000E-01), T (1.11111111111111E-01), T (1.00000000000000E-01), T (9.09090909090909E-02),
		T (8.33333333333333E-02), T (7.69230769230769E-02), T (7.14285714285714E-02), T (6.66666666666667E-02),
		T (6.25000000000000E-02), T (5.88235294117647E-02), T (5.55555555555556E-02), T (5.26315789473684E-02),
		T (5.00000000000000E-02), T (4.76190476190476E-02), T (4.54545454545455E-02), T (4.34782608695652E-02),
		T (4.16666666666667E-02), T (4.00000000000000E-02), T (3.84615384615385E-02), T (3.70370370370370E-02),
		T (3.57142857142857E-02), T (3.44827586206897E-02), T (3.33333333333333E-02), T (3.22580645161290E-02),
		T (3.12500000000000E-02), T (3.03030303030303E-02), T (2.94117647058823E-02), T (2.85714285714286E-02),
		T (2.77777777777778E-02), T (2.70270270270270E-02), T (2.63157894736842E-02), T (2.56410256410256E-02),
		T (2.50000000000000E-02), T (2.43902439024390E-02), T (2.38095238095238E-02), T (2.32558139534884E-02),
		T (2.27272727272727E-02), T (2.22222222222222E-02), T (2.17391304347826E-02), T (2.12765957446808E-02),
		T (2.08333333333333E-02), T (2.04081632653061E-02), T (2.00000000000000E-02), T (1.96078431372549E-02),
		T (1.92307692307692E-02), T (1.88679245283019E-02), T (1.85185185185185E-02), T (1.81818181818182E-02),
		T (1.78571428571429E-02), T (1.75438596491228E-02), T (1.72413793103448E-02), T (1.69491525423729E-02),
		T (1.66666666666667E-02), T (1.63934426229508E-02), T (1.61290322580645E-02), T (1.58730158730159E-02),
		T (1.56250000000000E-02), T (1.53846153846154E-02), T (1.51515151515152E-02), T (1.49253731343284E-02),
		T (1.47058823529412E-02), T (1.44927536231884E-02), T (1.42857142857143E-02), T (1.40845070422535E-02),
		T (1.38888888888889E-02), T (1.36986301369863E-02), T (1.35135135135135E-02), T (1.33333333333333E-02),
		T (1.31578947368421E-02), T (1.29870129870130E-02), T (1.28205128205128E-02), T (1.26582278481013E-02),
		T (1.25000000000000E-02), T (1.23456790123457E-02), T (1.21951219512195E-02), T (1.20481927710843E-02),
		T (1.19047619047619E-02), T (1.17647058823529E-02), T (1.16279069767442E-02), T (1.14942528735632E-02),
		T (1.13636363636364E-02), T (1.12359550561798E-02), T (1.11111111111111E-02), T (1.09890109890110E-02),
		T (1.08695652173913E-02), T (1.07526881720430E-02), T (1.06382978723404E-02), T (1.05263157894737E-02),
		T (1.04166666666667E-02), T (1.03092783505155E-02), T (1.02040816326531E-02), T (1.01010101010101E-02),
		T (1.00000000000000E-02), T (9.90099009900990E-03), T (9.80392156862745E-03), T (9.70873786407767E-03),
		T (9.61538461538462E-03), T (9.52380952380952E-03), T (9.43396226415094E-03), T (9.34579439252336E-03),
		T (9.25925925925926E-03), T (9.17431192660550E-03), T (9.09090909090909E-03), T (9.00900900900901E-03),
		T (8.92857142857143E-03), T (8.84955752212389E-03), T (8.77192982456140E-03), T (8.69565217391304E-03),
		T (8.62068965517241E-03), T (8.54700854700855E-03), T (8.47457627118644E-03), T (8.40336134453782E-03),
		T (8.33333333333333E-03), T (8.26446280991736E-03), T (8.19672131147541E-03), T (8.13008130081301E-03),
		T (8.06451612903226E-03), T (8.00000000000000E-03), T (7.93650793650794E-03), T (7.87401574803150E-03),
		T (7.81250000000000E-03), T (7.75193798449612E-03), T (7.69230769230769E-03), T (7.63358778625954E-03),
		T (7.57575757575758E-03), T (7.51879699248120E-03), T (7.46268656716418E-03), T (7.40740740740741E-03),
		T (7.35294117647059E-03), T (7.29927007299270E-03), T (7.24637681159420E-03), T (7.19424460431655E-03),
		T (7.14285714285714E-03), T (7.09219858156028E-03), T (7.04225352112676E-03), T (6.99300699300699E-03),
		T (6.94444444444444E-03), T (6.89655172413793E-03), T (6.84931506849315E-03), T (6.80272108843537E-03),
		T (6.75675675675676E-03), T (6.71140939597315E-03), T (6.66666666666667E-03), T (6.62251655629139E-03),
		T (6.57894736842105E-03), T (6.53594771241830E-03), T (6.49350649350649E-03), T (6.45161290322581E-03),
		T (6.41025641025641E-03), T (6.36942675159236E-03), T (6.32911392405063E-03), T (6.28930817610063E-03),
		T (6.25000000000000E-03), T (6.21118012422360E-03), T (6.17283950617284E-03), T (6.13496932515337E-03),
		T (6.09756097560976E-03), T (6.06060606060606E-03), T (6.02409638554217E-03), T (5.98802395209581E-03),
		T (5.95238095238095E-03), T (5.91715976331361E-03), T (5.88235294117647E-03), T (5.84795321637427E-03),
		T (5.81395348837209E-03), T (5.78034682080925E-03), T (5.74712643678161E-03), T (5.71428571428571E-03),
		T (5.68181818181818E-03), T (5.64971751412429E-03), T (5.61797752808989E-03), T (5.58659217877095E-03),
		T (5.55555555555556E-03), T (5.52486187845304E-03), T (5.49450549450549E-03), T (5.46448087431694E-03),
		T (5.43478260869565E-03), T (5.40540540540541E-03), T (5.37634408602151E-03), T (5.34759358288770E-03),
		T (5.31914893617021E-03), T (5.29100529100529E-03), T (5.26315789473684E-03), T (5.23560209424084E-03),
		T (5.20833333333333E-03), T (5.18134715025907E-03), T (5.15463917525773E-03), T (5.12820512820513E-03),
		T (5.10204081632653E-03), T (5.07614213197969E-03), T (5.05050505050505E-03), T (5.02512562814070E-03),
		T (5.00000000000000E-03), T (4.97512437810945E-03), T (4.95049504950495E-03), T (4.92610837438424E-03),
		T (4.90196078431373E-03), T (4.87804878048781E-03), T (4.85436893203883E-03), T (4.83091787439613E-03),
		T (4.80769230769231E-03), T (4.78468899521531E-03), T (4.76190476190476E-03), T (4.73933649289100E-03),
		T (4.71698113207547E-03), T (4.69483568075117E-03), T (4.67289719626168E-03), T (4.65116279069768E-03),
		T (4.62962962962963E-03), T (4.60829493087558E-03), T (4.58715596330275E-03), T (4.56621004566210E-03),
		T (4.54545454545455E-03), T (4.52488687782805E-03), T (4.50450450450451E-03), T (4.48430493273543E-03),
		T (4.46428571428571E-03), T (4.44444444444444E-03), T (4.42477876106195E-03), T (4.40528634361234E-03),
		T (4.38596491228070E-03), T (4.36681222707424E-03), T (4.34782608695652E-03), T (4.32900432900433E-03),
		T (4.31034482758621E-03), T (4.29184549356223E-03), T (4.27350427350427E-03), T (4.25531914893617E-03),
		T (4.23728813559322E-03), T (4.21940928270042E-03), T (4.20168067226891E-03), T (4.18410041841004E-03),
		T (4.16666666666667E-03), T (4.14937759336100E-03), T (4.13223140495868E-03), T (4.11522633744856E-03),
		T (4.09836065573771E-03), T (4.08163265306122E-03), T (4.06504065040650E-03), T (4.04858299595142E-03),
		T (4.03225806451613E-03), T (4.01606425702811E-03), T (4.00000000000000E-03), T (3.98406374501992E-03),
		T (3.96825396825397E-03), T (3.95256916996047E-03), T (3.93700787401575E-03), T (3.92156862745098E-03),
	};

	assert (x > 0);

	if (x < table_len)
	{
		return rcp_arr [x];
	}

	return T (1) / T (x);
}



template <class T>
constexpr T	lerp (T v0, T v1, T p) noexcept
{
	return v0 + p * (v1 - v0);
}



// Finds the x position of the extremum (min or max) in the parabolic-
// interpolated curve passes through (-1, r1), (0, r2) and (+1, r3).
// The curve is implicitely defined by:
// f(x) = ((r3 + r1) / 2 - r2) * x^2 + ((r3 - r1) / 2) * x + r2
// The points must not be aligned so the extremum exists.
// It is not necessariy located between -1 and 1.
template <class T>
constexpr T	find_extremum_pos_parabolic (T r1, T r2, T r3) noexcept
{
	const T        den = T (2) * r2 - (r3 + r1);
	assert (den != T (0));

	const T        pos = (r3 - r1) * T (0.5) / den;

	return pos;
}



// Uses the full data width
template <class T>
constexpr T	rotl (T x, int k) noexcept
{
	static_assert (std::is_integral <T>::value, "T must be integer");
	static_assert (std::is_unsigned <T>::value, "T must be unsigned");
	constexpr int  bitdepth = sizeof (T) * CHAR_BIT;
	assert (k >= 0);
	assert (k < bitdepth);

	return (x << k) | (x >> (bitdepth - k));
}



// Uses the full data width
template <class T>
constexpr T	rotr (T x, int k) noexcept
{
	static_assert (std::is_integral <T>::value, "T must be integer");
	static_assert (std::is_unsigned <T>::value, "T must be unsigned");
	constexpr int  bitdepth = sizeof (T) * CHAR_BIT;
	assert (k >= 0);
	assert (k < bitdepth);

	return (x >> k) | (x << (bitdepth - k));
}



namespace detail
{
	template <typename T, std::size_t... IS>
	constexpr std::array <T, sizeof... (IS)>
	make_array (const T &init_val, std::index_sequence <IS...>)
	{
		return {{ (static_cast <void> (IS), init_val)... }};
	}
}

// Default-initializes an array with a specified value
// Source: https://stackoverflow.com/a/41259045
template <std::size_t N, typename T>
constexpr std::array <T, N> make_array (const T &init_val)
{
	return detail::make_array (init_val, std::make_index_sequence <N> ());
}



template <class T>
T	read_unalign (const void *ptr) noexcept
{
	static_assert (
		std::is_trivially_copyable <T>::value, "T must be trivially copiable"
	);
	assert (ptr != nullptr);

	T              val;
	memcpy (&val, ptr, sizeof (val));
	return val;
}



template <class T>
void	write_unalign (void *ptr, T val) noexcept
{
	static_assert (
		std::is_trivially_copyable <T>::value, "T must be trivially copiable"
	);
	assert (ptr != nullptr);

	memcpy (ptr, &val, sizeof (val));
}



// std::copy is already optimized like this but uses memmove instead of
// memcpy.
template <typename T>
void    copy_no_overlap (T * fstb_RESTRICT dst_ptr, const T * fstb_RESTRICT src_ptr, int nbr_elt) noexcept
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_elt > 0);

	if (std::is_trivially_copyable <T>::value)
	{
		memcpy (dst_ptr, src_ptr, nbr_elt * sizeof (*dst_ptr));
	}
	else
	{
		std::copy (src_ptr, src_ptr + nbr_elt, dst_ptr);
	}
}



template <typename T>
bool	is_ptr_align_nz (const T *ptr, int a) noexcept
{
	assert (a > 0);
	assert (is_pow_2 (a));

	return (
		   ptr != nullptr
		&& (reinterpret_cast <intptr_t> (ptr) & (a - 1)) == 0
	);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace fstb



#endif	// fstb_fnc_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
