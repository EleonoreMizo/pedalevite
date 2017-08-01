/*****************************************************************************

        Param.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_phase2_Param_HEADER_INCLUDED)
#define mfx_pi_phase2_Param_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/


namespace mfx
{
namespace pi
{
namespace phase2
{



enum Param
{

	Param_SPEED = 0,
	Param_MIX,
	Param_FEEDBACK,
	Param_NBR_STAGES, // This is actually the number of pairs of stages
	Param_FREQ_MIN,
	Param_FREQ_MAX,
	Param_FDBK_POS,
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



}  // namespace phase2
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/phase2/Param.hpp"



#endif   // mfx_pi_phase2_Param_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
