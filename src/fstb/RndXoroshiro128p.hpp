/*****************************************************************************

        RndXoroshiro128p.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_RndXoroshiro128p_CODEHEADER_INCLUDED)
#define fstb_RndXoroshiro128p_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/fnc.h"

#include <array>

#include <cassert>
#include <climits>



namespace fstb
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	RndXoroshiro128p::set_seed (uint64_t s0, uint64_t s1) noexcept
{
   assert (s0 != 0 || s1 != 0);

   _s [0] = s0;
   _s [1] = s1;
}



uint64_t	RndXoroshiro128p::gen_int () noexcept
{
	const uint64_t s0  = _s [0];
	uint64_t       s1  = _s [1];
	const uint64_t res = s0 + s1;

	s1    ^= s0;
	_s [0] = fstb::rotl (s0, _a) ^ s1 ^ (s1 << _b);
	_s [1] = fstb::rotl (s1, _c);

	return res;
}



float	RndXoroshiro128p::gen_flt () noexcept
{
	return float (gen_int ()) * fstb::TWOPM64;
}



/*
This is the jump function for the generator. It is equivalent
to 2^64 calls to gen_*(); it can be used to generate 2^64
non-overlapping subsequences for parallel computations.
*/

void	RndXoroshiro128p::jump_2_64 () noexcept
{
	static constexpr Storage jump_norm {{
		0xDF900294D8F554A5, 0x170865DF4B3201FC
	}};
	compute_jump (jump_norm);
}



/*
This is the long-jump function for the generator. It is equivalent to
2^96 calls to gen_*(); it can be used to generate 2^32 starting points,
from each of which jump_2_64() will generate 2^32 non-overlapping
subsequences for parallel distributed computations.
*/

void	RndXoroshiro128p::jump_2_96 () noexcept
{
	static constexpr Storage jump_long {{
		0xD2A98B26625EEE7B, 0xDDDF9B1090AA7AC1
	}};
	compute_jump (jump_long);
}



void	RndXoroshiro128p::seed (result_type value) noexcept
{
	const auto     s0 = splitmix64 (value);
	const auto     s1 = splitmix64 (value);
	set_seed (s0, s1);
}



template <class Sseq>
void	RndXoroshiro128p::seed (Sseq &seq) noexcept
{
	std::array <typename Sseq::result_type, 4> sval_32;
	seq.generate (sval_32.begin (), sval_32.end ());
	const auto     s0 = sval_32 [0] + (uint64_t (sval_32 [1]) << 32);
	const auto     s1 = sval_32 [2] + (uint64_t (sval_32 [3]) << 32);
	set_seed (s0, s1);
}



// Naive implementation. May take ages before returning.
void	RndXoroshiro128p::discard (unsigned long long z) noexcept
{
#if ((ULLONG_MAX >> 32) >> 32) > 0
	constexpr auto step_96 = 1ULL << 96;
	while (z >= step_96)
	{
		jump_2_96 ();
		z -= step_96;
	}

	constexpr auto step_64 = 1ULL << 64;
	while (z >= step_64)
	{
		jump_2_64 ();
		z -= step_64;
	}
#endif

	// Not an error, but given the current implementation, this would be
	// way too long to complete.
	assert (z <= 10'000'000'000ULL);

	while (z > 0)
	{
		gen_int ();
		-- z;
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	RndXoroshiro128p::compute_jump (const Storage &jump) noexcept
{
	uint64_t       s0 = 0;
	uint64_t       s1 = 0;

	for (int i = 0; i < int (jump.size ()); ++i)
	{
		for (int b = 0; b < 64; ++b)
		{
			if ((jump [i] & (uint64_t (1) << b)) != 0)
			{
				s0 ^= _s [0];
				s1 ^= _s [1];
			}
			gen_int ();
		}
	}

	_s [0] = s0;
	_s [1] = s1;
}



// https://xorshift.di.unimi.it/splitmix64.c
uint64_t	RndXoroshiro128p::splitmix64 (uint64_t &state) noexcept
{
	state += 0x9E3779B97f4A7C15ULL;
	auto           z = state;
	z ^= z >> 30;
	z *= 0xBF58476D1CE4E5B9ULL;
	z ^= z >> 27;
	z *= 0x94D049BB133111EBULL;
	z ^= z >> 31;

	return z;
}



}  // namespace fstb



#endif   // fstb_RndXoroshiro128p_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
