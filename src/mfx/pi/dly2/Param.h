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
#if ! defined (mfx_pi_dly2_Param_HEADER_INCLUDED)
#define mfx_pi_dly2_Param_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/pi/dly2/Cst.h"



namespace mfx
{
namespace pi
{
namespace dly2
{



enum ParamTap
{
	ParamTap_GAIN_IN = 0,
	ParamTap_SPREAD,
	ParamTap_DLY_BASE,
	ParamTap_DLY_REL,
	ParamTap_PITCH,
	ParamTap_CUT_LO,
	ParamTap_CUT_HI,
	ParamTap_PAN,

	ParamTap_NBR_ELT
};

enum ParamLine
{
	ParamLine_GAIN_IN,
	ParamLine_DLY_BASE,
	ParamLine_DLY_REL,
	ParamLine_DLY_BBD_SPD,
	ParamLine_PITCH,
	ParamLine_FDBK,
	ParamLine_CUT_LO,
	ParamLine_CUT_HI,
	ParamLine_PAN,
	ParamLine_VOL,
	ParamLine_DUCK_AMT,

	// FX
	ParamLine_FX_FLT_T,
	ParamLine_FX_FLT_F,
	ParamLine_FX_FLT_R,
	ParamLine_FX_FLT_Q,
	ParamLine_FX_FLT_M,
	ParamLine_FX_DIST_A,
	ParamLine_FX_DIST_F,
	ParamLine_FX_SHLF_F,
	ParamLine_FX_SHLF_L,
	ParamLine_FX_FSH_F,
	ParamLine_FX_REV_MX,
	ParamLine_FX_REV_DC,
	ParamLine_FX_REV_DA,

	ParamLine_NBR_ELT
};

enum Param
{

	Param_TAPS_GAIN_IN = 0,
	Param_TAPS_VOL,
	Param_DRY_VOL,
	Param_DRY_SPREAD,
	Param_FREEZE,
	Param_NBR_LINES,
	Param_X_FDBK,
	Param_DUCK_SENS,
	Param_DUCK_TIME,

	Param_BASE_TAP,
	Param_BASE_LINE = Param_BASE_TAP  + Cst::_nbr_taps  * ParamTap_NBR_ELT,

	Param_NBR_ELT   = Param_BASE_LINE + Cst::_nbr_lines * ParamLine_NBR_ELT

}; // enum Param



}  // namespace dly2
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/dly2/Param.hpp"



#endif   // mfx_pi_dly2_Param_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
