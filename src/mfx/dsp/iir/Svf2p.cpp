/*****************************************************************************

        Svf2p.cpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "mfx/dsp/iir/Svf2p.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Svf2p::Svf2p ()
:	_sample_freq (44100)
,	_f0 (1000)
,	_q (0.5f)
,	_type (Type_RESONATOR)
,	_reso (0)
,	_g0 (0)
,	_g1 (0)
,	_g2 (0)
,	_v0m (1)
,	_v1m (0)
,	_v2m (0)
{
	update_eq ();
}



void	Svf2p::update_eq ()
{
	const float    k = float (1.0 / _q);
	conv_poles (_g0, _g1, _g2, _f0 / _sample_freq, k);

	switch (_type)
	{
	case	Type_LOWPASS:
		_v0m = 0;
		_v1m = 0;
		_v2m = 1;
		break;

	case	Type_HIGHPASS:
		_v0m =  1;
		_v1m = -k;
		_v2m = -1;
		break;

	case	Type_RESONATOR:
		_v0m = 1;
		_v1m = (_reso - 1) * k;	// _reso = 0: -k, _reso = 1: 0
		_v2m = 0;
		break;

	case	Type_BANDPASS:
		_v0m = 0;
		_v1m = 1;
		_v2m = 0;
		break;

	case	Type_LOWSHELF:
		{
			const float    r_s2 = float (sqrt (_reso));
			_v0m = 1;
			_v1m = (r_s2 - 1) * k;
			_v2m = _reso - 1;
		}
		break;

	case	Type_HIGHSHELF:
		{
			const float    r_s2 = float (sqrt (_reso));
			_v0m = _reso;
			_v1m = (r_s2 - _reso) * k;
			_v2m = 1 - _reso;
		}
		break;

	default:
		assert (false);
		break;
	}
}



// k     =  1 / Q
// f0_fs = f0 / fs
void	Svf2p::conv_poles (float &g0, float &g1, float &g2, double f0_fs, float k)
{
	assert (g0  != 0);
	assert (g1  != 0);
	assert (g2  != 0);
	assert (f0_fs > 0);
	assert (f0_fs < 0.5);
	assert (k > 0);

	const double   w     = fstb::PI * f0_fs;
	const float    s1    = float (sin (    w));
	const float    s2    = float (sin (2 * w));	// Can be generated with a quadrature osc: sin 2x = 2 * sin x * cos x
	const float    nrm   = 1.0f / (2 + k * s2);
	const float    s12sq = 2 * s1 * s1;

	g0  =               s2  * nrm;
	g1  = (-s12sq - k * s2) * nrm;
	g2  =   s12sq           * nrm;
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

result = b0 * low + b1 * band + b2 * high
		 = b0 * v2 +  b1 * v1   + b2 * (v0 - k * v1 - v2)
		 = v0 * b2 + v1 * (b1 - k * b2) + v2 * (b0 - b2)
v0m = b2
v1m = b1 - k * b2
v2m = b0 - b2
*/

void	Svf2p::conv_s_eq_to_svf (float &g0, float &g1, float &g2, float &v0m, float &v1m, float &v2m, const double b [3], const double a [3], double freq, double fs)
{
	assert (g0  != 0);
	assert (g1  != 0);
	assert (g2  != 0);
	assert (v0m != 0);
	assert (v1m != 0);
	assert (v2m != 0);
	assert (b   != 0);
	assert (a   != 0);
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

	v0m = float (              b2);
	v1m = float (     b1 - k * b2);
	v2m = float (b0      -     b2);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
