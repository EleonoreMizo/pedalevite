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



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace detail
{



template <int OFFSET>
int	get_prev_pow_2_internal (uint32_t x) noexcept
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

	p += OFFSET;

#elif (fstb_COMPILER == fstb_COMPILER_GCC)

	const auto     p = 31 + OFFSET - __builtin_clz (x);

#else

	int            p = OFFSET - 1;

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



// C++14 implementation for constexpr LUT
// source: https://stackoverflow.com/a/56207376
template <typename T>
constexpr T	rcpi_check_null (std::size_t x)
{
	if (x == 0)
	{
		return T (0);
	}
	return T (1) / T (x);
}

template <typename T, std::size_t... I>
constexpr auto	rcpi_lookup_helper (std::index_sequence <I...>)
{
	return std::array <T, sizeof... (I)> ({ rcpi_check_null <T> (I)... });
}

template <typename T, size_t N>
constexpr auto	rcpi_lookup ()
{
	return rcpi_lookup_helper <T> (std::make_index_sequence <N> ());
}



template <typename T, std::size_t... IS>
constexpr std::array <T, sizeof... (IS)>	make_array (const T &init_val, std::index_sequence <IS...>)
{
	return {{ (static_cast <void> (IS), init_val)... }};
}



} // namespace detail



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



/*
==============================================================================
Name: divmod_floor
Description:
	Integer division and remainder, rounds towards -oo (instead of towards 0).
	The remainder is always of the same sign as den.
	Examples:
	 23,  4 =>  5,  3
	-23,  4 => -6,  1
	 23, -4 => -6, -1
	-23, -4 =>  5, -3
	Result is undefined if not representable. Ex: (INT_MIN, -1)
Input parameters:
- num: dividend
- den: divisor, != 0
Returns: { quotient, remainder }
Throws: Nothing
==============================================================================
*/

template <class T>
constexpr std::array <T, 2>	divmod_floor (T num, T den) noexcept
{
	static_assert (std::is_integral <T>::value, "T must be an integer");
	assert (den != T (0));

	auto           d = num / den;
	auto           r = num % den;
	if (r != T (0) && (num < T (0)) != (den < T (0)))
	{
		r += den;
		d -= T (1);
	}

	return { d, r };
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



// Equality test with a tolerance in ULP.
// Numbers of opposite sign (excepted 0) are always evaluated as different.
// https://en.wikipedia.org/wiki/Unit_in_the_last_place
constexpr bool	is_eq_ulp (float v1, float v2, int32_t tol) noexcept
{
	assert (tol >= 0);

	if ((v1 < 0) != (v2 < 0))
	{
		return (v1 == v2);
	}

	union Combo
	{
		float          _f;
		int32_t        _i;
	};
	const Combo    c1 { v1 };
	const Combo    c2 { v2 };
	const auto     dif = std::abs (c2._i - c1._i);

	return (dif <= tol);
}



/*
==============================================================================
Name: get_prev_pow_2
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
	const auto     p = detail::get_prev_pow_2_internal <0> (x);
	assert ((uint64_t (1) << p      ) <= uint64_t (x));
	assert ((uint64_t (1) << (p + 1)) >  uint64_t (x));

	return p;
}



/*
==============================================================================
Name: get_next_pow_2
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

	const auto     x_org = x;
	fstb::unused (x_org);

	-- x;
	if (x == 0)
	{
		return 0;
	}

	const auto     p = detail::get_prev_pow_2_internal <1> (x);
	assert (p > 0);
	assert ((uint64_t (1) <<  p     ) >= uint64_t (x_org));
	assert ((uint64_t (1) << (p - 1)) <  uint64_t (x_org));

	return p;
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

	constexpr int  table_len = 256;
	constexpr auto rcp_arr   = detail::rcpi_lookup <T, table_len> ();

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
// The value at this point is y = r2 + 0.25 * x * (r3 - r1)
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
	constexpr auto bitdepth = int (sizeof (T) * CHAR_BIT);
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
	constexpr auto bitdepth = int (sizeof (T) * CHAR_BIT);
	assert (k >= 0);
	assert (k < bitdepth);

	return (x >> k) | (x << (bitdepth - k));
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



}	// namespace fstb



#endif	// fstb_fnc_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
