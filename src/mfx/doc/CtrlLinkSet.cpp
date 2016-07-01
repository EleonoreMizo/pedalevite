/*****************************************************************************

        CtrlLinkSet.cpp
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

#include "mfx/doc/CtrlLinkSet.h"

#include <cassert>



namespace mfx
{
namespace doc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



CtrlLinkSet::CtrlLinkSet (const CtrlLinkSet &other)
:	_bind_sptr (other._bind_sptr)
,	_mod_arr (other._mod_arr)
{
	duplicate_children ();
}



CtrlLinkSet &  CtrlLinkSet::operator = (const CtrlLinkSet &other)
{
	if (this != &other)
	{
		_bind_sptr = other._bind_sptr;
		_mod_arr   = other._mod_arr;

		duplicate_children ();
	}

	return *this;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	CtrlLinkSet::duplicate_children ()
{
	if (_bind_sptr.get () != 0)
	{
		_bind_sptr = LinkSPtr (new CtrlLink (*_bind_sptr));
	}
	for (auto &mod_sptr :_mod_arr)
	{
		if (mod_sptr.get () != 0)
		{
			mod_sptr = LinkSPtr (new CtrlLink (*mod_sptr));
		}
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace doc
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
