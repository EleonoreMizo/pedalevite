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

#include <cassert>



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



}  // namespace fstb



#endif   // fstb_RndXoroshiro128p_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
