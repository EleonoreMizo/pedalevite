/*****************************************************************************

        ChnMode.cpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/ChnMode.h"

#include <cassert>



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// Dir refers to the audio graph, so IN = audio input
int	ChnMode_get_nbr_chn (ChnMode mode, piapi::Dir dir)
{
	int            nbr_chn = 1;

	if (dir == piapi::Dir_IN)
	{
		switch (mode)
		{
		case ChnMode_1M_1M:
		case ChnMode_1M_1S:
			break;
		case ChnMode_1S_1S:
			nbr_chn = 2;
			break;
		default:
			assert (false);
			break;
		}
	}
	else
	{
		switch (mode)
		{
		case ChnMode_1M_1M:
			break;
		case ChnMode_1M_1S:
		case ChnMode_1S_1S:
			nbr_chn = 2;
			break;
		default:
			assert (false);
			break;
		}
	}

	return nbr_chn;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
