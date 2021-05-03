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

Compensation formulas taken from:
Stephano D'Angelo, Vesa Valimaki,
Generalized Moog Ladder Filter: Part 1 - Linear Analysis and Parameterization,
IEEE Transactions on Audio, Speech and Language Processing,
vol. 22, no. 12, pp. 1825-1832, December 2014

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

	void           set_sample_freq (double sample_freq) noexcept;

	void           set_freq (float freq) noexcept;
	void           set_reso (float reso) noexcept;
	void           set_gain_comp (float gc) noexcept;
	void           set_freq_comp (bool comp_flag) noexcept;
	float          process_sample (float x) noexcept;
	float          process_sample (float x, float stage_in_ptr [4]) noexcept;
	float          process_sample_pitch_mod (float x, float m) noexcept;
	float          process_sample_pitch_mod (float x, float m, float stage_in_ptr [4]) noexcept;
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept;
	void           process_block_pitch_mod (float dst_ptr [], const float src_ptr [], const float mod_ptr [], int nbr_spl) noexcept;
	void           clear_buffers () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::array <float, 4> StateArray;

	fstb_FORCEINLINE float
	               process_sample_internal (float x, float f, float &xx, float &y0, float &y1, float &y2) noexcept;
	inline void    update_gaincomp () noexcept;
	inline void    update_cutoff () noexcept;
	float          compute_alpha (float k) const noexcept;

	static inline constexpr float
	               tanh_xdx (float x) noexcept;

	float          _sample_freq = 0;    // Sampling rate, Hz. 0 = not set
	float          _piofs       = 0;    // pi / fs. 0 = not set

	float          _fc          = 1000; // Cutoff frequency, Hz. ]0 ; _sample_freq * 0.5[
	float          _reso        = 0;    // Normalized resonance. >= 1: self-oscillations
	float          _gaincomp    = 0;    // Gain compensation at DC, from 0 = none to 1 = full
	bool           _freq_comp_flag = true; // true = makes the cutoff frequency independant of the resonance

	bool           _dirty_flag  = true;
	float          _f           = 1;    // Frequency coefficient
	float          _fi          = 0;    // Frequency increment amplitude, used for fast FM
	float          _r           = 0;    // Raw resonance, >= 0. 4 = self osc.
	float          _gc_mul      = 1;    // Output multiplier for the gain compensation
	float          _alpha       = compute_alpha (_r);
	float          _alpha_inv   = 1.f / _alpha; // Frequency multiplier for the frequency compensation

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

