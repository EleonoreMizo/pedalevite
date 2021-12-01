/*****************************************************************************

        BufferFiller.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (BufferFiller_CODEHEADER_INCLUDED)
#define BufferFiller_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/Hash.h"

#include <type_traits>

#include <cassert>
#include <cstdlib>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// Generates random numbers in ]-1; -0.5[ U ]0.5; 1[
template <typename T>
void	BufferFiller::gen_rnd_non_zero (T buf_ptr [], int nbr_spl)
{
	static_assert (std::is_floating_point <T>::value, "");
	assert (buf_ptr != nullptr);
	assert (nbr_spl > 0);

	constexpr auto bits = 30;
	constexpr auto scl  = uint32_t (1 << bits);
	constexpr auto mask = scl - 1;
	constexpr auto mul  = T (1) / T (scl);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		const auto     rnd = fstb::Hash::hash (uint32_t (pos));
		const auto     mag = T (0.5) + (T (rnd & mask) + T (0.5)) * mul;
		const auto     sgn = T (((rnd >> bits) & 2) - 1);
		const auto     val = mag * sgn;
		buf_ptr [pos] = val;
	}
}



// Generates random numbers in [0; 1[
template <typename T>
void	BufferFiller::gen_rnd_positive (T buf_ptr [], int nbr_spl)
{
	static_assert (std::is_floating_point <T>::value, "");
	assert (buf_ptr != nullptr);
	assert (nbr_spl > 0);

	constexpr auto bits = 30;
	constexpr auto scl  = uint32_t (1 << bits);
	constexpr auto mask = scl - 1;
	constexpr auto mul  = T (1) / T (scl);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		const auto     rnd = fstb::Hash::hash (uint32_t (pos));
		buf_ptr [pos] = T (rnd & mask) * mul;
	}
}



template <typename T>
void	BufferFiller::gen_rnd_scaled (T buf_ptr [], int nbr_spl, T min_val, T max_val)
{
	static_assert (std::is_floating_point <T>::value, "");
	assert (buf_ptr != nullptr);
	assert (nbr_spl > 0);
	assert (min_val <= max_val);

	constexpr auto bits = 30;
	constexpr auto scl  = uint32_t (1 << bits);
	constexpr auto mask = scl - 1;
	constexpr auto mul  = T (1) / T (scl);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		const auto     rnd = fstb::Hash::hash (uint32_t (pos));
		buf_ptr [pos] =
			min_val + T (rnd & mask) * T (max_val - min_val) * mul;
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



#endif   // BufferFiller_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
