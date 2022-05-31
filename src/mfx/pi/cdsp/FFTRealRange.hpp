/*****************************************************************************

        FFTRealRange.hpp
        Author: Laurent de Soras, 2022

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law.You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_cdsp_FFTRealRange_CODEHEADER_INCLUDED)
#define mfx_pi_cdsp_FFTRealRange_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace pi
{
namespace cdsp
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: preallocate
Description:
	Allocates all the FFT objects for the specified range. Further calls to
	set_length() will be real-time safe.
Throws: anything related to memory allocation
==============================================================================
*/

template <int LL2_MIN, int LL2_MAX>
void	FFTRealRange <LL2_MIN, LL2_MAX>::FFTRealRange::preallocate ()
{
	if (! _prealloc_flag)
	{
		_r.set_length (LL2_MIN, true);
		_prealloc_flag = true;
	}
}



/*
==============================================================================
Name: set_length
Description:
	Sets the FFT length. Call this function at least once before calling any
	other functions. 
Input parameters:
- ll2: Log2 of the FFT length, in [LL2_MIN ; LL2_MAX].
Throws: anything related to memory allocation if not preallocated, nothing
	otherwise.
==============================================================================
*/

template <int LL2_MIN, int LL2_MAX>
void	FFTRealRange <LL2_MIN, LL2_MAX>::set_length (int ll2) 
{
	assert (ll2 >= _ll2_min);
	assert (ll2 <= _ll2_max);

	if (! _prealloc_flag)
	{
		_r.set_length (ll2, false);
	}
	_ll2 = ll2;
}



template <int LL2_MIN, int LL2_MAX>
long	FFTRealRange <LL2_MIN, LL2_MAX>::get_length () const noexcept
{
	assert (_ll2 >= 0);

	return _r.get_length (_ll2);
}



template <int LL2_MIN, int LL2_MAX>
void	FFTRealRange <LL2_MIN, LL2_MAX>::do_fft (DataType f [], const DataType x []) noexcept
{
	assert (_ll2 >= 0);
	assert (f != nullptr);
	assert (x != nullptr);

	_r.do_fft (_ll2, f, x);
}



template <int LL2_MIN, int LL2_MAX>
void	FFTRealRange <LL2_MIN, LL2_MAX>::do_ifft (const DataType f [], DataType x []) noexcept
{
	assert (_ll2 >= 0);
	assert (f != nullptr);
	assert (x != nullptr);

	_r.do_ifft (_ll2, f, x);
}



template <int LL2_MIN, int LL2_MAX>
void	FFTRealRange <LL2_MIN, LL2_MAX>::rescale (DataType x []) const noexcept
{
	assert (_ll2 >= 0);
	assert (x != nullptr);

	_r.rescale (_ll2, x);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace cdsp
}  // namespace pi
}  // namespace mfx



#endif // mfx_pi_cdsp_FFTRealRange_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
