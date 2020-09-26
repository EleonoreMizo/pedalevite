/*****************************************************************************

        ToneFreq.h
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_testgen_ToneFreq_HEADER_INCLUDED)
#define mfx_pi_testgen_ToneFreq_HEADER_INCLUDED

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



enum ToneFreq
{

	ToneFreq_30 = 0,
	ToneFreq_40,
	ToneFreq_50,
	ToneFreq_100,
	ToneFreq_500,
	ToneFreq_997,
	ToneFreq_1K,
	ToneFreq_3K,
	ToneFreq_10K,
	ToneFreq_15K,

	ToneFreq_NBR_ELT

}; // enum ToneFreq



}  // namespace testgen
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/testgen/ToneFreq.hpp"



#endif   // mfx_pi_testgen_ToneFreq_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
