/*****************************************************************************

        FltWrap.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_moog1_FltWrap_HEADER_INCLUDED)
#define mfx_pi_moog1_FltWrap_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "mfx/pi/moog1/FltInterface.h"



namespace mfx
{
namespace pi
{
namespace moog1
{



template <class F>
class FltWrap
:	public FltInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	fstb_FORCEINLINE void
	               init (float sample_freq, float max_mod_freq, float sig_scale) final;
	fstb_FORCEINLINE void
	               set_param (float freq, float reso, float gcomp) final;
	fstb_FORCEINLINE float
	               process_sample_pitch_mod (float x, float m, float stage_in_ptr []) final;
	fstb_FORCEINLINE void
	               process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) final;
	fstb_FORCEINLINE void
	               process_block_pitch_mod (float dst_ptr [], const float src_ptr [], const float mod_ptr [], int nbr_spl) final;
	fstb_FORCEINLINE void
	               clear_buffers () final;
	fstb_FORCEINLINE dsp::va::MoogLadderDAngeloData <4> &
	               use_data () final;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	F              _filter;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

}; // class FltWrap



}  // namespace moog1
}  // namespace pi
}  // namespace mfx



#include "mfx/pi/moog1/FltWrap.hpp"



#endif   // mfx_pi_moog1_FltWrap_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
