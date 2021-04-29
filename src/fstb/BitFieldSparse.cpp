/*****************************************************************************

        BitFieldSparse.cpp
        Author: Laurent de Soras, 2006

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

#include "fstb/BitFieldTools.h"
#include "fstb/BitFieldSparse.h"
#include "fstb/Err.h"
#include "fstb/fnc.h"

#include <algorithm>

#include <cassert>



namespace fstb
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



BitFieldSparse::BitFieldSparse ()
:	_lvl_arr ()
{
	set_nbr_elt (0);
}



BitFieldSparse::BitFieldSparse (int nbr_elt)
:	_lvl_arr ()
{
	assert (nbr_elt >= 0);

	set_nbr_elt (nbr_elt);
}



// All bits are cleared
void	BitFieldSparse::set_nbr_elt (int nbr_elt)
{
	assert (nbr_elt >= 0);

	typedef	BitFieldTools <GroupType, BITDEPTH_L2>	Tools;

	int            nbr_elt_l2 = 0;
	if (nbr_elt > 0)
	{
		nbr_elt_l2 = get_next_pow_2 (uint32_t (nbr_elt));
	}
	int				nbr_lvl = (nbr_elt_l2 + BITDEPTH_L2 - 1) / BITDEPTH_L2;
	nbr_lvl = std::max (nbr_lvl, 1);

	// Leaves the object in a safe state, even if an allocation fails and
	// throws.
	{
		LevelArray		lvl_arr_tmp (nbr_lvl);

		for (int lvl_index = 0; lvl_index < nbr_lvl; ++lvl_index)
		{
			BfLevel &		lvl = lvl_arr_tmp [lvl_index];

			lvl._nbr_elt = nbr_elt;

			int            nbr_groups = 0;
			if (nbr_elt > 0)
			{
				nbr_groups = Tools::calculate_nbr_groups (nbr_elt);
			}
			assert (nbr_groups >= 0);
			lvl._group_arr.resize (nbr_groups);

			nbr_elt = nbr_groups;
		}

		_lvl_arr.swap (lvl_arr_tmp);
	}

	clear ();
}



int	BitFieldSparse::get_nbr_elt () const noexcept
{
	return _lvl_arr [0]._nbr_elt;
}



void	BitFieldSparse::clear () noexcept
{
	typedef	BitFieldTools <GroupType, BITDEPTH_L2>	Tools;

	const int		nbr_lvl = int (_lvl_arr.size ());
	for (int lvl_index = 0; lvl_index < nbr_lvl; ++lvl_index)
	{
		BfLevel &		lvl = _lvl_arr [lvl_index];

		if (lvl._nbr_elt > 0)
		{
			const int      nbr_groups = Tools::calculate_nbr_groups (lvl._nbr_elt);
			const auto     it_beg     = lvl._group_arr.begin ();
			std::fill (it_beg, it_beg + nbr_groups, GroupType (0));
		}
	}
}



void	BitFieldSparse::fill () noexcept
{
	typedef	BitFieldTools <GroupType, BITDEPTH_L2>	Tools;

	const int		nbr_lvl = int (_lvl_arr.size ());
	for (int lvl_index = 0; lvl_index < nbr_lvl; ++lvl_index)
	{
		BfLevel &		lvl = _lvl_arr [lvl_index];

		if (lvl._nbr_elt > 0)
		{
			Tools::activate_range (&lvl._group_arr [0], 0, lvl._nbr_elt);
		}
	}
}



bool	BitFieldSparse::get_bit (int pos) const noexcept
{
	assert (pos >= 0);
	assert (pos < get_nbr_elt ());

	typedef	BitFieldTools <GroupType, BITDEPTH_L2>	Tools;

	const BfLevel& lvl = _lvl_arr [0];
	const bool     bit = Tools::get_bit (&lvl._group_arr [0], pos);

	return bit;
}



void	BitFieldSparse::set_bit (int pos, bool flag) noexcept
{
	assert (pos >= 0);
	assert (pos < get_nbr_elt ());

	if (flag)
	{
		fill_bit (pos);
	}
	else
	{
		clear_bit (pos);
	}
}



void	BitFieldSparse::clear_bit (int pos) noexcept
{
	assert (pos >= 0);
	assert (pos < get_nbr_elt ());

	typedef	BitFieldTools <GroupType, BITDEPTH_L2>	Tools;

	const int      nbr_lvl = int (_lvl_arr.size ());
	for (int lvl_index = 0; lvl_index < nbr_lvl; ++lvl_index)
	{
		BfLevel &      lvl = _lvl_arr [lvl_index];

		int            group;
		GroupType      mask;
		Tools::calculate_group_and_mask (group, mask, pos);

		GroupType		val = lvl._group_arr [group];
		val &= ~mask;
		lvl._group_arr [group] = val;
		// To be valid, the extra bits of the last group (after the meaningful bits) should be cleared.
		if (val == 0)
		{
			pos = group;
		}
		else
		{
			lvl_index = nbr_lvl;	// Stops here
		}
	}
}



void	BitFieldSparse::fill_bit (int pos) noexcept
{
	assert (pos >= 0);
	assert (pos < get_nbr_elt ());

	typedef	BitFieldTools <GroupType, BITDEPTH_L2>	Tools;

	const int		nbr_lvl = int (_lvl_arr.size ());
	for (int lvl_index = 0; lvl_index < nbr_lvl; ++lvl_index)
	{
		BfLevel &		lvl = _lvl_arr [lvl_index];

		int            group;
		GroupType      mask;
		Tools::calculate_group_and_mask (group, mask, pos);

		const GroupType	val = lvl._group_arr [group];
		lvl._group_arr [group] = val | mask;
		// To be valid, the extra bits of the last group (after the meaningful bits) should be cleared.
		if (val == 0)
		{
			pos = group;
		}
		else
		{
			lvl_index = nbr_lvl;	// Stops here
		}
	}
}



void	BitFieldSparse::activate_range (int pos, int nbr_elt) noexcept
{
	assert (pos >= 0);
	assert (pos < get_nbr_elt ());
	assert (nbr_elt > 0);
	assert (pos + nbr_elt <= get_nbr_elt ());


	/*** To do: fast implementation ***/


	// Naive implementation, in the meantime
	for (int cnt = 0; cnt < nbr_elt; ++cnt)
	{
		fill_bit (pos + cnt);
	}
}



void	BitFieldSparse::deactivate_range (int pos, int nbr_elt) noexcept
{
	assert (pos >= 0);
	assert (pos < get_nbr_elt ());
	assert (nbr_elt > 0);
	assert (pos + nbr_elt <= get_nbr_elt ());


	/*** To do: fast implementation ***/


	// Naive implementation, in the meantime
	for (int cnt = 0; cnt < nbr_elt; ++cnt)
	{
		clear_bit (pos + cnt);
	}
}



// Returns Err_NOT_FOUND if not found
// If found, returns something in [pos, stop_pos[.
int	BitFieldSparse::get_next_bit_set_from (int pos, int stop_pos) const noexcept
{
	assert (pos >= 0);
	assert (pos < get_nbr_elt ());
	assert (stop_pos <= get_nbr_elt ());

	if (stop_pos < 0)
	{
		stop_pos = get_nbr_elt ();
	}

	const int      on_pos = get_next_bit_set_from_rec (0, pos, stop_pos);

	return on_pos;
}



bool	BitFieldSparse::has_a_bit_set () const noexcept
{
	bool           bit_flag = false;

	if (get_nbr_elt () > 0)
	{
		const int      last_lvl_index = int (_lvl_arr.size ()) - 1;
		const BfLevel& last_lvl       = _lvl_arr [last_lvl_index];
		assert (last_lvl._group_arr.size () == 1);
		bit_flag = (last_lvl._group_arr [0] != 0);
	}

	return bit_flag;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	BitFieldSparse::get_next_bit_set_from_rec (int lvl_index, int pos, int stop) const noexcept
{
	assert (lvl_index >= 0);
	assert (lvl_index < int (_lvl_arr.size ()));
	assert (pos >= 0);
	assert (pos < _lvl_arr [lvl_index]._nbr_elt);
	assert (stop <= _lvl_arr [lvl_index]._nbr_elt);

	typedef	BitFieldTools <GroupType, BITDEPTH_L2>	Tools;

	const BfLevel&	lvl = _lvl_arr [lvl_index];

	if (lvl_index < int (_lvl_arr.size ()) - 1)
	{
		// Finishes the current group to be ready to start the search on the next level
		const int      group_end = (pos + BITDEPTH - 1) & ~(BITDEPTH - 1);
		if (pos < group_end)
		{
			const int      local_stop = std::min (group_end, stop);
			pos = Tools::get_next_bit_set_from (
				&lvl._group_arr [0],
				pos,
				local_stop
			);

			if (pos < 0 && group_end < stop)
			{
				pos = group_end;
			}
			else
			{
				return pos; // Could be "found" or "not found"
			}
		}

		// Searches on the next level
		int		      gpos;
		int            group_start;
		Tools::calculate_group_and_pos (group_start, gpos, pos);
		assert (gpos == 0);
		const int      group_stop = Tools::calculate_nbr_groups (stop);
		const int      group     = get_next_bit_set_from_rec (
			lvl_index + 1,
			group_start,
			group_stop
		);
		if (group < 0)
		{
			return Err_NOT_FOUND;
		}

		pos = group << BITDEPTH_L2;
		assert (pos < stop);
	}

	// Searches on this level
	pos = Tools::get_next_bit_set_from (&lvl._group_arr [0], pos, stop);

	return pos;
}



}  // namespace fstb



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
