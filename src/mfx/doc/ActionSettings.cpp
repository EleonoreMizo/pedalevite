/*****************************************************************************

        ActionSettings.cpp
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

#include "mfx/doc/ActionSettings.h"
#include "mfx/doc/SerRInterface.h"
#include "mfx/doc/SerWInterface.h"

#include <cassert>



namespace mfx
{
namespace doc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ActionSettings::ActionSettings (const FxId &fx_id, bool relative_flag, int val)
:	_fx_id (fx_id)
,	_relative_flag (relative_flag)
,	_val (val)
{
	// Nothing
}



ActionSettings::ActionSettings (SerRInterface &ser)
:	_fx_id (FxId::LocType_LABEL, "", PiType_MAIN)
{
	ser_read (ser);
}



void	ActionSettings::ser_write (SerWInterface &ser) const
{
	ser.begin_list ();

	_fx_id.ser_write (ser);
	ser.write (_relative_flag);
	ser.write (_val);

	ser.end_list ();
}



void	ActionSettings::ser_read (SerRInterface &ser)
{
	ser.begin_list ();

	_fx_id.ser_read (ser);
	ser.read (_relative_flag);
	ser.read (_val);

	ser.end_list ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ActionType	ActionSettings::do_get_type () const
{
	return ActionType_SETTINGS;
}



PedalActionSingleInterface *	ActionSettings::do_duplicate () const
{
	return new ActionSettings (*this);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace doc
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
