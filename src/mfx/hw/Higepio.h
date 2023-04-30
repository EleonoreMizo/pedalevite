/*****************************************************************************

        Higepio.h
        Author: Laurent de Soras, 2023

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_hw_Higepio_HEADER_INCLUDED)
#define mfx_hw_Higepio_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#if defined (PV_USE_WIRINGPI)
#	include "mfx/hw/HigepioWiringPi.h"
#else
#	include "mfx/hw/HigepioNative.h"
#endif



namespace mfx
{
namespace hw
{



#if defined (PV_USE_WIRINGPI)
typedef HigepioWiringPi Higepio;
#else
typedef HigepioNative   Higepio;
#endif



}  // namespace hw
}  // namespace mfx



//#include "mfx/hw/Higepio.hpp"



#endif   // mfx_hw_Higepio_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
