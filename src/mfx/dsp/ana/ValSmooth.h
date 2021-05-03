/*****************************************************************************

        ValSmooth.h
        Author: Laurent de Soras, 2019

Template parameters:

- T: floating point type

- NV: neutral/unknown/reset value

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_ana_ValSmooth_HEADER_INCLUDED)
#define mfx_dsp_ana_ValSmooth_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace ana
{



template <typename T, int NV>
class ValSmooth
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef T ValType;

	inline void    reset (T x = T (NV)) noexcept;
	inline T       proc_val (T x) noexcept;
	inline T       get_val () const noexcept;
	inline T       get_raw_val () const noexcept;
	inline void    clear_buffers () noexcept;
	inline void    set_responsiveness (T resp) noexcept;
	inline void    set_threshold (T thr) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	ValType        _val_last = ValType (NV);
	ValType        _val_smth = ValType (NV);
	ValType        _resp     = ValType (0.125); // Smoothing coefficient (LERP between the new and accumulated values)
	ValType        _thr      = ValType (0.02 ); // Smoothing is done only on close values (threshold is relative)



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const ValSmooth &other) const = delete;
	bool           operator != (const ValSmooth &other) const = delete;

}; // class ValSmooth



}  // namespace ana
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/ana/ValSmooth.hpp"



#endif   // mfx_dsp_ana_ValSmooth_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
