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
#if ! defined (mfx_pi_spkem_Param_HEADER_INCLUDED)
#define mfx_pi_spkem_Param_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace spkem
{



enum Param
{

	Param_MID_LVL = 0,
	Param_MID_FREQ,
	Param_TREBLE_LVL,
	Param_TREBLE_FREQ,
	Param_COMB_LVL,
	Param_CHANNELS,

	Param_NBR_ELT

}; // enum Param



}  // namespace spkem
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/spkem/Param.hpp"



#endif   // mfx_pi_spkem_Param_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
