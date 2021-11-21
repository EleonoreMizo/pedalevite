/*****************************************************************************

        XFadeShape.hpp
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_wnd_XFadeShape_CODEHEADER_INCLUDED)
#define mfx_dsp_wnd_XFadeShape_CODEHEADER_INCLUDED



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



template <typename SHP>
void	XFadeShape <SHP>::set_duration (double duration, float fade_ratio)
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



template <typename SHP>
void	XFadeShape <SHP>::set_sample_freq (double sample_freq)
{
	assert (sample_freq > 0);

	_sample_freq = sample_freq;
	make_shape ();
}



template <typename SHP>
bool	XFadeShape <SHP>::is_ready () const noexcept
{
	return (_sample_freq > 0);
}



template <typename SHP>
int	XFadeShape <SHP>::get_len () const noexcept
{
	assert (_len > 0);

	return _len;
}



template <typename SHP>
const float *	XFadeShape <SHP>::use_shape () const noexcept
{
	assert (_len > 0);

	return (&_shape [0]);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename SHP>
void	XFadeShape <SHP>::make_shape ()
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

		const float    ph   = 0.5f / _fade_ratio;
		fstb::Vf32     x;
		fstb::Vf32     step;
		fstb::ToolsSimd::start_lerp (x, step, 0.5f - ph, 0.5f + ph, len);
		const auto     mi   = fstb::ToolsSimd::set_f32_zero ();
		const auto     ma   = fstb::ToolsSimd::set1_f32 (1);
		for (int pos = 0; pos < len; pos += 4)
		{
			auto           xx = x;
			xx = fstb::ToolsSimd::min_f32 (xx, ma);
			xx = fstb::ToolsSimd::max_f32 (xx, mi);
			auto           v  = SHP::compute_gain (xx) [1];
			fstb::ToolsSimd::store_f32 (&_shape [pos], v);

			x += step;
		}

#else // Reference implementation

		const float    p    = 1.f / _fade_ratio;
		const float    step = p * fstb::rcp_uint <float> (len);
		const float    x    = 0.5f - p * 0.5f;

		for (int pos = 0; pos < len; ++pos)
		{
			const float    xx = fstb::limit (x, 0.0f, 1.0f);
			const float    v  = SHP::compute_gain (xx) [1];
			_shape [pos] = v;

			x += step;
		}

#endif
	}
}



}  // namespace wnd
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_wnd_XFadeShape_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
