/*****************************************************************************

        SignalPort.cpp
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

#include "mfx/doc/SerRInterface.h"
#include "mfx/doc/SerWInterface.h"
#include "mfx/doc/SignalPort.h"

#include <cassert>



namespace mfx
{
namespace doc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	SignalPort::operator == (const SignalPort &other) const
{
	return _slot_id == other._slot_id && _sig_index == other._sig_index;
}



bool	SignalPort::operator != (const SignalPort &other) const
{
	return ! (*this == other);
}



bool	SignalPort::operator < (const SignalPort &other) const
{
	if (_slot_id < other._slot_id)
	{
		return true;
	}
	else if (_slot_id == other._slot_id)
	{
		return _sig_index < other._sig_index;
	}

	return false;
}



void	SignalPort::ser_write (SerWInterface &ser) const
{
	ser.begin_list ();

	ser.write (_slot_id);
	ser.write (_sig_index);

	ser.end_list ();
}



void	SignalPort::ser_read (SerRInterface &ser)
{
	ser.begin_list ();

	ser.read (_slot_id);
	ser.read (_sig_index);

	ser.end_list ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace doc
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
