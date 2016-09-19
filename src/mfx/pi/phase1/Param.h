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
#if ! defined (mfx_pi_phase1_Param_HEADER_INCLUDED)
#define mfx_pi_phase1_Param_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace phase1
{



enum Param
{

	Param_SPEED = 0,
	Param_DEPTH,
	Param_FDBK_LEVEL,
	Param_FDBK_COLOR,
	Param_PHASE_MIX,
	Param_MANUAL,
	Param_PHASE_SET,
	Param_HOLD,
	Param_BPF_CUTOFF,
	Param_BPF_Q,

	Param_NBR_ELT

}; // enum Param



}  // namespace phase1
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/phase1/Param.hpp"



#endif   // mfx_pi_phase1_Param_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
