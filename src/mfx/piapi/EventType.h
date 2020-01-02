/*****************************************************************************

        EventType.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_piapi_EventType_HEADER_INCLUDED)
#define mfx_piapi_EventType_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace piapi
{



enum EventType
{
	EventType_NOTE_ON = 0,
	EventType_NOTE_OFF,
	EventType_PARAM,
	EventType_TRANSPORT,
	EventType_RESET,

	EventType_NBR_ELT
};



}  // namespace piapi
}  // namespace mfx



//#include "mfx/piapi/EventType.hpp"



#endif   // mfx_piapi_EventType_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
