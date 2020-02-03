/*****************************************************************************

        Param.h
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_dist3_Param_HEADER_INCLUDED)
#define mfx_pi_dist3_Param_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace dist3
{



enum Param
{

	Param_HPF_PRE = 0,
	Param_GAIN,
	Param_SRL_PRE,
	Param_CLASS,
	Param_BIAS_S,
	Param_BIAS_D,
	Param_BIAS_LPF,
	Param_BIAS_SRC,
	Param_TYPE,
	Param_SRL_POST,
	Param_PSU_OVRL,
	Param_PSU_LOAD,
	Param_PSU_FREQ,
	Param_LPF_POST,

	Param_NBR_ELT

}; // enum Param



}  // namespace dist3
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/dist3/Param.hpp"



#endif   // mfx_pi_dist3_Param_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
