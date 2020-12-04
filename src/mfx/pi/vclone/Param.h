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
#if ! defined (mfx_pi_vclone_Param_HEADER_INCLUDED)
#define mfx_pi_vclone_Param_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace vclone
{



enum Param
{

	Param_MODE = 0,
	Param_STRN,
	Param_CHN,
	Param_GMIN,
	Param_GMAX,
	Param_ATK,
	Param_RLS,
	Param_HOLD,

	Param_NBR_ELT

}; // enum Param



}  // namespace vclone
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/vclone/Param.hpp"



#endif   // mfx_pi_vclone_Param_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
