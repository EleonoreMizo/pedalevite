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
#include <cstring>



namespace fstb
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



BitFieldSparse::BitFieldSparse ()
:	_lvl_arr ()
{
	set_nbr_elt (0);
}



BitFieldSparse::BitFieldSparse (long nbr_elt)
:	_lvl_arr ()
{
	assert (nbr_elt >= 0);

	set_nbr_elt (nbr_elt);
}



// All bits are cleared
void	BitFieldSparse::set_nbr_elt (long nbr_elt)
{
	assert (nbr_elt >= 0);

	typedef	BitFieldTools <GroupType, BITDEPTH_L2>	Tools;

	long				nbr_elt_l2 = 0;
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

			long				nbr_groups = 0;
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



long	BitFieldSparse::get_nbr_elt () const
{
	return (_lvl_arr [0]._nbr_elt);
}



void	BitFieldSparse::clear ()
{
	using namespace std;

	typedef	BitFieldTools <GroupType, BITDEPTH_L2>	Tools;

	const int		nbr_lvl = _lvl_arr.size ();
	for (int lvl_index = 0; lvl_index < nbr_lvl; ++lvl_index)
	{
		BfLevel &		lvl = _lvl_arr [lvl_index];

		if (lvl._nbr_elt > 0)
		{
			const long		nbr_groups = Tools::calculate_nbr_groups (lvl._nbr_elt);
			memset (
				&lvl._group_arr [0],
				0,
				nbr_groups * sizeof (lvl._group_arr [0])
			);
		}
	}
}



// Throws: Nothing
void	BitFieldSparse::fill ()
{
	typedef	BitFieldTools <GroupType, BITDEPTH_L2>	Tools;

	const int		nbr_lvl = _lvl_arr.size ();
	for (int lvl_index = 0; lvl_index < nbr_lvl; ++lvl_index)
	{
		BfLevel &		lvl = _lvl_arr [lvl_index];

		if (lvl._nbr_elt > 0)
		{
			Tools::activate_range (&lvl._group_arr [0], 0, lvl._nbr_elt);
		}
	}
}



// Throws: Nothing
bool	BitFieldSparse::get_bit (long pos) const
{
	assert (pos >= 0);
	assert (pos < get_nbr_elt ());

	typedef	BitFieldTools <GroupType, BITDEPTH_L2>	Tools;

	const BfLevel&	lvl = _lvl_arr [0];
	const bool		bit = Tools::get_bit (&lvl._group_arr [0], pos);

	return (bit);
}



// Throws: Nothing
void	BitFieldSparse::set_bit (long pos, bool flag)
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



// Throws: Nothing
void	BitFieldSparse::clear_bit (long pos)
{
	assert (pos >= 0);
	assert (pos < get_nbr_elt ());

	typedef	BitFieldTools <GroupType, BITDEPTH_L2>	Tools;

	const int		nbr_lvl = _lvl_arr.size ();
	for (int lvl_index = 0; lvl_index < nbr_lvl; ++lvl_index)
	{
		BfLevel &		lvl = _lvl_arr [lvl_index];

		long           group;
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



// Throws: Nothing
void	BitFieldSparse::fill_bit (long pos)
{
	assert (pos >= 0);
	assert (pos < get_nbr_elt ());

	typedef	BitFieldTools <GroupType, BITDEPTH_L2>	Tools;

	const int		nbr_lvl = _lvl_arr.size ();
	for (int lvl_index = 0; lvl_index < nbr_lvl; ++lvl_index)
	{
		BfLevel &		lvl = _lvl_arr [lvl_index];

		long           group;
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



// Throws: Nothing
void	BitFieldSparse::activate_range (long pos, long nbr_elt)
{
	assert (pos >= 0);
	assert (pos < get_nbr_elt ());
	assert (nbr_elt > 0);
	assert (pos + nbr_elt <= get_nbr_elt ());


	/*** To do ***/
	assert (false);

}



// Throws: Nothing
void	BitFieldSparse::deactivate_range (long pos, long nbr_elt)
{
	assert (pos >= 0);
	assert (pos < get_nbr_elt ());
	assert (nbr_elt > 0);
	assert (pos + nbr_elt <= get_nbr_elt ());


	/*** To do ***/
	assert (false);

}



// Returns Err_NOT_FOUND if not found
// If found, returns something in [pos, stop_pos[.
// Throws: Nothing
long	BitFieldSparse::get_next_bit_set_from (long pos, long stop_pos) const
{
	assert (pos >= 0);
	assert (pos < get_nbr_elt ());
	assert (stop_pos <= get_nbr_elt ());

	if (stop_pos < 0)
	{
		stop_pos = get_nbr_elt ();
	}

	const long		on_pos = get_next_bit_set_from_rec (0, pos, stop_pos);

	return (on_pos);
}



bool	BitFieldSparse::has_a_bit_set () const
{
	bool				bit_flag = false;

	if (get_nbr_elt () > 0)
	{
		const int		last_lvl_index = _lvl_arr.size () - 1;
		const BfLevel&	last_lvl = _lvl_arr [last_lvl_index];
		assert (last_lvl._group_arr.size () == 1);
		bit_flag = (last_lvl._group_arr [0] != 0);
	}

	return (bit_flag);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



long	BitFieldSparse::get_next_bit_set_from_rec (int lvl_index, long pos, long stop) const
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
		const long		group_end = (pos + BITDEPTH - 1) & -BITDEPTH;
		if (pos < group_end)
		{
			const long		local_stop = std::min (group_end, stop);
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
				return (pos);	// Could be "found" or "not found"
			}
		}

		// Searches on the next level
		int		      gpos;
		long           group_start;
		Tools::calculate_group_and_pos (group_start, gpos, pos);
		assert (gpos == 0);
		const long		group_stop = Tools::calculate_nbr_groups (stop);
		const long		group = get_next_bit_set_from_rec (
			lvl_index + 1,
			group_start,
			group_stop
		);
		if (group < 0)
		{
			return (Err_NOT_FOUND);
		}

		pos = group << BITDEPTH_L2;
		assert (pos < stop);
	}

	// Searches on this level
	pos = Tools::get_next_bit_set_from (&lvl._group_arr [0], pos, stop);

	return (pos);
}



}  // namespace fstb



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
