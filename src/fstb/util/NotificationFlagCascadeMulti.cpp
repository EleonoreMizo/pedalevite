/*****************************************************************************

        NotificationFlagCascadeMulti.cpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlagCascadeMulti.h"

#include <cassert>



namespace fstb
{
namespace util
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
Member written to avoid GCC's warning:

defaulted move assignment for
'fstb::util::NotificationFlagCascadeMulti'
calls a non-trivial move assignment operator for virtual base
'fstb::util::ObservableMultiMixin'
[-Wvirtual-move-assign]

From the GCC doc:
   -Wno-virtual-move-assign
   Suppress warnings about inheriting from a virtual base with a non-trivial
C++11 move assignment operator. This is dangerous because if the virtual base
is reachable along more than one path, it is moved multiple times, which can
mean both objects end up in the moved-from state. If the move assignment
operator is written to avoid moving from a moved-from object, this warning
can be disabled.
*/

NotificationFlagCascadeMulti &	NotificationFlagCascadeMulti::operator = (NotificationFlagCascadeMulti &&other)
{
	if (this != &other)
	{
		// The common base class (ObservableInterface) is a pure interface so
		// there is no risk to move data twice.
		NotificationFlagCascadeMixin::operator = (other);
		ObservableMultiMixin::operator = (other);
	}

	return *this;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace util
}  // namespace fstb



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
