/*****************************************************************************

        ProcHalfSine.h
        Author: Laurent de Soras, 2021

Direct computation of a half-sine window, giving a Hann window (raised cosine)
once squared.

Uses a simple biquad sine oscillator.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_wnd_ProcHalfSine_HEADER_INCLUDED)
#define mfx_dsp_wnd_ProcHalfSine_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"



namespace mfx
{
namespace dsp
{
namespace wnd
{



template <typename T>
class ProcHalfSine
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef T DataType;

	void           setup (int length) noexcept;
	inline void    start () noexcept;
	inline T       process_sample () noexcept;

	void           process_frame_mul (T spl_ptr []) const noexcept;
	void           process_frame_mul (T * fstb_RESTRICT dst_ptr, const T * fstb_RESTRICT src_ptr) const noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	int            _length = 0;
	T              _k      = T (0); // Biquad oscillator coefficient parameter
	T              _z1     = T (0);
	T              _z2     = T (0);
	T              _z2_org = T (0);



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const ProcHalfSine &other) const = delete;
	bool           operator != (const ProcHalfSine &other) const = delete;

}; // class ProcHalfSine



}  // namespace wnd
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/wnd/ProcHalfSine.hpp"



#endif   // mfx_dsp_wnd_ProcHalfSine_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
