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
#if ! defined (mfx_pi_sfreeze_Param_HEADER_INCLUDED)
#define mfx_pi_sfreeze_Param_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/pi/sfreeze/Cst.h"



namespace mfx
{
namespace pi
{
namespace sfreeze
{


enum ParamSlot
{
	ParamSlot_FREEZE = 0,
	ParamSlot_GAIN,

	ParamSlot_NBR_ELT
};

enum Param
{
	Param_SLOT_BASE = 0,

	Param_XFADE = Param_SLOT_BASE + Cst::_nbr_slots * ParamSlot_NBR_ELT,
	Param_XFGAIN,
	Param_DMODE,
	Param_PHASE,
	Param_CRY_AMT,
	Param_CRY_RAD,
	Param_NBR_ELT

}; // enum Param



}  // namespace sfreeze
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/sfreeze/Param.hpp"



#endif   // mfx_pi_sfreeze_Param_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
