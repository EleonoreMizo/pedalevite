/*****************************************************************************

        MoogLadderDAngelo.h
        Author: Laurent de Soras, 2020

Generalized Moog ladder simulation.
Stable up to Fs / 8, requires 4x oversampling

Template parameters:

- N: filter order, >= 1

- SL: lowpass and input saturator. Requires:
	SL::SL ();
	float SL::operator () (float x) noexcept;
	Suggestions:
	- WsTanhFast for the tanh curve quoted in the paper. Tuning issues at high
		resonance.
	- WsSqLin for a faster approximation with less tuning issues.
	- WsSq for an even faster approximation, but with same issues as tanh
	- WsHardclip for good tuning but high aliasing and low harmonics

- SF: feedback saturator. Requires:
	SF::SF ();
	float SF::operator () (float x) noexcept;
	Suggestions:
	- WsBypass
	- WsNegCond <std::ratio <1, 1>, std::ratio <2, 1> > for zipper noises
	- WsBitcrush <std::ratio <1, 2>, false>
	- WsFloorOfs <std::ratio <0, 1> >
	- WsTruncMod <std::ratio <-3, 4>, std::ratio <3, 4> >

Algorithm from:
Stephano D'Angelo, Vesa Valimaki,
Generalized Moog Ladder Filter: Part II -
Explicit Nonlinear Model through a Novel Delay-Free Loop Implementation Method,
IEEE Transactions on Audio, Speech and Language Processing,
vol. 22, no. 12, pp. 1873-1883, December 2014

Other filter modes can be recreated by mixing the individual stage inputs
and the global output together. Below is a weight table for N = 4:

Filter      In0 In1 In2 In3 Out
-------------------------------
4-pole LPF   .   .   .   .   1
2-pole LPF   .   .   1   .   .
4-pole BPF   .   .   4  -8   4
2-pole BPF   .   2  -2   .   .
4-pole HPF   1  -4   6  -4   1
2-pole HPF   1  -2   1   .   .

Table from:
Vesa Valimaki, Antto Huovilainen,
Oscillator and Filter Algorithms for Virtual Analog Synthesis,
Computer Music Journal, vol. 30, no. 2, pp. 19-31, June 2006

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_va_MoogLadderDAngelo_HEADER_INCLUDED)
#define mfx_dsp_va_MoogLadderDAngelo_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "mfx/dsp/shape/WsBypass.h"
#include "mfx/dsp/shape/WsSqLin.h"
#include "mfx/dsp/va/MoogLadderDAngeloData.h"

#include <array>



namespace mfx
{
namespace dsp
{
namespace va
{



template <int N, class SL = shape::WsSqLin, class SF = shape::WsBypass>
class MoogLadderDAngelo
{
	static_assert (N >=  1, "Filter order must be >= 1.");
	// N! must fit in a uint64_t
	static_assert (N <= 20, "Filter order must be <= 20.");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _order = N;
	typedef SL ShaperLpf;
	typedef SF ShaperFdbk;

	typedef MoogLadderDAngeloData <N> FilterData;

	void           set_sample_freq (double sample_freq) noexcept;
	void           set_scale (float s) noexcept;
	void           set_freq_natural (float f) noexcept;
	void           set_freq_compensated (float f) noexcept;
	void           set_max_mod_freq (float f) noexcept;
	void           set_reso_raw (float k) noexcept;
	void           set_reso_norm (float kn) noexcept;
	void           set_gain_comp (float gc) noexcept;
	float          process_sample (float x) noexcept;
	float          process_sample (float x, float stage_in_ptr [N]) noexcept;
	float          process_sample_pitch_mod (float x, float m) noexcept;
	float          process_sample_pitch_mod (float x, float m, float stage_in_ptr [N]) noexcept;
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept;
	void           process_block_pitch_mod (float dst_ptr [], const float src_ptr [], const float mod_ptr [], int nbr_spl) noexcept;
	void           clear_buffers () noexcept;

	MoogLadderDAngeloData <N> &
	               use_data () noexcept;
	const MoogLadderDAngeloData <N> &
	               use_data () const noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	fstb_FORCEINLINE void
	               check_coef () noexcept;
	void           update_coef () noexcept;
	void           update_gaincomp () noexcept;
	fstb_FORCEINLINE float
	               process_sample_internal (float x, float g, float k0s) noexcept;
	fstb_FORCEINLINE float
	               process_sample_input (float x) noexcept;
	fstb_FORCEINLINE void
	               process_sample_stage (float &y, float &yo, int n, float g, float k0s) noexcept;
	fstb_FORCEINLINE float
	               process_sample_fdbk (float x, float y) noexcept;

	FilterData     _d;

	ShaperLpf      _shaper_input;
	std::array <ShaperLpf, N>
	               _shaper_lpf_arr;
	ShaperFdbk     _shaper_fdbk;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const MoogLadderDAngelo <N, SL, SF> &other) const = delete;
	bool           operator != (const MoogLadderDAngelo <N, SL, SF> &other) const = delete;

}; // class MoogLadderDAngelo



}  // namespace va
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/va/MoogLadderDAngelo.hpp"



#endif   // mfx_dsp_va_MoogLadderDAngelo_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
