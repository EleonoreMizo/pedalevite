/*****************************************************************************

        ParamControllers.cpp
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
#include "mfx/pi/param/Tools.h"
#include "mfx/uitk/pg/ParamControllers.h"
#include "mfx/uitk/pg/Tools.h"
#include "mfx/uitk/NodeEvt.h"
#include "mfx/uitk/PageMgrInterface.h"
#include "mfx/uitk/PageSwitcher.h"
#include "mfx/ui/Font.h"
#include "mfx/LocEdit.h"
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



ParamControllers::ParamControllers (PageSwitcher &page_switcher, LocEdit &loc_edit, const std::vector <CtrlSrcNamed> &csn_list)
:	_csn_list_base (csn_list)
,	_page_switcher (page_switcher)
,	_loc_edit (loc_edit)
,	_model_ptr (nullptr)
,	_view_ptr (nullptr)
,	_page_ptr (nullptr)
,	_page_size ()
,	_fnt_ptr (nullptr)
,	_menu_sptr (      std::make_shared <NWindow> (Entry_WINDOW    ))
,	_link_value_sptr (std::make_shared <NText  > (Entry_LINK_VALUE))
,	_link_title_sptr (std::make_shared <NText  > (Entry_LINK_TITLE))
,	_mod_title_sptr ( std::make_shared <NText  > (Entry_MOD_TITLE ))
,	_mod_list ()
,	_csn_list_full (_csn_list_base)
,	_cls_ptr (nullptr)
{
	_link_title_sptr->set_text ("Direct link");
	_mod_title_sptr ->set_text ("Modulations");
	_link_title_sptr->set_justification (0, 1, false);
	_mod_title_sptr ->set_justification (0, 1, false);

	_menu_sptr->set_autoscroll (true);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	ParamControllers::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt)
{
	fstb::unused (usr_ptr);
	assert (_loc_edit._slot_id >= 0);
	assert (_loc_edit._pi_type >= 0);
	assert (_loc_edit._param_index >= 0);

	_model_ptr = &model;
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;
	_fnt_ptr   = &fnt._m;

	const doc::Preset &  preset  = _view_ptr->use_preset_cur ();
	const int            slot_id = _loc_edit._slot_id;
	const doc::Slot &    slot    = preset.use_slot (slot_id);
	const doc::PluginSettings &   settings = slot.use_settings (_loc_edit._pi_type);
	auto           it_cls = settings._map_param_ctrl.find (_loc_edit._param_index);
	if (it_cls == settings._map_param_ctrl.end ())
	{
		_cls_ptr = nullptr;
	}
	else
	{
		_cls_ptr = &it_cls->second;
	}

	_link_value_sptr->set_font (*_fnt_ptr);
	_link_title_sptr->set_font (fnt._s);
	_mod_title_sptr ->set_font (fnt._s);

	const int      scr_w = _page_size [0];
	const int      h_m   = _fnt_ptr->get_char_h ();

	_menu_sptr->set_size (_page_size, Vec2d ());
	_menu_sptr->set_disp_pos (Vec2d ());

	_link_title_sptr->set_coord (Vec2d (0, 1 * h_m));
	_link_value_sptr->set_coord (Vec2d (0, 1 * h_m));
	_mod_title_sptr ->set_coord (Vec2d (0, 4 * h_m));

	_link_value_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());

	_page_ptr->push_back (_menu_sptr);

	set_controller_info ();
	_page_ptr->jump_to (conv_loc_edit_to_node_id ());
}



void	ParamControllers::do_disconnect ()
{
	// Nothing
}



MsgHandlerInterface::EvtProp	ParamControllers::do_handle_evt (const NodeEvt &evt)
{
	EvtProp        ret_val = EvtProp_PASS;

	const int      node_id = evt.get_target ();

	if (evt.is_cursor ())
	{
		if (   evt.get_cursor () == NodeEvt::Curs_ENTER
		    && (   node_id == Entry_LINK_TITLE
		        || (node_id >= 0 && node_id < int (_mod_list.size ()))))
		{
			update_loc_edit (node_id);
		}
	}

	else if (evt.is_button_ex ())
	{
		const Button   but = evt.get_button_ex ();
		switch (but)
		{
		case Button_S:
			if (   node_id == Entry_LINK_VALUE
			    || (node_id >= 0 && node_id < int (_mod_list.size ())))
			{
				update_loc_edit (node_id);
				_page_switcher.switch_to (PageType_CTRL_EDIT, nullptr);
				ret_val = EvtProp_CATCH;
			}
			else
			{
				ret_val = EvtProp_PASS;
			}
			break;
		case Button_E:
			_page_switcher.switch_to (pg::PageType_PARAM_EDIT, nullptr);
			ret_val = EvtProp_CATCH;
			break;
		default:
			// Nothing
			break;
		}
	}

	return ret_val;
}



void	ParamControllers::do_activate_preset (int index)
{
	fstb::unused (index);

	_page_switcher.switch_to (pg::PageType_PROG_EDIT, nullptr);
}



void	ParamControllers::do_remove_plugin (int slot_id)
{
	if (slot_id == _loc_edit._slot_id)
	{
		_page_switcher.switch_to (PageType_PROG_EDIT, nullptr);
	}
}



void	ParamControllers::do_set_param_ctrl (int slot_id, PiType type, int index, const doc::CtrlLinkSet &cls)
{
	fstb::unused (cls);

	if (   slot_id == _loc_edit._slot_id
	    && type    == _loc_edit._pi_type
	    && index   == _loc_edit._param_index)
	{
		set_controller_info ();
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	ParamControllers::set_controller_info ()
{
	assert (_fnt_ptr != nullptr);

	const std::vector <CtrlSrcNamed> csn_ports (
		Tools::make_port_list (*_model_ptr, *_view_ptr)
	);
	_csn_list_full = _csn_list_base;
	_csn_list_full.insert (
		_csn_list_full.begin (),
		csn_ports.begin (),
		csn_ports.end ()
	);

	const int      scr_w     = _page_size [0];
	const int      h_m       = _fnt_ptr->get_char_h ();

	const int      nbr_mod   =
		(_cls_ptr != nullptr) ? int (_cls_ptr->_mod_arr.size ()) : 0;
	const bool     bind_flag =
		(_cls_ptr != nullptr && _cls_ptr->_bind_sptr.get () != nullptr);

	PageMgrInterface::NavLocList  nav_list (1 + nbr_mod + 1);
	nav_list [0]._node_id = Entry_LINK_VALUE;

	_menu_sptr->clear_all_nodes ();
	_menu_sptr->push_back (_link_title_sptr);
	_menu_sptr->push_back (_link_value_sptr);
	_menu_sptr->push_back (_mod_title_sptr);

	if (bind_flag)
	{
		const std::string multilabel = Tools::find_ctrl_name (
			_cls_ptr->_bind_sptr->_source,
			_csn_list_full
		);
		const std::string txt = pi::param::Tools::print_name_bestfit (
			scr_w, multilabel.c_str (),
			*_link_value_sptr, &NText::get_char_width
		);
		_link_value_sptr->set_text (txt);
	}
	else
	{
		_link_value_sptr->set_text ("<Empty>");
	}

	_mod_list.resize (nbr_mod + 1);
	for (int m = 0; m <= nbr_mod; ++m)
	{
		TxtSPtr        mod_sptr { std::make_shared <NText> (m) };
		mod_sptr->set_font (*_fnt_ptr);
		mod_sptr->set_coord (Vec2d (0, h_m * (m + 4)));
		mod_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());

		if (m == nbr_mod)
		{
			mod_sptr->set_text ("<Empty>");
		}
		else
		{
			const std::string multilabel = Tools::find_ctrl_name (
				_cls_ptr->_mod_arr [m]->_source,
				_csn_list_full
			);
			const std::string txt = pi::param::Tools::print_name_bestfit (
				scr_w, multilabel.c_str (),
				*mod_sptr, &NText::get_char_width
			);
			mod_sptr->set_text (txt);
		}

		_mod_list [m] = mod_sptr;
		_menu_sptr->push_back (mod_sptr);

		nav_list [1 + m]._node_id = m;
	}

	_page_ptr->set_nav_layout (nav_list);

	_menu_sptr->invalidate_all ();
}



void	ParamControllers::update_loc_edit (int node_id)
{
	if (node_id == Entry_LINK_VALUE)
	{
		_loc_edit._ctrl_abs_flag = true;
		_loc_edit._ctrl_index    =
			(   _cls_ptr != nullptr
			 && _cls_ptr->_bind_sptr.get () != nullptr)
			? 0 : -1;
	}
	else
	{
		const int         nbr_mod = int (_mod_list.size ());
		if (node_id >= 0 && node_id < nbr_mod)
		{
			_loc_edit._ctrl_abs_flag = false;
			// N-1 -> -1
			_loc_edit._ctrl_index    = ((node_id + 1) % nbr_mod) - 1;
		}
	}
}



int	ParamControllers::conv_loc_edit_to_node_id () const
{
	if (! _loc_edit._ctrl_abs_flag)
	{
		const int         nbr_mod = int (_mod_list.size ());
		if (   _loc_edit._ctrl_index >= -1
		    && _loc_edit._ctrl_index < nbr_mod)
		{
			// -1 -> N-1
			return (_loc_edit._ctrl_index + nbr_mod) % nbr_mod;
		}
	}

	return Entry_LINK_VALUE;
}



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
