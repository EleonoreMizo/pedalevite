/*****************************************************************************

        Event.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_piapi_Event_HEADER_INCLUDED)
#define mfx_piapi_Event_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/piapi/EventNoteOn.h"
#include "mfx/piapi/EventNoteOff.h"
#include "mfx/piapi/EventParam.h"
#include "mfx/piapi/EventTransport.h"



namespace mfx
{
namespace piapi
{



union Event
{
	EventNoteOn    _note_on;
	EventNoteOff   _note_off;
	EventParam     _param;
	EventTransport _transport;
};



}  // namespace piapi
}  // namespace mfx



//#include "mfx/piapi/Event.hpp"



#endif   // mfx_piapi_Event_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
