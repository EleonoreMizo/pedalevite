/*****************************************************************************

        Poly.hpp
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_Poly_CODEHEADER_INCLUDED)
#define fstb_Poly_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace fstb
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class T>
constexpr T	Poly::horner (T x, T c0, T c1) noexcept
{
	return fma (c1, x, c0);
}

template <class T>
constexpr T	Poly::horner (T x, T c0, T c1, T c2) noexcept
{
	return horner (x, c0, fma (c2, x, c1));
}

template <class T>
constexpr T	Poly::horner (T x, T c0, T c1, T c2, T c3) noexcept
{
	return horner (x, c0, c1, fma (c3, x, c2));
}

template <class T>
constexpr T	Poly::horner (T x, T c0, T c1, T c2, T c3, T c4) noexcept
{
	return horner (x, c0, c1, c2, fma (c4, x, c3));
}

template <class T>
constexpr T	Poly::horner (T x, T c0, T c1, T c2, T c3, T c4, T c5) noexcept
{
	return horner (x, c0, c1, c2, c3, fma (c5, x, c4));
}

template <class T>
constexpr T	Poly::horner (T x, T c0, T c1, T c2, T c3, T c4, T c5, T c6) noexcept
{
	return horner (x, c0, c1, c2, c3, c4, fma (c6, x, c5));
}

template <class T>
constexpr T	Poly::horner (T x, T c0, T c1, T c2, T c3, T c4, T c5, T c6, T c7) noexcept
{
	return horner (x, c0, c1, c2, c3, c4, c5, fma (c7, x, c6));
}



template <class T, int N>
constexpr T	Poly::horner (T x, const T (&c) [N]) noexcept
{
	if (N == 0)
	{
		return T (0);
	}

	auto           r = c [N - 1];
	for (int i = N - 2; i >= 0; --i)
	{
		r = fma (x, r, c [i]);
	}

	return r;
}



// Estrin evaluation is slightly less precise than Horner.
// Speed improvement starts with 6 coefficients (5 on ARM).
template <class T>
constexpr T	Poly::estrin (T x, T c0, T c1, T c2, T c3) noexcept
{
	const auto     x2 = x * x;
	return fma (x2, fma (x, c3, c2), fma (x, c1, c0));
}

template <class T>
constexpr T	Poly::estrin (T x, T c0, T c1, T c2, T c3, T c4) noexcept
{
	const auto     x2 = x  * x;
	const auto     x4 = x2 * x2;
	return fma (
		x4,
		c4,
		fma (x2, fma (x, c3, c2), fma (x, c1, c0))
	);
}

template <class T>
constexpr T	Poly::estrin (T x, T c0, T c1, T c2, T c3, T c4, T c5) noexcept
{
	const auto     x2 = x  * x;
	const auto     x4 = x2 * x2;
	return fma (
		x4,
		fma (x, c5, c4),
		fma (x2, fma (x, c3, c2), fma (x, c1, c0))
	);
}

template <class T>
constexpr T	Poly::estrin (T x, T c0, T c1, T c2, T c3, T c4, T c5, T c6) noexcept
{
	const auto     x2 = x  * x;
	const auto     x4 = x2 * x2;
	return fma (
		x4,
		fma (x2, c6, fma (x, c5, c4)),
		fma (x2, fma (x, c3, c2), fma (x, c1, c0))
	);
}

template <class T>
constexpr T	Poly::estrin (T x, T c0, T c1, T c2, T c3, T c4, T c5, T c6, T c7) noexcept
{
	const auto     x2 = x  * x;
	const auto     x4 = x2 * x2;
	return fma (
		x4,
		fma (x2, fma (x, c7, c6), fma (x, c5, c4)),
		fma (x2, fma (x, c3, c2), fma (x, c1, c0))
	);
}



// Usually compiles to optimal code with -O3
// Based on a function by Vortico
// https://discord.com/channels/507604115854065674/507630527847596046/1073525377483419689
template <class T, int N>
constexpr T	Poly::estrin (T x, const T (&c) [N]) noexcept
{
	if (N == 0)
	{
		return T (0);
	}
	else if (N == 1)
	{
		return c [0];
	}

	constexpr auto M = (N + 1) / 2;
	T              b [M];
	for (int i = 0; i < M; ++i)
	{
		const auto     idx_even = 2 * i;
		b [i] = c [idx_even];
		const auto     idx_odd  = idx_even + 1;
		if (idx_odd < N)
		{
			b [i] = fma (x, c [idx_odd], b [i]);
		}
	}

	return estrin (x * x, b);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// We don't use std::fma for scalars, as it could be very slow, depending on
// the platform.
template <class T>
constexpr T	Poly::fma (T x, T a, T b) noexcept
{
	return x * a + b;
}



}  // namespace fstb



#endif   // fstb_Poly_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
