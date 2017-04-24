/*****************************************************************************

        CurveType.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_ramp_CurveType_HEADER_INCLUDED)
#define mfx_pi_ramp_CurveType_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace ramp
{



enum CurveType
{

		CurveType_INVALID = -1,

		CurveType_LINEAR  = 0,
		CurveType_ACC_1,
		CurveType_ACC_2,
		CurveType_ACC_3,
		CurveType_ACC_4,
		CurveType_SAT_1,
		CurveType_SAT_2,
		CurveType_SAT_3,
		CurveType_SAT_4,
		CurveType_FAST_1,
		CurveType_FAST_2,
		CurveType_SLOW_1,
		CurveType_SLOW_2,

		CurveType_NBR_ELT

}; // enum CurveType



}  // namespace ramp
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/ramp/CurveType.hpp"



#endif   // mfx_pi_ramp_CurveType_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
