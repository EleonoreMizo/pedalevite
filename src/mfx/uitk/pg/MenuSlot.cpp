/*****************************************************************************

        MenuSlot.cpp
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
#include "mfx/piapi/PluginDescInterface.h"
#include "mfx/uitk/pg/MenuSlot.h"
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



MenuSlot::MenuSlot (PageSwitcher &page_switcher, LocEdit &loc_edit, const std::vector <std::string> &fx_list, const std::vector <std::string> &ms_list)
:	_page_switcher (page_switcher)
,	_loc_edit (loc_edit)
,	_fx_list (fx_list)
,	_ms_list (ms_list)
,	_model_ptr (0)
,	_view_ptr (0)
,	_page_ptr (0)
,	_page_size ()
,	_fnt_ptr (0)
,	_state (State_NORMAL)
,	_save_bank_index (-1)
,	_save_preset_index (-1)
,	_save_slot_id (-1)
,	_menu_sptr (new NWindow (Entry_WINDOW))
,	_typ_sptr (new NText (Entry_TYPE   ))
,	_ins_sptr (new NText (Entry_INSERT ))
,	_del_sptr (new NText (Entry_DELETE ))
,	_mov_sptr (new NText (Entry_MOVE   ))
,	_prs_sptr (new NText (Entry_PRESETS))
,	_rst_sptr (new NText (Entry_RESET  ))
,	_chn_sptr (new NText (Entry_CHN    ))
,	_frs_sptr (new NText (Entry_FRESH  ))
,	_lbl_sptr (new NText (Entry_LABEL  ))
,	_label_param ()
{
	_del_sptr->set_text ("Delete");
	_mov_sptr->set_text ("Move\xE2\x80\xA6");
	_prs_sptr->set_text ("Presets\xE2\x80\xA6");
	_rst_sptr->set_text ("Reset");

	_menu_sptr->push_back (_typ_sptr);
	_menu_sptr->push_back (_ins_sptr);
	_menu_sptr->push_back (_del_sptr);
	_menu_sptr->push_back (_mov_sptr);
	_menu_sptr->push_back (_prs_sptr);
	_menu_sptr->push_back (_rst_sptr);
	_menu_sptr->push_back (_chn_sptr);
	_menu_sptr->push_back (_frs_sptr);
	_menu_sptr->push_back (_lbl_sptr);
	_menu_sptr->set_autoscroll (true);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	MenuSlot::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt)
{
	fstb::unused (usr_ptr);

	_model_ptr = &model;
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;
	_fnt_ptr   = &fnt._m;

	if (_state == State_EDIT_LABEL)
	{
		if (   _label_param._ok_flag
		    && _view_ptr->get_bank_index ()   == _save_bank_index
		    && _view_ptr->get_preset_index () == _save_preset_index
		    && _loc_edit._slot_id             == _save_slot_id)
		{
			_model_ptr->set_slot_label (_loc_edit._slot_id, _label_param._label);
		}
	}
	_state = State_NORMAL;

	// Updates _chain_flag, if possible
	fix_chain_flag ();

	_menu_sptr->set_size (_page_size, Vec2d ());
	_menu_sptr->set_disp_pos (Vec2d ());

	const int      scr_w = _page_size [0];
	const int      h_m   = _fnt_ptr->get_char_h ();

	_typ_sptr->set_font (*_fnt_ptr);
	_ins_sptr->set_font (*_fnt_ptr);
	_del_sptr->set_font (*_fnt_ptr);
	_mov_sptr->set_font (*_fnt_ptr);
	_prs_sptr->set_font (*_fnt_ptr);
	_rst_sptr->set_font (*_fnt_ptr);
	_chn_sptr->set_font (*_fnt_ptr);
	_frs_sptr->set_font (*_fnt_ptr);
	_lbl_sptr->set_font (*_fnt_ptr);

	_typ_sptr->set_coord (Vec2d (0, h_m * 0));
	_ins_sptr->set_coord (Vec2d (0, h_m * 1));
	_del_sptr->set_coord (Vec2d (0, h_m * 2));
	_mov_sptr->set_coord (Vec2d (0, h_m * 3));
	_prs_sptr->set_coord (Vec2d (0, h_m * 4));
	_rst_sptr->set_coord (Vec2d (0, h_m * 5));
	_chn_sptr->set_coord (Vec2d (0, h_m * 6));
	_frs_sptr->set_coord (Vec2d (0, h_m * 7));
	_lbl_sptr->set_coord (Vec2d (0, h_m * 8));

	_typ_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_ins_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_del_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_mov_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_prs_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_rst_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_chn_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_frs_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
	_lbl_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());

	_page_ptr->push_back (_menu_sptr);

	update_display ();
}



void	MenuSlot::do_disconnect ()
{
	// Nothing
}



MsgHandlerInterface::EvtProp	MenuSlot::do_handle_evt (const NodeEvt &evt)
{
	EvtProp        ret_val = EvtProp_PASS;

	const int      node_id = evt.get_target ();

	if (evt.is_button_ex ())
	{
		const doc::Preset &  preset = _view_ptr->use_preset_cur ();

		const Button   but = evt.get_button_ex ();
		switch (but)
		{
		case Button_S:
			ret_val = EvtProp_CATCH;
			switch (node_id)
			{
			case Entry_INSERT:
				{
					int            slot_index_new =
						Tools::find_chain_index (preset, _loc_edit._slot_id);
					if (slot_index_new < 0)
					{
						slot_index_new = int (preset._routing._chain.size ());
					}
					const int      slot_id = _model_ptr->add_slot ();
					if (_loc_edit._chain_flag)
					{
						_model_ptr->insert_slot_in_chain (slot_index_new, slot_id);
					}
				}
				break;
			case Entry_DELETE:
				if (_loc_edit._slot_id >= 0)
				{
					_model_ptr->remove_slot (_loc_edit._slot_id);
				}
				else
				{
					assert (false);
				}
				break;
			case Entry_MOVE:
				if (_loc_edit._chain_flag)
				{
					_page_switcher.call_page (PageType_MOVE_FX, 0, node_id);
				}
				break;
			case Entry_PRESETS:
				_page_switcher.switch_to (PageType_MENU_PRESETS, 0);
				break;
			case Entry_RESET:
				ret_val = reset_plugin ();
				break;
			case Entry_CHN:
				if (_loc_edit._slot_id >= 0)
				{
					const auto     it_slot =
						preset._slot_map.find (_loc_edit._slot_id);
					assert (it_slot != preset._slot_map.end ());
					if (! preset.is_slot_empty (it_slot))
					{
						const doc::Slot & slot = *(it_slot->second);
						const doc::PluginSettings &   settings =
							slot.use_settings (PiType_MAIN);
						bool           fm_flag = settings._force_mono_flag;
						fm_flag = ! fm_flag;
						_model_ptr->set_plugin_mono (_loc_edit._slot_id, fm_flag);
					}
				}
				break;
			case Entry_FRESH:
				if (_loc_edit._slot_id >= 0)
				{
					const auto     it_slot =
						preset._slot_map.find (_loc_edit._slot_id);
					assert (it_slot != preset._slot_map.end ());
					if (! preset.is_slot_empty (it_slot))
					{
						const doc::Slot & slot = *(it_slot->second);
						const doc::PluginSettings &   settings =
							slot.use_settings (PiType_MAIN);
						bool           fresh_flag = settings._force_reset_flag;
						fresh_flag = ! fresh_flag;
						_model_ptr->set_plugin_reset (_loc_edit._slot_id, fresh_flag);
					}
				}
				break;
			case Entry_LABEL:
				{
					assert (_loc_edit._slot_id >= 0);
					const doc::Slot & slot = preset.use_slot (_loc_edit._slot_id);
					_label_param._label        = slot._label;
					_label_param._sep_cur_flag = true;
					_state                     = State_EDIT_LABEL;
					_save_bank_index           = _view_ptr->get_bank_index ();
					_save_preset_index         = _view_ptr->get_preset_index ();
					_save_slot_id              = _loc_edit._slot_id;
					_page_switcher.call_page (
						PageType_EDIT_LABEL, &_label_param, node_id
					);
					ret_val = EvtProp_CATCH;
				}
				break;
			default:
				ret_val = EvtProp_PASS;
				break;
			}
			break;
		case Button_E:
			if (   _loc_edit._slot_id < 0
			    || preset.is_slot_empty (_loc_edit._slot_id))
			{
				_page_switcher.switch_to (pg::PageType_EDIT_PROG, 0);
			}
			else
			{
				_page_switcher.switch_to (pg::PageType_PARAM_LIST, 0);
			}
			ret_val = EvtProp_CATCH;
			break;
		case Button_L:
			if (node_id == Entry_TYPE)
			{
				change_type (-1);
			}
			break;
		case Button_R:
			if (node_id == Entry_TYPE)
			{
				change_type (+1);
			}
			break;
		default:
			// Nothing
			break;
		}
	}

	return ret_val;
}



void	MenuSlot::do_activate_preset (int index)
{
	fstb::unused (index);

	_page_switcher.switch_to (pg::PageType_EDIT_PROG, 0);
}



void	MenuSlot::do_remove_slot (int slot_id)
{
	if (slot_id == _loc_edit._slot_id)
	{
		_loc_edit._slot_id = -1;
		update_display ();
	}
}



void	MenuSlot::do_insert_slot_in_chain (int index, int slot_id)
{
	fstb::unused (slot_id);

	const doc::Preset &  preset = _view_ptr->use_preset_cur ();

	if (_loc_edit._slot_id < 0)
	{
		_loc_edit._slot_id    = preset._routing._chain.back ();
		_loc_edit._chain_flag = true;
		_page_ptr->jump_to (Entry_TYPE);
	}
	else if (   index + 1 < int (preset._routing._chain.size ())
	         && _loc_edit._slot_id == preset._routing._chain [index + 1])
	{
		_loc_edit._slot_id    = preset._routing._chain [index];
		_loc_edit._chain_flag = true;
	}
	update_display ();
}



void	MenuSlot::do_erase_slot_from_chain (int index)
{
	const doc::Preset &  preset = _view_ptr->use_preset_cur ();
	if (std::find (
		preset._routing._chain.begin (),
		preset._routing._chain.end (),
		_loc_edit._slot_id
	) == preset._routing._chain.end ())
	{
		if (index < int (preset._routing._chain.size ()))
		{
			_loc_edit._slot_id = preset._routing._chain [index];
		}
		else
		{
			_loc_edit._slot_id = -1;
		}
	}
	update_display ();
}



void	MenuSlot::do_set_slot_label (int slot_id, std::string /*name*/)
{
	if (slot_id == _loc_edit._slot_id)
	{
		update_display ();
	}
}



void	MenuSlot::do_set_plugin (int slot_id, const PluginInitData &/*pi_data*/)
{
	if (slot_id == _loc_edit._slot_id)
	{
		update_display ();
	}
}



void	MenuSlot::do_remove_plugin (int slot_id)
{
	if (slot_id == _loc_edit._slot_id)
	{
		update_display ();
	}
}



void	MenuSlot::do_set_plugin_mono (int slot_id, bool /*mono_flag*/)
{
	if (slot_id == _loc_edit._slot_id)
	{
		update_display ();
	}
}



void	MenuSlot::do_set_plugin_reset (int slot_id, bool /*reset_flag*/)
{
	if (slot_id == _loc_edit._slot_id)
	{
		update_display ();
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	MenuSlot::update_display ()
{
	const int      scr_w = _page_size [0];

	PageMgrInterface::NavLocList  nav_list;
	NavLoc         nav;

	const doc::Preset &  preset = _view_ptr->use_preset_cur ();
	const int      slot_id      = _loc_edit._slot_id;
	const bool     exist_flag   = (slot_id >= 0);
	const bool     full_flag    =
		(exist_flag && ! preset.is_slot_empty (slot_id));

	std::string    multilabel = "<Empty>";
	if (full_flag)
	{
		const doc::Slot & slot = preset.use_slot (slot_id);
		const piapi::PluginDescInterface &  desc =
			_model_ptr->get_model_desc (slot._pi_model);
		multilabel = desc.get_name ();
	}
	std::string    txt = pi::param::Tools::print_name_bestfit (
		scr_w, multilabel.c_str (),
		*_typ_sptr, &NText::get_char_width
	);
	_typ_sptr->set_text (txt);
	nav._node_id = Entry_TYPE;
	nav_list.push_back (nav);

	_ins_sptr->set_text (_loc_edit._chain_flag ? "Insert before" : "Insert");
	nav._node_id = Entry_INSERT;
	nav_list.push_back (nav);

	_del_sptr->show (exist_flag);
	_mov_sptr->show (exist_flag && _loc_edit._chain_flag);
	if (exist_flag)
	{
		nav._node_id = Entry_DELETE;
		nav_list.push_back (nav);

		if (_loc_edit._chain_flag)
		{
			nav._node_id = Entry_MOVE;
			nav_list.push_back (nav);
		}
	}

	_prs_sptr->show (full_flag);
	_rst_sptr->show (full_flag);
	_chn_sptr->show (full_flag);
	_frs_sptr->show (full_flag);
	_lbl_sptr->show (full_flag);
	if (full_flag)
	{
		const doc::Slot & slot = preset.use_slot (slot_id);

		nav._node_id = Entry_PRESETS;
		nav_list.push_back (nav);

		nav._node_id = Entry_RESET;
		nav_list.push_back (nav);

		const doc::PluginSettings &   settings = slot.use_settings (PiType_MAIN);
		const bool     fm_flag = settings._force_mono_flag;
		_chn_sptr->set_text (
			  fm_flag
			? "Chan : prefer mono"
			: "Chan : auto"
		);
		nav._node_id = Entry_CHN;
		nav_list.push_back (nav);

		const bool     fresh_flag = settings._force_reset_flag;
		_frs_sptr->set_text (
			  fresh_flag
			? "State: fresh"
			: "State: keep"
		);
		nav._node_id = Entry_FRESH;
		nav_list.push_back (nav);

		_lbl_sptr->set_text ("Name : " + slot._label);
		nav._node_id = Entry_LABEL;
		nav_list.push_back (nav);
	}

	_page_ptr->set_nav_layout (nav_list);

	_menu_sptr->invalidate_all ();
}



MsgHandlerInterface::EvtProp	MenuSlot::change_type (int dir)
{
	const int      slot_id_new = Tools::change_plugin (
		*_model_ptr,
		*_view_ptr,
		_loc_edit._slot_id,
		dir,
		(_loc_edit._chain_flag) ? _fx_list : _ms_list,
		_loc_edit._chain_flag
	);

	if (slot_id_new != _loc_edit._slot_id)
	{
		_loc_edit._slot_id = slot_id_new;
		fix_chain_flag ();
		update_display ();
	}

	return EvtProp_CATCH;
}



MsgHandlerInterface::EvtProp	MenuSlot::reset_plugin ()
{
	const doc::Preset &  preset = _view_ptr->use_preset_cur ();
	const int      slot_id      = _loc_edit._slot_id;
	const doc::Slot & slot      = preset.use_slot (slot_id);
	
	const doc::CtrlLinkSet  cls;
	for (int type_cnt = 0; type_cnt < PiType_NBR_ELT; ++type_cnt)
	{
		const PiType   pi_type = static_cast <PiType> (type_cnt);
		const std::string pi_model =
			  (pi_type == PiType_MIX)
			? Cst::_plugin_mix
			: slot._pi_model;
		const piapi::PluginState & def =
			_model_ptr->use_default_settings (pi_model);
		const int      nbr_param    = int (def._param_list.size ());
		for (int index = 0; index < nbr_param; ++index)
		{
			const float    val = float (def._param_list [index]);
			_model_ptr->set_param (slot_id, pi_type, index, val);
			_model_ptr->set_param_ctrl (slot_id, pi_type, index, cls);
			_model_ptr->set_param_pres (slot_id, pi_type, index, 0);
		}

		/*** To do: add the default controllers ***/

	}

	update_display ();

	return EvtProp_CATCH;
}



void	MenuSlot::fix_chain_flag ()
{
	if (_loc_edit._slot_id >= 0)
	{
		const doc::Preset &  preset  = _view_ptr->use_preset_cur ();
		const auto           it_slot = preset._slot_map.find (_loc_edit._slot_id);
		if (it_slot != preset._slot_map.end ())
		{
			auto          it = std::find (
				preset._routing._chain.begin (),
				preset._routing._chain.end (),
				_loc_edit._slot_id
			);
			_loc_edit._chain_flag = (it != preset._routing._chain.end ());
		}
	}
}



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
