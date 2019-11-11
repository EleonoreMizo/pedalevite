/*****************************************************************************

        Rec2Disk.cpp
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

#include "fstb/def.h"
#include "fstb/fnc.h"
#include "mfx/uitk/pg/Rec2Disk.h"
#include "mfx/uitk/NodeEvt.h"
#include "mfx/uitk/PageMgrInterface.h"
#include "mfx/uitk/PageSwitcher.h"
#include "mfx/ui/Font.h"
#include "mfx/Cst.h"
#include "mfx/Model.h"

#if fstb_IS (SYS, LINUX)
#include <sys/statvfs.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#endif // LINUX

#include <algorithm>

#include <cassert>
#include <climits>
#include <ctime>



namespace mfx
{
namespace uitk
{
namespace pg
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Rec2Disk::Rec2Disk (PageSwitcher &page_switcher)
:	_page_switcher (page_switcher)
,	_model_ptr (0)
,	_view_ptr (0)
,	_page_ptr (0)
,	_page_size ()
,	_fnt_ptr (0)
,	_rec_sptr (    new NText (Entry_REC    ))
,	_max_dur_sptr (new NText (Entry_MAX_DUR))
,	_avail_sptr (  new NText (Entry_AVAIL  ))
,	_time_limit (30)
,	_disk_avail (0)
,	_prev_refresh (-1)
,	_msg_arg ()
{
	// Nothing
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Rec2Disk::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt)
{
	_model_ptr = &model;
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;
	_fnt_ptr   = &fnt._m;

	_page_ptr->clear_all_nodes ();

	const int      h_m = _fnt_ptr->get_char_h ();

	_rec_sptr    ->set_font (*_fnt_ptr);
	_max_dur_sptr->set_font (*_fnt_ptr);
	_avail_sptr  ->set_font (*_fnt_ptr);

	_rec_sptr    ->set_coord (Vec2d (0, h_m * 0));
	_max_dur_sptr->set_coord (Vec2d (0, h_m * 1));
	_avail_sptr  ->set_coord (Vec2d (0, h_m * 2));

	_page_ptr->push_back (_rec_sptr    );
	_page_ptr->push_back (_max_dur_sptr);
	_page_ptr->push_back (_avail_sptr  );

	update_display ();
	_page_ptr->set_timer (Entry_REC, true);
}



void	Rec2Disk::do_disconnect ()
{
	_page_ptr->set_timer (Entry_REC, false);
}



MsgHandlerInterface::EvtProp	Rec2Disk::do_handle_evt (const NodeEvt &evt)
{
	EvtProp        ret_val = EvtProp_PASS;

	const int      node_id = evt.get_target ();

	if (evt.is_timer ())
	{
		time_t         timer;
		time (&timer);
		tm             utc (*gmtime (&timer));
		const int      rate    = 10; // Updates every 10 seconds
		const int      current = utc.tm_sec / rate;
		if (current != _prev_refresh)
		{
			_prev_refresh = current;
			update_rec_time ();
		}
	}

	else if (evt.is_button_ex ())
	{
		const Button   but = evt.get_button_ex ();
		switch (but)
		{
		case Button_S:
			ret_val = EvtProp_CATCH;
			switch (node_id)
			{
			case Entry_REC:
				toggle_rec (node_id);
				break;
			default:
				ret_val = EvtProp_PASS;
				break;
			}
			break;
		case Button_E:
			_page_switcher.return_page ();
			ret_val = EvtProp_CATCH;
			break;
		case Button_L:
			if (node_id == Entry_MAX_DUR)
			{
				change_limit (-1);
				ret_val = EvtProp_CATCH;
			}
			break;
		case Button_R:
			if (node_id == Entry_MAX_DUR)
			{
				change_limit (+1);
				ret_val = EvtProp_CATCH;
			}
			break;
		default:
			// Nothing
			break;
		}
	}
	return ret_val;
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Rec2Disk::update_display ()
{
	const bool     rec_flag = _model_ptr->is_d2d_recording ();

	PageMgrInterface::NavLocList  nav_list;
	NavLoc         nav;
	PageMgrInterface::add_nav (nav_list, Entry_REC);
	if (! rec_flag)
	{
		PageMgrInterface::add_nav (nav_list, Entry_MAX_DUR);
	}

#if fstb_IS (SYS, LINUX)

	int64_t        bytes_avail = 0;
	if (! rec_flag)
	{
		std::string    pathname   = Cst::_audiodump_dir;
		int64_t        free_bytes = 0;
		int64_t        file_bytes = 0;
		struct statvfs res_free;
		struct stat    res_file;
		int            ret_val = statvfs (pathname.c_str (), &res_free);
		if (ret_val == 0)
		{
			free_bytes = int64_t (res_free.f_bsize) * res_free.f_bfree;
			pathname   = build_rec_pathname ();
			ret_val    = stat (pathname.c_str (), &res_file);
		}
		if (ret_val == 0)
		{
			file_bytes  = res_file.st_size;
		}
		bytes_avail = free_bytes + file_bytes;
	}

	if (bytes_avail > 0)
	{
		const int64_t  margin = 100 * int64_t (1024 * 1024);
		if (bytes_avail < margin)
		{
			_disk_avail = 5;
		}
		else
		{
			_disk_avail = conv_bytes_to_min (bytes_avail - margin);
		}
		_time_limit = std::min (_time_limit, _disk_avail);
	}

#endif // LINUX

	update_rec_time ();

	std::string    txt;

	txt = "Limit : ";
	txt += print_duration (_time_limit);
	_max_dur_sptr->set_text (txt);

	txt = "Avail : ";
	if (_disk_avail > 0)
	{
		txt += print_duration (_disk_avail);
	}
	else
	{
		txt += "Unknown";
	}
	_avail_sptr->set_text (txt);

	_page_ptr->set_nav_layout (nav_list);
}



void	Rec2Disk::update_rec_time ()
{
	const bool     rec_flag = _model_ptr->is_d2d_recording ();
	std::string    txt ("Record: ");
	if (rec_flag)
	{
		txt += "ON ";
		const int64_t  nbr_frames = _model_ptr->get_d2d_size_frames ();
		const int      rec_dur    = conv_frames_to_min (nbr_frames);
		txt += print_duration (rec_dur);
	}
	else
	{
		txt += "Off";
	}
	_rec_sptr->set_text (txt);
}



void	Rec2Disk::toggle_rec (int node_id)
{
	int            ret_val = 0;

	if (_model_ptr->is_d2d_recording ())
	{
		ret_val = _model_ptr->stop_d2d_rec ();
		if (ret_val != 0)
		{
			Question::msg_box (
				"Stop recording", "Failed",
				_msg_arg, _page_switcher, node_id
			);
		}
	}
	else
	{
		std::string    pathname = build_rec_pathname ();
		const int      fs = fstb::round_int (_model_ptr->get_sample_freq ());
		const size_t   max_len = size_t (_time_limit * 60 * fs);
		ret_val = _model_ptr->start_d2d_rec (pathname.c_str (), max_len);
		if (ret_val != 0)
		{
			Question::msg_box (
				"Start recording", "Failed",
				_msg_arg, _page_switcher, node_id
			);
		}
	}

	update_display ();
}



void	Rec2Disk::change_limit (int dir)
{
	int            step = 5;
	if (dir > 0)
	{
		_time_limit /= step;
	}
	else if (dir < 0)
	{
		_time_limit += step - 1;
		_time_limit /= step;
	}
	_time_limit += dir;
	_time_limit *= step;

	step = 15;
	if (_time_limit > step * 4)
	{
		_time_limit /= step;
		_time_limit += (dir + 1) >> 1;
		_time_limit *= step;
	}

	step = 60;
	if (_time_limit > step * 4)
	{
		_time_limit /= step;
		_time_limit += (dir + 1) >> 1;
		_time_limit *= step;
	}

	_time_limit = std::max (_time_limit, 5);

	update_display ();
}



std::string	Rec2Disk::build_rec_pathname () const
{
	std::string    txt (Cst::_audiodump_dir);
	txt += "/";
	txt += Cst::_d2d_file;

	return txt;
}



std::string	Rec2Disk::print_duration (int minutes) const
{
	char           txt_0 [255+1];
	const int      dur_h = minutes / 60;
	const int      dur_m = minutes - dur_h * 60;
	fstb::snprintf4all (txt_0, sizeof (txt_0), "%2dh%02d", dur_h, dur_m);

	return txt_0;
}



int	Rec2Disk::conv_frames_to_min (int64_t nbr_frames) const
{
	const int      frames_per_s =
		fstb::round_int (_model_ptr->get_sample_freq ());
	int64_t        nbr_minutes  = nbr_frames / (frames_per_s * 60);
	nbr_minutes = std::min (nbr_minutes, int64_t (INT_MAX));

	return int (nbr_minutes);
}



int	Rec2Disk::conv_bytes_to_min (int64_t nbr_bytes) const
{
	const int      nbr_chn         = Cst::_nbr_chn_in + Cst::_nbr_chn_out;
	const int      bytes_per_frame = (sizeof (float) * CHAR_BIT / 8) * nbr_chn;
	const int64_t  nbr_frames      = nbr_bytes / bytes_per_frame;

	return conv_frames_to_min (nbr_frames);
}



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
