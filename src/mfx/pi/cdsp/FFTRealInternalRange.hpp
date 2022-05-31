/*****************************************************************************

        FFTRealInternalRange.hpp
        Author: Laurent de Soras, 2022

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law.You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_cdsp_FFTRealInternalRange_CODEHEADER_INCLUDED)
#define mfx_pi_cdsp_FFTRealInternalRange_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace pi
{
namespace cdsp
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int LL2_MIN, int LL2_MAX>
void	FFTRealInternalRange <LL2_MIN, LL2_MAX>::set_length (int ll2, bool force_flag) 
{
	assert (ll2 >= 0);

	_r_lo.set_length (ll2, force_flag);
	_r_hi.set_length (ll2, force_flag);
}



template <int LL2_MIN, int LL2_MAX>
long	FFTRealInternalRange <LL2_MIN, LL2_MAX>::get_length (int ll2) const noexcept
{
	assert (ll2 >= 0);

	if (ll2 <= _ll2_mid)
	{
		return _r_lo.get_length (ll2);
	}

	return _r_hi.get_length (ll2);
}



template <int LL2_MIN, int LL2_MAX>
void	FFTRealInternalRange <LL2_MIN, LL2_MAX>::do_fft (int ll2, DataType f [], const DataType x []) noexcept
{
	assert (ll2 >= 0);
	assert (f != nullptr);
	assert (x != nullptr);

	if (ll2 <= _ll2_mid)
	{
		_r_lo.do_fft (ll2, f, x);
	}
	else
	{
		_r_hi.do_fft (ll2, f, x);
	}
}



template <int LL2_MIN, int LL2_MAX>
void	FFTRealInternalRange <LL2_MIN, LL2_MAX>::do_ifft (int ll2, const DataType f [], DataType x []) noexcept
{
	assert (ll2 >= 0);
	assert (f != nullptr);
	assert (x != nullptr);

	if (ll2 <= _ll2_mid)
	{
		_r_lo.do_ifft (ll2, f, x);
	}
	else
	{
		_r_hi.do_ifft (ll2, f, x);
	}
}



template <int LL2_MIN, int LL2_MAX>
void	FFTRealInternalRange <LL2_MIN, LL2_MAX>::rescale (int ll2, DataType x []) const noexcept
{
	assert (ll2 >= 0);
	assert (x != nullptr);

	if (ll2 <= _ll2_mid)
	{
		_r_lo.rescale (ll2, x);
	}
	else
	{
		_r_hi.rescale (ll2, x);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace cdsp
}  // namespace pi
}  // namespace mfx



#endif // mfx_pi_cdsp_FFTRealInternalRange_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
