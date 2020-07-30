/*****************************************************************************

        XFadeShape.cpp
        Author: Laurent de Soras, 2017

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
#include "fstb/fnc.h"
#include "fstb/ToolsSimd.h"
#include "mfx/dsp/wnd/XFadeShape.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace wnd
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	XFadeShape::set_duration (double duration, float fade_ratio)
{
	assert (duration > 0);
	assert (fade_ratio > 0);
	assert (fade_ratio <= 1);

	if (duration != _duration || fade_ratio != _fade_ratio)
	{
		_duration  = duration;
		_fade_ratio = fade_ratio;
		if (is_ready ())
		{
			make_shape ();
		}
	}
}



void	XFadeShape::set_sample_freq (double sample_freq)
{
	assert (sample_freq > 0);

	_sample_freq = sample_freq;
	make_shape ();
}



bool	XFadeShape::is_ready () const
{
	return (_sample_freq > 0);
}



int	XFadeShape::get_len () const
{
	assert (_len > 0);

	return _len;
}



const float *	XFadeShape::use_shape () const
{
	assert (_len > 0);

	return (&_shape [0]);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	XFadeShape::make_shape ()
{
	const int      len = fstb::round_int (_sample_freq * _duration);
	if (len != _len)
	{
		_len = len;

		// +3 because we could read (or write) a full vector from the last
		// position
		const int      len_margin = len + 3;
		_shape.resize (len_margin);

#if 1

		const float    p    = 0.25f / _fade_ratio;
		fstb::ToolsSimd::VectF32   x;
		fstb::ToolsSimd::VectF32   step;
		fstb::ToolsSimd::start_lerp (x, step, -p, p, len);
		const auto     half = fstb::ToolsSimd::set1_f32 ( 0.5f );
		const auto     mi   = fstb::ToolsSimd::set1_f32 (-0.25f);
		const auto     ma   = fstb::ToolsSimd::set1_f32 (+0.25f);
		for (int pos = 0; pos < len; pos += 4)
		{
			auto           xx = x;
			xx = fstb::ToolsSimd::min_f32 (xx, ma);
			xx = fstb::ToolsSimd::max_f32 (xx, mi);
			auto           v  = fstb::Approx::sin_nick_2pi (xx);
			v *= half;
			v += half;
			fstb::ToolsSimd::store_f32 (&_shape [pos], v);

			x += step;
		}

#else // Reference implementation

		const float    p    = 0.25f / _fade_ratio;
		const float    dif  = p * 2;
		const float    step = dif * fstb::rcp_uint <float> (len);
		const float    x    = -p;

		for (int pos = 0; pos < len; ++pos)
		{
			const float    xx = fstb::limit (x, -0.25f, +0.25f);
			const float    v  = fstb::Approx::sin_nick_2pi (xx) * 0.5f + 0.5f;
			_shape [pos] = v;

			x += step;
		}

#endif
	}
}



}  // namespace wnd
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
