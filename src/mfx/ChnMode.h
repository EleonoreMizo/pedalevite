/*****************************************************************************

        ChnMode.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_ChnMode_HEADER_INCLUDED)
#define mfx_ChnMode_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{



enum ChnMode
{

	ChnMode_1M_1M,    // Mono to mono
	ChnMode_1M_1S,    // Mono to stereo
	// Future use
#if 0
	ChmMode_1S_1S,    // Stereo to stereo
	ChnMode_2M_1M,    // 2 mono to 1 mono
	ChnMode_2M_1S,    // 2 mono to 1 stereo
	ChnMode_2M_2M,    // 2 mono chains
#endif

	ChnMode_NBR_ELT

}; // class ChnMode



}  // namespace mfx



//#include "mfx/ChnMode.hpp"



#endif   // mfx_ChnMode_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
