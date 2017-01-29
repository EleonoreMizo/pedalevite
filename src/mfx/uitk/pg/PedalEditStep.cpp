/*****************************************************************************

        PedalEditStep.cpp
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
#include "mfx/doc/ActionPreset.h"
#include "mfx/uitk/pg/PedalEditContext.h"
#include "mfx/uitk/pg/PedalEditStep.h"
#include "mfx/uitk/pg/Tools.h"
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



PedalEditStep::PedalEditStep (PageSwitcher &page_switcher, PedalEditContext &ctx)
:	_page_switcher (page_switcher)
,	_ctx (ctx)
,	_model_ptr (0)
,	_view_ptr (0)
,	_page_ptr (0)
,	_page_size ()
,	_fnt_ptr (0)
,	_title_sptr (new NText (Entry_TITLE))
,	_add_sptr (  new NText (Entry_ADD  ))
,	_up_sptr (   new NText (Entry_UP   ))
,	_down_sptr ( new NText (Entry_DOWN ))
,	_menu_sptr (new NWindow (Entry_WINDOW))
,	_action_sptr_arr ()
{
	_title_sptr->set_justification (0.5f, 0, false);
	_add_sptr  ->set_text ("<Add action>");
	_up_sptr   ->set_text ("<Move up>");
	_down_sptr ->set_text ("<Move down>");
	_menu_sptr->set_autoscroll (true);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	PedalEditStep::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt)
{
	_model_ptr = &model;
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;
	_fnt_ptr   = &fnt._m;
	assert (_ctx._pedal >= 0);
	assert (_ctx._pedal < Cst::_nbr_pedals);
	assert (_ctx._trigger >= 0);
	assert (_ctx._trigger < doc::ActionTrigger_NBR_ELT);

	const int      h_m   = _fnt_ptr->get_char_h ();
	const int      scr_w = _page_size [0];
	const int      x_mid =  scr_w >> 1;

	_menu_sptr->set_size (_page_size, Vec2d ());
	_menu_sptr->set_disp_pos (Vec2d ());
	_menu_sptr->clear_all_nodes ();

	_title_sptr->set_font (fnt._m);
	_up_sptr   ->set_font (fnt._m);
	_down_sptr ->set_font (fnt._m);
	_add_sptr  ->set_font (fnt._m);

	_up_sptr   ->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_down_sptr ->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_add_sptr  ->set_frame (Vec2d (scr_w, 0), Vec2d ());

	_title_sptr->set_coord (Vec2d (x_mid, 0 * h_m));

	_page_ptr->push_back (_menu_sptr);

	check_ctx ();
	update_display ();
}



void	PedalEditStep::do_disconnect ()
{
	// Nothing
}



MsgHandlerInterface::EvtProp	PedalEditStep::do_handle_evt (const NodeEvt &evt)
{
	EvtProp        ret_val = EvtProp_PASS;

	const int      node_id = evt.get_target ();

	if (evt.is_button_ex ())
	{
		doc::PedalActionCycle & cycle =
			_ctx._content._action_arr [_ctx._trigger];
		const int      nbr_steps      = int (cycle._cycle.size ());
		doc::PedalActionCycle::ActionArray &   step =
			cycle._cycle [_ctx._step_index];
		const int      nbr_actions    = int (step.size ());

		const Button   but = evt.get_button_ex ();
		switch (but)
		{
		case Button_S:
			ret_val = EvtProp_CATCH;
			switch (node_id)
			{
			case Entry_UP:
				if (_ctx._step_index > 0)
				{
					std::swap (
						cycle._cycle [_ctx._step_index - 1],
						cycle._cycle [_ctx._step_index    ]
					);
					-- _ctx._step_index;
					update_model ();
				}
				break;
			case Entry_DOWN:
				if (_ctx._step_index + 1 < nbr_steps)
				{
					std::swap (
						cycle._cycle [_ctx._step_index    ],
						cycle._cycle [_ctx._step_index + 1]
					);
					++ _ctx._step_index;
					update_model ();
				}
				break;
			case Entry_ADD:
				{
					// Creates and add a non-empty action
					doc::PedalActionCycle::ActionSPtr   action_sptr (
						new doc::ActionPreset (false, 0)
					);
					step.push_back (action_sptr);
					_ctx._action_index = nbr_actions;
					update_model ();
					_page_switcher.call_page (
						PageType_PEDAL_EDIT_ACTION, 0, node_id
					);
				}
				break;
			default:
				if (   node_id >= Entry_ACTION_LIST
				    && node_id <  Entry_ACTION_LIST + nbr_actions)
				{
					_ctx._action_index = node_id - Entry_ACTION_LIST;
					_page_switcher.call_page (
						PageType_PEDAL_EDIT_ACTION, 0, node_id
					);
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



void	PedalEditStep::do_set_pedalboard_layout (const doc::PedalboardLayout &layout)
{
	check_ctx ();
	update_display ();
}



void	PedalEditStep::do_set_pedal (const PedalLoc &loc, const doc::PedalActionGroup &content)
{
	check_ctx ();
	update_display ();
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



const doc::PedalboardLayout &	PedalEditStep::use_layout () const
{
	return _ctx.use_layout (*_view_ptr);
}



void	PedalEditStep::check_ctx ()
{
	assert (_ctx._pedal >= 0);
	assert (_ctx._pedal < Cst::_nbr_pedals);
	assert (_ctx._trigger >= 0);
	assert (_ctx._trigger < doc::ActionTrigger_NBR_ELT);

	const doc::PedalActionCycle & cycle =
		_ctx._content._action_arr [_ctx._trigger];
	const int      nbr_steps   = int (cycle._cycle.size ());
	assert (nbr_steps > 0);
	_ctx._step_index   = fstb::limit (_ctx._step_index, 0, nbr_steps - 1);
	const doc::PedalActionCycle::ActionArray &   step =
		cycle._cycle [_ctx._step_index];
	const int      nbr_actions = int (step.size ());
	assert (nbr_actions > 0);
	_ctx._action_index = fstb::limit (_ctx._action_index, 0, nbr_actions - 1);
}



void	PedalEditStep::update_model ()
{
	_model_ptr->set_pedal (_ctx.conv_to_loc (*_view_ptr), _ctx._content);
}



void	PedalEditStep::update_display ()
{
	char           txt_0 [127+1];
	const char *   trigger_name_0 =
		doc::ActionTrigger_get_name (_ctx._trigger);
	fstb::snprintf4all (
		txt_0, sizeof (txt_0), "P%02d, %s, step %d",
		_ctx._pedal + 1, trigger_name_0, _ctx._step_index
	);
	_title_sptr->set_text (txt_0);

	PageMgrInterface::NavLocList  nav_list;

	const doc::PedalActionCycle & cycle =
		_ctx._content._action_arr [_ctx._trigger];
	const doc::PedalActionCycle::ActionArray &   step =
		cycle._cycle [_ctx._step_index];

	_menu_sptr->clear_all_nodes ();
	_menu_sptr->push_back (_title_sptr);
	_menu_sptr->push_back (_add_sptr);
	_menu_sptr->push_back (_up_sptr);
	_menu_sptr->push_back (_down_sptr);

	const int      nbr_steps   = int (cycle._cycle.size ());
	const int      nbr_actions = int (step.size ());

	const int      h_m   = _fnt_ptr->get_char_h ();
	const int      scr_w = _page_size [0];
	int            pos_y = 2;

	_action_sptr_arr.resize (nbr_actions);
	for (int action_cnt = 0; action_cnt < nbr_actions; ++action_cnt)
	{
		const int      node_id = Entry_ACTION_LIST + action_cnt;
		TxtSPtr        action_sptr (new NText (node_id));

		assert (step [action_cnt].get () != 0);
		const doc::PedalActionSingleInterface &   action = *(step [action_cnt]);

		const std::string desc = Tools::conv_pedal_action_to_short_txt (
			action, *_model_ptr, *_view_ptr
		);
		fstb::snprintf4all (
			txt_0, sizeof (txt_0), "%02d: %s", action_cnt, desc.c_str ()
		);

		action_sptr->set_font (*_fnt_ptr);
		action_sptr->set_coord (Vec2d (0, pos_y * h_m));
		action_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
		action_sptr->set_text (txt_0);
		_menu_sptr->push_back (action_sptr);
		PageMgrInterface::add_nav (nav_list, node_id);
		++ pos_y;
	}

	_add_sptr ->set_coord (Vec2d (0, pos_y * h_m));
	PageMgrInterface::add_nav (nav_list, Entry_ADD);
	++ pos_y;

	const bool     can_move_up_flag   = (_ctx._step_index     > 0        );
	const bool     can_move_down_flag = (_ctx._step_index + 1 < nbr_steps);

	_up_sptr->show (can_move_up_flag);
	if (can_move_up_flag)
	{
		_up_sptr->set_coord (Vec2d (0, pos_y * h_m));
		PageMgrInterface::add_nav (nav_list, Entry_UP);
		++ pos_y;
	}

	_down_sptr->show (can_move_down_flag);
	if (can_move_down_flag)
	{
		_down_sptr->set_coord (Vec2d (0, pos_y * h_m));
		PageMgrInterface::add_nav (nav_list, Entry_DOWN);
		++ pos_y;
	}

	_page_ptr->set_nav_layout (nav_list);

	_menu_sptr->invalidate_all ();
}



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
