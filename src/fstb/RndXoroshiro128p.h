/*****************************************************************************

        RndXoroshiro128p.h
        Authors:
        David Blackman and Sebastiano Vigna, 2016-2018
        Laurent de Soras, 2020

This is xoroshiro128+ 1.0, our best and fastest small-state generator
for floating-point numbers. We suggest to use its upper bits for
floating-point generation, as it is slightly faster than
xoroshiro128++/xoroshiro128**. It passes all tests we are aware of
except for the four lower bits, which might fail linearity tests (and
just those), so if low linear complexity is not considered an issue (as
it is usually the case) it can be used to generate 64-bit outputs, too;
moreover, this generator has a very mild Hamming-weight dependency
making our test (http://prng.di.unimi.it/hwd.php) fail after 5 TB of
output; we believe this slight bias cannot affect any application. If
you are concerned, use xoroshiro128++, xoroshiro128** or xoshiro256+.

We suggest to use a sign test to extract a random Boolean value, and
right shifts to extract subsets of bits.

The state must be seeded so that it is not everywhere zero. If you have
a 64-bit seed, we suggest to seed a splitmix64 generator and use its
output to fill s. 

NOTE: the parameters (a=24, b=16, b=37) of this version give slightly
better results in our test than the 2016 version (a=55, b=14, c=36).

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (fstb_RndXoroshiro128p_HEADER_INCLUDED)
#define fstb_RndXoroshiro128p_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <array>

#include <cstdint>



namespace fstb
{



class RndXoroshiro128p
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static constexpr uint64_t  _seed_0_def = 0x0123456789ABCDEFULL;
	static constexpr uint64_t  _seed_1_def = 0xB50A4F93E82D71C6ULL;

	inline void    set_seed (uint64_t s0 = _seed_0_def, uint64_t s1 = _seed_1_def) noexcept;
	inline uint64_t
	               gen_int () noexcept;
	inline float   gen_flt () noexcept;
	inline void    jump_2_64 () noexcept;
	inline void    jump_2_96 () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static constexpr int _a = 24;
	static constexpr int _b = 16;
	static constexpr int _c = 37;

	typedef std::array <uint64_t, 2> Storage;

	inline void    compute_jump (const Storage &jump) noexcept;

	Storage        _s {{ _seed_0_def, _seed_1_def }};



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const RndXoroshiro128p &other) const = delete;
	bool           operator != (const RndXoroshiro128p &other) const = delete;

}; // class RndXoroshiro128p



}  // namespace fstb



#include "fstb/RndXoroshiro128p.hpp"



#endif   // fstb_RndXoroshiro128p_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

