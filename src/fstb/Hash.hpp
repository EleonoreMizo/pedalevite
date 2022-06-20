/*****************************************************************************

        Hash.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_Hash_CODEHEADER_INCLUDED)
#define fstb_Hash_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"

#include <cassert>
#include <climits>



namespace fstb
{



// Newton-Raphson iteration to find the zero of g (y) = 1 / y - x
template <typename T>
inline constexpr T	Hash_nr_int (T x, T y) noexcept
{
	return y * (2 - y * x);
}



// x must be odd
template <typename T>
inline constexpr T	Hash_find_inverse_n (T x, int n) noexcept
{
	assert ((x & T (1)) != T (0));

	T              y = x;

	for (int k = 0; k < n; ++k)
	{
		y = Hash_nr_int (x, y);
	}

	return y;
}



inline constexpr uint32_t	Hash_find_inverse (uint32_t x) noexcept
{
	return Hash_find_inverse_n (x, 4);
}



inline constexpr uint64_t	Hash_find_inverse (uint64_t x) noexcept
{
	return Hash_find_inverse_n (x, 5);
}



template <typename T>
static inline constexpr T	Hash_reverse_xor_shift (T y, int shift) noexcept
{
	constexpr auto resol    = int (CHAR_BIT * sizeof (T));
	assert (shift < resol);

	int            delta    = resol - shift;
	T              x        = (y >> delta) << delta;
	int            reversed = shift;
	delta -= shift;
	do
	{
		T              xd =
			  ((y <<  reversed         ) >> reversed)
			^ ((x << (reversed - shift)) >> reversed);
		if (delta > 0)
		{
			xd = (xd >> delta) << delta;
		}
		x        += xd;
		delta    -= shift;
		reversed += shift;
	}
	while (reversed < resol);
	
	return x;
}



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



constexpr uint32_t	Hash::hash (uint32_t x) noexcept
{
	x ^= x >> 16;
	x *= uint32_t (0x7FEB352Dlu);
	x ^= x >> 15;
	x *= uint32_t (0x846CA68Blu);
	x ^= x >> 16;
 
	return x;
}



Vu32	Hash::hash (Vu32 x) noexcept
{
	x ^= x >> 16;
	x *= uint32_t (0x7FEB352Dlu);
	x ^= x >> 15;
	x *= uint32_t (0x846CA68Blu);
	x ^= x >> 16;
 
	return x;
}



constexpr uint32_t	Hash::hash_inv (uint32_t x) noexcept
{
#if 0
	x  = Hash_reverse_xor_shift (x, 16);
	x *= uint32_t (0x43021123lu);
	x  = Hash_reverse_xor_shift (x, 15);
	x *= uint32_t (0x1D69E2A5lu);
	x  = Hash_reverse_xor_shift (x, 16);
#else
	x ^= x >> 16;
	x *= uint32_t (0x43021123lu);
	x ^= x >> 15 ^ x >> 30;
	x *= uint32_t (0x1D69E2A5lu);
	x ^= x >> 16;
#endif

	return x;
}



Vu32	Hash::hash_inv (Vu32 x) noexcept
{
	x ^= x >> 16;
	x *= uint32_t (0x43021123lu);
	x ^= x >> 15 ^ x >> 30;
	x *= uint32_t (0x1D69E2A5lu);
	x ^= x >> 16;

	return x;
}



// SplittableRandom / SplitMix64
constexpr uint64_t	Hash::hash (uint64_t x) noexcept
{
	x ^= x >> 30;
	x *= uint64_t (0xBF58476D1CE4E5B9llu);
	x ^= x >> 27;
	x *= uint64_t (0x94D049BB133111EBllu);
	x ^= x >> 31;

	return x;
}



// Source:
// https://www.vincent-lunot.com/post/playing-with-pseudo-random-number-generators-part-3/
constexpr uint64_t	Hash::hash_inv (uint64_t x) noexcept
{
	x  = Hash_reverse_xor_shift (x, 31);
	x *= Hash_find_inverse (uint64_t (0x94D049BB133111EBllu));
	x  = Hash_reverse_xor_shift (x, 27);
	x *= Hash_find_inverse (uint64_t (0xBF58476D1CE4E5B9llu));
	x  = Hash_reverse_xor_shift (x, 30);

	return x;
}



constexpr uint64_t	Hash::rrmxmx (uint64_t x) noexcept
{
	constexpr int        s = 28;
	constexpr uint64_t   m = 0x9FB21C651E98DF25ULL;

	x ^= fstb::rotr (x, 49) ^ fstb::rotr (x, 24);
	x *= m;
	x ^= x >> s;
	x *= m;
	x ^= x >> s;

	return x;
}



constexpr uint64_t	Hash::nasam (uint64_t x) noexcept
{
	x ^= fstb::rotr (x, 25) ^ fstb::rotr (x, 47);
	x *= 0x9E6C63D0676A9A99ULL;
	x ^= (x >> 23) ^ (x >> 51);
	x *= 0x9E6D62D06F6A9A9BULL;
	x ^= (x >> 23) ^ (x >> 51);

	return x;
}



constexpr uint64_t	Hash::pelican (uint64_t x) noexcept
{
	x ^= fstb::rotr (x, 23) ^ fstb::rotr (x, 47) ^ 0xD1B54A32D192ED03ULL;
	x *= 0xAEF17502108EF2D9ULL;
	x ^= (x >> 43) ^ (x >> 31) ^ (x >> 23);
	x *= 0xDB4F0B9175AE2165ULL;
	x ^= x >> 28;

	return x;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace fstb



#endif   // fstb_Hash_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
