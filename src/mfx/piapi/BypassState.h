/*****************************************************************************

        BypassState.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_piapi_BypassState_HEADER_INCLUDED)
#define mfx_piapi_BypassState_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace piapi
{



enum BypassState
{

	BypassState_IGNORE = 0,
	BypassState_ASK,
	BypassState_PRODUCED,

	BypassState_NBR_ELT

}; // enum BypassState



}  // namespace piapi
}  // namespace mfx



//#include "mfx/piapi/BypassState.hpp"



#endif   // mfx_piapi_BypassState_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
