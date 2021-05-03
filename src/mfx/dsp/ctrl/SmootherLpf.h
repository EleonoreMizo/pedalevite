/*****************************************************************************

        SmootherLpf.h
        Author: Laurent de Soras, 2020

Parameter smoothing based on a simple 1st-order low-pass filter

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_ctrl_SmootherLpf_HEADER_INCLUDED)
#define mfx_dsp_ctrl_SmootherLpf_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "mfx/dsp/iir/Lpf1p.h"



namespace mfx
{
namespace dsp
{
namespace ctrl
{



template <typename T>
class SmootherLpf
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_sample_freq (double sample_freq) noexcept;

	void           set_time (float t) noexcept;
	fstb_FORCEINLINE void
	               set_val (T x) noexcept;
	fstb_FORCEINLINE T
	               process_sample () noexcept;
	fstb_FORCEINLINE T
	               skip_block (int nbr_spl) noexcept;
	fstb_FORCEINLINE T
	               get_val_cur () const noexcept;
	fstb_FORCEINLINE T
	               get_val_tgt () const noexcept;
	void           clear_buffers () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef iir::Lpf1p <T> Lpf;

	Lpf            _lpf;
	float          _sample_freq = 0; // Hz, > 0. 0 = not set.
	T              _val_tgt {};



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const SmootherLpf &other) const = delete;
	bool           operator != (const SmootherLpf &other) const = delete;

}; // class SmootherLpf



}  // namespace ctrl
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/ctrl/SmootherLpf.hpp"



#endif   // mfx_dsp_ctrl_SmootherLpf_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
