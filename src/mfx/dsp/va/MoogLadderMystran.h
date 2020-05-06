/*****************************************************************************

        MoogLadderMystran.h
        Author: Laurent de Soras, 2020

Algorithm by Teemu Voipio (mystran)
https://www.kvraudio.com/forum/viewtopic.php?p=4925309#p4925309

Original notice:

//// LICENSE TERMS: Copyright 2012 Teemu Voipio
// 
// You can use this however you like for pretty much any purpose,
// as long as you don't claim you wrote it. There is no warranty.
//
// Distribution of substantial portions of this code in source form
// must include this copyright notice and list of conditions.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_va_MoogLadderMystran_HEADER_INCLUDED)
#define mfx_dsp_va_MoogLadderMystran_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#include <array>



namespace mfx
{
namespace dsp
{
namespace va
{



class MoogLadderMystran
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _max_reso = 4;

	void           set_sample_freq (double sample_freq);

	void           set_freq (float freq);
	void           set_reso (float reso);
	void           set_gain_comp (float gc);
	float          process_sample (float x);
	float          process_sample (float x, float stage_in_ptr [4]);
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl);
	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::array <float, 4> StateArray;

	fstb_FORCEINLINE float
	               process_sample_internal (float x, float &xx, float &y0, float &y1, float &y2);
	inline void    update_gaincomp ();

	static inline float
	               tanh_xdx (float x);

	float          _sample_freq = 0;    // Sampling rate, Hz. 0 = not set
	float          _piofs       = 0;    // pi / fs. 0 = not set

	float          _fc          = 1000; // Cutoff frequency, Hz. ]0 ; _sample_freq * 0.5[
	float          _reso        = 0;    // Normalized resonance. >= 1: self-oscillations
	float          _gaincomp    = 0;    // Gain compensation at DC, from 0 = none to 1 = full

	float          _f           = 1;
	float          _r           = 0;
	float          _gc_mul      = 1;

	// State
	float          _zi          = 0;
	StateArray     _s_arr       = StateArray {{ 0, 0, 0, 0 }};



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const MoogLadderMystran &other) const = delete;
	bool           operator != (const MoogLadderMystran &other) const = delete;

}; // class MoogLadderMystran



}  // namespace va
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/va/MoogLadderMystran.hpp"



#endif   // mfx_dsp_va_MoogLadderMystran_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

