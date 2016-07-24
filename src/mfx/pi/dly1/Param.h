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
#if ! defined (mfx_pi_dly1_Param_HEADER_INCLUDED)
#define mfx_pi_dly1_Param_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace dly1
{



enum Param
{

	Param_LVL_IN = 0,
	Param_LVL_OUT,
	Param_L_TIME,
	Param_L_FDBK,
	Param_L_FILTER,
	Param_LINK,
	Param_R_TIME,
	Param_R_FDBK,
	Param_R_FILTER,
	Param_X_FDBK,

	Param_NBR_ELT

}; // enum Param



}  // namespace dly1
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/dly1/Param.hpp"



#endif   // mfx_pi_dly1_Param_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
