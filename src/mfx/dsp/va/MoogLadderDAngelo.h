/*****************************************************************************

        MoogLadderDAngelo.h
        Author: Laurent de Soras, 2020

Generalized Moog ladder simulation.
Stable up to Fs / 8, requires 4x oversampling

Template parameters:

- N: filter order, >= 1

Algorithm from:
Stephano D'Angelo, Vesa Valimaki,
Generalized Moog Ladder Filter: Part II -
Explicit Nonlinear Model through a Novel Delay-Free Loop Implementation Method,
IEEE Transactions on Audio, Speech and Language Processing,
vol. 22, no. 12, pp. 1873-1883, December 2014

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
#include "fstb/fnc.h"

#include <array>



namespace mfx
{
namespace dsp
{
namespace va
{



template <int N>
class MoogLadderDAngelo
{
	static_assert (N >=  1, "Filter order must be >= 1.");
	// N! must fit in a uint64_t
	static_assert (N <= 20, "Filter order must be <= 20.");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_sample_freq (double sample_freq);
	void           set_freq_natural (float f);
	void           set_freq_compensated (float f);
	void           set_reso_raw (float k);
	void           set_reso_norm (float kn);
	void           set_gain_comp (float gc);
	float          process_sample (float x);
	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           update_coef ();
	void           update_gaincomp ();

	static float   compute_alpha (float k);
	static fstb_FORCEINLINE float
	               clip_sigmoid (float x);

	float          _sample_freq = 0;    // Sampling rate, Hz. > 0. 0 = not init.
	float          _inv_fs      = 0;    // 1 / fs, > 0

	float          _fc          = 1000; // Cutoff frequency, Hz. ]0 ; _sample_freq * 0.5[
	float          _k           = 0;    // Resonance/feedback, >= 0
	float          _gaincomp    = 0;    // Gain compensation at DC, from 0 = none to 1 = full

	// Thermal voltage, volt. Should be 26 mV but actually defines the clipping
	// level. So we set it to unity.
	float          _vt          = 1;

	bool           _dirty_flag  = true; // The variables below require an update
	float          _gc_mul      = 1;    // Final multiplier for the gain compensation
	float          _alpha       = 0;    // Depends on _k
	float          _alpha_inv   = 0;    // 1 / _alpha
	const float    _knorm_factor = 1.f / fstb::ipowp (float (cos (fstb::PI / N)), N);
	float          _g           = 0;
	float          _vt2i        = 1 / (2 * _vt);
	float          _k0s         = 0;
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

	bool           operator == (const MoogLadderDAngelo <N> &other) const = delete;
	bool           operator != (const MoogLadderDAngelo <N> &other) const = delete;

}; // class MoogLadderDAngelo



}  // namespace va
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/va/MoogLadderDAngelo.hpp"



#endif   // mfx_dsp_va_MoogLadderDAngelo_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
