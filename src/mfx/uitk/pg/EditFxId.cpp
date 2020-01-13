/*****************************************************************************

        EditFxId.cpp
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
#include "mfx/pi/param/Tools.h"
#include "mfx/piapi/PluginDescInterface.h"
#include "mfx/uitk/pg/EditFxId.h"
#include "mfx/uitk/PageSwitcher.h"
#include "mfx/ui/Font.h"
#include "mfx/Model.h"
#include "mfx/View.h"

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace uitk
{
namespace pg
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



EditFxId::EditFxId (PageSwitcher &page_switcher)
:	_page_switcher (page_switcher)
,	_model_ptr (nullptr)
,	_view_ptr (nullptr)
,	_page_ptr (nullptr)
,	_state (State_NONE)
,	_arg_menu ()
,	_arg_fx_type ()
,	_arg_label ()
,	_chr_per_line (20)
,	_param_ptr (nullptr)
{
	// Nothing
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	EditFxId::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt)
{
	assert (usr_ptr != nullptr);

	_model_ptr = &model;
	_view_ptr  = &view;
	_page_ptr  = &page;
	_param_ptr = static_cast <Param *> (usr_ptr);

	_chr_per_line = page_size [0] / fnt._m.get_char_w ();

	switch (_state)
	{
	case State_NONE:  call_menu ();        break;
	case State_MENU:  handle_menu ();      break;
	case State_FX:    handle_fx_type ();   break;
	case State_LABEL: handle_label ();     break;
	default:
		assert (false);
		break;
	}
}



void	EditFxId::do_disconnect ()
{
	// Nothing
}



MsgHandlerInterface::EvtProp	EditFxId::do_handle_evt (const NodeEvt &evt)
{
	fstb::unused (evt);

	return EvtProp_PASS;
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	EditFxId::call_menu ()
{
	const EntryMenu   sel = 
			(_param_ptr->_fx_id._location_type == doc::FxId::LocType_LABEL)
		? EntryMenu_LABEL
		: EntryMenu_FX_TYPE;
	_arg_menu._title     = "Specify location";
	_arg_menu._selection = int (sel);
	_arg_menu._choice_arr.clear ();
	_arg_menu._choice_arr.push_back ("Effect type");
	_arg_menu._choice_arr.push_back ("Label");
	_arg_menu._check_set.clear ();
	_arg_menu._check_set.insert (_arg_menu._selection);

	_state = State_MENU;
	_page_switcher.call_page (PageType_QUESTION, &_arg_menu, sel);
}



void	EditFxId::handle_menu ()
{
	if (! _arg_menu._ok_flag)
	{
		_param_ptr->_ok_flag = false;
		_state               = State_NONE;
		_page_switcher.return_page ();
	}
	else
	{
		if (_arg_menu._selection == EntryMenu_FX_TYPE)
		{
			_arg_fx_type._choice_arr.clear ();
			add_fx_list (_view_ptr->use_pi_aud_list ());
			add_fx_list (_view_ptr->use_pi_sig_list ());
			_arg_fx_type._check_set.clear ();
			_arg_fx_type._selection = 0;
			if (_param_ptr->_fx_id._location_type == doc::FxId::LocType_CATEGORY)
			{
				const auto        it_beg = _arg_fx_type._choice_arr.begin ();
				const auto        it_end = _arg_fx_type._choice_arr.end ();
				const auto        it     = std::find (
					it_beg, it_end,
					_param_ptr->_fx_id._label_or_model
				);
				if (it != it_end)
				{
					_arg_fx_type._selection = int (std::distance (it, it_beg));
					_arg_fx_type._check_set.insert (_arg_fx_type._selection);
				}
			}

			_state = State_FX;
			_page_switcher.call_page (
				PageType_QUESTION, &_arg_fx_type, EntryMenu_FX_TYPE
			);
		}
		else
		{
			assert (_arg_menu._selection == EntryMenu_LABEL);
			if (_param_ptr->_fx_id._location_type == doc::FxId::LocType_LABEL)
			{
				_arg_label._label = _param_ptr->_fx_id._label_or_model;
			}
			else
			{
				_arg_label._label = "";
			}
			_arg_label._sep_cur_flag = _param_ptr->_cur_preset_flag;

			_state = State_LABEL;
			_page_switcher.call_page (
				PageType_EDIT_LABEL, &_arg_label, EntryMenu_LABEL
			);
		}
	}
}



void	EditFxId::handle_fx_type ()
{
	if (_arg_fx_type._ok_flag)
	{
		const std::vector <std::string> &  pi_aud_list =
			_view_ptr->use_pi_aud_list ();
		_param_ptr->_fx_id._location_type  = doc::FxId::LocType_CATEGORY;
		_param_ptr->_fx_id._label_or_model =
			pi_aud_list [_arg_fx_type._selection];
		_param_ptr->_ok_flag               = true;
		_state = State_NONE;
		_page_switcher.return_page ();
	}
	else
	{
		call_menu ();
	}
}



void	EditFxId::handle_label ()
{
	if (_arg_label._ok_flag)
	{
		_param_ptr->_fx_id._location_type  = doc::FxId::LocType_LABEL;
		_param_ptr->_fx_id._label_or_model = _arg_label._label;
		_param_ptr->_ok_flag               = true;
		_state = State_NONE;
		_page_switcher.return_page ();
	}
	else
	{
		call_menu ();
	}
}



void	EditFxId::add_fx_list (const std::vector <std::string> &fx_list)
{
	for (const auto &plug_id : fx_list)
	{
		const piapi::PluginDescInterface &  desc =
			_model_ptr->get_model_desc (plug_id);
		const std::string name_multi = desc.get_name ();

		const std::string name       = pi::param::Tools::print_name_bestfit (
			_chr_per_line, name_multi.c_str ()
		);

		_arg_fx_type._choice_arr.push_back (name);
	}
}



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
