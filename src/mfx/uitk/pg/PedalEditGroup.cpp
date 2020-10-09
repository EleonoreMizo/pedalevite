/*****************************************************************************

        PedalEditGroup.cpp
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
#include "fstb/fnc.h"
#include "mfx/uitk/pg/PedalEditGroup.h"
#include "mfx/uitk/pg/Tools.h"
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



PedalEditGroup::PedalEditGroup (PageSwitcher &page_switcher, PedalEditContext &ctx)
:	_page_switcher (page_switcher)
,	_ctx (ctx)
,	_model_ptr (nullptr)
,	_view_ptr (nullptr)
,	_page_ptr (nullptr)
,	_page_size ()
,	_fnt_ptr (nullptr)
,	_title_sptr (std::make_shared <NText> (Entry_TITLE  ))
,	_trig_sptr_arr ({{
		TxtSPtr ( std::make_shared <NText> (Entry_PRESS  ))
	,	TxtSPtr ( std::make_shared <NText> (Entry_HOLD   ))
	,	TxtSPtr ( std::make_shared <NText> (Entry_RELEASE))
	}})
{
	_title_sptr->set_justification (0.5f, 0, false);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	PedalEditGroup::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt)
{
	fstb::unused (usr_ptr);

	_model_ptr = &model;
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;
	_fnt_ptr   = &fnt._m;
	assert (_ctx._pedal >= 0);
	assert (_ctx._pedal < Cst::_nbr_pedals);

	const int      h_m   = _fnt_ptr->get_char_h ();
	const int      scr_w = _page_size [0];
	const int      x_mid =  scr_w >> 1;

	_title_sptr->set_font (fnt._m);
	_title_sptr->set_coord (Vec2d (x_mid, 0 * h_m));
	_page_ptr->push_back (_title_sptr);

	PageMgrInterface::NavLocList  nav_list;

	for (int trig_cnt = 0; trig_cnt < doc::ActionTrigger_NBR_ELT; ++trig_cnt)
	{
		_trig_sptr_arr [trig_cnt]->set_font (fnt._m);
		_trig_sptr_arr [trig_cnt]->set_coord (Vec2d (0, (2 + trig_cnt) * h_m));
		_trig_sptr_arr [trig_cnt]->set_frame (Vec2d (scr_w, 0), Vec2d ());
		_page_ptr->push_back (_trig_sptr_arr [trig_cnt]);
		PageMgrInterface::add_nav (nav_list, Entry_TRIG_BEG + trig_cnt);
	}

	_page_ptr->set_nav_layout (nav_list);

	update_display ();
}



void	PedalEditGroup::do_disconnect ()
{
	// Nothing
}



MsgHandlerInterface::EvtProp	PedalEditGroup::do_handle_evt (const NodeEvt &evt)
{
	EvtProp        ret_val = EvtProp_PASS;

	const int      node_id = evt.get_target ();

	if (evt.is_button_ex ())
	{
		const Button   but = evt.get_button_ex ();
		switch (but)
		{
		case Button_S:
			if (   node_id >= Entry_TRIG_BEG
			    && node_id < int (Entry_TRIG_BEG) + int (doc::ActionTrigger_NBR_ELT))
			{
				_ctx._trigger = doc::ActionTrigger (node_id - Entry_TRIG_BEG);
				_page_switcher.call_page (
					PageType_PEDAL_EDIT_CYCLE, nullptr, node_id
				);
				ret_val = EvtProp_CATCH;
			}
			else
			{
				assert (false);
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



void	PedalEditGroup::do_set_pedalboard_layout (const doc::PedalboardLayout &layout)
{
	fstb::unused (layout);

	update_display ();
}



void	PedalEditGroup::do_set_pedal (const PedalLoc &loc, const doc::PedalActionGroup &content)
{
	fstb::unused (loc, content);

	update_display ();
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



const doc::PedalboardLayout &	PedalEditGroup::use_layout () const
{
	return _ctx.use_layout (*_view_ptr);
}



void	PedalEditGroup::update_display ()
{
	char           txt_0 [127+1];
	fstb::snprintf4all (txt_0, sizeof (txt_0), "%02d: ", _ctx._pedal + 1);
	std::string    title (txt_0);

	const doc::PedalboardLayout & layout = use_layout ();
	_ctx._content = layout._pedal_arr [_ctx._pedal];
	PedalConf      conf;
	title += Tools::conv_pedal_conf_to_short_txt (
		conf, _ctx._content, *_model_ptr, *_view_ptr
	);

	_title_sptr->set_text (title);

	for (int trig_cnt = 0; trig_cnt < doc::ActionTrigger_NBR_ELT; ++trig_cnt)
	{
		const char *   name_0 =
			doc::ActionTrigger_get_name (doc::ActionTrigger (trig_cnt));
		fstb::snprintf4all (txt_0, sizeof (txt_0), "%-7s: ", name_0);
		std::string    txt (txt_0);

		const doc::PedalActionCycle & cy = _ctx._content._action_arr [trig_cnt];
		if (cy.is_empty_default ())
		{
			txt += "<empty>";
		}
		else if (cy._cycle.empty ())
		{
			txt += "<flags>";
		}
		else
		{
			const int      nbr_steps = int (cy._cycle.size ());
			fstb::snprintf4all (
				txt_0, sizeof (txt_0), "%d step%s",
				nbr_steps, (nbr_steps > 1) ? "s" : ""
			);
			txt += txt_0;
		}

		_trig_sptr_arr [trig_cnt]->set_text (txt);
	}
}



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
