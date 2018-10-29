/*****************************************************************************

        SweepDur.h
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_testgen_SweepDur_HEADER_INCLUDED)
#define mfx_pi_testgen_SweepDur_HEADER_INCLUDED

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



enum SweepDur
{
	SweepDur_6 = 0,
	SweepDur_15,
	SweepDur_30,
	SweepDur_60,
	SweepDur_180,

	SweepDur_NBR_ELT

}; // enum SweepDur



}  // namespace testgen
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/testgen/SweepDur.hpp"



#endif   // mfx_pi_testgen_SweepDur_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
