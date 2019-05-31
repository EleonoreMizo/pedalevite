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
#if ! defined (mfx_pi_distpwm2_Param_HEADER_INCLUDED)
#define mfx_pi_distpwm2_Param_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/pi/distpwm2/OscType.h"



namespace mfx
{
namespace pi
{
namespace distpwm2
{



enum ParamOsc
{
	ParamOsc_PULSE = 0,
	ParamOsc_LVL,
	ParamOsc_PF,

	ParamOsc_NBR_ELT
};

enum Param
{
	Param_LPF  = 0,
	Param_THR,
	Param_OSC_BASE,
	Param_OSC_STD  = Param_OSC_BASE + OscType_STD  * ParamOsc_NBR_ELT,
	Param_OSC_OCT  = Param_OSC_BASE + OscType_OCT  * ParamOsc_NBR_ELT,
	Param_OSC_SUB1 = Param_OSC_BASE + OscType_SUB1 * ParamOsc_NBR_ELT,
	Param_OSC_SUB2 = Param_OSC_BASE + OscType_SUB2 * ParamOsc_NBR_ELT,

	Param_NBR_ELT  = Param_OSC_BASE + OscType_NBR_ELT * ParamOsc_NBR_ELT

}; // enum Param



}  // namespace distpwm2
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/distpwm2/Param.hpp"



#endif   // mfx_pi_distpwm2_Param_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
