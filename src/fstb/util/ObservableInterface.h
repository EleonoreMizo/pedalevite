/*****************************************************************************

        ObservableInterface.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (fstb_util_ObservableInterface_HEADER_INCLUDED)
#define fstb_util_ObservableInterface_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace fstb
{
namespace util
{



class ObserverInterface;

class ObservableInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               ObservableInterface ()                        = default;
	               ObservableInterface (const ObservableInterface &other) = default;
	               ObservableInterface (ObservableInterface &&other)      = default;
	virtual        ~ObservableInterface ()                       = default;

	virtual ObservableInterface &
	               operator = (const ObservableInterface &other) = default;
	virtual ObservableInterface &
	               operator = (ObservableInterface &&other)      = default;

	inline void		add_observer (ObserverInterface &observer);
	inline void		remove_observer (ObserverInterface &observer);
	inline void		notify_observers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual void	do_add_observer (ObserverInterface &observer) = 0;
	virtual void	do_remove_observer (ObserverInterface &observer) = 0;
	virtual void	do_notify_observers () = 0;



}; // class ObservableInterface



}  // namespace util
}  // namespace fstb



#include "fstb/util/ObservableInterface.hpp"



#endif   // fstb_util_ObservableInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
