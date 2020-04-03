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
#if ! defined (mfx_pi_smood_Param_HEADER_INCLUDED)
#define mfx_pi_smood_Param_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace smood
{



enum Param
{

	Param_SPEED = 0,

	Param_WIDTH,
	Param_DEPTH,
	Param_MIX,
	Param_FEEDBACK,
	Param_STDIF,
	Param_CHNCROSS,
	Param_PAN,

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



}  // namespace smood
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/smood/Param.hpp"



#endif   // mfx_pi_smood_Param_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
