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
#if ! defined (mfx_pi_dist2_Param_HEADER_INCLUDED)
#define mfx_pi_dist2_Param_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace dist2
{



enum ParamStage
{
	ParamStage_HPF_PRE = 0,
	ParamStage_BIAS,
	ParamStage_GAIN,
	ParamStage_LPF_POST,

	ParamStage_NBR_ELT
};

enum Param
{

	Param_XOVER = 0,
	Param_PRE_LPF,
	Param_S1_BASE,
	Param_S2_BASE = Param_S1_BASE + ParamStage_NBR_ELT,
	Param_S12_MIX = Param_S2_BASE + ParamStage_NBR_ELT,
	Param_LB_MIX,

	Param_NBR_ELT

}; // enum Param



}  // namespace dist2
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/dist2/Param.hpp"



#endif   // mfx_pi_dist2_Param_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
