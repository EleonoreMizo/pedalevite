/*****************************************************************************

        SharedRscState.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_SharedRscState_HEADER_INCLUDED)
#define mfx_SharedRscState_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{



enum SharedRscState
{

	SharedRscState_FREE = 0,   // Empty slot
	SharedRscState_INUSE,      // Created and potentially used by the audio thread
	SharedRscState_RECYCLING,  // Scheduled for destruction by the main thread, message sent to the audio thread. Resource can be destroyed when the audio thread acknowledges.

	SharedRscState_NBR_ELT

}; // enum SharedRscState



}  // namespace mfx



//#include "mfx/SharedRscState.hpp"



#endif   // mfx_SharedRscState_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
