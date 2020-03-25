/*****************************************************************************

        FltMode.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_moog1_FltMode_HEADER_INCLUDED)
#define mfx_pi_moog1_FltMode_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace moog1
{



enum FltMode
{

   FltMode_LP4 = 0,
   FltMode_LP2,
   FltMode_BP4,
   FltMode_BP2,
   FltMode_HP4,
   FltMode_HP2,
   FltMode_N4,
   FltMode_N2,
   FltMode_N2X,
   FltMode_PK,

   FltMode_NBR_ELT

}; // class FltMode



}  // namespace moog1
}  // namespace pi
}  // namespace mfx



#endif   // mfx_pi_moog1_FltMode_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
