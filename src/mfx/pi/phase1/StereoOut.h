/*****************************************************************************

        StereoOut.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_phase1_StereoOut_HEADER_INCLUDED)
#define mfx_pi_phase1_StereoOut_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace phase1
{



enum StereoOut
{

	StereoOut_SPAT_MIX = 0, // Both channels are mixed to produce the phased signal
	StereoOut_SPAT_SEP,     // Each channel has its own PhasedVoice which uses L/R polarities for the sin part.
	StereoOut_BIMONO,       // Each channel has its own PhasedVoice and uses the mono settings

	StereoOut_NBR_ELT

}; // enum StereoOut



}  // namespace phase1
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/phase1/StereoOut.hpp"



#endif   // mfx_pi_phase1_StereoOut_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
