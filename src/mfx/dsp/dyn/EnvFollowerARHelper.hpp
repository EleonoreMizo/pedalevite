/*****************************************************************************

        EnvFollowerARHelper.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_dyn_EnvFollowerARHelper_CODEHEADER_INCLUDED)
#define mfx_dsp_dyn_EnvFollowerARHelper_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace dyn
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int ORD>
void	EnvFollowerARHelper <ORD>::set_atk_coef (float coef) noexcept
{
	assert (coef > 0);
	assert (coef <= 1);

	_coef_atk = coef;
}



template <int ORD>
void	EnvFollowerARHelper <ORD>::set_rls_coef (float coef) noexcept
{
	assert (coef > 0);
	assert (coef <= 1);

	_coef_rls = coef;
}



// x must be positive!
template <int ORD>
float	EnvFollowerARHelper <ORD>::process_sample (float x) noexcept
{
	for (int flt = 0; flt < ORD; ++flt)
	{
		const float    prev  = x;
		x  = _state [flt];
		const float    delta = prev - x;
		const float    coef  = (delta > 0) ? _coef_atk : _coef_rls;
		x += delta * coef;
		_state [flt] = x;
	}

	return x;
}



// src_ptr must contain only positive values!
// Can work in-place
template <int ORD>
void	EnvFollowerARHelper <ORD>::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

#if 0 // Naive way
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		dst_ptr [pos] = process_sample (src_ptr [pos]);
	}
#else
	float          state [ORD + 1];
	for (int flt = 0; flt < ORD; ++flt)
	{
		state [flt + 1] = _state [flt];
	}

	int            pos = 0;
	do
	{
		state [0] = src_ptr [pos];
		assert (state [0] >= 0);

		for (int flt = 0; flt < ORD; ++flt)
		{
			const float    delta = state [flt] - state [flt + 1];
			const float    coef  = (delta > 0) ? _coef_atk : _coef_rls;
			state [flt + 1] += delta * coef;
		}

		dst_ptr [pos] = state [ORD];

		++ pos;
	}
	while (pos < nbr_spl);

	for (int flt = 0; flt < ORD; ++flt)
	{
		_state [flt] = state [flt + 1];
	}
#endif
}



template <int ORD>
void	EnvFollowerARHelper <ORD>::clear_buffers () noexcept
{
	_state.fill (0);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace dyn
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_dyn_EnvFollowerARHelper_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
