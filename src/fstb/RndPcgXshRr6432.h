/*****************************************************************************

        RndPcgXshRr6432.h
        Authors:
		  Melissa E. O'Neill, 2014
		  Laurent de Soras, 2021

Implementation of the PCG XSH RR 64/32 (LCG), a PRNG with 32-bit output and a
64-bit state.

Source:
Melissa E. O'Neill,
PCG: A Family of Simple Fast Space-Efficient Statistically Good Algorithms for
Random Number Generation,
Harvey Mudd College, 2014
https://www.pcg-random.org/

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (fstb_RndPcgXshRr6432_HEADER_INCLUDED)
#define fstb_RndPcgXshRr6432_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cstdint>



namespace fstb
{



class RndPcgXshRr6432
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static constexpr uint64_t  _seed_def = 0x853C49E6748FEA9BULL;
	static constexpr uint64_t  _inc_def  = 0xDA3E39CB94B95BDBULL;
	static constexpr uint64_t  _mul      = 6364136223846793005ULL;

	inline void    set_seed (uint64_t s = _seed_def) noexcept;
	inline void    set_inc (uint64_t inc = _inc_def) noexcept;
	inline uint32_t
	               gen_int () noexcept;
	inline float   gen_flt () noexcept;

	// Compatibility with random generators of the standard library

	typedef uint32_t result_type;

	inline void    seed (result_type value = default_seed) noexcept;
	template <class Sseq>
	inline void    seed (Sseq &seq) noexcept;

	result_type    operator () () noexcept { return gen_int (); }
	inline void    discard (unsigned long long z) noexcept;

	static constexpr result_type
	               min () noexcept { return 0; }
	static constexpr result_type
	               max () noexcept { return 0xFFFFFFFFU; }

	static constexpr result_type
	               default_seed = 1;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	uint64_t       _state = _seed_def;
	uint64_t       _inc   = _inc_def;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const RndPcgXshRr6432 &other) const = delete;
	bool           operator != (const RndPcgXshRr6432 &other) const = delete;

}; // class RndPcgXshRr6432



}  // namespace fstb



#include "fstb/RndPcgXshRr6432.hpp"



#endif   // fstb_RndPcgXshRr6432_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
