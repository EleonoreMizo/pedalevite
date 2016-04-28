/*****************************************************************************

        ObservableSingleMixin.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_util_ObservableSingleMixin_CODEHEADER_INCLUDED)
#define fstb_util_ObservableSingleMixin_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"fstb/util/ObserverInterface.h"



namespace fstb
{
namespace util
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	ObservableSingleMixin::remove_single_observer ()
{
	_observer_ptr = 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	ObservableSingleMixin::do_add_observer (ObserverInterface &observer)
{
	_observer_ptr = &observer;
}



void	ObservableSingleMixin::do_remove_observer (ObserverInterface &observer)
{
	if (&observer == _observer_ptr)
	{
		remove_single_observer ();
	}
}



void	ObservableSingleMixin::do_notify_observers ()
{
	ObserverInterface *	obs_ptr = _observer_ptr;
	if (obs_ptr != 0)
	{
		obs_ptr->update (*this);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace util
}  // namespace fstb



#endif   // fstb_util_ObservableSingleMixin_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
