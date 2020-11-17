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

#if fstb_SYS == fstb_SYS_LINUX
	#include <wiringPi.h>
#elif fstb_SYS == fstb_SYS_WIN
	#include <Windows.h>
#else
	#error Unsupported system
#endif

#include <algorithm>

#include <cassert>
#include <ctime>



namespace mfx
{
namespace ui
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// -1 for manual calls to process_single_task()
TimeShareThread::TimeShareThread (std::chrono::microseconds granularity)
:	_granularity (granularity)
, 	_list_mtx ()
,	_cb_list ()
,	_base_time (get_time ())
,	_quit_flag (false)
,	_refresher ()
{
	if (_granularity.count () > 0)
	{
		_refresher = std::thread (&TimeShareThread::polling_loop, this);
	}
}



TimeShareThread::~TimeShareThread ()
{
	if (_granularity.count () > 0 && _refresher.joinable ())
	{
		_quit_flag = true;
		_refresher.join ();
	}
}



void	TimeShareThread::register_cb (TimeShareCbInterface &cb, std::chrono::microseconds interval_us)
{
	assert (interval_us.count () > 0);

	std::lock_guard <std::mutex>  lock (_list_mtx);

	const CbUnit   cbu = { &cb, interval_us, _base_time };
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



// Returns true if we can wait before the next call.
bool	TimeShareThread::process_single_task ()
{
	assert (_granularity.count () <= 0);

	static int      scan_pos = 0;
	
	return find_and_execute_task (scan_pos);
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
			std::this_thread::sleep_for (_granularity);
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
		const auto     time_cur     = get_time ();
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
std::chrono::nanoseconds	TimeShareThread::get_time ()
{
#if fstb_SYS == fstb_SYS_LINUX
	timespec       tp;
	clock_gettime (CLOCK_REALTIME, &tp);

	const long     ns_mul = 1000L * 1000L * 1000L;
	return std::chrono::nanoseconds (int64_t (tp.tv_sec) * ns_mul + tp.tv_nsec);

#elif fstb_SYS == fstb_SYS_WIN
	::LARGE_INTEGER t;
	::QueryPerformanceCounter (&t);
	static double per = 0;
	if (per == 0)
	{
		::LARGE_INTEGER f;
		::QueryPerformanceFrequency (&f);
		per = 1e9 / double (f.QuadPart);
	}
	return std::chrono::nanoseconds (int64_t (t.QuadPart * per));
#else
	#error

#endif
}



}  // namespace ui
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
