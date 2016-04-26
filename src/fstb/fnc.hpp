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

#include "def.h"

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



double	sinc (double x)
{
	if (x == 0)
	{
		return (1);
	}

	const double   xp = x * PI;

	return (sin (xp) / xp);
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
