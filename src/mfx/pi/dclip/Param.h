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
#if ! defined (mfx_pi_dclip_Param_HEADER_INCLUDED)
#define mfx_pi_dclip_Param_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace dclip
{



enum Param
{

	Param_GAIN = 0,
	Param_CUTOFF,
	Param_SHAPE,

	Param_NBR_ELT

}; // enum Param



}  // namespace dclip
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/dclip/Param.hpp"



#endif   // mfx_pi_dclip_Param_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
