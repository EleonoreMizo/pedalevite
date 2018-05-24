/*****************************************************************************

        BitFieldSparseIterator.hpp
        Author: Laurent de Soras, 2008

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_BitFieldSparseIterator_CODEHEADER_INCLUDED)
#define fstb_BitFieldSparseIterator_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/BitFieldSparse.h"

#include <cassert>



namespace fstb
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



BitFieldSparseIterator::BitFieldSparseIterator (BitFieldSparse &bfs)
:	_bfs (bfs)
,	_bit_index (-1)
,	_pos_end (0)
{
	assert (&bfs != 0);
}



void	BitFieldSparseIterator::start (int pos_start, int pos_end)
{
	assert (pos_start >= 0);
	assert (pos_end < _bfs.get_nbr_elt ());

	if (pos_end < 0)
	{
		_pos_end = _bfs.get_nbr_elt ();
	}
	else
	{
		_pos_end = pos_end;
	}

	if (pos_start >= _pos_end)
	{
		_bit_index = -1;
	}
	else
	{
		_bit_index = _bfs.get_next_bit_set_from (pos_start, _pos_end);
	}
}



bool	BitFieldSparseIterator::is_rem_elt () const
{
	return (_bit_index >= 0);
}



void	BitFieldSparseIterator::iterate ()
{
	assert (is_rem_elt ());

	const int      new_pos = _bit_index + 1;
	if (new_pos >= _pos_end)
	{
		_bit_index = -1;
	}
	else
	{
		_bit_index = _bfs.get_next_bit_set_from (new_pos, _pos_end);
	}
}



int	BitFieldSparseIterator::get_bit_index () const
{
	return _bit_index;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace fstb



#endif   // fstb_BitFieldSparseIterator_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
