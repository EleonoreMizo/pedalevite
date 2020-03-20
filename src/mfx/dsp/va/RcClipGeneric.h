/*****************************************************************************

        RcClipGeneric.h
        Author: Laurent de Soras, 2020

Diode clipper with R in series and C in parallel with both diodes

In ---/\/\/\--+---+---+--- Out
        R     |   |   |
             --- / \ ---
           C --- --- \ /
              |   |   |
Gnd ----------+---+---+
                  D2  D1

Characteristic of the diodes is a custom function, so fancy responses are
possible.

Integration with the Trapezoidal Rule.
Uses classical Newton-Raphson iterations to find the diode voltage.
The algorithm is loosely based on Modified Nodal Analysis (MNA)

Template parameters:

- F: class computing the characteristic function I = f(V) as well as df/dV.
	Requires:
	void F::eval (float &y, float &dy, float x);
	float F::get_max_step (float x);

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
#if ! defined (mfx_dsp_va_RcClipGeneric_HEADER_INCLUDED)
#define mfx_dsp_va_RcClipGeneric_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"



namespace mfx
{
namespace dsp
{
namespace va
{



template <class F>
class RcClipGeneric
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef F IvFunc;

	explicit       RcClipGeneric (IvFunc &&fnc);
	               RcClipGeneric ()                               = default;
	               RcClipGeneric (const RcClipGeneric <F> &other) = default;
	               RcClipGeneric (RcClipGeneric <F> &&other)      = default;

	               ~RcClipGeneric ()                              = default;

	RcClipGeneric <F> &
	               operator = (const RcClipGeneric <F> &other)    = default;
	RcClipGeneric <F> &
	               operator = (RcClipGeneric <F> &&other)         = default;

	IvFunc &       use_fnc ();
	const IvFunc & use_fnc () const;

	void           set_sample_freq (double sample_freq);
	void           set_capa (float c);
	void           set_cutoff_freq (float f);
	float          process_sample (float x);
	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           update_internal_coef_rc ();

	IvFunc         _fnc;
	float          _sample_freq = 0; // Sampling rate, Hz. > 0. 0 = not init.

	float          _max_it      = 10;    // Maximum number of NR iterations
	float          _max_dif_a   = 1e-6f; // Absolute precision to reach

	// Circuit parameters
	float          _r    = 2200;     // Serial resistor, ohm
	float          _c    = 10e-9f;   // Parallel capacitor, farad

	// Internal variables
	float          _inv_fs    = 0;     // Integration step, s. > 0. 0 = not init.
	float          _gr        = 1.f / _r;
	float          _geqc      = 0;
	float          _gr_p_geqc = 0;

	// States
	float          _ic  = 0;   // Capacitor current
	float          _v2  = 0;   // Diode voltage, stored to init the NR iteration for the next sample



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const RcClipGeneric <F> &other) const = delete;
	bool           operator != (const RcClipGeneric <F> &other) const = delete;

}; // class RcClipGeneric



}  // namespace va
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/va/RcClipGeneric.hpp"



#endif   // mfx_dsp_va_RcClipGeneric_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

