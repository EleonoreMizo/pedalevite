/*****************************************************************************

        MoogLadderDAngeloData.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_va_MoogLadderDAngeloData_HEADER_INCLUDED)
#define mfx_dsp_va_MoogLadderDAngeloData_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <array>



namespace mfx
{
namespace dsp
{
namespace va
{



template <int N>
class MoogLadderDAngeloData
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _order = N;

	inline float   compute_g_max (float fmax_over_fs);
	inline float   compute_k0_max (float gmax);
	static inline float
	               compute_alpha (float k);
	static inline float
	               compute_knorm_factor ();

	float          _sample_freq = 0;    // Sampling rate, Hz. > 0. 0 = not init.
	float          _inv_fs      = 0;    // 1 / fs, > 0

	float          _fc          = 1000; // Cutoff frequency, Hz. ]0 ; _sample_freq * 0.5[
	float          _k           = 0;    // Resonance/feedback, >= 0
	float          _gaincomp    = 0;    // Gain compensation at DC, from 0 = none to 1 = full

	// Thermal voltage, volt. Should be 26 mV but actually defines the clipping
	// level (= 2 * _vt). So we set it to unity by default.
	float          _vt          = 0.5f;

	bool           _dirty_flag  = true; // The variables below require an update
	float          _gc_mul      = 1;    // Final multiplier for the gain compensation
	float          _gc_mul_s    = 1;    // Same, for the stages
	float          _alpha       = 0;    // Depends on _k
	float          _alpha_inv   = 0;    // 1 / _alpha
	float          _knorm_factor = compute_knorm_factor ();
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



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

}; // class MoogLadderDAngeloData



}  // namespace va
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/va/MoogLadderDAngeloData.hpp"



#endif   // mfx_dsp_va_MoogLadderDAngeloData_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
