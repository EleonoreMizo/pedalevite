/*****************************************************************************

        PedalboardLayout.cpp
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

#include "mfx/doc/PedalboardLayout.h"
#include "mfx/doc/SerRInterface.h"
#include "mfx/doc/SerWInterface.h"
#include "mfx/doc/ActionBank.h"
#include "mfx/doc/ActionProg.h"
#include "mfx/doc/ActionTempo.h"
#include "mfx/doc/ActionToggleTuner.h"

#include <utility>

#include <cassert>



namespace mfx
{
namespace doc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	PedalboardLayout::merge_layout (const PedalboardLayout &other)
{
	for (size_t pag_index = 0; pag_index < _pedal_arr.size (); ++pag_index)
	{
		PedalActionGroup &         pag_cur =       _pedal_arr [pag_index];
		const PedalActionGroup &   pag_oth = other._pedal_arr [pag_index];

		for (size_t pac_index = 0; pac_index < pag_cur._action_arr.size (); ++pac_index)
		{
			PedalActionCycle &      pac_cur = pag_cur._action_arr [pac_index];
			const PedalActionCycle& pac_oth = pag_oth._action_arr [pac_index];

			pac_cur.merge_cycle (pac_oth);
		}
	}
}



void	PedalboardLayout::ser_write (SerWInterface &ser) const
{
	ser.begin_list ();

	ser.begin_list ();
	bool           empty_flag = true;
	for (size_t pos = 0; pos < _pedal_arr.size () && empty_flag; ++pos)
	{
		empty_flag = _pedal_arr [pos].is_empty_default ();
	}
	if (! empty_flag)
	{
		for (const auto &p : _pedal_arr)
		{
			p.ser_write (ser);
		}
	}
	ser.end_list ();

	ser.end_list ();
}



void	PedalboardLayout::ser_read (SerRInterface &ser)
{
	const int      version = ser.get_doc_version ();

	ser.begin_list ();

	int            nbr_elt;
	ser.begin_list (nbr_elt);
	if (nbr_elt == 0)
	{
		for (auto &p : _pedal_arr)
		{
			p = PedalActionGroup ();
		}
	}
	else
	{
		assert (nbr_elt == int (_pedal_arr.size ()));
		for (auto &p : _pedal_arr)
		{
			p.ser_read (ser);
		}
		if (version < 9)
		{
			// From version 9, pedal rows have been swapped so the first pedal
			// is now on the lowest row (closest to the player).
			const int   row_len = int (_pedal_arr.size () / 2);
			for (int col = 0; col < row_len; ++col)
			{
				std::swap (_pedal_arr [col], _pedal_arr [col + row_len]);
			}
		}
	}
	ser.end_list ();

	ser.end_list ();
}



void	PedalboardLayout::set_default_conf ()
{
	// Programs
	for (int p = 0; p < 7; ++p)
	{
		const int      pedal = (p < 4) ? p + 1 : p + 3;
		PedalActionCycle &  cycle =
			_pedal_arr [pedal]._action_arr [ActionTrigger_PRESS];
		PedalActionCycle::ActionArray   action_arr;
		action_arr.push_back (
			std::static_pointer_cast <PedalActionSingleInterface> (
				std::make_shared <ActionProg> (false, p)
			)
		);
		cycle._cycle.push_back (action_arr);
	}

	// Tuner
	{
		PedalActionCycle &  cycle =
			_pedal_arr [5]._action_arr [ActionTrigger_PRESS];
		PedalActionCycle::ActionArray   action_arr;
		action_arr.push_back (
			std::static_pointer_cast <PedalActionSingleInterface> (
				std::make_shared <ActionToggleTuner> ()
			)
		);
		cycle._cycle.push_back (action_arr);
	}

	// Tempo
	{
		PedalActionCycle &  cycle =
			_pedal_arr [11]._action_arr [ActionTrigger_PRESS];
		PedalActionCycle::ActionArray   action_arr;
		action_arr.push_back (
			std::static_pointer_cast <PedalActionSingleInterface> (
				std::make_shared <ActionTempo> ()
			)
		);
		cycle._cycle.push_back (action_arr);
	}

	// Prog-/Bank-, Prog+/Bank+
	{
		for (int p = 0; p < 2; ++p)
		{
			PedalActionGroup &  group = _pedal_arr [p * 6];
			const int      d = (p == 0) ? -1 : +1;
			{
				PedalActionCycle &  cycle =
					group._action_arr [ActionTrigger_RELEASE];
				PedalActionCycle::ActionArray   action_arr;
				action_arr.push_back (
					std::static_pointer_cast <PedalActionSingleInterface> (
						std::make_shared <ActionProg> (true, d)
					)
				);
				cycle._cycle.push_back (action_arr);
			}
			{
				PedalActionCycle &  cycle =
					group._action_arr [ActionTrigger_HOLD];
				PedalActionCycle::ActionArray   action_arr;
				action_arr.push_back (
					std::static_pointer_cast <PedalActionSingleInterface> (
						std::make_shared <ActionBank> (true, d)
					)
				);
				cycle._cycle.push_back (action_arr);
			}
		}
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace doc
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
