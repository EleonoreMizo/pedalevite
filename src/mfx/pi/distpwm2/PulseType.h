/*****************************************************************************

        PulseType.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_distpwm2_PulseType_HEADER_INCLUDED)
#define mfx_pi_distpwm2_PulseType_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace distpwm2
{



enum PulseType
{

	PulseType_RECT = 0,
	PulseType_RAMP,
	PulseType_CYCLE,
	PulseType_SINE,
	PulseType_SAW,

	PulseType_NBR_ELT

}; // enum PulseType



}  // namespace distpwm2
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/distpwm2/PulseType.hpp"



#endif   // mfx_pi_distpwm2_PulseType_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
