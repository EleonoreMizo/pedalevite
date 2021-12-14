/*****************************************************************************

        SlewRateLimiter.hpp
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_shape_SlewRateLimiter_CODEHEADER_INCLUDED)
#define mfx_dsp_shape_SlewRateLimiter_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace shape
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T>
void	SlewRateLimiter <T>::set_max_rate_p (T rate_max) noexcept
{
	assert (rate_max > T (0));

	_rate_max_p = rate_max;
}



template <typename T>
void	SlewRateLimiter <T>::set_max_rate_n (T rate_max) noexcept
{
	assert (rate_max > T (0));

	_rate_max_n = rate_max;
}



template <typename T>
void	SlewRateLimiter <T>::set_state (T state) noexcept
{
	_state = state;
}



template <typename T>
T	SlewRateLimiter <T>::process_sample (T x) noexcept
{
	const auto     val_min = _state - _rate_max_n;
	const auto     val_max = _state + _rate_max_p;
	_state = fstb::limit (x, val_min, val_max);

	return _state;
}



template <typename T>
void	SlewRateLimiter <T>::process_block (T dst_ptr [], const T src_ptr [], int nbr_spl) noexcept
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	float          state = _state;

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		const auto     x       = src_ptr [pos];
		const auto     val_min = state - _rate_max_n;
		const auto     val_max = state + _rate_max_p;
		state = fstb::limit (x, val_min, val_max);
		dst_ptr [pos] = state;
	}

	_state = state;
}



template <typename T>
void	SlewRateLimiter <T>::clear_buffers () noexcept
{
	_state = T (0);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_shape_SlewRateLimiter_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
