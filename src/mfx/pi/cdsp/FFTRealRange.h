/*****************************************************************************

        FFTRealRange.h
        Author: Laurent de Soras, 2022

Wrapper around ffft::FFTRealFixLen allowing to select different lengths at
runtime.

Call set_length() at least once before using any of the wrapped functions.

Template parameters:

- LL2_MIN, LL2_MAX: log2 of the minimum and maximum lengths. Same remarks as
	for ffft::FFTRealFixLen LL2 template parameter.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law.You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_cdsp_FFTRealRange_HEADER_INCLUDED)
#define mfx_pi_cdsp_FFTRealRange_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/pi/cdsp/FFTRealInternalRange.h"



namespace mfx
{
namespace pi
{
namespace cdsp
{



template <int LL2_MIN, int LL2_MAX>
class FFTRealRange
{
	static_assert (LL2_MIN >= 0, "");
	static_assert (LL2_MIN <= LL2_MAX, "");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef typename ffft::FFTRealFixLen <LL2_MIN>::DataType DataType;

	void           preallocate ();
	void           set_length (int ll2);

	inline long    get_length () const noexcept;
	inline void    do_fft (DataType f [], const DataType x []) noexcept;
	inline void    do_ifft (const DataType f [], DataType x []) noexcept;
	inline void    rescale (DataType x []) const noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static constexpr int _ll2_min = LL2_MIN;
	static constexpr int _ll2_max = LL2_MAX;

	FFTRealInternalRange <_ll2_min, _ll2_max>
	               _r;

	int            _ll2 = -1; // Negative: not set (invalid).
	bool           _prealloc_flag = false;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const FFTRealRange &other) const = delete;
	bool           operator != (const FFTRealRange &other) const = delete;

}; // class FFTRealRange



}  // namespace cdsp
}  // namespace pi
}  // namespace mfx



#include "mfx/pi/cdsp/FFTRealRange.hpp"



#endif // mfx_pi_cdsp_FFTRealRange_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
