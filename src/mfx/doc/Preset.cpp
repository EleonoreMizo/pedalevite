/*****************************************************************************

        Preset.cpp
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

#include "mfx/doc/Preset.h"
#include "mfx/doc/SerRInterface.h"
#include "mfx/doc/SerWInterface.h"

#include <cassert>



namespace mfx
{
namespace doc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Preset::Preset (const Preset &other)
:	_slot_list (other._slot_list)
,	_name (other._name)
,	_layout (other._layout)
{
	duplicate_slot_list ();
}



Preset &	Preset::operator = (const Preset &other)
{
	if (&other != this)
	{
		_slot_list = other._slot_list;
		_name      = other._name;
		_layout    = other._layout;
		duplicate_slot_list ();
	}

	return *this;
}



bool	Preset::is_slot_empty (int index) const
{
	assert (index >= 0);
	assert (index < int (_slot_list.size ()));

	return (   _slot_list [index].get () == 0
	        || _slot_list [index]->is_empty ());
}



void	Preset::ser_write (SerWInterface &ser) const
{
	ser.begin_list ();

	ser.write (_name);
	_layout.ser_write (ser);

	ser.begin_list ();
	for (const auto &s_sptr : _slot_list)
	{
		s_sptr->ser_write (ser);
	}
	ser.end_list ();

	ser.end_list ();
}



void	Preset::ser_read (SerRInterface &ser)
{
	ser.begin_list ();

	ser.read (_name);
	_layout.ser_read (ser);

	int            nbr_elt;
	ser.begin_list (nbr_elt);
	_slot_list.resize (nbr_elt);
	for (auto &s_sptr : _slot_list)
	{
		s_sptr = SlotSPtr (new Slot);
		s_sptr->ser_read (ser);
	}
	ser.end_list ();

	ser.end_list ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Preset::duplicate_slot_list ()
{
	for (SlotSPtr &slot_sptr : _slot_list)
	{
		if (slot_sptr.get () != 0)
		{
			slot_sptr = SlotSPtr (new Slot (*slot_sptr));
		}
	}
}



}  // namespace doc
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
