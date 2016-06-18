/*****************************************************************************

        Question.cpp
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

#include "mfx/uitk/pg/Question.h"
#include "mfx/uitk/NodeEvt.h"
#include "mfx/uitk/PageMgrInterface.h"
#include "mfx/uitk/PageSwitcher.h"
#include "mfx/ui/Font.h"
#include "mfx/Model.h"
#include "mfx/View.h"

#include <cassert>



namespace mfx
{
namespace uitk
{
namespace pg
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Question::Question (PageSwitcher &page_switcher)
:	_page_switcher (page_switcher)
,	_view_ptr (0)
,	_page_ptr (0)
,	_page_size ()
,	_title_sptr (new NText (1000))
,	_choice_list ()
,	_arg_ptr (0)
{
	_title_sptr->set_justification (0.5f, 0, false);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Question::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const ui::Font &fnt_s, const ui::Font &fnt_m, const ui::Font &fnt_l)
{
	assert (usr_ptr != 0);
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;
	_arg_ptr   = reinterpret_cast <QArg *> (usr_ptr);

	assert (! _arg_ptr->_choice_arr.empty ());
	assert (_arg_ptr->_selection >= 0);
	assert (_arg_ptr->_selection < int (_arg_ptr->_choice_arr.size ()));

	const int      x_mid =  _page_size [0]      >> 1;
	const int      w_sel = (_page_size [0] * 3) >> 2;
	const int      h_l   = fnt_l.get_char_h ();
	const int      h_m   = fnt_m.get_char_h ();

	_title_sptr->set_font (fnt_l);
	_title_sptr->set_coord (Vec2d (x_mid, h_m * 0));
	_title_sptr->set_text (_arg_ptr->_title);
	_page_ptr->push_back (_title_sptr);

	const int      nbr_choices = int (_arg_ptr->_choice_arr.size ());
	_choice_list.resize (nbr_choices);
	for (int c_cnt = 0; c_cnt < nbr_choices; ++c_cnt)
	{
		TxtSPtr        choice_sptr (new NText (c_cnt));
		choice_sptr->set_font (fnt_m);
		choice_sptr->set_justification (0.5f, 0, false);
		choice_sptr->set_frame (Vec2d (w_sel, 0), Vec2d ());
		choice_sptr->set_coord (Vec2d (x_mid, h_m * (c_cnt + 2)));
		_choice_list [c_cnt] = choice_sptr;
		_page_ptr->push_back (choice_sptr);
	}

	PageMgrInterface::NavLocList  nav_list (nbr_choices);
	for (int pos = 0; pos < int (nav_list.size ()); ++pos)
	{
		NavLoc &       loc = nav_list [pos];
		loc._node_id = pos;
	}
	page.set_nav_layout (nav_list);
	page.jump_to (_arg_ptr->_selection);
}



void	Question::do_disconnect ()
{
	// Nothing
}



MsgHandlerInterface::EvtProp	Question::do_handle_evt (const NodeEvt &evt)
{
	EvtProp        ret_val = EvtProp_PASS;

	const int      node_id = evt.get_target ();

	if (evt.is_button_ex ())
	{
		const Button   but = evt.get_button_ex ();
		switch (but)
		{
		case Button_S:
			_arg_ptr->_selection = node_id;
			_arg_ptr->_ok_flag   = true;
			ret_val = EvtProp_CATCH;
			_page_switcher.return_page ();
			break;
		case Button_E:
			_arg_ptr->_selection = node_id;
			_arg_ptr->_ok_flag   = false;
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



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
