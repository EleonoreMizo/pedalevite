/*****************************************************************************

        RndPcgXshRr6432.hpp
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_RndPcgXshRr6432_CODEHEADER_INCLUDED)
#define fstb_RndPcgXshRr6432_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/fnc.h"

#include <array>



namespace fstb
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	RndPcgXshRr6432::set_seed (uint64_t s) noexcept
{
   _state = s;
}



void	RndPcgXshRr6432::set_inc (uint64_t inc) noexcept
{
   _inc = inc;
}



uint32_t	RndPcgXshRr6432::gen_int () noexcept
{
	const auto     s   = _state;

	_state = s * _mul + _inc;

	const auto     xsh = uint32_t (((s >> 18U) ^ s) >> 27U);
	const auto     rot = int (s >> 59U);
	const auto     res = fstb::rotr (xsh, rot);

	return res;
}



float	RndPcgXshRr6432::gen_flt () noexcept
{
	return float (gen_int ()) * fstb::TWOPM32;
}


void	RndPcgXshRr6432::seed (result_type value) noexcept
{
	_state = value;
}



template <class Sseq>
void	RndPcgXshRr6432::seed (Sseq &seq) noexcept
{
	std::array <typename Sseq::result_type, 2> sval_32;
	seq.generate (sval_32.begin (), sval_32.end ());
	const auto     s = sval_32 [0] + (uint64_t (sval_32 [1]) << 32);
	set_seed (s);
}



/*
Multi-step advance functions (jump-ahead, jump-back)

The method used here is based on Brown, "Random Number Generation
with Arbitrary Stride,", Transactions of the American Nuclear
Society (Nov. 1994).  The algorithm is very similar to fast
exponentiation.

Even though z is an unsigned integer, we can pass a
signed integer to go backwards, it just goes "the long way round".
*/

void	RndPcgXshRr6432::discard (unsigned long long z) noexcept
{
	uint64_t       cur_mult = _mul;
	uint64_t       cur_plus = _inc;
	uint64_t       acc_mult = 1;
	uint64_t       acc_plus = 0;

	while (z > 0)
	{
		if ((z & 1) != 0)
		{
			acc_mult *= cur_mult;
			acc_plus *= cur_mult;
			acc_plus += cur_plus;
		}
		cur_plus *= cur_mult + 1;
		cur_mult *= cur_mult;
		z >>= 1;
	}

	_state *= acc_mult;
	_state += acc_plus;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace fstb



#endif   // fstb_RndPcgXshRr6432_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
