/*****************************************************************************

        RankSelL.cpp
        Author: Laurent de Soras, 2019

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

#include "mfx/dsp/fir/RankSelL.h"

#include <cassert>
#include <climits>



namespace mfx
{
namespace dsp
{
namespace fir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	RankSelL::reserve (int len)
{
	assert (len > 0);

	_node_list.reserve (len);
}



// x = filling value
// Content is lost
// rank is reset to the middle element
void	RankSelL::set_len (int len, float x)
{
	assert (len > 0);

	_node_list.resize (len);
	_rank = len >> 1;

	fill (x);
}



void	RankSelL::set_rank (int rank)
{
	assert (rank >= 0);
	assert (rank < int (_node_list.size ()));

	_rank = rank;
}



void	RankSelL::fill (float x)
{
	const int      len = int (_node_list.size ());

	// Links in the reverse order to emulate newest values inserted front
	_ni_first = len - 1;
	_ni_last  = 0;
	for (int ni = 0; ni < len; ++ni)
	{
		Node &         node = _node_list [ni];
		node._val = x;
		if (ni == _ni_last)
		{
			node._next = _nil;
		}
		else
		{
			node._next = ni - 1;
		}
		if (ni == _ni_first)
		{
			node._prev = _nil;
		}
		else
		{
			node._prev = ni + 1;
		}
	}

	_ni_w    = 0;
	_ni_rank = len - 1 - _rank;
}



float	RankSelL::process_sample (float x)
{
	insert_new_remove_old (x);

	return _node_list [_ni_rank]._val;
}



float	RankSelL::get_nth (int rank) const
{
	assert (rank >= 0);
	assert (rank < int (_node_list.size ()));

	/*** To do: optimize by searching both directions ***/
	int            ni  = _ni_first;
	int            pos = 0;
	if (rank >= _rank)
	{
		ni  = _ni_rank;
		pos = _rank;
	}
	while (pos < rank)
	{
		ni = _node_list [ni]._next;
		++ pos;
	}

	return _node_list [ni]._val;
}



void	RankSelL::process_block (float dst_ptr [], float src_ptr [], int nbr_spl)
{
	assert (dst_ptr != 0);
	assert (src_ptr != 0);
	assert (nbr_spl > 0);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		dst_ptr [pos] = process_sample (src_ptr [pos]);
	}
}



void	RankSelL::clear_buffers ()
{
	fill (0);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// If the new value already exists in the list, it is always inserted before.
// So when the value is removed, it is always the last from the set.
void	RankSelL::insert_new_remove_old (float x)
{
	const int      len = int (_node_list.size ());
	if (len == 1)
	{
		_node_list.front ()._val = x;
		return;
	}

	// len is assumed >= 2 here.

	float          rank_val   = _node_list [_ni_rank]._val;
	const float    rem_val    = _node_list [_ni_w]._val;

	// At each operation, indicates the cumulative shift of the rank
	int            rank_shift = 0;

	// Checks if we're going to delete the rank
	if (_ni_w == _ni_rank)
	{
		// Keeps _ni_rank valid
		if (_ni_rank == _ni_last)
		{
			_ni_rank = _node_list [_ni_rank]._prev;
			assert (_ni_rank != _nil);
			-- rank_shift;
		}
		else
		{
			_ni_rank = _node_list [_ni_rank]._next;
			assert (_ni_rank != _nil);
			++ rank_shift;
		}

		// Updates its value
		rank_val = _node_list [_ni_rank]._val;
	}
	assert (_rank + rank_shift >= 0);
	assert (_rank + rank_shift < len);

	// Deletes the node from the list
	if (_ni_w == _ni_first)
	{
		_ni_first = _node_list [_ni_w]._next;
		assert (_ni_first != _nil);
		_node_list [_ni_first]._prev = _nil;
	}
	else
	{
		_node_list [_node_list [_ni_w]._prev]._next = _node_list [_ni_w]._next;
	}
	if (_ni_w == _ni_last)
	{
		_ni_last = _node_list [_ni_w]._prev;
		assert (_ni_last != _nil);
		_node_list [_ni_last]._next = _nil;
	}
	else
	{
		_node_list [_node_list [_ni_w]._next]._prev = _node_list [_ni_w]._prev;
	}
	if (rem_val < rank_val)
	{
		-- rank_shift;
	}
	assert (_rank + rank_shift >= 0);
	assert (_rank + rank_shift < len - 1);

	// At this point _ni_w is not part of the list anymore. We can use if as
	// a free node.
	_node_list [_ni_w]._val = x;

	// Finds the insertion point: prev < inserted <= current.
	// _nil = insert after _ni_last
	int            _ni_ins =
		  (rank_val < x)
		? _node_list [_ni_rank]._next
		: _ni_first;
	while (_ni_ins != _nil && _node_list [_ni_ins]._val < x)
	{
		_ni_ins = _node_list [_ni_ins]._next;
	}

	// Inserts the node
	if (_ni_ins == _ni_first)
	{
		// At the beginning
		_node_list [_ni_w    ]._next = _ni_first;
		_node_list [_ni_w    ]._prev = _nil;
		_node_list [_ni_first]._prev = _ni_w;
		_ni_first = _ni_w;
		++ rank_shift;
	}
	else if (_ni_ins == _nil)
	{
		// At the end
		_node_list [_ni_w    ]._next = _nil;
		_node_list [_ni_w    ]._prev = _ni_last;
		_node_list [_ni_last ]._next = _ni_w;
		_ni_last = _ni_w;
	}
	else
	{
		// Somewhere else in the list
		const int      ni_prev = _node_list [_ni_ins]._prev;
		_node_list [_ni_w    ]._prev = ni_prev;
		_node_list [_ni_w    ]._next = _ni_ins;
		_node_list [_ni_ins  ]._prev = _ni_w;
		_node_list [ni_prev  ]._next = _ni_w;

		if (rank_val >= x)
		{
			++ rank_shift;
		}
	}
	assert (_rank + rank_shift >= 0);
	assert (_rank + rank_shift < len);

	// Updates the rank
	while (rank_shift < 0)
	{
		_ni_rank = _node_list [_ni_rank]._next;
		assert (_ni_rank != _nil);
		++ rank_shift;
	}
	while (rank_shift > 0)
	{
		_ni_rank = _node_list [_ni_rank]._prev;
		assert (_ni_rank != _nil);
		-- rank_shift;
	}

	// Gets ready for the next sample
	++ _ni_w;
	if (_ni_w >= len)
	{
		_ni_w = 0;
	}
}



// We cannot assume _ni_rank is valid here.
int	RankSelL::find_ni (int rank)
{
	int            ni  = _nil;
	const int      len = int (_node_list.size ());

	if ((rank >> 1) > len)
	{
		ni = _ni_last;
		++ rank;
		while (rank < len)
		{
			ni = _node_list [ni]._prev;
			assert (ni != _nil);
			++ rank;
		}
	}
	else
	{
		ni = _ni_first;
		while (rank > 0)
		{
			ni = _node_list [ni]._next;
			assert (ni != _nil);
			-- rank;
		}
	}

	return ni;
}



// Debugging stuff
void	RankSelL::check_ok ()
{
	const int      len = int (_node_list.size ());
	assert (len > 0);
	assert (_ni_first != _nil);
	assert (_ni_last != _nil);
	assert (_ni_rank != _nil);
	assert (_ni_w != _nil);
	assert (_rank >= 0);
	assert (_rank < len);

	int            ni_fw = _ni_first;
	int            ni_bk = _ni_last;
	float          val   = _node_list [_ni_first]._val;
	for (int pos = 0; pos < len; ++pos)
	{
		assert (ni_fw != _nil);
		assert (ni_bk != _nil);
		if (pos == _rank)
		{
			assert (ni_fw == _ni_rank);
		}
		if (pos == len - 1)
		{
			assert (ni_fw == _ni_last);
			assert (ni_bk == _ni_first);
		}
		assert (_node_list [ni_fw]._val >= val);
		val   = _node_list [ni_fw]._val;
		ni_fw = _node_list [ni_fw]._next;
		ni_bk = _node_list [ni_bk]._prev;
	}
	assert (ni_fw == _nil);
	assert (ni_bk == _nil);
}



}  // namespace fir
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
