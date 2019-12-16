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
,	_buf_info_list ()
{
	clear ();
}



void	BufAlloc::clear ()
{
	_free_index = _reserved_size;
	for (int i = _reserved_size; i < int (_buf_list.size ()); ++i)
	{
		_buf_list [i] = i;
		_buf_info_list [i]._buf_list_pos = i;
		_buf_info_list [i]._use_count    = 0;
	}
}



bool	BufAlloc::has_room () const
{
	return (_free_index < int (_buf_list.size ()));
}



void	BufAlloc::use_more (int buf, int use_count)
{
	assert (buf >= _reserved_size);
	assert (buf < Cst::_max_nbr_buf);
	assert (use_count > 0);

	const int      pos = find_buf_pos (buf);
	if (pos < 0)
	{
		assert (false);
	}
	else
	{
		assert (_buf_info_list [buf]._use_count > 0);
		_buf_info_list [buf]._use_count += use_count;
	}
}



void	BufAlloc::use_more_if_std (int buf, int use_count)
{
	if (buf >= _reserved_size)
	{
		use_more (buf, use_count);
	}
}



int	BufAlloc::alloc (int use_count)
{
	assert (has_room ());
	assert (use_count > 0);

	const int      buf = _buf_list [_free_index];
	assert (_buf_info_list [buf]._use_count == 0);
	_buf_info_list [buf]._use_count += use_count;

	++ _free_index;

	return buf;
}



void	BufAlloc::ret_if_std (int buf, int use_count)
{
	if (buf >= _reserved_size)
	{
		ret (buf, use_count);
	}
}



void	BufAlloc::ret (int buf, int use_count)
{
	assert (buf >= _reserved_size);
	assert (buf < Cst::_max_nbr_buf);
	assert (use_count > 0);

	const int      pos = find_buf_pos (buf);
	if (pos < 0)
	{
		assert (false);
	}
	else
	{
		assert (_buf_info_list [buf]._use_count >= use_count);
		_buf_info_list [buf]._use_count -= use_count;
		if (_buf_info_list [buf]._use_count <= 0)
		{
			assert (_free_index > _reserved_size);
			assert (_buf_info_list [buf]._use_count == 0);

			-- _free_index;
			std::swap (
				_buf_info_list [buf                    ]._buf_list_pos,
				_buf_info_list [_buf_list [_free_index]]._buf_list_pos
			);
			std::swap (_buf_list [pos], _buf_list [_free_index]);
		}
	}
}



int	BufAlloc::get_use_count (int buf) const
{
	assert (buf >= _reserved_size);
	assert (buf < Cst::_max_nbr_buf);

	return _buf_info_list [buf]._use_count;
}



int	BufAlloc::get_nbr_alloc_buf () const
{
	return _free_index - _reserved_size;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	BufAlloc::find_buf_pos (int buf) const
{
	assert (buf >= _reserved_size);
	assert (buf < Cst::_max_nbr_buf);

	assert (_buf_info_list [buf]._use_count > 0);

	return _buf_info_list [buf]._buf_list_pos;
}



}  // namespace cmd
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
