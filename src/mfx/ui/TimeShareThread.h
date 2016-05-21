/*****************************************************************************

        TimeShareThread.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_ui_TimeShareThread_HEADER_INCLUDED)
#define mfx_ui_TimeShareThread_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <mutex>
#include <thread>
#include <vector>



namespace mfx
{
namespace ui
{



class TimeShareCbInterface;

class TimeShareThread
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       TimeShareThread (int granularity);
	virtual        ~TimeShareThread ();

	// Do not call from a callback
	void           register_cb (TimeShareCbInterface &cb, int interval_us);
	void           remove_cb (TimeShareCbInterface &cb);

	bool           process_single_task ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	class CbUnit
	{
	public:
		TimeShareCbInterface *
		               _cb_ptr;
		int64_t        _interval;        // Between two complete tasks. Nanoseconds
		int64_t        _next_time;       // Nanoseconds
	};

	typedef std::vector <CbUnit> CbList;

	void           polling_loop ();
	bool           find_and_execute_task (int &scan_pos);
	void           fix_scanpos (int &scan_pos);

	static int64_t get_time ();

	const int      _granularity;

	std::mutex     _list_mtx;           // Access to the callback list
	CbList         _cb_list;
	const int64_t  _base_time;

	volatile bool  _quit_flag;
	std::thread    _refresher;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TimeShareThread ()                               = delete;
	               TimeShareThread (const TimeShareThread &other)   = delete;
	TimeShareThread &
	               operator = (const TimeShareThread &other)        = delete;
	bool           operator == (const TimeShareThread &other) const = delete;
	bool           operator != (const TimeShareThread &other) const = delete;

}; // class TimeShareThread



}  // namespace ui
}  // namespace mfx



//#include "mfx/ui/TimeShareThread.hpp"



#endif   // mfx_ui_TimeShareThread_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
