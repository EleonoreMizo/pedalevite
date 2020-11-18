/*****************************************************************************

        Param.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_bmp1_Param_HEADER_INCLUDED)
#define mfx_pi_bmp1_Param_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace bmp1
{



enum Param
{

	Param_SUS = 0,
	Param_TONE,
	Param_VOL,

	Param_PREGAIN,
	Param_OVRSPL,

	Param_NBR_ELT

}; // enum Param



}  // namespace bmp1
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/bmp1/Param.hpp"



#endif   // mfx_pi_bmp1_Param_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
