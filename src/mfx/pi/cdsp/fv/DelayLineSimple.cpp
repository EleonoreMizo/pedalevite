/*****************************************************************************

        DelayLineSimple.cpp
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

#include "fstb/fnc.h"
#include "mfx/pi/cdsp/fv/DelayLineSimple.h"

#include <algorithm>

#include <cassert>
#include <cstring>



namespace mfx
{
namespace pi
{
namespace cdsp
{
namespace fv
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DelayLineSimple::set_delay (int len)
{
	assert (len > 0);

	_delay     = len;
	_line_size = 1 << fstb::get_next_pow_2 (len * 2); // * 2 because we want to be able to read or write a full delay in any order.
	_line_mask = _line_size - 1;
	_line_data.resize (_line_size);
	if (_write_pos >= _line_size)
	{
		_write_pos = 0;
	}
}



void	DelayLineSimple::clear_buffers ()
{
	memset (&_line_data [0], 0, _line_data.size () * sizeof (_line_data [0]));
	_write_pos = 0;
}



int	DelayLineSimple::get_max_rw_len () const
{
	const int      read_pos = compute_read_pos ();
	const int      len_w    = _line_size - _write_pos;
	const int      len_r    = _line_size -   read_pos;
	const int      max_len  = std::min (std::min (len_w, len_r), _delay);

	return max_len;
}



const float *	DelayLineSimple::use_read_data () const
{
	const int      read_pos = compute_read_pos ();

	return &_line_data [read_pos];
}



float *	DelayLineSimple::use_write_data ()
{
	return &_line_data [_write_pos];
}



void	DelayLineSimple::step (int len)
{
	assert (len > 0);
	assert (len <= _delay);

	_write_pos = (_write_pos + len) & _line_mask;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	DelayLineSimple::compute_read_pos () const
{
	return (_write_pos - _delay) & _line_mask;
}



}  // namespace fv
}  // namespace cdsp
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
