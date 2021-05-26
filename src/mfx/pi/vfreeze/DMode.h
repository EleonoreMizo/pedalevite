/*****************************************************************************

        DMode.h
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_vfreeze_DMode_HEADER_INCLUDED)
#define mfx_pi_vfreeze_DMode_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace vfreeze
{



enum DMode
{

   DMode_MIX = 0,
   DMode_CUT,
   DMode_MUTE,

   DMode_NBR_ELT

}; // enum DMode



}  // namespace vfreeze
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/vfreeze/DMode.hpp"



#endif   // mfx_pi_vfreeze_DMode_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
