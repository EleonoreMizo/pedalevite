/*****************************************************************************

        SplitThiele8.h
        Author: Laurent de Soras, 2021

8th-order crossover Thiele filter implemented with 4x 2-pole SVFs.

Thiele filters:
Neville Thiele, Loudspeaker Crossovers with Notched Response,
Journal of the Audio Engineering Society, vol. 48, no. 9, pp. 786-799,
2000-09

Implementation reference:
Andrew Simper, Teemu Voipio, Vadim Zavalishin,
https://www.kvraudio.com/forum/viewtopic.php?p=7017185#p7017185

Original formula:

SVF2pole1 (in , R1 -> lp2, bp2   , hp2   )
SVF2pole2 (lp2, R2 -> lp4, lp2bp2, lp2hp2)
SVF2pole3 (lp4, R1 -> lp6)
SVF2pole4 (lp6, R2 -> lp8)
ap4 = m0 * hp2 + m1 * bp2 + m3 * lp2hp2 + m4 * lp2bp2 + m5 * lp4
hp8 = ap4 - lp8
with:
hp2    = in  - R1 * bp2    - lp2
lp2hp2 = lp2 - R2 * lp2bp2 - lp4
and:
ap4 =   ((1 - R1 * s + s^2) * (1 - R2 * s + s^2))
      / ((1 + R1 * s + s^2) * (1 + R2 * s + s^2))
therefore:
m0 = 1
m1 = -R1 - 2 * R2
m3 = 1 + 2 * R2 * (R1 + R2)
m4 = R2
m5 = 1

Simplification:

ap4 = in + (m1 - R1) * bp2 + (1 - m3) * (lp4 - lp2) + (m4 - m3 * R2) * lp2bp2
ap4 = in + n1 * bp2 + n4 * (lp4 - lp2) + n3 * lp2bp2
n1  = m1 - R1      = -2 * (R1 + R2)          
n4  =  1 - m3      = -2 * (R1 + R2) * R2     
n3  = m4 - m3 * R2 = -2 * (R1 + R2) * R2 * R2

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

	void           set_sample_freq (double sample_freq) noexcept;
	void           set_split_freq (float f) noexcept;
	void           set_thiele_coef (float k) noexcept;
	inline bool    is_dirty () const noexcept;
	void           update_coef () noexcept;
	void           copy_param_from (const SplitThiele8 &other) noexcept;

	void           clear_buffers () noexcept;
	inline std::array <float, 2>
	               process_sample_split (float x) noexcept;
	inline float   process_sample_compensate (float x) noexcept;
	void           process_block_split (float lo_ptr [], float hi_ptr [], const float src_ptr [], int nbr_spl) noexcept;
	void           process_block_compensate (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           update_filters () noexcept;
	inline std::array <float, 2>
	               compute_ap4_lp4 (float x, SvfCore <> &filt_1, SvfCore <> &filt_2) noexcept;

	float          _sample_freq = 0; // Hz, > 0. 0 = not set
	float          _inv_fs      = 0; // s, > 0. 0 = not set
	float          _split_freq  = 0; // Hz, ]0, fs/2[. 0 = not set

	// Frequency ratio for the first notch in the response curve, in [0 ; 1[.
	// 0 = standard Linkwitz-Riley
	float          _thiele_k    = 0;

	bool           _dirty_flag  = true; // Filters require an update

	float          _r1          = 0; // 1 / Q1
	float          _r2          = 0; // 1 / Q2
	float          _n1          = 0;
	float          _n3          = 0;
	float          _n4          = 0;

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
