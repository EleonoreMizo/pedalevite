/*****************************************************************************

        ActionTempoSet.cpp
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

#include "mfx/doc/ActionTempoSet.h"
#include "mfx/doc/SerRInterface.h"
#include "mfx/doc/SerWInterface.h"
#include "mfx/Cst.h"

#include <cassert>



namespace mfx
{
namespace doc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ActionTempoSet::ActionTempoSet (double tempo_bpm)
:	_tempo_bpm (tempo_bpm)
{
	assert (tempo_bpm >= Cst::_tempo_min);
	assert (tempo_bpm <= Cst::_tempo_max);


}



ActionTempoSet::ActionTempoSet (SerRInterface &ser)
:	_tempo_bpm (120)
{
	ser_read (ser);
}



void	ActionTempoSet::ser_write (SerWInterface &ser) const
{
	ser.begin_list ();

	ser.write (_tempo_bpm);

	ser.end_list ();
}



void	ActionTempoSet::ser_read (SerRInterface &ser)
{
	ser.begin_list ();

	ser.read (_tempo_bpm);

	ser.end_list ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ActionType	ActionTempoSet::do_get_type () const
{
	return ActionType_TEMPO_SET;
}



PedalActionSingleInterface *	ActionTempoSet::do_duplicate () const
{
	return new ActionTempoSet (*this);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace doc
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
