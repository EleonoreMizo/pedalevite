/*****************************************************************************

        SegmentHelper.cpp
        Author: Laurent de Soras, 2022

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"
#include "mfx/dsp/ana/r128/SegmentHelper.h"

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace ana
{
namespace r128
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	SegmentHelper::set_sample_freq (double sample_freq)
{
	assert (sample_freq > 0);

	_sample_freq = sample_freq;
	update_period ();
}



void	SegmentHelper::set_period (double per_s)
{
	assert (per_s > 0);

	_per_s = per_s;
	if (_sample_freq > 0)
	{
		update_period ();
	}
}



void	SegmentHelper::start (int nbr_spl) noexcept
{
	assert (is_setup ());
	assert (nbr_spl > 0);

	_blk_len = nbr_spl;
	_blk_pos = 0;
	_seg_len = 0;
	compute_seg_len ();
	step_frame ();
}



bool	SegmentHelper::is_rem_elt () const noexcept
{
	assert (is_setup ());

	return (_blk_pos < _blk_len);
}



void	SegmentHelper::iterate () noexcept
{
	assert (is_started ());

	_blk_pos += _seg_len;
	compute_seg_len ();
	step_frame ();
}



int	SegmentHelper::get_seg_len () const noexcept
{
	assert (is_started ());
	assert (_seg_len > 0);

	return _seg_len;
}



// Should be called after get_seg_len() samples have been processed.
bool	SegmentHelper::is_frame_ready () const noexcept
{
	assert (is_started ());

	return (_pos_spl == 0);
}



void	SegmentHelper::clear_buffers () noexcept
{
	_pos_spl = 0;
	_blk_len = 0;
	_blk_pos = 0;
	_seg_len = 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	SegmentHelper::is_setup () const noexcept
{
	return (_sample_freq > 0 && _per_spl > 0);
}



bool	SegmentHelper::is_started () const noexcept
{
	return (is_setup () && _blk_pos < _blk_len);
}



void	SegmentHelper::update_period () noexcept
{
	assert (_sample_freq > 0);

	_per_spl = std::max (fstb::round_int (_sample_freq * _per_s), 1);
	_pos_spl = std::min (_pos_spl, std::max (_per_spl - 1, 0));
}



void	SegmentHelper::compute_seg_len () noexcept
{
	_seg_len = _blk_len - _blk_pos;
	if (_per_spl > 0)
	{
		_seg_len = std::min (_seg_len, _per_spl - _pos_spl);
	}
}



void	SegmentHelper::step_frame () noexcept
{
	_pos_spl += _seg_len;
	if (_pos_spl >= _per_spl)
	{
		_pos_spl = 0;
	}
}



}  // namespace r128
}  // namespace ana
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
