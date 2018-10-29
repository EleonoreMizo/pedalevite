/*****************************************************************************

        LoopPause.h
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_testgen_LoopPause_HEADER_INCLUDED)
#define mfx_pi_testgen_LoopPause_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace testgen
{



enum LoopPause
{

	LoopPause_OFF = 0,
	LoopPause_1,
	LoopPause_3,
	LoopPause_10,
	LoopPause_30,

	LoopPause_NBR_ELT

}; // enum LoopPause



}  // namespace testgen
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/testgen/LoopPause.hpp"



#endif   // mfx_pi_testgen_LoopPause_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
