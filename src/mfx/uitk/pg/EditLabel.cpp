/*****************************************************************************

        EditLabel.cpp
        Author: Laurent de Soras, 2017

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
#include "mfx/uitk/pg/EditLabel.h"
#include "mfx/uitk/NodeEvt.h"
#include "mfx/uitk/PageMgrInterface.h"
#include "mfx/uitk/PageSwitcher.h"
#include "mfx/ui/Font.h"
#include "mfx/View.h"

#include <cassert>



namespace mfx
{
namespace uitk
{
namespace pg
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



EditLabel::EditLabel (PageSwitcher &page_switcher)
:	_page_switcher (page_switcher)
,	_view_ptr (0)
,	_page_ptr (0)
,	_page_size ()
,	_menu_sptr (new NWindow (Entry_WINDOW))
,	_name_sptr (new NText (Entry_NAME))
,	_edit_sptr (new NText (Entry_EDIT))
,	_label_sptr_arr ()
,	_arg_ptr (0)
,	_label_arr ()
,	_edit_text_arg ()
,	_edit_text_flag (false)
{
	_name_sptr->set_bold (true, true);
	_edit_sptr->set_text ("<Edit>");
	_menu_sptr->set_autoscroll (true);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	EditLabel::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt)
{
	fstb::unused (model);

	assert (usr_ptr != 0);
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;
	_fnt_ptr   = &fnt._m;
	_arg_ptr   = reinterpret_cast <Param *> (usr_ptr);

	if (_edit_text_flag)
	{
		_edit_text_flag = false;
		if (_edit_text_arg._ok_flag)
		{
			_arg_ptr->_label   = _edit_text_arg._text;
			_arg_ptr->_ok_flag = true;
			_page_switcher.return_page ();
		}
	}

	const int      h_m   = _fnt_ptr->get_char_h ();
	const int      scr_w = _page_size [0];

	_menu_sptr->clear_all_nodes ();
	_menu_sptr->set_size (_page_size, Vec2d ());
	_menu_sptr->set_disp_pos (Vec2d ());

	PageMgrInterface::NavLocList  nav_list;

	_name_sptr->set_font (*_fnt_ptr);
	_name_sptr->set_coord (Vec2d (0, 0 * h_m));
	_name_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_name_sptr->set_text (_arg_ptr->_label);
	_menu_sptr->push_back (_name_sptr);
	PageMgrInterface::add_nav (nav_list, Entry_NAME);

	_edit_sptr->set_font (*_fnt_ptr);
	_edit_sptr->set_coord (Vec2d (0, 3 * h_m / 2));
	_edit_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_menu_sptr->push_back (_edit_sptr);
	PageMgrInterface::add_nav (nav_list, Entry_EDIT);

	_label_arr.clear ();
	std::set <std::string>  labels_all (_view_ptr->collect_labels (false));
	std::set <std::string>  labels_cur;
	int            y       = 3 * h_m;
	int            node_id = Entry_LIST;
	if (_arg_ptr->_sep_cur_flag)
	{
		labels_cur = _view_ptr->collect_labels (true);
		for (const auto &label : labels_cur)
		{
			const auto     it = labels_all.find (label);
			if (it != labels_all.end ())
			{
				labels_all.erase (it);
			}
		}
		add_label_list (nav_list, labels_cur, Entry_LIST, y);
		const int      nbr_labels_cur = int (labels_cur.size ());
		if (nbr_labels_cur > 0)
		{
			y       += nbr_labels_cur * h_m + (h_m >> 1);
			node_id += nbr_labels_cur;
		}
	}
	add_label_list (nav_list, labels_all, node_id, y);

	page.push_back (_menu_sptr);
	page.set_nav_layout (nav_list);
	page.jump_to (Entry_NAME);
	_menu_sptr->invalidate_all ();
}



void	EditLabel::do_disconnect ()
{
	// Nothing
}



MsgHandlerInterface::EvtProp	EditLabel::do_handle_evt (const NodeEvt &evt)
{
	EvtProp        ret_val = EvtProp_PASS;

	const int      node_id = evt.get_target ();

	if (evt.is_button_ex ())
	{
		const Button   but = evt.get_button_ex ();
		switch (but)
		{
		case Button_S:
			if (node_id == Entry_NAME)
			{
				_arg_ptr->_ok_flag = true;
				_page_switcher.return_page ();
			}
			else if (node_id == Entry_EDIT)
			{
				_edit_text_arg._title = "Effect label:";
				_edit_text_arg._text  = _arg_ptr->_label;
				_edit_text_flag       = true;
				_page_switcher.call_page (
					PageType_EDIT_TEXT, &_edit_text_arg, node_id
				);
			}
			else
			{
				const int         pos = node_id - Entry_LIST;
				if (pos >= 0 && pos < int (_label_arr.size ()))
				{
					_arg_ptr->_label   = _label_arr [pos];
					_arg_ptr->_ok_flag = true;
					_page_switcher.return_page ();
				}
				else
				{
					assert (false);
				}
			}
			ret_val = EvtProp_CATCH;
			break;
		case Button_E:
			_arg_ptr->_ok_flag = false;
			_page_switcher.return_page ();
			ret_val = EvtProp_CATCH;
			break;
		default:
			// Nothing
			break;
		}
	}

	return ret_val;
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	EditLabel::add_label_list (PageMgrInterface::NavLocList &nav_list, const std::set <std::string> &labels, int node_id_base, int y)
{
	const int      h_m   = _fnt_ptr->get_char_h ();
	const int      scr_w = _page_size [0];

	int            label_cnt = 0;
	for (const std::string &label : labels)
	{
		const int      node_id = node_id_base + label_cnt;
		TxtSPtr        label_sptr (new NText (node_id));
		
		label_sptr->set_font (*_fnt_ptr);
		label_sptr->set_coord (Vec2d (0, y + label_cnt * h_m));
		label_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
		label_sptr->set_text (label);

		_menu_sptr->push_back (label_sptr);
		PageMgrInterface::add_nav (nav_list, node_id);

		_label_arr.push_back (label);

		++ label_cnt;
	}
}



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
