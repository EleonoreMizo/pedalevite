/*****************************************************************************

        ChnMode.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_vclone_ChnMode_HEADER_INCLUDED)
#define mfx_pi_vclone_ChnMode_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace vclone
{



enum ChnMode
{

   ChnMode_LINK = 0,
   ChnMode_INDEP,
   ChnMode_L,
   ChnMode_R,

   ChnMode_NBR_ELT

}; // enum ChnMode



}  // namespace vclone
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/vclone/ChnMode.hpp"



#endif   // mfx_pi_vclone_ChnMode_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
