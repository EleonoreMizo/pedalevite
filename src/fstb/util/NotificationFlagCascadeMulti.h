/*****************************************************************************

        NotificationFlagCascadeMulti.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (fstb_util_NotificationFlagCascadeMulti_HEADER_INCLUDED)
#define fstb_util_NotificationFlagCascadeMulti_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlagCascadeMixin.h"
#include "fstb/util/ObservableMultiMixin.h"



namespace fstb
{
namespace util
{



#if defined (_MSC_VER)
	#pragma warning (push)
	#pragma warning (disable : 4250)
#endif // inherits via dominance

class NotificationFlagCascadeMulti
:	public NotificationFlagCascadeMixin
,	public virtual ObservableMultiMixin
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               NotificationFlagCascadeMulti () = default;
	               NotificationFlagCascadeMulti (const NotificationFlagCascadeMulti &other) = default;
	               NotificationFlagCascadeMulti (NotificationFlagCascadeMulti &&other) = default;

						~NotificationFlagCascadeMulti () = default;

	NotificationFlagCascadeMulti &
	               operator = (const NotificationFlagCascadeMulti &other) = default;
	NotificationFlagCascadeMulti &
	               operator = (NotificationFlagCascadeMulti &&other);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

}; // class NotificationFlagCascadeMulti

#if defined (_MSC_VER)
	#pragma warning (pop)
#endif



}  // namespace util
}  // namespace fstb



//#include "fstb/util/NotificationFlagCascadeMulti.hpp"



#endif   // fstb_util_NotificationFlagCascadeMulti_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
