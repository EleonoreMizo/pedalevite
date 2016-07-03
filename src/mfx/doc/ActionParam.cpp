/*****************************************************************************

        ActionParam.cpp
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

#include "mfx/doc/ActionParam.h"
#include "mfx/doc/SerRInterface.h"
#include "mfx/doc/SerWInterface.h"

#include <cassert>



namespace mfx
{
namespace doc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ActionParam::ActionParam (const FxId &fx_id, int index, float val)
:	_fx_id (fx_id)
,	_index (index)
,	_val (val)
{
	// Nothing
}



ActionParam::ActionParam (SerRInterface &ser)
:	_fx_id (FxId::LocType_LABEL, "", PiType_MAIN)
{
	ser_read (ser);
}



void	ActionParam::ser_write (SerWInterface &ser) const
{
	ser.begin_list ();

	_fx_id.ser_write (ser);
	ser.write (_index);
	ser.write (_val);

	ser.end_list ();
}



void	ActionParam::ser_read (SerRInterface &ser)
{
	ser.begin_list ();

	_fx_id.ser_read (ser);
	ser.read (_index);
	ser.read (_val);

	ser.end_list ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ActionType	ActionParam::do_get_type () const
{
	return ActionType_PARAM;
}



PedalActionSingleInterface *	ActionParam::do_duplicate () const
{
	return new ActionParam (*this);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace doc
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
