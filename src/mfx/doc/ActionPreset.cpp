/*****************************************************************************

        ActionPreset.cpp
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

#include "mfx/doc/ActionPreset.h"
#include "mfx/doc/SerRInterface.h"
#include "mfx/doc/SerWInterface.h"

#include <cassert>



namespace mfx
{
namespace doc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ActionPreset::ActionPreset (bool relative_flag, int val)
:	_relative_flag (relative_flag)
,	_val (val)
{
	// Nothing
}



ActionPreset::ActionPreset (SerRInterface &ser)
{
	ser_read (ser);
}



void	ActionPreset::ser_write (SerWInterface &ser) const
{
	ser.begin_list ();

	ser.write (_relative_flag);
	ser.write (_val);

	ser.end_list ();
}



void	ActionPreset::ser_read (SerRInterface &ser)
{
	ser.begin_list ();

	ser.read (_relative_flag);
	ser.read (_val);

	ser.end_list ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ActionType	ActionPreset::do_get_type () const
{
	return ActionType_PRESET;
}



PedalActionSingleInterface *	ActionPreset::do_duplicate () const
{
	return new ActionPreset (*this);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace doc
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
