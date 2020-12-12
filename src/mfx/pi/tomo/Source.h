/*****************************************************************************

        Source.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_tomo_Source_HEADER_INCLUDED)
#define mfx_pi_tomo_Source_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace tomo
{



enum Source
{

	Source_MIX6 = 0,
	Source_MIX3,
	Source_L,
	Source_R,

	Source_NBR_ELT

}; // enum Source



}  // namespace tomo
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/tomo/Source.hpp"



#endif   // mfx_pi_tomo_Source_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
