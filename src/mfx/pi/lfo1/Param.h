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
#if ! defined (mfx_pi_lfo1_Param_HEADER_INCLUDED)
#define mfx_pi_lfo1_Param_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace lfo1
{



enum Param
{

	Param_SPEED = 0,  // Speed (normal mode) or Period (slow mode)
	Param_AMP,
	Param_WAVEFORM,
	Param_SNH,
	Param_SMOOTH,
	Param_CHAOS,
	Param_PH_DIST_AMT,
	Param_PH_DIST_OFS,
	Param_SIGN,
	Param_POLARITY,
	Param_VAR1,
	Param_VAR2,
	Param_PHASE_SET,

	Param_NBR_ELT

}; // enum Param



}  // namespace lfo1
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/lfo1/Param.hpp"



#endif   // mfx_pi_lfo1_Param_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
