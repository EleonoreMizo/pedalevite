/*****************************************************************************

        FFTRealInternalWrapper.hpp
        Author: Laurent de Soras, 2022

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law.You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_cdsp_FFTRealInternalWrapper_CODEHEADER_INCLUDED)
#define mfx_pi_cdsp_FFTRealInternalWrapper_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace pi
{
namespace cdsp
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int LL2>
FFTRealInternalWrapper <LL2>::FFTRealInternalWrapper (const FFTRealInternalWrapper <LL2> &other)
{
	if (other._fft_uptr)
	{
		_fft_uptr = std::make_unique <Wrapped> (*other._fft_uptr);
	}
}



template <int LL2>
FFTRealInternalWrapper <LL2> & FFTRealInternalWrapper <LL2>::operator =	(const FFTRealInternalWrapper <LL2> &other)
{
	if (this != &other)
	{
		if (other._fft_uptr)
		{
			_fft_uptr = std::make_unique <Wrapped> (*other._fft_uptr);
		}
		else
		{
			_fft_uptr.reset ();
		}
	}

	return *this;
}



template <int LL2>
void	FFTRealInternalWrapper <LL2>::set_length (int ll2, bool force_flag)
{
	assert (ll2 >= 0);

	if (ll2 == LL2 || force_flag)
	{
		_fft_uptr = std::make_unique <Wrapped> ();
	}
	else
	{
		_fft_uptr.reset ();
	}
}



template <int LL2>
long	FFTRealInternalWrapper <LL2>::get_length (int ll2) const noexcept
{
	fstb::unused (ll2);
	assert (_fft_uptr);
	assert (ll2 >= 0);

	return _fft_uptr->get_length ();
}



template <int LL2>
void	FFTRealInternalWrapper <LL2>::do_fft (int ll2, DataType f [], const DataType x []) noexcept
{
	fstb::unused (ll2);
	assert (_fft_uptr);
	assert (ll2 >= 0);
	assert (f != nullptr);
	assert (x != nullptr);

	_fft_uptr->do_fft (f, x);
}



template <int LL2>
void	FFTRealInternalWrapper <LL2>::do_ifft (int ll2, const DataType f [], DataType x []) noexcept
{
	fstb::unused (ll2);
	assert (_fft_uptr);
	assert (ll2 >= 0);
	assert (f != nullptr);
	assert (x != nullptr);

	_fft_uptr->do_ifft (f, x);
}



template <int LL2>
void	FFTRealInternalWrapper <LL2>::rescale (int ll2, DataType x []) const noexcept
{
	fstb::unused (ll2);
	assert (_fft_uptr);
	assert (ll2 >= 0);
	assert (x != nullptr);

	_fft_uptr->rescale (x);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace cdsp
}  // namespace pi
}  // namespace mfx



#endif // mfx_pi_cdsp_FFTRealInternalWrapper_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
