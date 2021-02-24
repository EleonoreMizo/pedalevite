/*****************************************************************************

        SplitThiele8.h
        Author: Laurent de Soras, 2021

8th-order crossover Thiele filter implemented with 4x 2-pole SVFs.

Implementation reference:
Andrew Simper, Teemu Voipio, Vadim Zavalishin,
https://www.kvraudio.com/forum/viewtopic.php?p=7017185#p7017185

Thiele filters:
Neville Thiele, Loudspeaker Crossovers with Notched Response,
Journal of the Audio Engineering Society, vol. 48, no. 9, pp. 786-799,
2000-09

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_iir_SplitThiele8_HEADER_INCLUDED)
#define mfx_dsp_iir_SplitThiele8_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/iir/SvfCore.h"
#include "mfx/dsp/iir/SvfMixerLow.h"
#include "mfx/dsp/iir/SvfMixerPeak.h"

#include <array>



namespace mfx
{
namespace dsp
{
namespace iir
{



class SplitThiele8
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_sample_freq (double sample_freq);
	void           set_split_freq (float f);
	void           set_thiele_coef (float k);
	void           update_coef ();

	void           clear_buffers ();
	inline std::array <float, 2>
	               process_sample_split (float x);
	inline float   process_sample_compensate (float x);
	void           process_block_split (float lo_ptr [], float hi_ptr [], const float src_ptr [], int nbr_spl);
	void           process_block_compensate (float dst_ptr [], const float src_ptr [], int nbr_spl);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           update_filters ();
	inline std::array <float, 2>
	               compute_ap4_lp4 (float x, SvfCore <> &flt_1, SvfCore <> &flt_2);

	float          _sample_freq = 0; // Hz, > 0. 0 = not set
	float          _inv_fs      = 0; // s, > 0. 0 = not set
	float          _split_freq  = 0; // Hz, ]0, fs/2[. 0 = not set

	// Frequency ratio for the first notch in the response curve, in [0 ; 1[.
	// 0 = standard Linkwitz-Riley
	float          _thiele_k    = 0;

	bool           _dirty_flag  = true; // Filters require an update

	float          _r1          = 0; // 1 / Q1
	float          _r2          = 0; // 1 / Q2
	float          _m1          = 0;
	float          _m3          = 0;
	float          _m4          = 0;

	// Main filters
	SvfCore <>     _split_1;   // Multimode output (LPF, BPF, HPF)
	SvfCore <>     _split_2;   // Multimode output (LPF, BPF, HPF)
	SvfCore <>     _split_3;   // Custom output with order 0-2 coefs from the s equation
	SvfCore <SvfMixerLow>      // LPF
	               _split_4;

	// Phase compensation filters
	SvfCore <>     _comp_1;    // Multimode output (LPF, BPF, HPF)
	SvfCore <>     _comp_2;    // Specific APF output



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const SplitThiele8 &other) const = delete;
	bool           operator != (const SplitThiele8 &other) const = delete;

}; // class SplitThiele8



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/iir/SplitThiele8.hpp"



#endif   // mfx_dsp_iir_SplitThiele8_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
