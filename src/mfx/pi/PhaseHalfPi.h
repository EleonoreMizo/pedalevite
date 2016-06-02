/*****************************************************************************

        PhaseHalfPi.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_PhaseHalfPi_HEADER_INCLUDED)
#define mfx_pi_PhaseHalfPi_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#if fstb_IS (ARCHI, X86)
	#include "hiir/PhaseHalfPiSse.h"
#elif fstb_IS (ARCHI, ARM)
	#include "hiir/PhaseHalfPiNeon.h"
#else
	#include "hiir/PhaseHalfPiFpu.h"
#endif



namespace mfx
{
namespace pi
{



#if fstb_IS (ARCHI, X86)
	typedef hiir::PhaseHalfPiSse <8> PhaseHalfPi;
#elif fstb_IS (ARCHI, ARM)
	typedef hiir::PhaseHalfPiNeon <8> PhaseHalfPi;
#else
	typedef hiir::PhaseHalfPiFpu <8> PhaseHalfPi;
#endif



}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/PhaseHalfPi.hpp"



#endif   // mfx_pi_PhaseHalfPi_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
