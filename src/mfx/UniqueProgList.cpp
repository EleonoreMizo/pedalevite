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
#include "mfx/UniqueProgList.h"
#include "mfx/View.h"

#include <utility>

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
			if (! prog._slot_map.empty () && ! prog._routing._chain.empty ())
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

	return std::move (prog_list);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	UniqueProgList::is_prog_eq (const doc::Preset &lhs, const doc::Preset &rhs) const
{
	const int      nbr_slots_l = int (lhs._routing._chain.size ());
	const int      nbr_slots_r = int (rhs._routing._chain.size ());

	int            chain_pos_l =  0;
	int            chain_pos_r =  0;
	int            slot_id_l   = -1;
	int            slot_id_r   = -1;
	while (chain_pos_l < nbr_slots_l || chain_pos_r < nbr_slots_r)
	{
		bool           l_flag = false;
		if (chain_pos_l < nbr_slots_l)
		{
			slot_id_l = lhs._routing._chain [chain_pos_l];
			if (lhs.is_slot_empty (slot_id_l))
			{
				++ chain_pos_l;
			}
			else
			{
				l_flag = true;
			}
		}

		bool            r_flag = false;
		if (chain_pos_r < nbr_slots_r)
		{
			slot_id_r = rhs._routing._chain [chain_pos_r];
			if (rhs.is_slot_empty (slot_id_r))
			{
				++ chain_pos_r;
			}
			else
			{
				r_flag = true;
			}
		}

		if (l_flag && r_flag)
		{
			const doc::Slot & slot_l = lhs.use_slot (slot_id_l);
			const doc::Slot & slot_r = rhs.use_slot (slot_id_r);
			if (! is_slot_eq (slot_l, slot_r))
			{
				return false;
			}

			++ chain_pos_l;
			++ chain_pos_r;
		}
		else if (   (l_flag && chain_pos_r >= nbr_slots_r)
		         || (r_flag && chain_pos_l >= nbr_slots_l))
		{
			return false;
		}
	}

	return true;
}



bool	UniqueProgList::is_slot_eq (const doc::Slot &lhs, const doc::Slot &rhs) const
{
	assert (! lhs.is_empty ());
	assert (! rhs.is_empty ());

	if (lhs._pi_model != rhs._pi_model)
	{
		return false;
	}

	for (int type_cnt = 0; type_cnt < PiType_NBR_ELT; ++type_cnt)
	{
		const PiType   type = PiType (type_cnt);
		const doc::PluginSettings &   settings_l =
			lhs.use_settings (type);
		const doc::PluginSettings &   settings_r =
			rhs.use_settings (type);
		if (! settings_l.is_similar (settings_r))
		{
			return false;
		}
	}

	return true;
}



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
