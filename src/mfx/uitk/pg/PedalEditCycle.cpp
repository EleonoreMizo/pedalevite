/*****************************************************************************

        PedalEditCycle.cpp
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

#include "fstb/fnc.h"
#include "mfx/uitk/pg/PedalEditCycle.h"
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



PedalEditCycle::PedalEditCycle (PageSwitcher &page_switcher, PedalEditContext &ctx)
:	_page_switcher (page_switcher)
,	_ctx (ctx)
,	_model_ptr (0)
,	_view_ptr (0)
,	_page_ptr (0)
,	_page_size ()
,	_fnt_ptr (0)
,	_title_sptr (      new NText (Entry_TITLE      ))
,	_inherit_sptr (    new NText (Entry_INHERIT    ))
,	_overridable_sptr (new NText (Entry_OVERRIDABLE))
,	_reset_sptr (      new NText (Entry_RESET      ))
,	_add_sptr (        new NText (Entry_ADD        ))
,	_menu_sptr (       new NWindow (Entry_WINDOW))
,	_step_sptr_arr ()
{
	_title_sptr->set_justification (0.5f, 0, false);
	_add_sptr  ->set_text ("<Add step>");
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	PedalEditCycle::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const ui::Font &fnt_s, const ui::Font &fnt_m, const ui::Font &fnt_l)
{
	_model_ptr = &model;
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;
	_fnt_ptr   = &fnt_m;
	assert (_ctx._pedal >= 0);
	assert (_ctx._pedal < Cst::_nbr_pedals);

	const int      h_m   = _fnt_ptr->get_char_h ();
	const int      scr_w = _page_size [0];
	const int      x_mid =  scr_w >> 1;

	_menu_sptr->set_size (_page_size, Vec2d ());
	_menu_sptr->set_disp_pos (Vec2d ());
	_menu_sptr->clear_all_nodes ();

	_title_sptr      ->set_font (fnt_m);
	_inherit_sptr    ->set_font (fnt_m);
	_overridable_sptr->set_font (fnt_m);
	_reset_sptr      ->set_font (fnt_m);
	_add_sptr        ->set_font (fnt_m);

	_inherit_sptr    ->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_overridable_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_reset_sptr      ->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_add_sptr        ->set_frame (Vec2d (scr_w, 0), Vec2d ());

	_title_sptr      ->set_coord (Vec2d (x_mid, 0 * h_m));

	_page_ptr->push_back (_menu_sptr);

	update_display ();
}



void	PedalEditCycle::do_disconnect ()
{
	// Nothing
}



MsgHandlerInterface::EvtProp	PedalEditCycle::do_handle_evt (const NodeEvt &evt)
{
	EvtProp        ret_val = EvtProp_PASS;

	const int      node_id = evt.get_target ();

	if (evt.is_button_ex ())
	{
		doc::PedalActionCycle & cycle =
			_ctx._content._action_arr [_ctx._trigger];
		const int      nbr_steps = int (
			_ctx._content._action_arr [_ctx._trigger]._cycle.size ()
		);

		const Button   but = evt.get_button_ex ();
		switch (but)
		{
		case Button_S:
			ret_val = EvtProp_CATCH;
			switch (node_id)
			{
			case Entry_INHERIT:
				cycle._inherit_flag     = ! cycle._inherit_flag;
				update_display ();
				break;
			case Entry_OVERRIDABLE:
				cycle._overridable_flag = ! cycle._overridable_flag;
				update_display ();
				break;
			case Entry_RESET:
				cycle._reset_on_pc_flag = ! cycle._reset_on_pc_flag;
				update_display ();
				break;
			case Entry_ADD:
				_ctx._step_index = nbr_steps;
				_page_switcher.call_page (PageType_NOT_YET, 0, node_id);
				/*** To do ***/
				break;
			default:
				if (   node_id >= Entry_STEP_LIST
				    && node_id <  Entry_STEP_LIST + nbr_steps)
				{
					_ctx._step_index = node_id - Entry_STEP_LIST;
					_page_switcher.call_page (PageType_NOT_YET, 0, node_id);
					/*** To do ***/
				}
				else
				{
					assert (false);
					ret_val = EvtProp_PASS;
				}
				break;
			}
			break;
		case Button_E:
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



void	PedalEditCycle::do_set_pedalboard_layout (const doc::PedalboardLayout &layout)
{
	update_display ();
}



void	PedalEditCycle::do_set_pedal (const PedalLoc &loc, const doc::PedalActionGroup &content)
{
	update_display ();
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



const doc::PedalboardLayout &	PedalEditCycle::use_layout () const
{
	return _ctx.use_layout (*_view_ptr);
}



void	PedalEditCycle::update_display ()
{
	char           txt_0 [127+1];
	const char *   trigger_name_0 =
		doc::ActionTrigger_get_name (_ctx._trigger);
	fstb::snprintf4all (
		txt_0, sizeof (txt_0), "Pedal %02d, %s",
		_ctx._pedal + 1, trigger_name_0
	);
	_title_sptr->set_text (txt_0);

	PageMgrInterface::NavLocList  nav_list;

	const doc::PedalActionCycle & cycle =
		_ctx._content._action_arr [_ctx._trigger];

	show_flag (*_inherit_sptr    , "Inherit"    , 11, cycle._inherit_flag);
	show_flag (*_overridable_sptr, "Overridable", 11, cycle._overridable_flag);
	show_flag (*_reset_sptr      , "Reset/prog" , 11, cycle._reset_on_pc_flag);

	_menu_sptr->clear_all_nodes ();
	_menu_sptr->push_back (_title_sptr      );
	_menu_sptr->push_back (_inherit_sptr    );
	_menu_sptr->push_back (_overridable_sptr);
	_menu_sptr->push_back (_reset_sptr      );

	const int      nbr_steps = int (cycle._cycle.size ());
	const bool     can_more_flag =
		(_ctx._trigger == doc::ActionTrigger_PRESS || nbr_steps < 1);

	const int      h_m   = _fnt_ptr->get_char_h ();
	const int      scr_w = _page_size [0];
	int            pos_y = 2;

	_step_sptr_arr.resize (nbr_steps);
	for (int step_cnt = 0; step_cnt < nbr_steps; ++step_cnt)
	{
		const int      node_id = Entry_STEP_LIST + step_cnt;
		TxtSPtr        step_sptr (new NText (node_id));

		doc::PedalActionCycle::ActionArray &   step =
			_ctx._content._action_arr [_ctx._trigger]._cycle [step_cnt];

		const int      nbr_actions = int (step.size ());
		fstb::snprintf4all (
			txt_0, sizeof (txt_0), "Step %2d    : %d %s",
			step_cnt + 1, nbr_actions, (nbr_actions > 1) ? "act." : "action"
		);

		step_sptr->set_font (*_fnt_ptr);
		step_sptr->set_coord (Vec2d (0, pos_y * h_m));
		step_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
		step_sptr->set_text (txt_0);
		_menu_sptr->push_back (step_sptr);
		PageMgrInterface::add_nav (nav_list, node_id);
		++ pos_y;
	}

	_add_sptr->show (can_more_flag);
	if (can_more_flag)
	{
		_add_sptr->set_coord (Vec2d (0, pos_y * h_m));
		_menu_sptr->push_back (_add_sptr);
		PageMgrInterface::add_nav (nav_list, Entry_ADD);
		++ pos_y;
	}

	_inherit_sptr    ->set_coord (Vec2d (0,  pos_y      * h_m));
	_overridable_sptr->set_coord (Vec2d (0, (pos_y + 1) * h_m));
	_reset_sptr      ->set_coord (Vec2d (0, (pos_y + 2) * h_m));

	PageMgrInterface::add_nav (nav_list, Entry_INHERIT    );
	PageMgrInterface::add_nav (nav_list, Entry_OVERRIDABLE);
	PageMgrInterface::add_nav (nav_list, Entry_RESET      );

	_page_ptr->set_nav_layout (nav_list);

	_menu_sptr->invalidate_all ();
}



void	PedalEditCycle::show_flag (NText &ctrl, const char *name_0, int name_size, bool flag)
{
	char           txt_0 [127+1];
	fstb::snprintf4all (
		txt_0, sizeof (txt_0), "%-*s: %s",
		name_size, name_0, (flag) ? "Yes" : "No"
	);
	ctrl.set_text (txt_0);
}



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
