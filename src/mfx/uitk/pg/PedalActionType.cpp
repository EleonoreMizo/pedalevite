/*****************************************************************************

        PedalActionType.cpp
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

#include "mfx/uitk/pg/PedalActionType.h"
#include "mfx/uitk/pg/Tools.h"
#include "mfx/uitk/NodeEvt.h"
#include "mfx/uitk/PageMgrInterface.h"
#include "mfx/uitk/PageSwitcher.h"
#include "mfx/ui/Font.h"
#include "mfx/Cst.h"
#include "mfx/Model.h"
#include "mfx/PedalLoc.h"
#include "mfx/View.h"

#include <cassert>



namespace mfx
{
namespace uitk
{
namespace pg
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



PedalActionType::PedalActionType (PageSwitcher &page_switcher)
:	_page_switcher (page_switcher)
,	_model_ptr (0)
,	_view_ptr (0)
,	_page_ptr (0)
,	_page_size ()
,	_fnt_ptr (0)
,	_title_sptr (    new NText (Entry_TITLE    ))
,	_clear_sptr (    new NText (Entry_CLEAR    ))
,	_push_sptr (     new NText (Entry_PUSH     ))
,	_momentary_sptr (new NText (Entry_MOMENTARY))
,	_toggle_sptr (   new NText (Entry_TOGGLE   ))
{
	_title_sptr->set_justification (0.5f, 0, false);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	PedalActionType::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const ui::Font &fnt_s, const ui::Font &fnt_m, const ui::Font &fnt_l)
{
	assert (usr_ptr != 0);

	_model_ptr = &model;
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;
	_fnt_ptr   = &fnt_m;
	_ctx       = *reinterpret_cast <const PedalEditContext *> (usr_ptr);
	assert (_ctx._pedal >= 0);
	assert (_ctx._pedal < Cst::_nbr_pedals);

	const int      h_m = _fnt_ptr->get_char_h ();

	_title_sptr    ->set_font (fnt_m);
	_clear_sptr    ->set_font (fnt_m);
	_push_sptr     ->set_font (fnt_m);
	_momentary_sptr->set_font (fnt_m);
	_toggle_sptr   ->set_font (fnt_m);

	_title_sptr    ->set_coord (Vec2d (page_size [0] / 2, 0 * h_m));
	_clear_sptr    ->set_coord (Vec2d (0, 2 * h_m));
	_push_sptr     ->set_coord (Vec2d (0, 3 * h_m));
	_momentary_sptr->set_coord (Vec2d (0, 4 * h_m));
	_toggle_sptr   ->set_coord (Vec2d (0, 5 * h_m));

	_page_ptr->push_back (_title_sptr    );
	_page_ptr->push_back (_clear_sptr    );
	_page_ptr->push_back (_push_sptr     );
	_page_ptr->push_back (_momentary_sptr);
	_page_ptr->push_back (_toggle_sptr   );

	PageMgrInterface::NavLocList  nav_list (4);
	nav_list [0]._node_id = Entry_CLEAR    ;
	nav_list [1]._node_id = Entry_PUSH     ;
	nav_list [2]._node_id = Entry_MOMENTARY;
	nav_list [3]._node_id = Entry_TOGGLE   ;
	_page_ptr->set_nav_layout (nav_list);

	update_display ();
}



void	PedalActionType::do_disconnect ()
{
	// Nothing
}



MsgHandlerInterface::EvtProp	PedalActionType::do_handle_evt (const NodeEvt &evt)
{
	EvtProp        ret_val = EvtProp_PASS;

	const int      node_id = evt.get_target ();

	if (evt.is_button_ex ())
	{
		const Button   but = evt.get_button_ex ();
		switch (but)
		{
		case Button_S:
			switch (node_id)
			{
			case Entry_CLEAR:
				_model_ptr->set_pedal (
					_ctx.conv_to_loc (*_view_ptr),
					doc::PedalActionGroup ()
				);
				break;
			case Entry_PUSH:
				/*** To do ***/
				break;
			case Entry_MOMENTARY:
				/*** To do ***/
				break;
			case Entry_TOGGLE:
				/*** To do ***/
				break;
			default:
				assert (false);
				break;
			}
			_page_switcher.call_page (PageType_NOT_YET, 0, node_id);
			ret_val = EvtProp_CATCH;
			break;
		case Button_E:
			_page_switcher.switch_to (pg::PageType_PEDALBOARD_CONFIG, &_ctx);
			ret_val = EvtProp_CATCH;
			break;
		default:
			// Nothing
			break;
		}
	}

	return ret_val;
}



void	PedalActionType::do_set_pedalboard_layout (const doc::PedalboardLayout &layout)
{
	update_display ();
}



void	PedalActionType::do_set_pedal (const PedalLoc &loc, const doc::PedalActionGroup &content)
{
	update_display ();
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



const doc::PedalboardLayout &	PedalActionType::use_layout () const
{
	return _ctx.use_layout (*_view_ptr);
}



void	PedalActionType::update_display ()
{
	char           txt_0 [127+1];
	fstb::snprintf4all (txt_0, sizeof (txt_0), "%02d: ", _ctx._pedal + 1);
	std::string    title (txt_0);

	const doc::PedalboardLayout & layout = use_layout ();
	PedalConf      conf;
	title += Tools::conv_pedal_conf_to_short_txt (
		conf, layout, _ctx._pedal, *_model_ptr, *_view_ptr
	);

	static const char *  cm_0 = "\xE2\x9C\x93";  // U+2713 CHECK MARK
	std::string    cl_h = (conf == PedalConf_EMPTY    ) ? cm_0 : " ";
	std::string    pu_h = (conf == PedalConf_PUSH     ) ? cm_0 : " ";
	std::string    mo_h = (conf == PedalConf_MOMENTARY) ? cm_0 : " ";
	std::string    to_h = (conf == PedalConf_TOGGLE   ) ? cm_0 : " ";

	_title_sptr    ->set_text (title);
	_clear_sptr    ->set_text (cl_h + " Clear/empty");
	_push_sptr     ->set_text (pu_h + " Push");
	_momentary_sptr->set_text (mo_h + " Momentary");
	_toggle_sptr   ->set_text (to_h + " Toggle");
}



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
