/*****************************************************************************

        CnxEnd.cpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/doc/CnxEnd.h"
#include "mfx/doc/SerRInterface.h"
#include "mfx/doc/SerWInterface.h"

#include <cassert>



namespace mfx
{
namespace doc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



CnxEnd::CnxEnd (Type type, int slot_id, int pin)
:	_type (type)
,	_slot_id (slot_id)
,	_pin (pin)
{
	assert (type >= 0);
	assert (type < Type_NBR_ELT);
	assert (slot_id >= 0);
	assert (slot_id == 0 || type == Type_NORMAL);
	assert (pin >= 0);
}



void	CnxEnd::set (Type type, int slot_id, int pin)
{
	assert (type >= 0);
	assert (type < Type_NBR_ELT);
	assert (slot_id >= 0);
	assert (slot_id == 0 || type == Type_NORMAL);
	assert (pin >= 0);

	_type    = type;
	_slot_id = slot_id;
	_pin     = pin;
}



void	CnxEnd::ser_write (SerWInterface &ser) const
{
	ser.write (_type);
	ser.write (_slot_id);
	ser.write (_pin);
}



void	CnxEnd::ser_read (SerRInterface &ser)
{
	ser.read (_type);
	ser.read (_slot_id);
	ser.read (_pin);

	assert (is_valid ());
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace doc
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
