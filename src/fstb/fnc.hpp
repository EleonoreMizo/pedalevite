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



namespace fstb
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class T>
T	limit (T x, T mi, T ma)
{
	return ((x < mi) ? mi : ((x > ma) ? ma : x));
}



template <class T>
void	sort_2_elt (T &mi, T &ma, T a, T b)
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
bool	is_pow_2 (T x)
{
	return ((x & -x) == x);
}



double	round (double x)
{
	return (floor (x + 0.5));
}



int	round_int (double x)
{
	assert (x <= double (INT_MAX));
	assert (x >= double (INT_MIN));

#if (fstb_ARCHI == fstb_ARCHI_X86)

 #if defined (_MSC_VER)

	assert (x <= double (INT_MAX/2));
	assert (x >= double (INT_MIN/2));

	static const float	round_to_nearest = 0.5f;
	int				i;

  #if defined (_WIN64) || defined (__64BIT__) || defined (__amd64__) || defined (__x86_64__)

	const double	xx = x + x + round_to_nearest;
	const __m128d	x_128d = _mm_set_sd (xx);
	i = _mm_cvtsd_si32 (x_128d);
	i >>= 1;

  #else

	__asm
	{
		fld            x
		fadd           st, st (0)
		fadd           round_to_nearest
		fistp          i
		sar            i, 1
	}

  #endif

	assert (i == int (floor (x + 0.5)));

	return (i);

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

	return (i);

 #else

	// Slow
	return (int (floor (x + 0.5)));

 #endif  // Compiler

#else // fstb_ARCHI_X86

	// Slow
	return (int (floor (x + 0.5)));

#endif // fstb_ARCHI_X86
}



int64_t round_int64 (double x)
{
	return (int64_t (round (x)));
}



// May not give the right result for very small negative values.
int	floor_int (double x)
{
	assert (x <= double (INT_MAX));
	assert (x >= double (INT_MIN));

#if (fstb_ARCHI == fstb_ARCHI_X86)

 #if defined (_MSC_VER)

	assert (x <= double (INT_MAX/2));
	assert (x >= double (INT_MIN/2));

	int            i;
	static const float   round_toward_m_i = -0.5f;

  #if defined (_WIN64) || defined (__64BIT__) || defined (__amd64__) || defined (__x86_64__)

	const double   xx = x + x + round_toward_m_i;
	const __m128d  x_128d = _mm_set_sd (xx);
	i = _mm_cvtsd_si32 (x_128d);
	i >>= 1;

  #else

	__asm
	{
		fld            x
		fadd           st, st (0)
		fadd           round_toward_m_i
		fistp          i
		sar            i, 1
	}

  #endif

	using namespace std;
	assert (i == int (floor (x)) || fabs (i - x) < 1e-10);

	return (i);

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

	return (i);

 #else

	// Slow
	return (int (floor (x)));

 #endif // Compiler

#else  // fstb_ARCHI_X86

	// Slow
	return (int (floor (x)));

#endif // fstb_ARCHI_X86
}



int64_t	floor_int64 (double x)
{
	return (int64_t (floor (x)));
}



// May not give the right result for very small positive values.
int	ceil_int (double x)
{
	assert (x <= double (INT_MAX));
	assert (x >= double (INT_MIN));

#if (fstb_ARCHI == fstb_ARCHI_X86)

 #if (defined (_MSC_VER))

	assert (x <= double (INT_MAX/2));
	assert (x >= double (INT_MIN/2));

	int            i;
	static const float   round_toward_p_i = -0.5f;

  #if defined (_WIN64) || defined (__64BIT__) || defined (__amd64__) || defined (__x86_64__)

	const double   xx = round_toward_p_i - (x + x);
	const __m128d  x_128d = _mm_set_sd (xx);
	i = _mm_cvtsd_si32 (x_128d);
	i >>= 1;

  #else

	__asm
	{
		fld            x
		fadd           st, st (0)
		fsubr          round_toward_p_i
		fistp          i
		sar            i, 1
	}

  #endif

	assert (-i == int (ceil (x)) || fabs (-i - x) < 1e-10);

	return (-i);

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

	using namespace std;
	assert (-i == int (ceil (x)) || fabs (-i - x) < 1e-10);

	return (-i);

 #else

	// Slow
	return (int (ceil (x)));

 #endif

#else

	// Slow
	return (int (ceil (x)));

#endif
}



template <class T>
int	conv_int_fast (T x)
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

	return (p);
}



template <class T>
bool	is_null (T val, T eps)
{
	static_assert (std::is_floating_point <T>::value, "T must be floating point");
	assert (eps >= 0);

	return (fabs (val) <= eps);
}



template <class T>
bool	is_eq (T v1, T v2, T eps)
{
	static_assert (std::is_floating_point <T>::value, "T must be floating point");
	assert (eps >= 0);

	return (is_null (v2 - v1, eps));
}



template <class T>
bool	is_eq_rel (T v1, T v2, T tol)
{
	static_assert (std::is_floating_point <T>::value, "T must be floating point");
	assert (tol >= 0);

	const T        v1a = T (fabs (v1));
	const T        v2a = T (fabs (v2));
	const T        eps = std::max (v1a, v2a) * tol;

	return (is_eq (v1, v2, eps));
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

int	get_prev_pow_2 (uint32_t x)
{
	assert (x > 0);

#if (fstb_ARCHI == fstb_ARCHI_X86)

 #if defined (_MSC_VER)

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

	return (int (p));

 #endif

#endif

	{
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

		return (int (p));
	}
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

int	get_next_pow_2 (uint32_t x)
{
	assert (x > 0);

#if (fstb_ARCHI == fstb_ARCHI_X86)

 #if defined (_MSC_VER)

  #if ((_MSC_VER / 100) < 14)

	int				p;
	-- x;

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

	return (int (p));

 #endif

#endif

	{
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

		return (int (p));
	}
}



double	sinc (double x)
{
	if (x == 0)
	{
		return (1);
	}

	const double   xp = x * PI;

	return (sin (xp) / xp);
}



double  pseudo_exp (double x, double c)
{
	assert (x >= 0);
	assert (c > 0);

	const double   num = exp (c * x) - 1;
	const double   den = exp (c    ) - 1;

	return (num / den);
}



double  pseudo_log (double y, double c)
{
	assert (y >= 0);
	assert (c > 0);

	const double   num = log (y * (exp (c) - 1) + 1);

	return (num / c);
}



template <class T, int S, bool L>
class fnc_ShiftGeneric
{
public:
	static_assert (S < int (sizeof (T) * CHAR_BIT), "Shift too large");
	static T sh (T x) { return (x << S); }
};
template <class T, int S>
class fnc_ShiftGeneric <T, S, false>
{
public:
	static_assert (S < int (sizeof (T) * CHAR_BIT), "Shift too large");
	static T sh (T x) { return (x >> S); }
};

template <class T, int S>
T	sshift_l (T x)
{
	static_assert (std::is_integral <T>::value, "T must be integer");
	return (fnc_ShiftGeneric <T, (S < 0) ? -S : S, (S > 0)>::sh (x));
}

template <class T, int S>
T	sshift_r (T x)
{
	static_assert (std::is_integral <T>::value, "T must be integer");
	return (fnc_ShiftGeneric <T, (S < 0) ? -S : S, (S < 0)>::sh (x));
}



// U must be a signed integer type
template <class T, class U>
T	ipow (T x, U n)
{
	const U			abs_n = std::abs (n);
	const T			z (ipowp (x, abs_n));

	return ((n < U (0)) ? T (1) / z : z);
}



// U must be an integer type (signed or not)
template <class T, class U>
T	ipowp (T x, U n)
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

	return (z);
}



template <class T>
T	rcp_uint (int x)
{
	static const int  table_len           = 256;
	static const T    rcp_arr [table_len] =
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
T	lerp (T v0, T v1, T p)
{
	return v0 + p * (v1 - v0);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T>
bool	is_ptr_align_nz (const T *ptr, int a)
{
	assert (a > 0);
	assert (is_pow_2 (a));

	return (ptr != 0 && (reinterpret_cast <intptr_t> (ptr) & (a - 1)) == 0);
}



}	// namespace fstb



#endif	// fstb_fnc_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
