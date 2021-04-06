/*****************************************************************************

        Svf2p.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_iir_Svf2p_CODEHEADER_INCLUDED)
#define mfx_dsp_iir_Svf2p_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Svf2p::set_sample_freq (float fs)
{
	assert (fs > 0);

	_sample_freq =        fs;
	_inv_fs      = 1.0f / fs;
}



float	Svf2p::get_sample_freq () const
{
	return _sample_freq;
}



void	Svf2p::set_freq (float f0)
{
	assert (f0 > 0);
	assert (f0 < _sample_freq * 0.5f);

	_f0 = f0;
}



float	Svf2p::get_freq () const
{
	return _f0;
}



void	Svf2p::set_type (Type type)
{
	assert (type >= 0);
	assert (type < Type_NBR_ELT);

	_type = type;
}



Svf2p::Type	Svf2p::get_type () const
{
	return _type;
}



void	Svf2p::set_reso (float reso)
{
	_reso = reso;
}



float	Svf2p::get_reso () const
{
	return _reso;
}



void	Svf2p::set_q (float q)
{
	assert (q > 0);

	_q = q;
}



float	Svf2p::get_q () const
{
	return _q;
}



float	Svf2p::get_g0 () const
{
	return _g0;
}



float	Svf2p::get_g1 () const
{
	return _g1;
}



float	Svf2p::get_g2 () const
{
	return _g2;
}



float	Svf2p::get_v0m () const
{
	return _v0m;
}



float	Svf2p::get_v1m () const
{
	return _v1m;
}



float	Svf2p::get_v2m () const
{
	return _v2m;
}



/*
H(s) = (b2 * s^2 + b1 * s + b0) / (s^2 * a2  + s * a1      + 1)
     = (b2 * s^2 + b1 * s + b0) / (s^2 / f^2 + s * (f * q) + 1)

f = sqrt (1 / a2)
q = sqrt (a2) / a1

low  =               v2
band =          v1
high = v0 - k * v1 - v2

result = v0 * v0m + v1 * v1m  + v2 * v2m

result = b0 * low + b1 * f * band + b2 * high
		 = b0 * v2 +  b1 * f * v1   + b2 * (v0 - k * v1 - v2)
		 = v0 * b2 + v1 * (b1 * f - k * b2) + v2 * (b0 - b2)
v0m = b2
v1m = b1 * f - k * b2
v2m = b0 - b2
*/

template <typename TE>
void	Svf2p::conv_s_eq_to_svf (float &g0, float &g1, float &g2, float &v0m, float &v1m, float &v2m, const TE b [3], const TE a [3], double freq, double fs)
{
	assert (b != nullptr);
	assert (a != nullptr);
	assert (a [0] != 0);
	assert (freq > 0);
	assert (fs > 0);
	assert (freq < 0.5 * fs);

	// Normalizes to make a0 = 1
	const double      a0i = 1.0 / a [0];

	const double      a1 = a [1] * a0i;
	const double      a2 = a [2] * a0i;
	const double      b0 = b [0] * a0i;
	const double      b1 = b [1] * a0i;
	const double      b2 = b [2] * a0i;

	const double      t0 = sqrt (a2);
	const double      f0 =  1 / t0;
	const double      k  = a1 / t0;

	conv_poles (g0, g1, g2, freq * f0 / fs, float (k));

	v0m = float (                   b2);
	v1m = float (     b1 * f0 - k * b2);
	v2m = float (b0           -     b2);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_iir_Svf2p_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
