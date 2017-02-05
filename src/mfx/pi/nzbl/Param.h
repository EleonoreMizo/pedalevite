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
#if ! defined (mfx_pi_nzbl_Param_HEADER_INCLUDED)
#define mfx_pi_nzbl_Param_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/pi/nzbl/Cst.h"



namespace mfx
{
namespace pi
{
namespace nzbl
{




enum ParamNotch
{
	ParamNotch_FREQ = 0,
	ParamNotch_Q,
	ParamNotch_LVL,

	ParamNotch_NBR_ELT
};

enum Param
{
	Param_LVL = 0,
	Param_OUT,
	Param_BASE_NOTCH,

	Param_NBR_ELT = Param_BASE_NOTCH + ParamNotch_NBR_ELT * Cst::_nbr_notches

}; // enum Param



}  // namespace nzbl
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/nzbl/Param.hpp"



#endif   // mfx_pi_nzbl_Param_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
