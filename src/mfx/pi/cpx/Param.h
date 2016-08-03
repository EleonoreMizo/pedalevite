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
#if ! defined (mfx_pi_cpx_Param_HEADER_INCLUDED)
#define mfx_pi_cpx_Param_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace cpx
{



enum Param
{

	Param_RATIO_L = 0,
	Param_RATIO_H,
	Param_KNEE_LVL,   // Stored as log2
	Param_KNEE_SHAPE, // Stored as log2
	Param_ATTACK,
	Param_RELEASE,
	Param_GAIN,       // Stored as log2

	Param_NBR_ELT

}; // class Param



}  // namespace cpx
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/cpx/Param.hpp"



#endif   // mfx_pi_cpx_Param_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
