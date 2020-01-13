/*****************************************************************************

        EndMsg.cpp
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

#include "mfx/uitk/pg/EndMsg.h"
#include "mfx/uitk/NodeEvt.h"
#include "mfx/uitk/PageMgrInterface.h"
#include "mfx/uitk/PageSwitcher.h"
#include "mfx/ui/Font.h"
#include "mfx/CmdLine.h"
#include "mfx/Model.h"
#include "mfx/View.h"

#if fstb_IS (SYS, LINUX)
	#include <unistd.h>

	#include <future>
#endif

#include <vector>

#include <cassert>
#include <cstdint>



namespace mfx
{
namespace uitk
{
namespace pg
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



EndMsg::EndMsg (PageSwitcher &page_switcher, const CmdLine &cmd_line)
:	_page_switcher (page_switcher)
,	_cmd_line (cmd_line)
,	_model_ptr (nullptr)
,	_view_ptr (nullptr)
,	_page_ptr (nullptr)
,	_page_size ()
,	_line_list ()
,	_end_type (EndType_NONE)
,	_action_date (0)
,	_msg_arg ()
{
	// Nothing
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	EndMsg::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt)
{
	assert (usr_ptr != nullptr);

	_model_ptr = &model;
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;
	_end_type  = EndType (reinterpret_cast <intptr_t> (usr_ptr));

	std::string    txt;
	switch (_end_type)
	{
	case EndType_RESTART:
		txt = "Restarting\xE2\x80\xA6";
		break;
	case EndType_REBOOT:
		txt = "Rebooting\xE2\x80\xA6";
		break;
	case EndType_SHUTDOWN:
		txt =
			"Shuting down\xE2\x80\xA6\n"
			"Please wait a\n"
			"few seconds\n"
			"before turning\n"
			"the device off.";
		break;
	default:
		break;
	}

	size_t         pos = 0;
	std::vector <std::string> line_arr;
	while (pos < txt.length ())
	{
		size_t         pos_nxt = txt.find ('\n', pos);
		if (pos_nxt == std::string::npos)
		{
			line_arr.push_back (txt.substr (pos));
			pos_nxt = txt.length ();
		}
		else
		{
			line_arr.push_back (txt.substr (pos, pos_nxt - pos));
			++ pos_nxt;
		}
		pos = pos_nxt;
	}

	const int      x_mid =  _page_size [0] >> 1;
	const int      h_l   = fnt._l.get_char_h ();

	_line_list.clear ();
	const int      nbr_lines = int (line_arr.size ());
	const int      y_base    = (_page_size [1] - nbr_lines * h_l) >> 1;
	for (int l_cnt = 0; l_cnt < nbr_lines; ++l_cnt)
	{
		TxtSPtr        line_sptr { std::make_shared <NText> (l_cnt) };
		line_sptr->set_font (fnt._l);
		line_sptr->set_justification (0.5f, 0, false);
		line_sptr->set_frame (Vec2d (_page_size [0], 0), Vec2d ());
		line_sptr->set_coord (Vec2d (x_mid, y_base + h_l * l_cnt));
		line_sptr->set_text (line_arr [l_cnt]);
		_line_list.push_back (line_sptr);
		_page_ptr->push_back (line_sptr);
	}

	const std::chrono::microseconds  cur_date (_model_ptr->get_cur_date ());
	_action_date = cur_date + std::chrono::seconds (1);

	_page_ptr->set_timer (0, true);
}



void	EndMsg::do_disconnect ()
{
	_page_ptr->set_timer (0, false);
}



MsgHandlerInterface::EvtProp	EndMsg::do_handle_evt (const NodeEvt &evt)
{
	EvtProp        ret_val = EvtProp_PASS;

	if (evt.is_timer ())
	{
#if fstb_IS (SYS, LINUX)
		const auto     cur_date = _model_ptr->get_cur_date ();
		if (cur_date >= _action_date)
		{
			int            ret_val_sys = 0;

			switch (_end_type)
			{
			case EndType_RESTART:
				{
					char * const * argv = _cmd_line.use_argv ();
					char * const * envp = _cmd_line.use_envp ();
					ret_val_sys = execve (argv [0], argv, envp);
				}
				break;
			case EndType_REBOOT:
				ret_val_sys = system ("sudo shutdown -r now");
				break;
			case EndType_SHUTDOWN:
				ret_val_sys = system ("sudo shutdown -h now");
				break;
			default:
				break;
			}

			if (ret_val_sys != 0)
			{
				const int      node_id = evt.get_target ();
				Question::msg_box (
					"Command failed.", "Cancel",
					_msg_arg, _page_switcher, node_id
				);
			}
		}
#endif

		ret_val = EvtProp_CATCH;
	}

	return (ret_val);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
