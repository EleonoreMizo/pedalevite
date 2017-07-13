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
#include "mfx/doc/SerRInterface.h"
#include "mfx/doc/SerWInterface.h"

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



bool	CtrlLinkSet::is_empty () const
{
	return (_bind_sptr.get () == 0 && _mod_arr.empty ());
}



bool	CtrlLinkSet::is_similar (const CtrlLinkSet &other) const
{
	bool           same_flag = (_bind_sptr->is_similar (*other._bind_sptr));
	const size_t   nbr_mod   = _mod_arr.size ();
	same_flag &= (nbr_mod == other._mod_arr.size ());
	for (size_t index = 0; index < nbr_mod && same_flag; ++index)
	{
		same_flag = (_mod_arr [index]->is_similar (*(other._mod_arr [index])));
	}

	return same_flag;
}



void	CtrlLinkSet::ser_write (SerWInterface &ser) const
{
	ser.begin_list ();

	ser.begin_list ();
	if (_bind_sptr.get () != 0)
	{
		_bind_sptr->ser_write (ser);
	}
	ser.end_list ();

	ser.begin_list ();
	for (const auto &l_sptr : _mod_arr)
	{
		l_sptr->ser_write (ser);
	}
	ser.end_list ();

	ser.end_list ();
}



void	CtrlLinkSet::ser_read (SerRInterface &ser)
{
	ser.begin_list ();

	int            nbr_elt;
	ser.begin_list (nbr_elt);
	if (nbr_elt == 1)
	{
		_bind_sptr = LinkSPtr (new CtrlLink);
		_bind_sptr->ser_read (ser);
	}
	else
	{
		assert (nbr_elt == 0);
		_bind_sptr.reset ();
	}
	ser.end_list ();

	ser.begin_list (nbr_elt);
	_mod_arr.resize (nbr_elt);
	for (auto &l_sptr : _mod_arr)
	{
		l_sptr = LinkSPtr (new CtrlLink);;
		l_sptr->ser_read (ser);
	}
	ser.end_list ();

	ser.end_list ();
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
