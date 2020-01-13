/*****************************************************************************

        RankSelA.cpp
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

#include "mfx/dsp/fir/RankSelA.h"

#include <cassert>
#include <climits>



namespace mfx
{
namespace dsp
{
namespace fir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	RankSelA::reserve (int len)
{
	assert (len > 0);

	_elt_arr.reserve (len);
}



// x = filling value
// rank is reset to the middle element
// Content is preserved if possible
void	RankSelA::set_len (int len, float x)
{
	assert (len > 0);

	const int      len_old = int (_elt_arr.size ());

	if (len < len_old)
	{
		const int      last_elt = _index_cur - len;
		int            pos_dst  = 0;
		for (int pos_src = 0; pos_src < len_old && pos_dst < len; ++pos_src)
		{
			Element &      elt = _elt_arr [pos_src];
			if (elt._index >= last_elt)
			{
				_elt_arr [pos_dst] = elt;
				++ pos_dst;
			}
		}
		assert (pos_dst == len);
	}

	_elt_arr.resize (len);

	if (len > len_old)
	{
		int            pos_src = len_old - 1;
		int            idx_ins = _index_cur - 1 - len_old;
		for (int pos_dst = len - 1; pos_dst >= 0 && pos_src < pos_dst; --pos_dst)
		{
			if (pos_src >= 0 && x >= _elt_arr [pos_src]._val)
			{
				_elt_arr [pos_dst] = _elt_arr [pos_src];
				-- pos_src;
			}
			else
			{
				_elt_arr [pos_dst] = Element ({ x, idx_ins });
				-- idx_ins;
			}
		}
	}

	_rank = len >> 1;
}



void	RankSelA::set_rank (int rank)
{
	assert (rank >= 0);
	assert (rank < int (_elt_arr.size ()));

	_rank = rank;
}



void	RankSelA::fill (float x)
{
	const int      len = int (_elt_arr.size ());
	for (int pos = 0; pos < len; ++pos)
	{
		Element &      elt = _elt_arr [pos];
		elt._index = _index_cur - 1 - pos;
		elt._val   = x;
	}
}



float	RankSelA::process_sample (float x)
{
	insert_new_remove_old (x);
	check_index ();

	return _elt_arr [_rank]._val;
}



float	RankSelA::get_nth (int rank) const
{
	assert (rank >= 0);
	assert (rank < int (_elt_arr.size ()));

	return _elt_arr [rank]._val;
}



void	RankSelA::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		insert_new_remove_old (src_ptr [pos]);
		dst_ptr [pos] = _elt_arr [_rank]._val;
	}

	check_index ();
}



void	RankSelA::clear_buffers ()
{
	_index_cur = 0;
	fill (0);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	RankSelA::insert_new_remove_old (float x)
{
	const int      len      = int (_elt_arr.size ());
	const int      del_idx  = _index_cur - len;
	int            pos_ins  = -1;
	int            pos_del  = -1;

	for (int pos = 0; pos < len && (pos_ins < 0 || pos_del < 0); ++pos)
	{
		const Element &   elt = _elt_arr [pos];
		if (elt._index <= del_idx)
		{
			assert (pos_del < 0);
			pos_del = pos;
		}
		if (elt._val >= x && pos_ins < 0)
		{
			pos_ins = pos;
		}
	}
	assert (pos_del >= 0);
	if (pos_ins < 0)
	{
		pos_ins = len;
	}

	if (pos_del < pos_ins)
	{
		-- pos_ins;
		for (int pos = pos_del; pos < pos_ins; ++pos)
		{
			_elt_arr [pos] = _elt_arr [pos + 1];
		}
	}
	else if (pos_del > pos_ins)
	{
		for (int pos = pos_del; pos > pos_ins; --pos)
		{
			_elt_arr [pos] = _elt_arr [pos - 1];
		}
	}

	_elt_arr [pos_ins] = Element ({ x, _index_cur });

	++ _index_cur;
}



void	RankSelA::check_index ()
{
	static const int  big_index = INT_MAX / 2;

	if (_index_cur >= big_index)
	{
		for (auto &elt : _elt_arr)
		{
			elt._index -= big_index;
		}
		_index_cur -= big_index;
	}
}



}  // namespace fir
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
