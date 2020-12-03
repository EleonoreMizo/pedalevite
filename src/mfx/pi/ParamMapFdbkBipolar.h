/*****************************************************************************

        ParamMapFdbkBipolar.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_pi_ParamMapFdbkBipolar_HEADER_INCLUDED)
#define	mfx_pi_ParamMapFdbkBipolar_HEADER_INCLUDED
#pragma once

#if defined (_MSC_VER)
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"mfx/pi/param/MapSatBipolar.h"



namespace mfx
{
namespace pi
{



typedef param::MapSatBipolar <
   std::ratio <995, 1000>,
   std::ratio <1, 2>
> ParamMapFdbkBipolar;



}	// namespace pi
}	// namespace mfx



//#include	"mfx/pi/ParamMapFdbkBipolar.hpp"



#endif	// mfx_pi_ParamMapFdbkBipolar_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
