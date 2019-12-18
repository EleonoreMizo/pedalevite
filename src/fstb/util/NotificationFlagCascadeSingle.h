/*****************************************************************************

        NotificationFlagCascadeSingle.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (fstb_util_NotificationFlagCascadeSingle_HEADER_INCLUDED)
#define fstb_util_NotificationFlagCascadeSingle_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlagCascadeMixin.h"
#include "fstb/util/ObservableSingleMixin.h"



namespace fstb
{
namespace util
{



#if defined (_MSC_VER)
	#pragma warning (push)
	#pragma warning (disable : 4250)
#endif // inherits via dominance

class NotificationFlagCascadeSingle
:	public NotificationFlagCascadeMixin
,	public virtual ObservableSingleMixin
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	virtual        ~NotificationFlagCascadeSingle () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

}; // class NotificationFlagCascadeSingle

#if defined (_MSC_VER)
	#pragma warning (pop)
#endif



}  // namespace util
}  // namespace fstb



//#include "fstb/util/NotificationFlagCascadeSingle.hpp"



#endif   // fstb_util_NotificationFlagCascadeSingle_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
