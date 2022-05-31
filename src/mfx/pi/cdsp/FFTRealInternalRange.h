/*****************************************************************************

        FFTRealInternalRange.h
        Author: Laurent de Soras, 2022

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law.You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_cdsp_FFTRealInternalRange_HEADER_INCLUDED)
#define mfx_pi_cdsp_FFTRealInternalRange_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/pi/cdsp/FFTRealInternalWrapper.h"

#include <type_traits>



namespace mfx
{
namespace pi
{
namespace cdsp
{



template <int LL2_MIN, int LL2_MAX>
class FFTRealInternalRange
{
	static_assert (LL2_MIN >= 0, "");
	static_assert (LL2_MIN <= LL2_MAX, "");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef typename ffft::FFTRealFixLen <LL2_MIN>::DataType DataType;

	void           set_length (int ll2, bool force_flag);

	inline long    get_length (int ll2) const noexcept;
	inline void    do_fft (int ll2, DataType f [], const DataType x []) noexcept;
	inline void    do_ifft (int ll2, const DataType f [], DataType x []) noexcept;
	inline void    rescale (int ll2, DataType x []) const noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static constexpr int _ll2_min = LL2_MIN;
	static constexpr int _ll2_max = LL2_MAX;
	static constexpr int _ll2_mid = (_ll2_min + _ll2_max) / 2;

	template <int LB, int UB>
	using Part = typename std::conditional <
		(LB < UB),
		FFTRealInternalRange <LB, UB>,
		FFTRealInternalWrapper <UB>
	>::type;

	Part <_ll2_min, _ll2_mid>
	               _r_lo;
	Part <_ll2_mid + 1, _ll2_max>
	               _r_hi;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const FFTRealInternalRange &other) const = delete;
	bool           operator != (const FFTRealInternalRange &other) const = delete;

}; // class FFTRealInternalRange



}  // namespace cdsp
}  // namespace pi
}  // namespace mfx



#include "mfx/pi/cdsp/FFTRealInternalRange.hpp"



#endif // mfx_pi_cdsp_FFTRealInternalRange_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
