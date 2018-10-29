/*****************************************************************************

        Param.h
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_testgen_Param_HEADER_INCLUDED)
#define mfx_pi_testgen_Param_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace testgen
{



enum Param
{

	Param_STATE = 0, // On (start), Off
	Param_TYPE,      // Pink noise, White noise, Tone, Sweep, Pulse
	Param_LVL,
	Param_NZ_COR,
	Param_TONE_FRQ,
	Param_SWEEP_DUR,
	Param_LOOP_PAUSE,

	Param_NBR_ELT

}; // enum Param



}  // namespace testgen
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/testgen/Param.hpp"



#endif   // mfx_pi_testgen_Param_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
