/*****************************************************************************

        MoogLadderDAngelo.h
        Author: Laurent de Soras, 2020

Generalized Moog ladder simulation.
Stable up to Fs / 8, requires 4x oversampling

Template parameters:

- N: filter order, >= 1

- SL: lowpass and input saturator. Requires:
	SL::SL ();
	float SL::operator () (float x);
	Suggestions:
	- WsTanhFast for the tanh curve quoted in the paper. Tuning issues at high
		resonance.
	- WsSqLin for a faster approximation with less tuning issues.
	- WsSq for an even faster approximation, but with same issues as tanh
	- WsHardclip for good tuning but high aliasing and low harmonics

- SF: feedback saturator. Requires:
	SF::SF ();
	float SF::operator () (float x);
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

	void           set_sample_freq (double sample_freq);
	void           set_scale (float s);
	void           set_freq_natural (float f);
	void           set_freq_compensated (float f);
	void           set_max_mod_freq (float f);
	void           set_reso_raw (float k);
	void           set_reso_norm (float kn);
	void           set_gain_comp (float gc);
	float          process_sample (float x);
	float          process_sample (float x, float stage_in_ptr [N]);
	float          process_sample_pitch_mod (float x, float m);
	float          process_sample_pitch_mod (float x, float m, float stage_in_ptr [N]);
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl);
	void           process_block_pitch_mod (float dst_ptr [], const float src_ptr [], const float mod_ptr [], int nbr_spl);
	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	fstb_FORCEINLINE void
	               check_coef ();
	void           update_coef ();
	void           update_gaincomp ();
	fstb_FORCEINLINE float
	               process_sample_internal (float x, float g, float k0s);
	fstb_FORCEINLINE float
	               process_sample_input (float x);
	fstb_FORCEINLINE void
	               process_sample_stage (float &y, float &yo, int n, float g, float k0s);
	fstb_FORCEINLINE float
	               process_sample_fdbk (float x, float y);
	float          compute_g_max (float fmax_over_fs);
	float          compute_k0_max (float gmax);
	static float   compute_alpha (float k);
	static float   compute_knorm_factor ();

	float          _sample_freq = 0;    // Sampling rate, Hz. > 0. 0 = not init.
	float          _inv_fs      = 0;    // 1 / fs, > 0

	float          _fc          = 1000; // Cutoff frequency, Hz. ]0 ; _sample_freq * 0.5[
	float          _k           = 0;    // Resonance/feedback, >= 0
	float          _gaincomp    = 0;    // Gain compensation at DC, from 0 = none to 1 = full

	ShaperLpf      _shaper_input;
	std::array <ShaperLpf, N>
	               _shaper_lpf_arr;
	ShaperFdbk     _shaper_fdbk;

	// Thermal voltage, volt. Should be 26 mV but actually defines the clipping
	// level. So we set it to unity by default.
	float          _vt          = 1;

	bool           _dirty_flag  = true; // The variables below require an update
	float          _gc_mul      = 1;    // Final multiplier for the gain compensation
	float          _gc_mul_s    = 1;    // Same, for the stages
	float          _alpha       = 0;    // Depends on _k
	float          _alpha_inv   = 0;    // 1 / _alpha
	const float    _knorm_factor = compute_knorm_factor ();
	float          _g           = 0;
	float          _gi          = 0;
	float          _gmax        = compute_g_max (0.49f);  // Maximum value for the modulated g
	float          _vt2         = 2 * _vt;
	float          _vt2i        = 1 / _vt2;
	float          _k0s         = 0;    // Coefficient for the LPF
	float          _k0si        = 0;    // _k0s derivative for 1 V/oct modulations
	float          _k0smax      = compute_k0_max (_gmax); // Maximum value for the modulated k0s
	float          _k0g         = 0;
	std::array <float, N>               // Direct filter coefficients
	               _r_arr;
	std::array <float, N>               // Feedback filter coefficients
	               _q_arr;
	std::array <int, N>                 // Binomial coefficients (N k), starting at k = 1
	               _bin_arr;

	// States
	std::array <float, N>
	               _si_arr;
	std::array <float, N>
	               _sf_arr;
	std::array <float, N>
	               _sg_arr;



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
