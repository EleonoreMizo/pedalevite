/*****************************************************************************

        BufAlloc.cpp
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

#include "mfx/cmd/BufAlloc.h"

#include <cassert>



namespace mfx
{
namespace cmd
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



BufAlloc::BufAlloc (int reserved_size)
:	_buf_list ()
,	_reserved_size (reserved_size)
,	_free_index (reserved_size)
{
	clear ();
}



void	BufAlloc::clear ()
{
	_free_index = _reserved_size;
	for (int i = _reserved_size; i < int (_buf_list.size ()); ++i)
	{
		_buf_list [i] = i;
	}
}



bool	BufAlloc::has_room () const
{
	return (_free_index < int (_buf_list.size ()));
}



int	BufAlloc::alloc ()
{
	assert (has_room ());

	const int      buf = _buf_list [_free_index];
	++ _free_index;

	return buf;
}



void	BufAlloc::ret (int buf)
{
	assert (buf >= _reserved_size);
	assert (buf < Cst::_max_nbr_buf);
	assert (_free_index > _reserved_size);

	const int      pos = find_buf_pos (buf);
	if (pos < 0)
	{
		assert (false);
	}
	else
	{
		-- _free_index;
		std::swap (_buf_list [pos], _buf_list [_free_index]);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	BufAlloc::find_buf_pos (int buf) const
{
	assert (buf >= _reserved_size);
	assert (buf < Cst::_max_nbr_buf);

	int            found_pos = -1;
	for (int pos = _reserved_size; pos < _free_index && found_pos < 0; ++pos)
	{
		if (_buf_list [pos] == buf)
		{
			found_pos = pos;
		}
	}

	return found_pos;
}



}  // namespace cmd
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
