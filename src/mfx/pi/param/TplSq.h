/*****************************************************************************

        TplSq.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_param_TplSq_HEADER_INCLUDED)
#define mfx_pi_param_TplSq_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/pi/param/MapSq.h"
#include "mfx/pi/param/TplMapped.h"



namespace mfx
{
namespace pi
{
namespace param
{



template <bool INVFLAG>
using TplSq = TplMapped <MapSq <INVFLAG> >;



}  // namespace param
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/param/TplSq.hpp"



#endif   // mfx_pi_param_TplSq_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
