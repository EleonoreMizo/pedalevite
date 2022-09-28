/*****************************************************************************

        ParamList.cpp
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
#include "fstb/fnc.h"
#include "mfx/pi/dwm/Param.h"
#include "mfx/pi/param/Tools.h"
#include "mfx/piapi/PluginDescInterface.h"
#include "mfx/uitk/pg/ParamList.h"
#include "mfx/uitk/pg/Tools.h"
#include "mfx/uitk/NodeEvt.h"
#include "mfx/uitk/PageMgrInterface.h"
#include "mfx/uitk/PageSwitcher.h"
#include "mfx/ui/Font.h"
#include "mfx/Cst.h"
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



ParamList::ParamList (PageSwitcher &page_switcher, LocEdit &loc_edit)
:	_page_switcher (page_switcher)
,	_loc_edit (loc_edit)
,	_model_ptr (nullptr)
,	_view_ptr (nullptr)
,	_page_ptr (nullptr)
,	_page_size ()
,	_fnt_ptr (nullptr)
,	_menu_sptr (    std::make_shared <NWindow> (Entry_WINDOW  ))
,	_fx_setup_sptr (std::make_shared <NText  > (Entry_FX_SETUP))
,	_gui_sptr (     std::make_shared <NText  > (Entry_GUI     ))
,	_param_list ()
,	_mixer_flag (true)
,	_gui_flag (false)
,	_gui_page (PageType_INVALID)
,	_unit_flag (false)
{
	_fx_setup_sptr->set_text ("FX Setup\xE2\x80\xA6");
	_gui_sptr     ->set_text ("Graphic editing\xE2\x80\xA6");
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	ParamList::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt)
{
	fstb::unused (usr_ptr);

	_model_ptr = &model;
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;
	_fnt_ptr   = &fnt._m;

	const int      scr_w = _page_size [0];
	const int      frm_w = (scr_w * 3) >> 2;
	const int      h_m   = _fnt_ptr->get_char_h ();

	const int      w_m   = _fnt_ptr->get_char_w (U'M');
	_unit_flag = (scr_w >= 40 * w_m);

	_menu_sptr->set_size (_page_size, Vec2d ());
	_menu_sptr->set_disp_pos (Vec2d ());

	_fx_setup_sptr->set_font (*_fnt_ptr);
	_gui_sptr     ->set_font (*_fnt_ptr);

	_fx_setup_sptr->set_coord (Vec2d (0, 0 * h_m));
	_gui_sptr     ->set_coord (Vec2d (0, 1 * h_m));

	_fx_setup_sptr->set_frame (Vec2d (frm_w, 0), Vec2d ());
	_gui_sptr     ->set_frame (Vec2d (frm_w, 0), Vec2d ());

	_page_ptr->push_back (_menu_sptr);

	set_param_info ();
	_page_ptr->jump_to (conv_loc_edit_to_node_id ());
}



void	ParamList::do_disconnect ()
{
	// Nothing
}



MsgHandlerInterface::EvtProp	ParamList::do_handle_evt (const NodeEvt &evt)
{
	EvtProp        ret_val = EvtProp_PASS;

	const int      node_id = evt.get_target ();

	if (evt.is_cursor ())
	{
		const NodeEvt::Curs  curs = evt.get_cursor ();
		if (is_nid_in_param_list (node_id))
		{
			if (curs == NodeEvt::Curs_ENTER)
			{
				update_loc_edit (node_id);
			}

			// Reflects the (un)selection on the value and unit nodes
			PiType         type  = PiType_INVALID;
			int            index = -1;
			conv_node_id_to_param (type, index, node_id);
			const int      list_pos = conv_param_to_list_pos (type, index);
			NodeIdRow      nid_row;
			conv_param_to_node_id (nid_row, type, index);

			assert (node_id == nid_row [Col_NAME]);

			NodeEvt        evt_v (NodeEvt::create_cursor (nid_row [Col_VAL ], curs));
			_param_list [Col_VAL ] [list_pos]->handle_evt (evt_v);

			if (_unit_flag)
			{
				NodeEvt        evt_u (NodeEvt::create_cursor (nid_row [Col_UNIT], curs));
				_param_list [Col_UNIT] [list_pos]->handle_evt (evt_u);
			}
		}
	}

	else if (evt.is_button_ex ())
	{
		const Button   but = evt.get_button_ex ();
		switch (but)
		{
		case Button_S:
			ret_val = EvtProp_CATCH;
			if (node_id == Entry_FX_SETUP)
			{
				_page_switcher.switch_to (PageType_SLOT_MENU, nullptr);
			}
			else if (node_id == Entry_GUI)
			{
				if (_gui_flag)
				{
					_page_switcher.switch_to (_gui_page, nullptr);
				}
				else
				{
					assert (false);
					ret_val = EvtProp_PASS;
				}
			}
			else if (is_nid_in_param_list (node_id))
			{
				update_loc_edit (node_id);
				if (_loc_edit._param_index >= 0)
				{
					_page_switcher.switch_to (pg::PageType_PARAM_EDIT, nullptr);
				}
			}
			else
			{
				assert (false);
				ret_val = EvtProp_PASS;
			}
			break;
		case Button_E:
			_page_switcher.switch_to (pg::PageType_PROG_EDIT, nullptr);
			ret_val = EvtProp_CATCH;
			break;
		case Button_L:
			ret_val = change_param (node_id, -1);
			break;
		case Button_R:
			ret_val = change_param (node_id, +1);
			break;
		default:
			// Nothing
			break;
		}
	}

	return ret_val;
}



void	ParamList::do_activate_prog (int index)
{
	fstb::unused (index);

	_page_switcher.switch_to (PageType_PROG_EDIT, nullptr);
}



void	ParamList::do_set_param (int slot_id, int index, float val, PiType type)
{
	fstb::unused (val);

	if (slot_id == _loc_edit._slot_id)
	{
		update_param_txt (type, index);
	}
}



void	ParamList::do_remove_plugin (int slot_id)
{
	if (slot_id == _loc_edit._slot_id)
	{
		_page_switcher.switch_to (PageType_PROG_EDIT, nullptr);
	}
}



void	ParamList::do_set_param_ctrl (int slot_id, PiType type, int index, const doc::CtrlLinkSet &cls)
{
	fstb::unused (type, index, cls);

	if (slot_id == _loc_edit._slot_id)
	{
		set_param_info ();
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	ParamList::check_gui (const std::string &pi_model)
{
	_gui_flag = false;
	_gui_page = PageType_INVALID;

	static const int  peq_len = 3;
	if (pi_model.substr (0, peq_len) == "peq")
	{
		// 4 bands
		if (pi_model.length () == peq_len)
		{
			_gui_flag = true;
			_gui_page = PageType_FX_PEQ;
		}

		// Other number of bands
		else
		{
			const char *   nbands_0  = pi_model.c_str () + peq_len;
			char *         end_0     = const_cast <char *> (nbands_0);
			long           nbr_bands = strtol (nbands_0, &end_0, 10);
			if (   end_0 != nbands_0
			    && end_0 - nbands_0 + peq_len == int (pi_model.length ()))
			{
				if (nbr_bands > 0)
				{
					_gui_flag = true;
					_gui_page = PageType_FX_PEQ;
				}
			}
		}
	}
	else if (pi_model == "lfo1" || pi_model == "lfo1slow")
	{
		_gui_flag = true;
		_gui_page = PageType_FX_LFO;
	}
	else if (pi_model == "compex")
	{
		_gui_flag = true;
		_gui_page = PageType_FX_CPX;
	}
}



void	ParamList::set_param_info ()
{
	assert (_fnt_ptr != nullptr);

	const int      h_m   = _fnt_ptr->get_char_h ();
	const int      scr_w = _page_size [0];

	const int            slot_id = _loc_edit._slot_id;
	const doc::Program & prog    = _view_ptr->use_prog_cur ();
	const doc::Slot &    slot    = prog.use_slot (slot_id);

	check_gui (slot._pi_model);

	const doc::PluginSettings *   settings_main_ptr = nullptr;
	auto           it_settings = slot._settings_all.find (slot._pi_model);
	_mixer_flag = true;
	if (it_settings != slot._settings_all.end ())
	{
		settings_main_ptr = &it_settings->second;
		const piapi::PluginDescInterface &   desc =
			_model_ptr->get_model_desc (slot._pi_model);
		int         nbr_i = 1;
		int         nbr_o = 1;
		int         nbr_s = 0;
		desc.get_nbr_io (nbr_i, nbr_o, nbr_s);
		if (nbr_i == 0 || nbr_o == 0)
		{
			_mixer_flag = false;
		}
	}

	// Order: mixer, main
	std::array <const doc::PluginSettings *, PiType_NBR_ELT> settings_ptr_arr =
	{{
		&slot._settings_mixer,
		settings_main_ptr
	}};
	const std::array <int, PiType_NBR_ELT> nbr_param_arr =
	{{
		(_mixer_flag)
		? int (slot._settings_mixer._param_list.size ()) : 0,
		(settings_main_ptr != nullptr)
		? int (settings_main_ptr->_param_list.size ())   : 0
	}};
	const int      nbr_param_tot = nbr_param_arr [0] + nbr_param_arr [1];

	PageMgrInterface::NavLocList  nav_list;
	_param_list [Col_NAME].resize (nbr_param_tot);
	_param_list [Col_VAL ].resize (nbr_param_tot);
	if (_unit_flag)
	{
		_param_list [Col_UNIT].resize (nbr_param_tot);
	}
	else
	{
		_param_list [Col_UNIT].clear ();
	}
	_menu_sptr->clear_all_nodes ();

	const int      win_h = _menu_sptr->get_bounding_box ().get_size () [1];
	_page_ptr->set_page_step (win_h / h_m);

	int            pos_base = 1;
	_menu_sptr->push_back (_fx_setup_sptr);
	PageMgrInterface::add_nav (nav_list, Entry_FX_SETUP);

	_gui_sptr->show (_gui_flag);
	if (_gui_flag)
	{
		_menu_sptr->push_back (_gui_sptr);
		PageMgrInterface::add_nav (nav_list, Entry_GUI);
		++ pos_base;
	}

	static const std::array <PiType, PiType_NBR_ELT>   type_arr =
	{{
		PiType_MIX,
		PiType_MAIN
	}};

	int            w_unit = 0;
	if (_unit_flag)
	{
		w_unit = 8 * _fnt_ptr->get_char_w (U'M');
	}
	const int      x_val  = scr_w - w_unit;
	const int      x_unit = x_val + _fnt_ptr->get_char_w (U' ');

	int            param_pos = 0;
	for (int type_cnt = 0; type_cnt < PiType_NBR_ELT; ++type_cnt)
	{
		const PiType   type      =      type_arr [type_cnt];
		const int      nbr_param = nbr_param_arr [type_cnt];

		for (int index = 0; index < nbr_param; ++index)
		{
			bool           ctrl_flag = false;
			if (settings_ptr_arr [type_cnt] != nullptr)
			{
				ctrl_flag = settings_ptr_arr [type_cnt]->has_ctrl (index);
			}

			const int      pos_disp = param_pos + pos_base;
			const int      nid_base = param_pos * Col_NBR_ELT;

			const int      nid_name = nid_base + Col_NAME;
			TxtSPtr        name_sptr { std::make_shared <NText> (nid_name) };
			name_sptr->set_coord (Vec2d (0, h_m * pos_disp));
			name_sptr->set_font (*_fnt_ptr);
			name_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
			name_sptr->set_bold (ctrl_flag, true);
			_param_list [Col_NAME] [param_pos] = name_sptr;
			_menu_sptr->push_back (name_sptr);

			const int      nid_val  = nid_base + Col_VAL;
			TxtSPtr        val_sptr { std::make_shared <NText> (nid_val) };
			val_sptr->set_coord (Vec2d (x_val, h_m * pos_disp));
			val_sptr->set_font (*_fnt_ptr);
			val_sptr->set_justification (1, 0, false);
			val_sptr->set_bold (ctrl_flag, true);
			_param_list [Col_VAL] [param_pos] = val_sptr;
			_menu_sptr->push_back (val_sptr);

			if (_unit_flag)
			{
				const int      nid_unit = nid_base + Col_UNIT;
				TxtSPtr        unit_sptr { std::make_shared <NText> (nid_unit) };
				unit_sptr->set_coord (Vec2d (x_unit, h_m * pos_disp));
				unit_sptr->set_font (*_fnt_ptr);
				unit_sptr->set_justification (0, 0, false);
				unit_sptr->set_bold (ctrl_flag, true);
				_param_list [Col_UNIT] [param_pos] = unit_sptr;
				_menu_sptr->push_back (unit_sptr);
			}

			PageMgrInterface::add_nav (nav_list, nid_name);

			update_param_txt (type, index);

			++ param_pos;
		}
	}

	_page_ptr->set_nav_layout (nav_list);

	_menu_sptr->invalidate_all ();
}



void	ParamList::update_param_txt (PiType type, int index)
{
	NodeIdRow      nid_row;
	if (conv_param_to_node_id (nid_row, type, index))
	{
		const int      list_pos  = conv_param_to_list_pos (type, index);
		TxtSPtr &      name_sptr = _param_list [Col_NAME] [list_pos];
		TxtSPtr &      val_sptr  = _param_list [Col_VAL ] [list_pos];
		NText *        unit_ptr  =
			  (_unit_flag)
			? _param_list [Col_UNIT] [list_pos].get ()
			: nullptr;
		const int      slot_id   = _loc_edit._slot_id;

		Tools::set_param_text (
			*_model_ptr, *_view_ptr, _page_size [0], index, -1, slot_id, type,
			name_sptr.get (), *val_sptr, unit_ptr, nullptr, false
		);
	}
}



void	ParamList::update_loc_edit (int node_id)
{
	conv_node_id_to_param (_loc_edit._pi_type, _loc_edit._param_index, node_id);
}



int	ParamList::get_param_list_length () const
{
	return int (_param_list [Col_NAME].size ());
}



// Returns the first node of the pair (param name)
int	ParamList::conv_loc_edit_to_node_id () const
{
	// Default on the first parameter of the main effect
	int            list_pos = conv_param_to_list_pos (PiType_MAIN, 0);

	if (_loc_edit._param_index >= 0)
	{
		list_pos = conv_param_to_list_pos (
			_loc_edit._pi_type,
			_loc_edit._param_index
		);
	}

	// Could become negative is there is no parameter
	list_pos = std::min (list_pos, get_param_list_length () - 1);

	int            node_id = Entry_FX_SETUP;
	if (list_pos >= 0)
	{
		node_id = list_pos * Col_NBR_ELT;
	}

	return node_id;
}



// On output: nid_row [2] contains -1 if the unit is not displayed
// Returns false if the parameter does not exist
bool	ParamList::conv_param_to_node_id (NodeIdRow &nid_row, PiType type, int index) const
{
	assert (type >= 0);
	assert (type < PiType_NBR_ELT);
	assert (index >= 0);

	const int      line_pos = conv_param_to_list_pos (type, index);
	if (line_pos >= get_param_list_length ())
	{
		std::fill (nid_row.begin (), nid_row.end (), -1);
		return false;
	}

	const int      node_id = line_pos * Col_NBR_ELT;
	nid_row [0] = node_id + 0;
	nid_row [1] = node_id + 1;
	nid_row [2] = (_unit_flag) ? node_id + 2 : -1;

	return true;
}



void	ParamList::conv_node_id_to_param (PiType &type, int &index, int node_id)
{
	type = PiType_MIX;
	if (! is_nid_in_param_list (node_id))
	{
		index = -1;
	}
	else
	{
		const int      nbr_param_mix = (_mixer_flag) ? pi::dwm::Param_NBR_ELT : 0;
		int            line_pos      = node_id / Col_NBR_ELT;
		if (line_pos >= nbr_param_mix)
		{
			type      = PiType_MAIN;
			line_pos -= nbr_param_mix;
		}
		index = line_pos;
	}
}



// It is possible to call it with a non-existing parameter
int	ParamList::conv_param_to_list_pos (PiType type, int index) const
{
	assert (type >= 0);
	assert (type < PiType_NBR_ELT);
	assert (index >= 0);

	int            list_pos = index;
	if (_mixer_flag && type == PiType_MAIN)
	{
		list_pos += pi::dwm::Param_NBR_ELT;
	}

	return list_pos;
}



bool	ParamList::is_nid_in_param_list (int node_id) const
{
	int            len = get_param_list_length () * Col_NBR_ELT;
	if (! _unit_flag)
	{
		-- len;
	}

	return (node_id >= 0 && node_id < len);
}



MsgHandlerInterface::EvtProp	ParamList::change_param (int node_id, int dir)
{
	assert (node_id >= 0);
	assert (dir != 0);

	EvtProp        ret_val = EvtProp_PASS;
	PiType         type;
	int            index;
	conv_node_id_to_param (type, index, node_id);
	if (index >= 0)
	{
		const int      step_scale =
			_page_ptr->get_shift (PageMgrInterface::Shift::R) ? 1 : 0;
		ret_val = Tools::change_param (
			*_model_ptr, *_view_ptr, _loc_edit._slot_id, type,
			index, float (Cst::_step_param / pow (10, step_scale)), 0, dir
		);
	}

	return ret_val;
}



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
