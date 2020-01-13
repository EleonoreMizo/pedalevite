/*****************************************************************************

        ObservableSingleMixin.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (fstb_util_ObservableSingleMixin_HEADER_INCLUDED)
#define fstb_util_ObservableSingleMixin_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"fstb/util/ObservableInterface.h"



namespace fstb
{
namespace util
{



class ObservableSingleMixin
:	public virtual ObservableInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	inline void    remove_single_observer ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// ObservableInterface
	inline void    do_add_observer (ObserverInterface &observer) override;
	inline void    do_remove_observer (ObserverInterface &observer) override;
	inline void    do_notify_observers () override;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	ObserverInterface *                 // nullptr = no observer
	               _observer_ptr = nullptr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

}; // class ObservableSingleMixin



}  // namespace util
}  // namespace fstb



#include "fstb/util/ObservableSingleMixin.hpp"



#endif   // fstb_util_ObservableSingleMixin_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
