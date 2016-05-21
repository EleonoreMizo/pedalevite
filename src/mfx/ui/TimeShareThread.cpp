/*****************************************************************************

        TimeShareThread.cpp
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

#include "fstb/def.h"

#include "mfx/ui/TimeShareCbInterface.h"
#include "mfx/ui/TimeShareThread.h"

#if fstb_IS (ARCHI, ARM)
#elif fstb_IS (ARCHI, X86)
	#include <Windows.h>
#else
	#error Unsupported architecture
#endif

#include <algorithm>

#include <cassert>
#include <ctime>



namespace mfx
{
namespace ui
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



TimeShareThread::TimeShareThread (int granularity)
:	_granularity (granularity)
, 	_list_mtx ()
,	_cb_list ()
,	_base_time (get_time ())
,	_quit_flag (false)
,	_refresher (&TimeShareThread::polling_loop, this)
{
	assert (granularity > 0);
}



TimeShareThread::~TimeShareThread ()
{
	if (_refresher.joinable ())
	{
		_quit_flag = true;
		_refresher.join ();
	}
}



void	TimeShareThread::register_cb (TimeShareCbInterface &cb, int interval_us)
{
	assert (interval_us > 0);

	std::lock_guard <std::mutex>  lock (_list_mtx);

	const CbUnit   cbu = { &cb, interval_us * 1000, _base_time };
	_cb_list.push_back (cbu);
}



void	TimeShareThread::remove_cb (TimeShareCbInterface &cb)
{
	std::lock_guard <std::mutex>  lock (_list_mtx);

	auto           it = std::find_if (_cb_list.begin (), _cb_list.end (),
		[&] (const CbUnit &cbu) { return (cbu._cb_ptr == &cb); }
	);
	if (it == _cb_list.end ())
	{
		assert (false);
	}
	else
	{
		_cb_list.erase (it);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	TimeShareThread::polling_loop ()
{
	int            scan_pos = 0;

	while (! _quit_flag)
	{
		const bool     wait_flag = find_and_execute_task (scan_pos);

		if (wait_flag && ! _quit_flag)
		{
#if fstb_IS (ARCHI, ARM)
			::delayMicroseconds (_granularity);
#else
			::Sleep ((_granularity + 999) / 1000);
#endif
		}
	}

	_quit_flag = false;
}



bool	TimeShareThread::find_and_execute_task (int &scan_pos)
{
	bool           wait_flag = true;

	std::lock_guard <std::mutex>  lock (_list_mtx);

	fix_scanpos (scan_pos); // Required to handle callback removal

	if (! _cb_list.empty ())
	{
		const int64_t  time_cur     = get_time ();
		const int      scan_pos_org = scan_pos;
		bool           done_flag    = false;
		do
		{
			CbUnit &       cbu = _cb_list [scan_pos];
			if (time_cur >= cbu._next_time)
			{
				const bool     cont_flag = cbu._cb_ptr->process_timeshare_op ();
				if (! cont_flag)
				{
					cbu._next_time = time_cur + cbu._interval;
				}

				wait_flag = false;
				done_flag = true;
			}

			++ scan_pos;
			fix_scanpos (scan_pos);
		}
		while (! done_flag && scan_pos != scan_pos_org);
	}

	return wait_flag;
}



void	TimeShareThread::fix_scanpos (int &scan_pos)
{
	if (scan_pos >= int (_cb_list.size ()))
	{
		scan_pos = 0;
	}
}



// Nanoseconds
int64_t	TimeShareThread::get_time ()
{
#if fstb_IS (ARCHI, ARM)
	timespec       tp;
	clock_gettime (CLOCK_REALTIME, &tp);

	const long     ns_mul = 1000L * 1000L * 1000L;
	return int64_t (tp.tv_sec) * ns_mul + tp.tv_nsec;

#else
	::LARGE_INTEGER t;
	::QueryPerformanceCounter (&t);
	static double per = 0;
	if (per == 0)
	{
		::LARGE_INTEGER f;
		::QueryPerformanceFrequency (&f);
		per = 1e9 / double (f.QuadPart);
	}
	return int64_t (t.QuadPart * per);

#endif
}



}  // namespace ui
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
