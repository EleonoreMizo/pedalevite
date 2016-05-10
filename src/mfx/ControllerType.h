/*****************************************************************************

        ControllerType.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_ControllerType_HEADER_INCLUDED)
#define mfx_ControllerType_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/ui/UserInputType.h"



namespace mfx
{



enum ControllerType
{
	ControllerType_INVALID      = -1,

	ControllerType_UIT_ZONE_BEG = 0,
	ControllerType_UIT_ZONE_LST = ui::UserInputType_NBR_ELT - 1,

	ControllerType_MIDI,
	ControllerType_SIG_PROBE,
	ControllerType_LFO,
	ControllerType_SEQ,

	ControllerType_NBR_ELT

}; // enum ControllerType



}  // namespace mfx



//#include "mfx/ControllerType.hpp"



#endif   // mfx_ControllerType_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
