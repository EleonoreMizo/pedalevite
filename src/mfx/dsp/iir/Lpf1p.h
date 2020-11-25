/*****************************************************************************

        Lpf1p.h
        Author: Laurent de Soras, 2020

Simple smoothing low-pass filter with one pole and no zero.

Equation:
y += c * (x - y)

Template parameters:

- T: processed data type, floating point

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_iir_Lpf1p_HEADER_INCLUDED)
#define mfx_dsp_iir_Lpf1p_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"



namespace mfx
{
namespace dsp
{
namespace iir
{



template <typename T>
class Lpf1p
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef T DataType;

	fstb_FORCEINLINE void
	               set_coef (T c);
	fstb_FORCEINLINE T
	               process_sample (T x);
	fstb_FORCEINLINE T &
	               use_state ();
	fstb_FORCEINLINE const T &
	               use_state () const;
	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	T              _coef { 1.f };
	T              _mem_y {};



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Lpf1p &other) const = delete;
	bool           operator != (const Lpf1p &other) const = delete;

}; // class Lpf1p



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/iir/Lpf1p.hpp"



#endif   // mfx_dsp_iir_Lpf1p_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
