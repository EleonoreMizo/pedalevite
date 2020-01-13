/*****************************************************************************

        ActionClick.cpp
        Author: Laurent de Soras, 2017

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

#include "mfx/doc/ActionClick.h"
#include "mfx/doc/SerRInterface.h"
#include "mfx/doc/SerWInterface.h"

#include <cassert>



namespace mfx
{
namespace doc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ActionClick::ActionClick (Mode mode, bool bar_flag)
:	_mode (mode)
,	_bar_flag (bar_flag)
{
	assert (mode >= 0);
	assert (mode < Mode_NBR_ELT);
}



ActionClick::ActionClick (SerRInterface &ser)
{
	ser_read (ser);
}



void	ActionClick::ser_write (SerWInterface &ser) const
{
	ser.begin_list ();

	ser.write (_mode);
	ser.write (_bar_flag);

	ser.end_list ();
}



void	ActionClick::ser_read (SerRInterface &ser)
{
	ser.begin_list ();

	ser.read (_mode);
	ser.read (_bar_flag);

	ser.end_list ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ActionType	ActionClick::do_get_type () const
{
	return ActionType_CLICK;
}



std::shared_ptr <PedalActionSingleInterface>	ActionClick::do_duplicate () const
{
	return std::static_pointer_cast <PedalActionSingleInterface> (
		std::make_shared <ActionClick> (*this)
	);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace doc
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
