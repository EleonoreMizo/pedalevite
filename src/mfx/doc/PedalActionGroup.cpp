/*****************************************************************************

        PedalActionGroup.cpp
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

#include "mfx/doc/PedalActionGroup.h"
#include "mfx/doc/SerRInterface.h"
#include "mfx/doc/SerWInterface.h"

#include <cassert>



namespace mfx
{
namespace doc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	PedalActionGroup::is_empty_default () const
{
	bool           empty_flag = true;
	for (size_t pos = 0; pos < _action_arr.size () && empty_flag; ++pos)
	{
		empty_flag = _action_arr [pos].is_empty_default ();
	}

	return empty_flag;
}



void	PedalActionGroup::ser_write (SerWInterface &ser) const
{
	ser.begin_list ();

	ser.begin_list ();
	for (const auto &c : _action_arr)
	{
		c.ser_write (ser);
	}
	ser.end_list ();

	ser.end_list ();
}



void	PedalActionGroup::ser_read (SerRInterface &ser)
{
	ser.begin_list ();

	int            nbr_elt;
	ser.begin_list (nbr_elt);
	assert (nbr_elt == int (_action_arr.size ()));
	for (auto &c : _action_arr)
	{
		c.ser_read (ser);
	}
	ser.end_list ();

	ser.end_list ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace doc
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
