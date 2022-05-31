/*****************************************************************************

        FFTRealInternalWrapper.h
        Author: Laurent de Soras, 2022

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law.You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_cdsp_FFTRealInternalWrapper_HEADER_INCLUDED)
#define mfx_pi_cdsp_FFTRealInternalWrapper_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "ffft/FFTRealFixLen.h"

#include <memory>



namespace mfx
{
namespace pi
{
namespace cdsp
{



template <int LL2>
class FFTRealInternalWrapper
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef ffft::FFTRealFixLen <LL2> Wrapped;
	typedef typename Wrapped::DataType DataType;

	               FFTRealInternalWrapper () = default;
	               FFTRealInternalWrapper (const FFTRealInternalWrapper &other);
	               FFTRealInternalWrapper (FFTRealInternalWrapper &&other) = default;
	FFTRealInternalWrapper &
	               operator = (const FFTRealInternalWrapper &other);
	FFTRealInternalWrapper &
	               operator = (FFTRealInternalWrapper &&other) = default;

	void           set_length (int ll2, bool force_flag);

	inline long    get_length (int ll2) const noexcept;
	inline void    do_fft (int ll2, DataType f [], const DataType x []) noexcept;
	inline void    do_ifft (int ll2, const DataType f [], DataType x []) noexcept;
	inline void    rescale (int ll2, DataType x []) const noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	std::unique_ptr <Wrapped>
	               _fft_uptr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const FFTRealInternalWrapper &other) const = delete;
	bool           operator != (const FFTRealInternalWrapper &other) const = delete;

}; // class FFTRealInternalWrapper



}  // namespace cdsp
}  // namespace pi
}  // namespace mfx



#include "mfx/pi/cdsp/FFTRealInternalWrapper.hpp"



#endif // mfx_pi_cdsp_FFTRealInternalWrapper_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
