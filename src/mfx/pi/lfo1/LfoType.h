/*****************************************************************************

        LfoType.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_lfo1_LfoType_HEADER_INCLUDED)
#define mfx_pi_lfo1_LfoType_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace lfo1
{



enum LfoType
{

		LfoType_INVALID = -1,

		LfoType_SINE = 0,
		LfoType_TRIANGLE,
		LfoType_SQUARE,
		LfoType_SAW,
		LfoType_PARABOLA,
		LfoType_BIPHASE,
		LfoType_N_PHASE,
		LfoType_VARISLOPE,
		LfoType_NOISE_FLT,

		LfoType_NBR_ELT

}; // enum LfoType



}  // namespace lfo1
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/lfo1/LfoType.hpp"



#endif   // mfx_pi_lfo1_LfoType_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
