/*****************************************************************************

        PedalActionCycle.cpp
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

#include "mfx/doc/ActionBank.h"
#include "mfx/doc/ActionClick.h"
#include "mfx/doc/ActionParam.h"
#include "mfx/doc/ActionPreset.h"
#include "mfx/doc/ActionSettings.h"
#include "mfx/doc/ActionTempo.h"
#include "mfx/doc/ActionTempoSet.h"
#include "mfx/doc/ActionToggleFx.h"
#include "mfx/doc/ActionToggleTuner.h"
#include "mfx/doc/PedalActionCycle.h"
#include "mfx/doc/SerRInterface.h"
#include "mfx/doc/SerWInterface.h"
#include "mfx/Cst.h"

#include <algorithm>
#include <utility>

#include <cassert>



namespace mfx
{
namespace doc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



PedalActionCycle::PedalActionCycle ()
:	_cycle ()
,	_inherit_flag (true)
,	_overridable_flag (true)
,	_reset_on_pc_flag (true)
{
	// Nothing
}



PedalActionCycle::PedalActionCycle (const PedalActionCycle &other)
:	_cycle (duplicate_actions (other._cycle))
,	_inherit_flag (other._inherit_flag)
,	_overridable_flag (other._overridable_flag)
,	_reset_on_pc_flag (other._reset_on_pc_flag)
{
	// Nothing
}



PedalActionCycle::PedalActionCycle (PedalActionCycle &&other)
:	_cycle (std::move (other._cycle))
,	_inherit_flag (std::move (other._inherit_flag))
,	_overridable_flag (std::move (other._overridable_flag))
,	_reset_on_pc_flag (std::move (other._reset_on_pc_flag))
{
	// Nothing
}



PedalActionCycle &	PedalActionCycle::operator = (const PedalActionCycle &other)
{
	if (&other != this)
	{
		_cycle            = duplicate_actions (other._cycle);
		_inherit_flag     = other._inherit_flag;
		_overridable_flag = other._overridable_flag;
		_reset_on_pc_flag = other._reset_on_pc_flag;
	}

	return *this;
}



PedalActionCycle &	PedalActionCycle::operator = (PedalActionCycle &&other)
{
	if (&other != this)
	{
		_cycle            = std::move (other._cycle);
		_inherit_flag     = std::move (other._inherit_flag);
		_overridable_flag = std::move (other._overridable_flag);
		_reset_on_pc_flag = std::move (other._reset_on_pc_flag);
	}

	return *this;
}



bool	PedalActionCycle::is_empty_default () const
{
	return (_cycle.empty () && _inherit_flag && _overridable_flag && _reset_on_pc_flag);
}



void	PedalActionCycle::merge_cycle (const PedalActionCycle &other)
{
	if (   _cycle.empty ()
	    || (_overridable_flag && ! other._cycle.empty ()))
	{
		_cycle = duplicate_actions (other._cycle);
	}
	else if (_inherit_flag && ! other._cycle.empty ())
	{
		/*** To do: change the algorithm for cycles of different lengths.
		We should repeat the shortest cycle, especially when it is made
		of a single element. ***/
		ActionCycle    tmp (duplicate_actions (other._cycle));
		const int      cycle_size_cur = int (_cycle.size ());
		const int      cycle_size_new = int (tmp.size ());
		const int      cycle_size_add = std::min (cycle_size_cur, cycle_size_new);
		for (int pos = 0; pos < cycle_size_add; ++pos)
		{
			if (pos < cycle_size_cur)
			{
				_cycle [pos].insert (
					_cycle [pos].end (),
					tmp [pos].begin (),
					tmp [pos].end ()
				);
			}
		}
		if (cycle_size_new > cycle_size_cur)
		{
			_cycle.insert (
				_cycle.end (),
				tmp.begin () + cycle_size_cur,
				tmp.end ()
			);
		}

		if (other._reset_on_pc_flag)
		{
			_reset_on_pc_flag = true;
		}
	}
}



void	PedalActionCycle::ser_write (SerWInterface &ser) const
{
	ser.begin_list ();

	ser.write (_inherit_flag);
	ser.write (_overridable_flag);
	if (Cst::_format_version >= 5)
	{
		ser.write (_reset_on_pc_flag);
	}

	ser.begin_list ();
	for (const auto &aa : _cycle)
	{
		ser.begin_list ();
		for (const auto &a_sptr : aa)
		{
			ser.begin_list ();
			const ActionType  type = a_sptr->get_type ();
			ser.write (type);
			a_sptr->ser_write (ser);
			ser.end_list ();
		}
		ser.end_list ();
	}
	ser.end_list ();

	ser.end_list ();
}



void	PedalActionCycle::ser_read (SerRInterface &ser)
{
	const int      version = ser.get_doc_version ();

	ser.begin_list ();

	ser.read (_inherit_flag);
	ser.read (_overridable_flag);

	_reset_on_pc_flag = true;
	if (version >= 5)
	{
		ser.read (_reset_on_pc_flag);
	}

	int            nbr_elt;
	ser.begin_list (nbr_elt);
	_cycle.resize (nbr_elt);
	for (auto &aa : _cycle)
	{
		int            nbr_p;
		ser.begin_list (nbr_p);
		aa.resize (nbr_p);
		for (auto &a_sptr : aa)
		{
			int            chk;
			ser.begin_list (chk);
			assert (chk == 2);
			ActionType     type = ActionType_NBR_ELT; // Any illegal value
			ser.read (type);
			switch (type)
			{
			case ActionType_PRESET:
				a_sptr = std::make_shared <ActionPreset> (ser);
				break;
			case ActionType_TOGGLE_FX:
				a_sptr = std::make_shared <ActionToggleFx> (ser);
				break;
			case ActionType_TOGGLE_TUNER:
				a_sptr = std::make_shared <ActionToggleTuner> (ser);
				break;
			case ActionType_PARAM:
				a_sptr = std::make_shared <ActionParam> (ser);
				break;
			case ActionType_TEMPO:
				a_sptr = std::make_shared <ActionTempo> (ser);
				break;
			case ActionType_BANK:
				a_sptr = std::make_shared <ActionBank> (ser);
				break;
			case ActionType_SETTINGS:
				a_sptr = std::make_shared <ActionSettings> (ser);
				break;
			case ActionType_TEMPO_SET:
				a_sptr = std::make_shared <ActionTempoSet> (ser);
				break;
			case ActionType_CLICK:
				a_sptr = std::make_shared <ActionClick> (ser);
				break;

			case ActionType_LOOP_REC:
			case ActionType_LOOP_PLAY_STOP:
			case ActionType_LOOP_UNDO:
			case ActionType_EVENT:
			default:
				assert (false);
			}
			ser.end_list ();
		}
		ser.end_list ();
	}
	ser.end_list ();

	ser.end_list ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



PedalActionCycle::ActionCycle	PedalActionCycle::duplicate_actions (const ActionCycle &cycle)
{
	ActionCycle    lhs;

	for (const ActionArray &action_arr : cycle)
	{
		ActionArray    array_new;
		for (ActionSPtr action_sptr : action_arr)
		{
			if (action_sptr.get () == nullptr)
			{
				assert (false);
			}
			else
			{
				action_sptr = ActionSPtr (action_sptr->duplicate ());
			}
			array_new.push_back (action_sptr);
		}
		assert (! array_new.empty ());
		lhs.push_back (array_new);
	}

	return lhs;
}



}  // namespace doc
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
