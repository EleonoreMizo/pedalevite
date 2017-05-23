/*****************************************************************************

        DelayLineSimpleMod.cpp
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

#include "fstb/fnc.h"
#include "mfx/pi/fv/DelayLineSimpleMod.h"

#include <algorithm>

#include <cassert>
#include <cstring>



namespace mfx
{
namespace pi
{
namespace fv
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DelayLineSimpleMod::set_delay (int len, int mod_per, int mod_depth)
{
	assert (len > 0);
	assert (mod_per > 0 || (mod_per == 0 && mod_depth == 0));
	assert (mod_depth >= 0);

	_delay     = len;
	// * 2 because we want to be able to read or write a full delay in any order.
	_line_size = 1 << fstb::get_next_pow_2 ((len + mod_depth) * 2);
	_line_mask = _line_size - 1;
	_line_data.resize (_line_size);
	if (_write_pos >= _line_size)
	{
		_write_pos = 0;
	}
	_mod_per   = mod_per;
	_mod_depth = mod_depth;
	_mod_pos_t = _mod_per;
	_mod_pos_d = 0;
	_mod_dir   = 1;
}



void	DelayLineSimpleMod::clear_buffers ()
{
	memset (&_line_data [0], 0, _line_data.size () * sizeof (_line_data [0]));
	_write_pos = 0;
	_mod_pos_t = _mod_per;
	_mod_pos_d = 0;
	_mod_dir   = 1;
}



int	DelayLineSimpleMod::get_max_rw_len () const
{
	const int      dly_cur  = compute_delay ();
	const int      read_pos = compute_read_pos (dly_cur);
	const int      len_w    = _line_size - _write_pos;
	const int      len_r    = _line_size -   read_pos;
	const int      tmp_len1 = std::min (len_w, len_r);
	int            tmp_len2 = dly_cur;
	if (_mod_depth > 0)
	{
		tmp_len2 = std::min (tmp_len2, _mod_pos_t);
	}
	const int      max_len  = std::min (tmp_len1, tmp_len2);

	return max_len;
}



const float *	DelayLineSimpleMod::use_read_data () const
{
	const int      read_pos = compute_read_pos ();

	return &_line_data [read_pos];
}



float *	DelayLineSimpleMod::use_write_data ()
{
	return &_line_data [_write_pos];
}



void	DelayLineSimpleMod::step (int len)
{
	assert (len > 0);
	assert (len <= _delay);
	assert (len <= _mod_pos_t || _mod_depth == 0);

	_write_pos  = (_write_pos + len) & _line_mask;

	if (_mod_depth > 0)
	{
		_mod_pos_t -= len;
		if (_mod_pos_t <= 0)
		{
			if (std::abs (_mod_pos_d) == _mod_depth)
			{
				_mod_dir = -_mod_dir;
			}
			_mod_pos_d += _mod_dir;
			_mod_pos_t = len;
		}
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	DelayLineSimpleMod::compute_read_pos () const
{
	const int      dly_cur = compute_delay ();

	return compute_read_pos (dly_cur);
}



int	DelayLineSimpleMod::compute_read_pos (int dly_cur) const
{
	return (_write_pos - dly_cur) & _line_mask;
}



int	DelayLineSimpleMod::compute_delay () const
{
	return _delay + _mod_pos_d;
}



}  // namespace fv
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
