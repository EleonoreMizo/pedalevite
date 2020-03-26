/*****************************************************************************

        Shape.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_dclip_Shape_HEADER_INCLUDED)
#define mfx_pi_dclip_Shape_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace dclip
{



enum Shape
{

   Shape_SYM_H = 0,
   Shape_ASYM_H,
   Shape_SYM_S,
   Shape_ASYM_S,

   Shape_NBR_ELT

}; // enum Shape



}  // namespace dclip
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/dclip/Shape.hpp"



#endif   // mfx_pi_dclip_Shape_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
