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
	

	/*** To do ***/
	assert (false);


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
