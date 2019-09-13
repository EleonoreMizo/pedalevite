/*****************************************************************************

        ThreadLinux.cpp
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

#include "mfx/hw/ThreadLinux.h"

#include <pthread.h>
#include <sched.h>

#include <cassert>



namespace mfx
{
namespace hw
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	ThreadLinux::set_priority (std::thread &thrd, int prio_below_max, FILE *err_msg_ptr)
{
	assert (prio_below_max <= 0);

	int            ret_val = 0;

	const int      policy   = SCHED_FIFO;
	int            max_prio = 0;
	if (ret_val == 0)
	{
		max_prio = ::sched_get_priority_max (policy);
		if (max_prio < 0)
		{
			ret_val = max_prio;
			if (err_msg_ptr != nullptr)
			{
				fprintf (
					err_msg_ptr,
					"Error: cannot retrieve the maximum priority value.\n"
				);
			}
		}
	}
	if (ret_val == 0)
	{
		::sched_param  tparam;
		memset (&tparam, 0, sizeof (tparam));
		tparam.sched_priority = max_prio + prio_below_max;

		ret_val = ::pthread_setschedparam (
			thrd.native_handle (),
			policy,
			&tparam
		);
		if (ret_val != 0)
		{
			if (err_msg_ptr != nullptr)
			{
				fprintf (err_msg_ptr, "Error: cannot set thread priority.\n");
			}
		}
	}

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace hw
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
