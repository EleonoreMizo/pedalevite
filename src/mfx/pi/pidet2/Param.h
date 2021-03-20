/*****************************************************************************

        Param.h
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_pidet2_Param_HEADER_INCLUDED)
#define mfx_pi_pidet2_Param_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace pidet2
{



enum Param
{

	Param_FREQ_MIN = 0,
	Param_FREQ_MAX,
	Param_OUTPUT,

	Param_NBR_ELT

}; // enum Param



}  // namespace pidet2
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/pidet2/Param.hpp"



#endif   // mfx_pi_pidet2_Param_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
