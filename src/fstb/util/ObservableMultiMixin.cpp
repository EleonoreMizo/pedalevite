/*****************************************************************************

        ObservableMultiMixin.cpp
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

#include "fstb/util/ObservableMultiMixin.h"
#include "fstb/util/ObserverInterface.h"

#include <algorithm>



namespace fstb
{
namespace util
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name : do_add_observer
Description :
	Add an observer to the list. An observer can be only once in the list ; if
	it is already listed, it won't be added again.
Input/output parameters :
	- observer : reference on the object to notify.
Throws : std::vector related exception
==============================================================================
*/

void	ObservableMultiMixin::do_add_observer (ObserverInterface &observer)
{
	ObserverList::const_iterator end_it = _observer_list.end ();
	ObserverList::const_iterator beg_it = _observer_list.begin ();
	ObserverList::const_iterator obs_it = std::find (beg_it, end_it, &observer);
	if (obs_it == end_it)
	{
		_observer_list.push_back (&observer);
	}
}



/*
==============================================================================
Name : do_remove_observer
Description :
	Remove the observer from the list, if it is listed here.
Input/output parameters :
	- observer : reference on the object to remove.
Throws : std::vector related exception
==============================================================================
*/

void	ObservableMultiMixin::do_remove_observer (ObserverInterface &observer)
{
	ObserverList::iterator end_it = _observer_list.end ();
	ObserverList::iterator beg_it = _observer_list.begin ();
	ObserverList::iterator obs_it = std::find (beg_it, end_it, &observer);
	if (obs_it != end_it)
	{
		_observer_list.erase (obs_it);
	}
}



/*
==============================================================================
Name : do_notify_observers
Description :
	Notify all observers. Order of notification is not specified.
Throws : Depends on the observers
==============================================================================
*/

void	ObservableMultiMixin::do_notify_observers ()
{
	ObserverList::iterator end_it = _observer_list.end ();
	ObserverList::iterator beg_it = _observer_list.begin ();
	std::for_each (beg_it, end_it, NotificationFtor (*this));
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ObservableMultiMixin::NotificationFtor::NotificationFtor (ObservableMultiMixin &subject)
: _subject (subject)
{
	// Nothing
}



void	ObservableMultiMixin::NotificationFtor::operator () (ObserverInterface *observer_ptr) const
{
	observer_ptr->update (_subject);
}



}  // namespace util
}  // namespace fstb



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
