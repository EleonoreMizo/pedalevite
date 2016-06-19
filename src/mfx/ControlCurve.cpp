/*****************************************************************************

        ControlCurve.cpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/ControlCurve.h"

#include <cassert>



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



const char *  ControlCurve_get_name (ControlCurve c)
{
	assert (c >= 0);
	assert (c < ControlCurve_NBR_ELT);

	switch (c)
	{
	case ControlCurve_LINEAR: return ("Linear"); break;
	case ControlCurve_SQ:	  return ("Square"); break;
	case ControlCurve_CB:	  return ("Cubic" ); break;
	case ControlCurve_SAT2:	  return ("Sat. 1"); break;
	case ControlCurve_SAT3:	  return ("Sat. 2"); break;
	case ControlCurve_S1:	  return ("S 2"   ); break;
	case ControlCurve_S2:	  return ("S 1"   ); break;
	default:
		assert (false);
		break;
	}

	return "\?\?\?";
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
