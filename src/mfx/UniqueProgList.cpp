/*****************************************************************************

        UniqueProgList.cpp
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

#include "mfx/doc/Setup.h"
#include "mfx/Cst.h"
#include "mfx/ToolsRouting.h"
#include "mfx/UniqueProgList.h"
#include "mfx/View.h"

#include <iterator>

#include <cassert>



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



UniqueProgList::ProgList	UniqueProgList::build (const View &view)
{
	ProgList       prog_list;

	const doc::Setup &   setup = view.use_setup ();

	for (int bank_cnt = 0; bank_cnt < Cst::_nbr_banks; ++bank_cnt)
	{
		const doc::Bank & bank = setup._bank_arr [bank_cnt];

		for (int prog_cnt = 0; prog_cnt < Cst::_nbr_presets_per_bank; ++prog_cnt)
		{
			const doc::Preset &  prog = bank._preset_arr [prog_cnt];
			if (prog.use_routing ().has_slots ())
			{
				bool          eq_flag = false;
				for (auto &coord : prog_list)
				{
					const doc::Preset &  prog_cmp =
						setup._bank_arr [coord._bank]._preset_arr [coord._prog];
					if (is_prog_eq (prog, prog_cmp))
					{
						eq_flag = true;
						break;
					}
				}

				if (! eq_flag)
				{
					prog_list.push_back ({ bank_cnt, prog_cnt });
				}
			}
		}
	}

	return prog_list;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
Possible strategy:

0. First eliminate neutral slots (empty slot, single-pin i/o) from each
program. This step is optional.

1. For each program, build a map with [slot] -> slot_id set.
Requires absolute order on slots.
We have to handle the case where programs have multiple identical slots (hence
a set in the map).
Each slot_id set would contain "similar" slots, not strictly identical to
allow rounding errors and unsignificant deviations.

2. Try to match slots between both programs through these maps (with the same
similarity checks) and unify both maps (2 sets of slot_id per entry)

3. Check if all the connections are equivalent in both graphs.

Note (for information, not required here):
https://en.wikipedia.org/wiki/Graph_isomorphism_problem
*/

bool	UniqueProgList::is_prog_eq (const doc::Preset &lhs, const doc::Preset &rhs) const
{
	SlotMap        slot_map_lhs;
	SlotMap        slot_map_rhs;

	// For each program, builds a map with [slot] -> slot_id set.
	// We have to handle the case where programs have multiple identical slots
	// (hence a set in the map).
	// Each slot_id set contains "similar" slots, not strictly identical to
	// allow rounding errors and unsignificant deviations.
	build_slot_map (slot_map_lhs, lhs);
	build_slot_map (slot_map_rhs, rhs);

	// Tries to match slots between both programs through these maps (with
	// similarity checks) and unifies both maps (2 sets of slot_id per entry)
	SlotSetPairSet slot_set_pair_set;
	bool           id_flag =
		merge_slot_maps (slot_set_pair_set, slot_map_lhs, slot_map_rhs);

	// Checks if all the connections are equivalent in both graphs.
	if (id_flag)
	{
		id_flag = check_connections (slot_set_pair_set, lhs, rhs);
	}

	if (id_flag)
	{


		/*** To do: what else? _port_map? ***/


	}

	return id_flag;
}



void	UniqueProgList::build_slot_map (SlotMap &slot_map, const doc::Preset &prog) const
{
	slot_map.clear ();

	for (const auto &vt_slot : prog._slot_map)
	{
		if (! prog.is_slot_empty (vt_slot))
		{
			const int         slot_id =  vt_slot.first;
			const doc::Slot & slot    = *vt_slot.second;

			bool        ins_flag = false;
			if (! slot_map.empty ())
			{
				// Try with the first element >= and the one just <.
				SlotMap::iterator it = slot_map.lower_bound (slot);
				if (it != slot_map.end ())
				{
					if (it->first.is_similar (slot))
					{
						it->second.insert (slot_id);
						ins_flag = true;
					}
				}
				if (! ins_flag && it != slot_map.begin ())
				{
					it = std::prev (it);
					if (it->first.is_similar (slot))
					{
						it->second.insert (slot_id);
						ins_flag = true;
					}
				}
			}
			if (! ins_flag)
			{
				slot_map [slot].insert (slot_id);
				ins_flag = true;
			}
		}
	}
}



// Returns true is both maps look identical.
// When returning false, slot_map_m is invalid.
bool	UniqueProgList::merge_slot_maps (SlotSetPairSet &slot_set_pair_set, const SlotMap &slot_map_1, const SlotMap &slot_map_2) const
{
	bool           id_flag = (slot_map_1.size () == slot_map_2.size ());

	slot_set_pair_set.clear ();

	SlotMap::const_iterator it_1 = slot_map_1.begin ();
	SlotMap::const_iterator it_2 = slot_map_2.begin ();
	while (id_flag && it_1 != slot_map_1.end ())
	{
		if (   ! it_1->first.is_similar (it_2->first)
		    || it_1->second.size () != it_2->second.size ())
		{
			id_flag = false;
		}

		else
		{
			slot_set_pair_set.insert ({{ it_1->second, it_2->second }});

			++ it_1;
			++ it_2;
		}
	}

	return id_flag;
}



bool	UniqueProgList::check_connections (const SlotSetPairSet &slot_set_pair_set, const doc::Preset &lhs, const doc::Preset &rhs) const
{
	const doc::Routing::CnxSet &  cs_l = lhs.use_routing ()._cnx_audio_set;
	const doc::Routing::CnxSet &  cs_r = rhs.use_routing ()._cnx_audio_set;

	bool           id_flag = (cs_l.size () == cs_r.size ());

	if (id_flag)
	{
		// Builds a more usable map
		std::map <int, SlotIdSet>  slot_map;
		for (const auto &ssp : slot_set_pair_set)
		{
			for (int slot_id_l : ssp [0])
			{
				slot_map [slot_id_l] = ssp [1];
			}
		}

		// Check connections
//		for (const auto &cnx_l : cs_l)
		{


			/*** To do ***/

	
		}
	}

	return id_flag;
}



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
