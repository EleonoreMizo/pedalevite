/*****************************************************************************

        DiodeClipNR.h
        Author: Laurent de Soras, 2020

Diode clipper with R in series and C in parallel with both diodes

In ---/\/\/\--+---+---+--- Out
        R     |   |   |
             --- / \ ---
           C --- --- \ /
              |   |   |
Gnd ----------+---+---+
                  D2  D1

Integration with the Trapezoidal Rule.
Uses classical Newton-Raphson iterations to find the diode voltage.
The algorithm is loosely based on Modified Nodal Analysis (MNA)

Ref:
http://www.ecircuitcenter.com/SpiceTopics/Non-Linear%20Analysis/Non-Linear%20Analysis.htm
http://qucs.sourceforge.net/tech/node14.html
http://qucs.sourceforge.net/tech/node26.html
http://qucs.sourceforge.net/tech/node16.html

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_va_DiodeClipNR_HEADER_INCLUDED)
#define mfx_dsp_va_DiodeClipNR_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace va
{



class DiodeClipNR
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               DiodeClipNR ()                         = default;
	               DiodeClipNR (const DiodeClipNR &other) = default;
	               DiodeClipNR (DiodeClipNR &&other)      = default;

	               ~DiodeClipNR ()                        = default;

	DiodeClipNR &  operator = (const DiodeClipNR &other)  = default;
	DiodeClipNR &  operator = (DiodeClipNR &&other)       = default;

	void           set_sample_freq (double sample_freq);
	void           set_d1_is (float is);
	void           set_d2_is (float is);
	void           set_d1_n (float n);
	void           set_d2_n (float n);
	void           set_capa (float c);
	void           set_cutoff_freq (float f);
	float          process_sample (float x);
	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           update_internal_coef ();

	float          _sample_freq = 0; // Sampling rate, Hz. > 0. 0 = not init.

	float          _max_it = 10;

	// Circuit parameters
	float          _vt  = 0.026f;    // Diode thermal voltage, volt. Sets the diode clipping threshold, around 0.65 V for 0.026
	float          _is1 = 0.1e-15f;  // Diode 1 saturation current, ampere
	float          _is2 = 0.1e-6f;   // Diode 2 saturation current, ampere
	float          _n1  = 1;         // Diode 1 ideality factor
	float          _n2  = 4;         // Diode 2 ideality factor
	float          _r   = 2200;      // Serial resistor, ohm
	float          _c   = 10e-9f;    // Parallel capacitor, farad

	// Internal variables
	float          _inv_fs = 0;      // Integration step, s. > 0. 0 = not init.

	// States
	float          _ic  = 0;
	float          _v2  = 0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const DiodeClipNR &other) const = delete;
	bool           operator != (const DiodeClipNR &other) const = delete;

}; // class DiodeClipNR



}  // namespace va
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/va/DiodeClipNR.hpp"



#endif   // mfx_dsp_va_DiodeClipNR_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

