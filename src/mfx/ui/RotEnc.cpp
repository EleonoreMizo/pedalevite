/*****************************************************************************

        RotEnc.cpp
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

#include "mfx/ui/RotEnc.h"

#include <cassert>



namespace mfx
{
namespace ui
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	RotEnc::reset ()
{
	_pin_state     = 0;
	_cumulated_pos = 0;
}



int	RotEnc::set_new_state (bool a0_flag, bool a1_flag)
{
	const int         state_new = ((a0_flag) ? 1 : 0) + ((a1_flag) ? 2 : 0);

	static const int  pos_inc_arr [1 << _nbr_pins] [1 << _nbr_pins] =
	{
		// 00  01  10  11 Old   New
		{   0, -1, +1,  0 }, // 00
		{  +1,  0,  0, -1 }, // 01
		{  -1,  0,  0, +1 }, // 10
		{   0, +1, -1,  0 }  // 11
	};

	const int      inc_fine = pos_inc_arr [state_new] [_pin_state];
	_pin_state = state_new;
	_cumulated_pos += inc_fine;

	int            inc = 0;
	if (state_new == 0)
	{
		if (_cumulated_pos > 0)
		{
			inc = 1;
		}
		else if (_cumulated_pos < 0)
		{
			inc = -1;
		}

		_cumulated_pos = 0;
	}

	return inc;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace ui
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
