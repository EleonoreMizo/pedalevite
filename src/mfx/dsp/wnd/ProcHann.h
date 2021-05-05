/*****************************************************************************

        ProcHann.h
        Author: Laurent de Soras, 2021

Direct computation of a Hann window (raised cosine), or a half-sine window
giving a Hann window once squared.

Uses a simple Reinsch sine oscillator.

Template parameters:

- T: data type, floating point

- HSFLAG: indicates the generated window is a half-sine instead of a Hann.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_wnd_ProcHann_HEADER_INCLUDED)
#define mfx_dsp_wnd_ProcHann_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"



namespace mfx
{
namespace dsp
{
namespace wnd
{



template <typename T, bool HSFLAG = false>
class ProcHann
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

	static fstb_FORCEINLINE T
	               iterate (T &u, T &v, T k);

	int            _length = 0;
	T              _k      = T (0); // Reinsch oscillator coefficient parameter
	T              _u      = T (0);
	T              _v      = T (0);
	T              _v_org  = T (0);



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const ProcHann &other) const = delete;
	bool           operator != (const ProcHann &other) const = delete;

}; // class ProcHann



}  // namespace wnd
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/wnd/ProcHann.hpp"



#endif   // mfx_dsp_wnd_ProcHann_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
