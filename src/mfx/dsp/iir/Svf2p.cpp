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

#include "fstb/Approx.h"
#include "fstb/def.h"
#include "fstb/ToolsSimd.h"
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



Svf2p::Svf2p () noexcept
{
	update_eq ();
}



void	Svf2p::update_eq () noexcept
{
	const float    k = float (1.0 / _q);
	conv_poles (_g0, _g1, _g2, _f0 * _inv_fs, k);

	switch (_type)
	{
	case Type_LOWPASS:
		_v0m = 0;
		_v1m = 0;
		_v2m = 1;
		break;

	case Type_HIGHPASS:
		_v0m =  1;
		_v1m = -k;
		_v2m = -1;
		break;

	case Type_RESONATOR:
		_v0m = 1;
		_v1m = (_reso - 1) * k;	// _reso = 0: -k, _reso = 1: 0
		_v2m = 0;
		break;

	case Type_BANDPASS:
		_v0m = 0;
		_v1m = 1;
		_v2m = 0;
		break;

	case Type_LOWSHELF:
		{
			const float    r_s2 = float (sqrt (_reso));
			_v0m = 1;
			_v1m = (r_s2 - 1) * k;
			_v2m = _reso - 1;
		}
		break;

	case Type_HIGHSHELF:
		{
			const float    r_s2 = float (sqrt (_reso));
			_v0m = _reso;
			_v1m = (r_s2 - _reso) * k;
			_v2m = 1 - _reso;
		}
		break;

	case Type_ALLPASS:
		_v0m = 1;
		_v1m = -2 * k;
		_v2m = 0;
		break;

	default:
		assert (false);
		break;
	}
}



// k     =  1 / Q
// f0_fs = f0 / fs
void	Svf2p::conv_poles (float &g0, float &g1, float &g2, double f0_fs, float k) noexcept
{
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



void	Svf2p::approx_s1s2 (fstb::Vf32 &s1, fstb::Vf32 &s2, fstb::Vf32 f0_fs) noexcept
{
	f0_fs += f0_fs;
	s1 = fstb::Approx::sin_rbj_halfpi (f0_fs);
	s2 = fstb::Approx::sin_rbj_pi (f0_fs);
}



void	Svf2p::conv_poles (fstb::Vf32 &g0, fstb::Vf32 &g1, fstb::Vf32 &g2, fstb::Vf32 f0_fs, fstb::Vf32 k) noexcept
{
	fstb::Vf32     s1;
	fstb::Vf32     s2;
	approx_s1s2 (s1, s2, f0_fs);
	const auto     two       = fstb::ToolsSimd::set1_f32 (2);
	const auto     minus_two = fstb::ToolsSimd::set1_f32 (-2);
	const auto     nrm = fstb::ToolsSimd::rcp_approx2 (two + k * s2);
	const auto     s12 = s1 * s1;
	g0  =                        s2  * nrm;
	g1  = (minus_two * s12 - k * s2) * nrm;
	g2  =        two * s12           * nrm;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
