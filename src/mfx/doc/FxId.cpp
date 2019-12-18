/*****************************************************************************

        FxId.cpp
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

#include "mfx/doc/FxId.h"
#include "mfx/doc/SerRInterface.h"
#include "mfx/doc/SerWInterface.h"

#include <cassert>



namespace mfx
{
namespace doc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



FxId::FxId (LocType loc_type, std::string label, PiType type)
:	_location_type (loc_type)
,	_label_or_model (label)
,	_type (type)
{
	assert (loc_type >= 0);
	assert (loc_type < LocType_NBR_ELT);
	assert (type >= 0);
	assert (type < PiType_NBR_ELT);
}



bool	FxId::is_valid () const
{
	return (_location_type != LocType_INVALID);
}



void	FxId::ser_write (SerWInterface &ser) const
{
	assert (is_valid ());

	ser.begin_list ();

	ser.write (_location_type);
	ser.write (_label_or_model);
	ser.write (_type);

	ser.end_list ();
}



void	FxId::ser_read (SerRInterface &ser)
{
	ser.begin_list ();

	ser.read (_location_type);
	ser.read (_label_or_model);
	ser.read (_type);

	ser.end_list ();
}




bool	FxId::operator == (const FxId &other) const
{
	assert (is_valid ());
	assert (other.is_valid ());

	return (   _location_type  == other._location_type
	        && _label_or_model == other._label_or_model
	        && _type           == other._type);
}



bool	FxId::operator != (const FxId &other) const
{
	return ! (*this == other);
}




/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace doc
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
