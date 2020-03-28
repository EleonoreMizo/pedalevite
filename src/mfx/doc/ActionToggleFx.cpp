/*****************************************************************************

        ActionToggleFx.cpp
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

#include "mfx/doc/ActionToggleFx.h"
#include "mfx/doc/SerRInterface.h"
#include "mfx/doc/SerWInterface.h"

#include <cassert>



namespace mfx
{
namespace doc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ActionToggleFx::ActionToggleFx (const FxId &fx_id)
:	_fx_id (fx_id)
{
	assert (fx_id._type == PiType_MAIN);
}



ActionToggleFx::ActionToggleFx (SerRInterface &ser)
:	_fx_id (FxId::LocType_LABEL, "", PiType_MAIN)
{
	ser_read (ser);
}



void	ActionToggleFx::ser_write (SerWInterface &ser) const
{
	ser.begin_list ();

	_fx_id.ser_write (ser);

	ser.end_list ();
}



void	ActionToggleFx::ser_read (SerRInterface &ser)
{
	ser.begin_list ();

	_fx_id.ser_read (ser);

	ser.end_list ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ActionType	ActionToggleFx::do_get_type () const
{
	return ActionType_TOGGLE_FX;
}



std::shared_ptr <PedalActionSingleInterface>	ActionToggleFx::do_duplicate () const
{
	return std::static_pointer_cast <PedalActionSingleInterface> (
		std::make_shared <ActionToggleFx> (*this)
	);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace doc
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
