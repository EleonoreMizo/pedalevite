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

#include "mfx/doc/PedalActionCycle.h"

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
{
	// Nothing
}



PedalActionCycle::PedalActionCycle (const PedalActionCycle &other)
:	_cycle (duplicate_actions (other._cycle))
,	_inherit_flag (true)
,	_overridable_flag (true)
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
	}

	return *this;
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
		for (int pos = 0; pos < cycle_size_new; ++pos)
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
	}
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
			if (action_sptr.get () == 0)
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

	return std::move (lhs);
}



}  // namespace doc
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
