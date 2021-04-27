/*****************************************************************************

        Button.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_Button_HEADER_INCLUDED)
#define mfx_uitk_Button_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace uitk
{



enum Button
{
	Button_INVALID = -1,

	Button_S = 0, // Select
	Button_E,     // Escape
	Button_U,     // Up
	Button_D,     // Down
	Button_L,     // Left
	Button_R,     // Right
	Button_SHL,   // Left shift
	Button_SHR,   // Right shift

	Button_NBR_ELT

}; // enum Button



}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/Button.hpp"



#endif   // mfx_uitk_Button_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
