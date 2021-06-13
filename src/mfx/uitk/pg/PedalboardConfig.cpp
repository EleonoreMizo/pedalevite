/*****************************************************************************

        PedalboardConfig.cpp
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
#include "mfx/uitk/pg/PedalboardConfig.h"
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



PedalboardConfig::PedalboardConfig (PageSwitcher &page_switcher, PedalEditContext &ctx)
:	_page_switcher (page_switcher)
,	_ctx (ctx)
,	_model_ptr (nullptr)
,	_view_ptr (nullptr)
,	_page_ptr (nullptr)
,	_page_size ()
,	_fnt_ptr (nullptr)
,	_menu_sptr (std::make_shared <NWindow> (Entry_WINDOW))
,	_pedal_list ()
{
	for (int ped_cnt = 0; ped_cnt < Cst::_nbr_pedals; ++ped_cnt)
	{
		_pedal_list [ped_cnt] =
			std::make_shared <NText> (Entry_PEDAL_LIST + ped_cnt);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	PedalboardConfig::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt)
{
	fstb::unused (usr_ptr);

	_model_ptr = &model;
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;
	_fnt_ptr   = &fnt._m;

	const int      h_m     = _fnt_ptr->get_char_h ();
	const int      scr_w   = _page_size [0];
	const int      frame_w = scr_w * 3 / 4;

	_menu_sptr->set_size (_page_size, Vec2d ());
	_menu_sptr->set_disp_pos (Vec2d ());
	_menu_sptr->clear_all_nodes ();

	PageMgrInterface::NavLocList  nav_list (Cst::_nbr_pedals);

	for (int ped_cnt = 0; ped_cnt < Cst::_nbr_pedals; ++ped_cnt)
	{
		TxtSPtr &      txt_sptr = _pedal_list [ped_cnt];
		txt_sptr->set_coord (Vec2d (0, ped_cnt * h_m));
		txt_sptr->set_font (*_fnt_ptr);
		txt_sptr->set_frame (Vec2d (frame_w, 0), Vec2d (0, 0));
		const int      node_id = Entry_PEDAL_LIST + ped_cnt;
		_menu_sptr->push_back (txt_sptr);
		nav_list [ped_cnt]._node_id = node_id;
	}

	_page_ptr->push_back (_menu_sptr);
	_page_ptr->set_nav_layout (nav_list);

	update_display ();

	_page_ptr->jump_to (Entry_PEDAL_LIST);
}



void	PedalboardConfig::do_disconnect ()
{
	// Nothing
}



MsgHandlerInterface::EvtProp	PedalboardConfig::do_handle_evt (const NodeEvt &evt)
{
	EvtProp        ret_val = EvtProp_PASS;

	const int      node_id = evt.get_target ();

	if (evt.is_button_ex ())
	{
		const Button   but = evt.get_button_ex ();
		switch (but)
		{
		case Button_S:
			{
				const int   pedal = node_id - Entry_PEDAL_LIST;
				if (pedal >= 0 && pedal < Cst::_nbr_pedals)
				{
					_ctx._pedal   = pedal;
					const doc::PedalboardLayout & layout = use_layout ();
					_ctx._content = layout._pedal_arr [pedal];
					_page_switcher.call_page (
						PageType_PEDAL_ACTION_TYPE, nullptr, node_id
					);
					ret_val = EvtProp_CATCH;
				}
			}
			break;
		case Button_E:
			_page_switcher.switch_to (_ctx._ret_page, nullptr);
			ret_val = EvtProp_CATCH;
			break;
		default:
			// Nothing
			break;
		}
	}

	return ret_val;
}



void	PedalboardConfig::do_set_pedalboard_layout (const doc::PedalboardLayout &layout)
{
	fstb::unused (layout);

	update_display ();
}



void	PedalboardConfig::do_set_pedal (const PedalLoc &loc, const doc::PedalActionGroup &content)
{
	fstb::unused (loc, content);

	update_display ();
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



const doc::PedalboardLayout &	PedalboardConfig::use_layout () const
{
	return _ctx.use_layout (*_view_ptr);
}



void	PedalboardConfig::update_display ()
{
	const doc::PedalboardLayout & layout = use_layout ();

	for (int ped_cnt = 0; ped_cnt < Cst::_nbr_pedals; ++ped_cnt)
	{
		NText &        ntxt = *(_pedal_list [ped_cnt]);

		char           txt_0 [127+1];
		fstb::snprintf4all (txt_0, sizeof (txt_0), "%02d: ", ped_cnt + 1);
		std::string    txt (txt_0);

		PedalConf      conf;
		txt += Tools::conv_pedal_conf_to_short_txt (conf, layout, ped_cnt, *_model_ptr, *_view_ptr);

		ntxt.set_text (txt);
	}
}



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
