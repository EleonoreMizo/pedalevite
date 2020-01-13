/*****************************************************************************

        ObservableMultiMixin.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (fstb_util_ObservableMultiMixin_HEADER_INCLUDED)
#define fstb_util_ObservableMultiMixin_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/ObservableInterface.h"

#include <vector>



namespace fstb
{
namespace util
{



class ObservableMultiMixin
:	public virtual ObservableInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               ObservableMultiMixin ()                             = default;
	               ObservableMultiMixin (const ObservableMultiMixin &other) = default;
	               ObservableMultiMixin (ObservableMultiMixin &&other) = default;

	               ~ObservableMultiMixin ()                            = default;

	ObservableMultiMixin &
	               operator = (const ObservableMultiMixin &other)      = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// ObservableInterface
	void	            do_add_observer (ObserverInterface &observer) override;
	void	            do_remove_observer (ObserverInterface &observer) override;
	void	            do_notify_observers () override;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::vector <ObserverInterface *> ObserverList;

	class NotificationFtor
	{
	public:
		               NotificationFtor (ObservableMultiMixin &subject);
		void           operator () (ObserverInterface *observer_ptr) const;

	private:
		ObservableMultiMixin &
		               _subject;
	};

	ObserverList   _observer_list;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	ObservableMultiMixin &
	               operator = (ObservableMultiMixin &&other) = delete;

}; // class ObservableMultiMixin



}  // namespace util
}  // namespace fstb



//#include "fstb/util/ObservableMultiMixin.hpp"



#endif   // fstb_util_ObservableMultiMixin_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
