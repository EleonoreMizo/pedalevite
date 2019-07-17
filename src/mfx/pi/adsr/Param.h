/*****************************************************************************

        Param.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_adsr_Param_HEADER_INCLUDED)
#define mfx_pi_adsr_Param_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace adsr
{



enum Param
{
	Param_TRIG_ON = 0,
	Param_TRIG_OFF,
	Param_VELO_SENS,
	Param_ATK_T,
	Param_DCY_T,
	Param_SUS_T,
	Param_SUS_L,
	Param_RLS_T,

	Param_NBR_ELT

}; // enum Param



}  // namespace adsr
}  // namespace pi
}  // namespace mfx



#endif   // mfx_pi_adsr_Param_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
