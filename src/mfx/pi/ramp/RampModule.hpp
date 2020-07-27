/*****************************************************************************

        RampModule.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_pi_ramp_RampModule_CODEHEADER_INCLUDED)
#define mfx_pi_ramp_RampModule_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"



namespace mfx
{
namespace pi
{
namespace ramp
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int P, int D>
constexpr float	RampModule::accelerate (float x)
{
	return (x + (D - 1) * fstb::ipowpc <P> (x)) * (1.0f / D);
}



constexpr float	RampModule::fast (float x)
{
	return x * x * (3 - 2 * x);
}



constexpr float	RampModule::slow (float x)
{
	return x * 2 - fast (x);
}



}  // namespace ramp
}  // namespace pi
}  // namespace mfx



#endif   // mfx_pi_ramp_RampModule_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
