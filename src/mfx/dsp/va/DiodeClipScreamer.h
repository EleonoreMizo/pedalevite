/*****************************************************************************

        DiodeClipScreamer.h
        Author: Laurent de Soras, 2020

The high-pass filtered dry audio is mixed with the distorted sound without
any limiting (the power supply voltage is assumed "infinite"), so be careful
when using high-gain input. With the default settings, the distored signal
alone clips around -10 dB.

You must call update_eq () between parameter changes and audio processing.

See also: https://www.electrosmash.com/tube-screamer-analysis#clip-stage

Algorithm from:

Rafael C. D. Paiva, Stefano D’Angelo, Jyri Pakarinen, Vesa Valimaki,
Emulation of Operational Amplifiers and Diodes in Audio Distortion Circuits,
IEEE Transactions on Circuits and Systems II: Express Briefs, vol. 59, no. 10,
pp. 688-692, October 2012

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_va_DiodeClipScreamer_HEADER_INCLUDED)
#define mfx_dsp_va_DiodeClipScreamer_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace va
{



class DiodeClipScreamer
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               DiodeClipScreamer ()                               = default;
	               DiodeClipScreamer (const DiodeClipScreamer &other) = default;
	               DiodeClipScreamer (DiodeClipScreamer &&other)      = default;

	               ~DiodeClipScreamer ()                              = default;

	DiodeClipScreamer &
	               operator = (const DiodeClipScreamer &other)        = default;
	DiodeClipScreamer &
	               operator = (DiodeClipScreamer &&other)             = default;

	void           set_sample_freq (double sample_freq);
	void           set_dist (float dist);
	void           set_sat_lvl (float lvl);
	void           set_input_hpf_capa (float c);
	void           set_input_hpf_freq (float f);
	void           set_fdbk_hpf_capa (float c);
	void           set_fdbk_hpf_freq (float f);
	void           set_fdbk_lpf_capa (float c);
	void           set_fdbk_lpf_freq (float f);

	void           update_eq ();
	float          process_sample (float x);
	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           update_internal_coef ();

	float          _sample_freq = 0;    // Sampling rate, Hz. > 0. 0 = not init
	float          _dist        = 1;    // Distortion potentiometer, [0 ; 1]
	bool           _dirty_flag  = true; // Coefficient update required

	// Circuit parameters
	float          _rn      = 10000;    // Input HPF restistor, ohm
	float          _cn      = 1e-6f;    // Input HPF capacitor, farad. Simple HPF on the input.
	float          _ri      = 4700;     // Resistor for feedback bypass, ohm
	float          _ci      = 47e-9f;   // Capacitor for feedback bypass, farad. Acts as a HPF on the gain.
	float          _rfmax   = 500000;   // Feedback gain resistor, ohm
	float          _cf      = 51e-12f;  // Feedback capacitor, farad. Acts as a variable LPF on the gain. Set it to 200-500 pF to reduce the aliasing.
	float          _rs      = 0.568f;   // 1N914 series resistance, ohm
	float          _is      = 2.52e-9f; // 1N914 reverse bias saturation current, ampere
	float          _nd      = 1.752f;   // 1N914 ideality factor
	float          _vt      = 25.86e-3f;// Thermal voltage kT/q. Use it to easily set the diode clipping threshold.

	// Internal coefficients
	float          _wvn_kx  = 0; // 0.5 * wvn_k
	float          _wvi_k   = 0; 
	float          _ki      = 0; 
	float          _wd_k2   = 0; 
	float          _wd_k4   = 0; 
	float          _wi0_r   = 0;
	float          _wpf_kt  = 0;
	float          _wsd_krx = 0; // 0.5 * wsd_kr
	float          _wd_k1   = 0;
	float          _wd_k3x  = 0;

	// State
	float          _wcn_s   = 0;
	float          _wci_s   = 0;
	float          _wcf_s   = 0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const DiodeClipScreamer &other) const = delete;
	bool           operator != (const DiodeClipScreamer &other) const = delete;

}; // class DiodeClipScreamer



}  // namespace va
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/va/DiodeClipScreamer.hpp"



#endif   // mfx_dsp_va_DiodeClipScreamer_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

