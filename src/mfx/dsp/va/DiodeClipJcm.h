/*****************************************************************************

        DiodeClipJcm.h
        Author: Laurent de Soras, 2020

Output level is slightly above 6 dB for a 0 dB 1000 Hz signal at full
distortion.

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
#if ! defined (mfx_dsp_va_DiodeClipJcm_HEADER_INCLUDED)
#define mfx_dsp_va_DiodeClipJcm_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace va
{



class DiodeClipJcm
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_sample_freq (double sample_freq) noexcept;
	void           set_dist (float dist) noexcept;
	void           set_sat_lvl (float lvl) noexcept;
	void           set_input_hpf_capa (float c) noexcept;
	void           set_input_hpf_freq (float f) noexcept;
	void           set_fdbk_lpf_capa (float c) noexcept;
	void           set_fdbk_lpf_freq (float f) noexcept;

	void           update_eq () noexcept;
	float          process_sample (float x) noexcept;
	void           clear_buffers () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           update_internal_coef () noexcept;

	float          _sample_freq = 0;    // Sampling rate, Hz. > 0. 0 = not init
	float          _dist        = 1;    // Distortion potentiometer, [0 ; 1]
	bool           _dirty_flag  = true; // Coefficient update required

	// Circuit parameters
	float          _r1      = 22000;    // Input HPF restistor, ohm
	float          _c1      = 47e-9f;   // Input HPF capacitor, farad. Simple HPF on the input.
	float          _r2      = 12000;    // Serial resistor after the input, ohm
	float          _c2      = 1e-9f;    // Additional serial capacitor, farad. Hard to tell exactly what it does.
	float          _rp      = 220000;   // Feedback potentiometer, ohm
	float          _cf      = 47e-12f;  // Feedback capacitof, farad. Acts a variable LPF on the gain. Set it to 100-200 pF to reduce the aliasing.
	float          _rs      = 8.163f;   // NSCW100 series resistance, ohm
	float          _is      = 16.88e-9f;// NSCW100 reverse bias saturation current, ampere
	float          _nd      = 9.626f;   // NSCW100 ideality factor
	float          _vt      = 25.86e-3f;// Thermal voltage kT/q. Use it to easily set the diode clipping threshold.

	// Internal coefficients
	float          _wc1_r   = 0;
	float          _wc2_r   = 0;
	float          _wcf_r   = 0;
	float          _wrs_r   = 0;
	float          _wd_k2   = 0;
	float          _wd_k4   = 0;
	float          _ki      = 0;

	float          _wrp_r   = 0;
	float          _wp2_kt  = 0;
	float          _wp2_r   = 0;
	float          _ws2_r   = 0;
	float          _wp1_kt  = 0;
	float          _wp1_r   = 0;
	float          _ws1_kl  = 0;
	float          _ws2_kl  = 0;

	float				_wi0_r   = 0;
	float				_wpf_kt  = 0;
	float				_wpf_r   = 0;
	float				_wsd_krx = 0; // = 0.5 * wsd_kr
	float				_wsd_r   = 0;
	float				_wd_k1   = 0;
	float				_wd_k3x  = 0;

	float          _kic1x   = 0;
	float          _kic2    = 0;
	float          _kc1x    = 0;
	float          _kc1c1   = 0;
	float          _kc1c2   = 0;
	float          _kc2c1x  = 0;
	float          _kc2c2   = 0;

	// State memory
	float          _wc1_s   = 0;
	float          _wc2_s   = 0;
	float          _wcf_s   = 0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const DiodeClipJcm &other) const = delete;
	bool           operator != (const DiodeClipJcm &other) const = delete;

}; // class DiodeClipJcm



}  // namespace va
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/va/DiodeClipJcm.hpp"



#endif   // mfx_dsp_va_DiodeClipJcm_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
