/*****************************************************************************

        FltInterface.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_moog1_FltInterface_HEADER_INCLUDED)
#define mfx_pi_moog1_FltInterface_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/va/MoogLadderDAngeloData.h"



namespace mfx
{
namespace pi
{
namespace moog1
{



class FltInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               FltInterface ()                               = default;
	               FltInterface (const FltInterface &other)      = default;
	               FltInterface (FltInterface &&other)           = default;
	virtual        ~FltInterface ()                              = default;

	virtual FltInterface &
	               operator = (const FltInterface &other)        = default;
	virtual FltInterface &
	               operator = (FltInterface &&other)             = default;

	virtual void   init (float sample_freq, float max_mod_freq, float sig_scale) = 0;
	virtual void   set_param (float freq, float reso, float gcomp) = 0;
	virtual float  process_sample_pitch_mod (float x, float m, float stage_in_ptr []) = 0;
	virtual void   process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) = 0;
	virtual void   process_block_pitch_mod (float dst_ptr [], const float src_ptr [], const float mod_ptr [], int nbr_spl) = 0;
	virtual void   clear_buffers () = 0;
	virtual dsp::va::MoogLadderDAngeloData <4> &
	               use_data () = 0;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



}; // class FltInterface



}  // namespace moog1
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/moog1/FltInterface.hpp"



#endif   // mfx_pi_moog1_FltInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
