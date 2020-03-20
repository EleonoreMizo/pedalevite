/*****************************************************************************

        DiodeClipDAngelo.h
        Author: Laurent de Soras, 2020

Diode clipper with R in series and C in parallel with both diodes

In ---/\/\/\--+---+---+--- Out
        R     |   |   |
             --- / \ ---
           C --- --- \ /
              |   |   |
Gnd ----------+---+---+

Based on:
Stefano D'Angelo, Leonardo Gabrielli, Luca Turchet,
Fast Approximation of the Lambert W Function for Virtual Analog Modelling,
DAFx-19

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_va_DiodeClipDAngelo_HEADER_INCLUDED)
#define mfx_dsp_va_DiodeClipDAngelo_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"



namespace mfx
{
namespace dsp
{
namespace va
{



class DiodeClipDAngelo
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               DiodeClipDAngelo ()                              = default;
	               DiodeClipDAngelo (const DiodeClipDAngelo &other) = default;
	               DiodeClipDAngelo (DiodeClipDAngelo &&other)      = default;

						~DiodeClipDAngelo ()                             = default;

	DiodeClipDAngelo &
	               operator = (const DiodeClipDAngelo &other)       = default;
	DiodeClipDAngelo &
	               operator = (DiodeClipDAngelo &&other)            = default;

	void           set_sample_freq (double sample_freq);
	void           set_d1_is (float is);
	void           set_d2_is (float is);
	void           set_d1_n (float n);
	void           set_d2_n (float n);
	void           set_capa (float c);
	void           set_cutoff_freq (float f);
	fstb_FORCEINLINE float
	               process_sample (float x);
	void           process_block (float dst_ptr [], float src_ptr [], int nbr_spl);
	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           update_internal_coef_fs ();
	void           update_internal_coef ();
	fstb_FORCEINLINE float
	               process_sample_internal (float x, float &mem_p);

	float          _sample_freq = 0; // Sampling rate, Hz. > 0. 0 = not init

	// Integration variables. They depend on the discretization method and the
	// sampling rate
	// v'[n] = b0 * v[n] + b1 * v[n-1] - a1 * v'[n-1]
	float          _b0 = 0;
	float          _b1 = 0;
	float          _a1 = 0;

	// Circuit parameters
	float          _vt  = 0.026f;   // Diode thermal voltage, volt. Sets the diode clipping threshold, around 0.65 V for 0.026
	float          _is1 = 0.1e-15f; // Diode saturation current, ampere
	float          _is2 = 0.1e-6f;  // Diode saturation current, ampere
	float          _n1  = 1;
	float          _n2  = 4;
	float          _r   = 2200;     // Serial resistor, ohm
	float          _c   = 10e-9f;   // Parallel capacitor, farad

	// Internal coefficients
	float          _k1  = 0;
	float          _k2  = 0;
	float          _k31 = 0;
	float          _k32 = 0;
	float          _k41 = 0;
	float          _k42 = 0;
	float          _k51 = 0;
	float          _k52 = 0;
	float          _k6  = 0;
	float          _k71 = 0;
	float          _k72 = 0;

	// State
	float          _mem_p = 0;
	bool           _dirty_flag = true;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const DiodeClipDAngelo &other) const = delete;
	bool           operator != (const DiodeClipDAngelo &other) const = delete;

}; // class DiodeClipDAngelo



}  // namespace va
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/va/DiodeClipDAngelo.hpp"



#endif   // mfx_dsp_va_DiodeClipDAngelo_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

