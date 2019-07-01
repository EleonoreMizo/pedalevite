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
#if ! defined (mfx_pi_osdet2_Param_HEADER_INCLUDED)
#define mfx_pi_osdet2_Param_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace osdet2
{



enum Param
{

	Param_VELO_CLIP = 0,
	Param_HP,
	Param_LP1,
	Param_LP2,
	Param_A,
	Param_B,
	Param_BETA,
	Param_LAMBDA,
	Param_ALPHA,
	Param_RLS_THR,
	Param_RLS_RATIO,

	Param_NBR_ELT

}; // enum Param



}  // namespace osdet2
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/osdet2/Param.hpp"



#endif   // mfx_pi_osdet2_Param_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
