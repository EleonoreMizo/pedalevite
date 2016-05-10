/*****************************************************************************

        NotificationFlagCascadeMixin.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (fstb_util_NotificationFlagCascadeMixin_HEADER_INCLUDED)
#define fstb_util_NotificationFlagCascadeMixin_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlagInterface.h"
#include "fstb/util/ObservableInterface.h"



namespace fstb
{
namespace util
{



class NotificationFlagCascadeMixin
:	public NotificationFlagInterface
,	public virtual ObservableInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               NotificationFlagCascadeMixin ()  = default;
	inline explicit
	               NotificationFlagCascadeMixin (bool state_flag);
	               NotificationFlagCascadeMixin (const NotificationFlagCascadeMixin &other) = default;
	virtual        ~NotificationFlagCascadeMixin () = default;

	NotificationFlagCascadeMixin &
	               operator = (const NotificationFlagCascadeMixin &other) = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:
	// ObserverInterface via NotificationFlagInterface
	virtual inline void
	               do_update (ObservableInterface &subject);

	// NotificationFlagInterface
	virtual inline bool
	               do_get_state () const;
	virtual inline void
	               do_reset ();
	virtual inline void
	               do_set ();




/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           _state_flag = false;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const NotificationFlagCascadeMixin &other) const = delete;
	bool           operator != (const NotificationFlagCascadeMixin &other) const = delete;

}; // class NotificationFlagCascadeMixin



}  // namespace util
}  // namespace fstb



#include "fstb/util/NotificationFlagCascadeMixin.hpp"



#endif   // fstb_util_NotificationFlagCascadeMixin_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
