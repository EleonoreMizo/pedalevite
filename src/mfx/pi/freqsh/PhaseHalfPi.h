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
#if ! defined (mfx_pi_freqsh_PhaseHalfPi_HEADER_INCLUDED)
#define mfx_pi_freqsh_PhaseHalfPi_HEADER_INCLUDED

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
namespace freqsh
{



template <int N>
using PhaseHalfPi =
#if fstb_IS (ARCHI, X86)
	hiir::PhaseHalfPiSse <N>;
#elif fstb_IS (ARCHI, ARM)
	hiir::PhaseHalfPiNeon <N>;
#else
	hiir::PhaseHalfPiFpu <N>;
#endif



}  // namespace freqsh
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/freqsh/PhaseHalfPi.hpp"



#endif   // mfx_pi_freqsh_PhaseHalfPi_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
