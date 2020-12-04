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
#if ! defined (mfx_pi_verb2_Param_HEADER_INCLUDED)
#define mfx_pi_verb2_Param_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace verb2
{



enum Param
{

	Param_LVL_DRY = 0,
	Param_LVL_EARLY,
	Param_LVL_DFS,

	Param_PREDELAY,

	Param_EARLY_NBR,
	Param_EARLY_DUR,
	Param_EARLY_DCY,
	Param_EARLY_STR,

	Param_DFS_INP_LC,
	Param_DFS_INP_HC,
	Param_DFS_INP_DIF,
	Param_DFS_DECAY,
	Param_DFS_SIZE,
	Param_DFS_PITCH,
	Param_DFS_PMODE,
	Param_DFS_TNK_LC,
	Param_DFS_TNK_HC,
	Param_DFS_TNK_DIF,

	Param_DFS_FREEZE,
	Param_DFS_FLUSH,

	Param_NBR_ELT

}; // enum Param



}  // namespace verb2
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/verb2/Param.hpp"



#endif   // mfx_pi_verb2_Param_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
