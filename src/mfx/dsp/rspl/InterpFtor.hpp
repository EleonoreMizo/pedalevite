/*****************************************************************************

        InterpFtor.hpp
        Author: Laurent de Soras, 2019

To do: implements parametrized Catmull-Rom splines:
https://en.wikipedia.org/wiki/Centripetal_Catmull%E2%80%93Rom_spline
More for graphics than for audio.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_rspl_InterpFtor_CODEHEADER_INCLUDED)
#define mfx_dsp_rspl_InterpFtor_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/Scale.h"

#if fstb_ARCHI == fstb_ARCHI_ARM
	#include <arm_neon.h>
#endif

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace rspl
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



float	InterpFtor::Hold::operator () (float /*frac_pos*/, const float data []) const
{
	assert (data != nullptr);

	return data [0];
}



float	InterpFtor::Hold::operator () (uint32_t /*frac_pos*/, const float data []) const
{
	assert (data != nullptr);

	return data [0];
}



int32_t	InterpFtor::Hold::operator () (uint32_t /*frac_pos*/, const int16_t data []) const
{
	assert (data != nullptr);

	return data [0];
}



float	InterpFtor::Linear::operator () (float frac_pos, const float data []) const
{
	assert (frac_pos >= 0);
	assert (frac_pos <= 1);
	assert (data != nullptr);

	return data [0] + (data [1] - data [0]) * frac_pos;
}



float	InterpFtor::Linear::operator () (uint32_t frac_pos, const float data []) const
{
	return operator () (frac_pos * float (fstb::TWOPM32), data);
}



// Result is still on a signed 16-bit scale
int32_t	InterpFtor::Linear::operator () (uint32_t frac_pos, const int16_t data []) const
{
	assert (data != nullptr);

#if fstb_ARCHI == fstb_ARCHI_ARM

	const int32_t     x0 = data [0];
	const int32_t     x1 = data [1];
	const int32x2_t   d  = vdup_n_s32 (x1 - x0);
	const int32x2_t   f  = vdup_n_s32 (int32_t (frac_pos >> 1));
	const int32x2_t   r  = vqrdmulh_s32 (d, f);

	return vget_lane_s32 (r, 0) + x0;

#else

	// Unoptimized fallback. Probably slow as hell
	const int32_t  x0  = data [0];
	const int32_t  x1  = data [1];
	const int32_t  dif = x1 - x0;
	const int32_t  y   = int32_t ((int64_t (frac_pos) * int64_t (dif)) >> 32);

	return y + x0;

#endif
}



float	InterpFtor::CubicHermite::operator () (float frac_pos, const float data []) const
{
	assert (frac_pos >= 0);
	assert (frac_pos <= 1);
	assert (data != nullptr);

	const float    xm1 = data [-1];
	const float    x0  = data [ 0];
	const float    x1  = data [ 1];
	const float    x2  = data [ 2];

#if 0

	// Less operations, more dependencies (difficult to parallelize)
   const float    c = (x1 - xm1) * 0.5f;
	const float    v = x0 - x1;
	const float    w = c + v;
   const float    a = w + v + (x2 - x0) * 0.5f;
   const float    b = -w -a;

	return (((a * frac_pos) + b) * frac_pos + c) * frac_pos + x0;

#elif 1

	// Modified version by James McCartney <asynth@io.com>
	const float    c = 0.5f * (x1 - xm1);
	const float    a = 1.5f * (x0 - x1) + 0.5f * (x2 - xm1);
	const float    b = xm1 - x0 + c - a;

	return (((a * frac_pos) + b) * frac_pos + c) * frac_pos + x0;

#else

	// Original matrix-style, less dependencies
	const float    a = 3 * (x0-x1) - xm1 + x2;
	const float    b = 4*x1 + 2*xm1 - 5*x0 - x2;
	const float    c = x1 - xm1;

	return ((((a * frac_pos) + b) * frac_pos + c) * frac_pos * 0.5f + x0;

#endif
}



float	InterpFtor::CubicHermite::operator () (uint32_t frac_pos, const float data []) const
{
	return operator () (frac_pos * float (fstb::TWOPM32), data);
}



// Result is still on a signed 16-bit scale, but can exceed 16-bit capacity
int32_t	InterpFtor::CubicHermite::operator () (uint32_t frac_pos, const int16_t data []) const
{
	const int32_t  xm1 = data [-1];
	const int32_t  x0  = data [ 0];
	const int32_t  x1  = data [ 1];
	const int32_t  x2  = data [ 2];

#if fstb_ARCHI == fstb_ARCHI_ARM

	const int32_t  c = (x1 - xm1) >> 1;
	const int32_t  a = (3 * (x0 - x1) + (x2 - xm1)) >> 1;
	const int32_t  b = (xm1 - x0) + (c - a);

	const int32x2_t   fx = vdup_n_s32 (int32_t (frac_pos >> 1));
	const int32x2_t   ax = vdup_n_s32 (a);
	const int32x2_t   bx = vdup_n_s32 (b);
	const int32x2_t   cx = vdup_n_s32 (c);
	int32x2_t         t  = ax;
	t = vqrdmulh_s32 (t, fx);
	t = vadd_s32 (t, bx);
	t = vqrdmulh_s32 (t, fx);
	t = vadd_s32 (t, cx);
	t = vqrdmulh_s32 (t, fx);

	return vget_lane_s32 (t, 0) + x0;

	// Maybe we could find an optimisation using Estrin's scheme ?
	//   a * f^3 + b * f^2 + c * f + x0
	//    = (a * f^2 + c) * f + (b * f^2 + x0)
	// or = (a * f + b) * f^2 + (c * f + x0)

#else

	// Unoptimized fallback. Probably slow as hell
	const int32_t  c  = x1 - xm1;
	const int32_t  a  = 3 * (x0 - x1) + (x2 - xm1);
	const int32_t  b  = ((xm1 - x0) << 2) + ((c - a) << 1);

	int32_t        v  = a << 2;
	int32_t        f  = int32_t (frac_pos >> 1);
	v = fstb::Scale <32>::mul (v, f) + b;
	v = fstb::Scale <32>::mul (v, f) + c;
	v = fstb::Scale <32>::mul (v, f) + x0;

	return v;

#endif
}



float	InterpFtor::CubicCatmullRom::operator () (float frac_pos, const float data []) const
{
	assert (frac_pos >= 0);
	assert (frac_pos <= 1);
	assert (data != nullptr);

	const float    xm1 = data [-1];
	const float    x0  = data [ 0];
	const float    x1  = data [ 1];
	const float    x2  = data [ 2];

	const float    a3  = -0.5f * xm1 + 1.5f * x0 - 1.5f * x1 + 0.5f * x2;
	const float    a2  =         xm1 - 2.5f * x0 + 2.0f * x1 - 0.5f * x2;
	const float    a1  = -0.5f * xm1                         + 0.5f * x2;
	const float    a0  = x0;

	const float    y   = ((a3 * frac_pos + a2) * frac_pos + a1) * frac_pos + a0;

	return y;
}



float	InterpFtor::CubicCatmullRom::operator () (uint32_t frac_pos, const float data []) const
{
	return operator () (frac_pos * float (fstb::TWOPM32), data);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace rspl
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_rspl_InterpFtor_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
