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
#if ! defined (mfx_pi_scrush_Param_HEADER_INCLUDED)
#define mfx_pi_scrush_Param_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace scrush
{



enum Param
{

	Param_STEP = 0,
	Param_BIAS,
	Param_THR,
	Param_SHAPE,
	Param_DIF,
	Param_LB,
	Param_HB,

	Param_NBR_ELT

}; // enum Param



}  // namespace scrush
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/scrush/Param.hpp"



#endif   // mfx_pi_scrush_Param_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
