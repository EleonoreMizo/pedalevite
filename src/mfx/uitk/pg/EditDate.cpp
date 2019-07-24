/*****************************************************************************

        EditDate.cpp
        Author: Laurent de Soras, 2019

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
#include "fstb/fnc.h"
#include "mfx/uitk/pg/EditDate.h"
#include "mfx/uitk/NodeEvt.h"
#include "mfx/uitk/PageMgrInterface.h"
#include "mfx/uitk/PageSwitcher.h"
#include "mfx/ui/Font.h"
#include "mfx/CmdLine.h"

#if fstb_IS (SYS, LINUX)
#include <unistd.h>
#endif

#include <cassert>
#if fstb_IS (SYS, LINUX)
#include <cstdlib>
#endif


namespace mfx
{
namespace uitk
{
namespace pg
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



EditDate::EditDate (PageSwitcher &page_switcher, const CmdLine &cmd_line)
:	_page_switcher (page_switcher)
,	_cmd_line (cmd_line)
,	_page_ptr (0)
,	_page_size ()
,	_fnt_l_ptr (0)
,	_fnt_ptr (0)
,	_title_sptr ( new NText (Entry_T  ))
,	_year_sptr (  new NText (Entry_Y  ))
,	_month_sptr ( new NText (Entry_M  ))
,	_day_sptr (   new NText (Entry_D  ))
,	_hour_sptr (  new NText (Entry_H  ))
,	_minute_sptr (new NText (Entry_MIN))
,	_change_flag (false)
{
	_title_sptr->set_text ("UTC date + time");
	_title_sptr->set_justification (0.5f, 0, false);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	EditDate::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt)
{
	_page_ptr  = &page;
	_page_size = page_size;
	_fnt_l_ptr = &fnt._l;
	_fnt_ptr   = &fnt._m;

	time_t         timer;
	time (&timer);
	_utc              = *gmtime (&timer);
	_change_flag      = false;
	_time_change_flag = false;

	const int      scr_w = _page_size [0];
	const int      h_m   = _fnt_ptr->get_char_h ();

	_title_sptr ->set_font (*_fnt_l_ptr);
	_year_sptr  ->set_font (*_fnt_ptr);
	_month_sptr ->set_font (*_fnt_ptr);
	_day_sptr   ->set_font (*_fnt_ptr);
	_hour_sptr  ->set_font (*_fnt_ptr);
	_minute_sptr->set_font (*_fnt_ptr);

	_title_sptr ->set_coord (Vec2d (scr_w >> 1, 0));
	_year_sptr  ->set_coord (Vec2d (0, h_m * 2));
	_month_sptr ->set_coord (Vec2d (0, h_m * 3));
	_day_sptr   ->set_coord (Vec2d (0, h_m * 4));
	_hour_sptr  ->set_coord (Vec2d (0, h_m * 5));
	_minute_sptr->set_coord (Vec2d (0, h_m * 6));

	_year_sptr  ->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_month_sptr ->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_day_sptr   ->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_hour_sptr  ->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_minute_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());

	_page_ptr->push_back (_title_sptr );
	_page_ptr->push_back (_year_sptr  );
	_page_ptr->push_back (_month_sptr );
	_page_ptr->push_back (_day_sptr   );
	_page_ptr->push_back (_hour_sptr  );
	_page_ptr->push_back (_minute_sptr);

#if fstb_IS (SYS, LINUX)

	PageMgrInterface::NavLocList  nav_list;
	PageMgrInterface::add_nav (nav_list, Entry_Y  );
	PageMgrInterface::add_nav (nav_list, Entry_M  );
	PageMgrInterface::add_nav (nav_list, Entry_D  );
	PageMgrInterface::add_nav (nav_list, Entry_H  );
	PageMgrInterface::add_nav (nav_list, Entry_MIN);
	_page_ptr->set_nav_layout (nav_list);

#endif

	update_display ();
	_page_ptr->set_timer (Entry_T, true);
}



void	EditDate::do_disconnect ()
{
	_page_ptr->set_timer (Entry_T, false);

#if fstb_IS (SYS, LINUX)

	if (_change_flag)
	{
		// Merges changed time and current time
		time_t         timer;
		time (&timer);
		tm             now_utc (*gmtime (&timer));
		if (! _time_change_flag)
		{
			_utc.tm_hour = now_utc.tm_hour;
			_utc.tm_min  = now_utc.tm_min;
		}
		_utc.tm_sec = now_utc.tm_sec;

		// Sets the new date
		char           txt_0 [255+1];
		strftime (txt_0, sizeof (txt_0),
			"sudo date --utc -s \"%Y-%m-%d %H:%M:%S\"",
			&_utc
		);
		system (txt_0);
		_change_flag = false;

		// Changing the date breaks many time-based functions, especially the
		// sleep-related commands. So restarting the program immediately is a
		// lesser evil.
		char * const * argv = _cmd_line.use_argv ();
		char * const * envp = _cmd_line.use_envp ();
		execve (argv [0], argv, envp);
	}

#endif
}



MsgHandlerInterface::EvtProp	EditDate::do_handle_evt (const NodeEvt &evt)
{
	EvtProp        ret_val = EvtProp_PASS;

	const int      node_id = evt.get_target ();

	if (evt.is_timer ())
	{
		if (_time_change_flag)
		{
			_page_ptr->set_timer (Entry_T, false);
		}
		else
		{
			time_t         timer;
			time (&timer);
			tm             now_utc (*gmtime (&timer));
			_utc.tm_hour = now_utc.tm_hour;
			_utc.tm_min  = now_utc.tm_min;
			_utc.tm_sec  = now_utc.tm_sec;
			refresh_time ();
		}
	}

	else if (evt.is_button_ex ())
	{
		const Button   but = evt.get_button_ex ();
		switch (but)
		{
		case Button_E:
			_page_switcher.return_page ();
			break;
		case Button_L:
			change_entry (node_id, -1);
			break;
		case Button_R:
			change_entry (node_id, +1);
			break;
		default:
			// Nothing
			break;
		}
	}

	return ret_val;
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	EditDate::update_display ()
{
	update_field (*_year_sptr  , "Year  : %Y"   , _utc);
	update_field (*_month_sptr , "Month : %m %b", _utc);
	update_field (*_day_sptr   , "Day   : %d %a", _utc);
	refresh_time ();
}



void	EditDate::refresh_time ()
{
	update_field (*_hour_sptr  , "Hour  : %H"   , _utc);
	update_field (*_minute_sptr, "Minute: %M"   , _utc);
}



void	EditDate::update_field (NText &label, const char format_0 [], const tm &utc)
{
	assert (format_0 != 0);

	char           txt_0 [255+1];
	strftime (txt_0, sizeof (txt_0), format_0, &utc);
	label.set_text (txt_0);
}



void	EditDate::change_entry (int node_id, int dir)
{
	switch (node_id)
	{
	case Entry_Y:
		_utc.tm_year += dir;
		break;
	case Entry_M:
		_utc.tm_mon  += dir;
		break;
	case Entry_D:
		_utc.tm_mday += dir;
		break;
	case Entry_H:
		_utc.tm_hour += dir;
		_time_change_flag = true;
		break;
	case Entry_MIN:
		_utc.tm_min  += dir;
		_time_change_flag = true;
		break;
	}

	// Checks consistency
	_utc.tm_year = fstb::limit (_utc.tm_year, 84, 2099 - 1900);
	const bool     mul004_flag = ((_utc.tm_year %   4) == 0);
	const bool     mul100_flag = ((_utc.tm_year % 100) == 0);
	const bool     mul400_flag = ((_utc.tm_year % 400) == 0);
	_utc.tm_mon  = (_utc.tm_mon + 12) % 12;
	static const int  dpm_arr [12] =
	{
		31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
	};
	int            dpm_cur = dpm_arr [_utc.tm_mon];
	if (_utc.tm_mon == 1 && mul004_flag && (! mul100_flag || mul400_flag))
	{
		++ dpm_cur;
	}
	_utc.tm_mday = (_utc.tm_mday - 1 + dpm_cur) % dpm_cur + 1;
	_utc.tm_hour = (_utc.tm_hour + 24) % 24;
	_utc.tm_min  = (_utc.tm_min  + 60) % 60;
	_utc.tm_sec  = (_utc.tm_sec  + 60) % 60;

	_change_flag = true;

	update_display ();
}



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
