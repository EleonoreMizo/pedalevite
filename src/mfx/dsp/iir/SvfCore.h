/*****************************************************************************

        SvfCore.h
        Author: Laurent de Soras, 2020

Template parameters:

- MX: output mixer, creating the final output from the input (v0) and the raw
	outputs (v1 and v2).
	Requires:
	static float MX::mix (float v0, float v1, float v2,
		float v0m, float v1m, float v2m);
	static void MX::inc (float &v0m, float &v1m, float &v2m,
		float v0mi, float v1mi, float v2mi);

Implements:

v0     = input
t0     = v0 - ic2eq
t1     = g0 * t0 + g1 * ic1eq
t2     = g2 * t0 + g0 * ic1eq
v1     = t1 + ic1eq
v2     = t2 + ic2eq
ic1eq  = 2 * t1 + ic1eq
ic2eq  = 2 * t2 + ic2eq
output = v0 * v0m + v1 * v1m + v2 * v2m

Specifically, for the output:

low   =                v2
band  =           v1
high  = v0 -  k * v1 - v2
notch = v0 -  k * v1      = low + high
bell  = v0 + gi * v1      = input + gi * band

Coefficients are not calculated in this class and should follow:

k   = 1 / Q
w   = PI * f0 / fs
s1  = sin (    w)
s2  = sin (2 * w)
nrm = 1 / (2 + k * s2)
g0  =                     s2  * nrm
g1  = (-2 * s1 * s1 - k * s2) * nrm
g2  =   2 * s1 * s1           * nrm

Reference:
Andrew Simper,
Linear Trapezoidal State Variable Filter (SVF) in state increment form: state += val,
2014-06-07, updated 2014-07-03,
http://www.cytomic.com/technical-papers

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_iir_SvfCore_HEADER_INCLUDED)
#define mfx_dsp_iir_SvfCore_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "mfx/dsp/iir/SvfMixerDefault.h"



namespace mfx
{
namespace dsp
{
namespace iir
{



template <class MX = SvfMixerDefault>
class SvfCore
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef MX Mixer;

	               SvfCore ()                        = default;
	               SvfCore (const SvfCore &other)    = default;
	               SvfCore (SvfCore &&other)         = default;
	               ~SvfCore ()                       = default;

	SvfCore &      operator = (const SvfCore &other) = default;
	SvfCore &      operator = (SvfCore &&other)      = default;

	void           neutralise ();
	void           set_coefs (float g0, float g1, float g2);
	void           get_coefs (float &g0, float &g1, float &g2) const;
	void           set_mix (float v0m, float v1m, float v2m);
	void           get_mix (float &v0m, float &v1m, float &v2m) const;
	void           copy_z_eq (const SvfCore <MX> &other);

	void           clear_buffers ();

	fstb_FORCEINLINE float
	               process_sample (float x);
	fstb_FORCEINLINE float
	               process_sample (float x, float g0, float g1, float g2);
	fstb_FORCEINLINE float
	               process_sample (float x, float g0, float g1, float g2, float v0m, float v1m, float v2m);
	fstb_FORCEINLINE float
	               process_sample_inc (float x, float g0i, float g1i, float g2i, float v0mi, float v1mi, float v2mi);

	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl);
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl, const float g0_ptr [], const float g1_ptr [], const float g2_ptr []);
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl, const float g0_ptr [], const float g1_ptr [], const float g2_ptr [], const float v0m_ptr [], const float v1m_ptr [], const float v2m_ptr []);
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl, float g0i, float g1i, float g2i, float v0mi, float v1mi, float v2mi);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	fstb_FORCEINLINE void
	               iterate (float v0, float &v1, float &v2, float g0, float g1, float g2);
	static fstb_FORCEINLINE void
	               increment (float &g0, float &g1, float &g2, float &v0m, float &v1m, float &v2m, float g0i, float g1i, float g2i, float v0mi, float v1mi, float v2mi);

	// Coefficients
	float          _g0    = 0;
	float          _g1    = 0;
	float          _g2    = 0;

	// Mixers
	float          _v0m   = 1;
	float          _v1m   = 0;
	float          _v2m   = 0;

	// Internal states
	float          _ic1eq = 0;
	float          _ic2eq = 0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const SvfCore &other) const = delete;
	bool           operator != (const SvfCore &other) const = delete;

}; // class SvfCore



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/iir/SvfCore.hpp"



#endif   // mfx_dsp_iir_SvfCore_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
