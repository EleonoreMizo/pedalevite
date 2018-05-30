/*****************************************************************************

        Param.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_ramp_Param_HEADER_INCLUDED)
#define mfx_pi_ramp_Param_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace ramp
{



enum Param
{

	Param_TIME = 0,
	Param_AMP,
	Param_CURVE,
	Param_SNH,
	Param_SMOOTH,
	Param_DIR,
	Param_POS,
	Param_DELAY,
	Param_STATE,

	Param_NBR_ELT

}; // enum Param



}  // namespace ramp
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/ramp/Param.hpp"



#endif   // mfx_pi_ramp_Param_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
