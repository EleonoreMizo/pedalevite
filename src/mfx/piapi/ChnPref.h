/*****************************************************************************

        ChnPref.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_piapi_ChnPref_HEADER_INCLUDED)
#define mfx_piapi_ChnPref_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace piapi
{



// As a general rule, the number output of channels is always greater or equal
// to the input channels, excepted in MONO where the number of output channels
// is lower or equal to the input channels.
enum class ChnPref
{

	// No preference, the number of output channels is generally the same as
	// the input channels, but can be greater.
	NONE = 0,

	// Mono output is preferred, even if the input is stereo. This is useful
	// for channel splitters or other utilities.
	MONO,

	// Stereo output is preferred but not forced. Useful for spacializing
	// effects
	STEREO

}; // enum class ChnPref



}  // namespace piapi
}  // namespace mfx



//#include "mfx/piapi/ChnPref.hpp"



#endif   // mfx_piapi_ChnPref_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
